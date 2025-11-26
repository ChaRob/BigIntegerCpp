#include "BigInteger.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

using ll = long long;

namespace bigint
{
	BigInteger::BigInteger(std::string _data) : m_isNegative(false)
	{
		MakeDigit(_data);
	}

	BigInteger::BigInteger(const char* _data) : m_isNegative(false)
	{
		if (_data == nullptr)
		{
			m_digit.clear();
			m_digit.push_back(0);
			return;
		}
		MakeDigit(std::string(_data));
	}

	BigInteger::BigInteger(int _value) : m_isNegative(false)
	{
		MakeDigit(std::to_string(_value));
	}

	BigInteger::BigInteger(long long _value) : m_isNegative(false)
	{
		MakeDigit(std::to_string(_value));
	}

	BigInteger::BigInteger(unsigned int _value) : m_isNegative(false)
	{
		MakeDigit(std::to_string(_value));
	}

	BigInteger::BigInteger(unsigned long long _value) : m_isNegative(false)
	{
		MakeDigit(std::to_string(_value));
	}

	BigInteger::BigInteger(double _value) : m_isNegative(false)
	{
		if (std::isnan(_value) || std::isinf(_value))
		{
			throw std::invalid_argument("BigInteger: NaN or Inf is not supported");
		}

		// 소수 부분 제거
		long double tmp = std::trunc(static_cast<long double>(_value));

		if (tmp == 0.0L)
		{
			m_digit.clear();
			m_digit.push_back(0);
			m_isNegative = false;
			return;
		}

		// 음수일 시 부호 변환
		if (tmp < 0.0L)
		{
			m_isNegative = true;
			tmp = -tmp;
		}

		m_digit.clear();
		while (tmp > 0.0L)
		{
			// base마다 자른 chunk 넣기
			long double quotient = std::floor(tmp / m_base);
			int chunk = static_cast<int>(tmp - quotient * m_base);
			m_digit.push_back(chunk);
			tmp = quotient;
		}

		Normalize();
	}

	BigInteger::~BigInteger()
	{
		m_digit.clear();
	}

	BigInteger BigInteger::operator+(const BigInteger& _other) const
	{
		BigInteger result;
		// 부호가 같은 경우 절대값으로 더하고 부호 붙이기
		if (m_isNegative == _other.m_isNegative)
		{
			result = AddAbs(*this, _other);
			result.m_isNegative = m_isNegative;

			if (result.m_digit.size() == 1 && result.m_digit[0] == 0)
				result.m_isNegative = false;

			return result;
		}

		// 부호가 다른 경우 두 수를 비교 후 연산
		int cmp = CompareAbs(_other);

		if (cmp == 0) return BigInteger("0");

		const BigInteger* bigger = nullptr;
		const BigInteger* smaller = nullptr;
		if (cmp > 0)
		{
			bigger = this;
			smaller = &_other;
		}
		else if (cmp < 0)
		{
			bigger = &_other;
			smaller = this;
		}
		result = SubAbs(*bigger, *smaller);
		result.m_isNegative = bigger->m_isNegative;

		if (result.m_digit.size() == 1 && result.m_digit[0] == 0)
			result.m_isNegative = false;

		return result;
	}

	BigInteger BigInteger::operator-(const BigInteger& _other) const
	{
		BigInteger temp = _other;
		// 부호 반전
		if (!(temp.m_digit.size() == 1 && temp.m_digit[0] == 0))
		{
			temp.m_isNegative = !temp.m_isNegative;
		}

		return (*this) + temp;
	}

	BigInteger BigInteger::operator*(const BigInteger& _other) const
	{
		BigInteger result;
		// XOR 연산으로 부호 처리
		result.m_isNegative = (m_isNegative != _other.m_isNegative);

		// 자릿수 미리 확보
		result.m_digit.assign(m_digit.size() + _other.m_digit.size(), 0);
		for (int i = 0; i < m_digit.size(); i++)
		{
			ll carry = 0;

			for (int j = 0; j < _other.m_digit.size(); j++)
			{
				ll value = result.m_digit[i + j]
					+ (ll)m_digit[i] * _other.m_digit[j]
					+ carry;

				result.m_digit[i + j] = value % m_base;
				carry = value / m_base;
			}

			// 마지막 자리의 carry는 고정된 위치에만 더함
			result.m_digit[i + _other.m_digit.size()] += (int)carry;
		}

		result.Normalize();

		if (result.m_digit.size() == 1 && result.m_digit[0] == 0)
			result.m_isNegative = false;

		return result;
	}

	BigInteger BigInteger::operator/(const BigInteger& _other) const
	{
		// 0으로 나누기
		if (_other.m_digit.size() == 1 && _other.m_digit[0] == 0)
		{
			throw std::runtime_error("Division by zero");
		}

		// 절댓값 버전 준비
		BigInteger absA = *this;
		absA.m_isNegative = false;

		BigInteger absB = _other;
		absB.m_isNegative = false;

		BigInteger quotient;
		BigInteger remainder;
		DivModAbs(absA, absB, quotient, remainder);

		// 부호 = 서로 다른 부호면 음수
		quotient.m_isNegative = (m_isNegative != _other.m_isNegative);

		// 몫이 0이면 부호는 항상 양수로
		if (quotient.m_digit.size() == 1 && quotient.m_digit[0] == 0)
			quotient.m_isNegative = false;

		return quotient;
	}

	BigInteger BigInteger::operator%(const BigInteger& _other) const
	{
		// 0으로 나누기
		if (_other.m_digit.size() == 1 && _other.m_digit[0] == 0)
		{
			throw std::runtime_error("Division by zero");
		}

		// 절댓값 버전 준비
		BigInteger absA = *this;
		absA.m_isNegative = false;

		BigInteger absB = _other;
		absB.m_isNegative = false;

		BigInteger quotient;
		BigInteger remainder;
		DivModAbs(absA, absB, quotient, remainder);

		// C/C++ 기본 정수와 동일하게:
		//   a % b 의 부호는 항상 a 와 같음
		remainder.m_isNegative = m_isNegative;

		// 나머지가 0이면 부호는 양수로 정리
		if (remainder.m_digit.size() == 1 && remainder.m_digit[0] == 0)
			remainder.m_isNegative = false;

		return remainder;
	}

	bool BigInteger::operator==(const BigInteger& _other) const
	{
		return Compare(_other) == 0;
	}

	bool BigInteger::operator!=(const BigInteger& _other) const
	{
		return Compare(_other) != 0;
	}

	bool BigInteger::operator<(const BigInteger& _other) const
	{
		return Compare(_other) < 0;
	}

	bool BigInteger::operator<=(const BigInteger& _other) const
	{
		return Compare(_other) <= 0;
	}

	bool BigInteger::operator>(const BigInteger& _other) const
	{
		return Compare(_other) > 0;
	}

	bool BigInteger::operator>=(const BigInteger& _other) const
	{
		return Compare(_other) >= 0;
	}

	std::ostream& operator<<(std::ostream& _os, const BigInteger& _value)
	{
		_os << _value.ToString();
		return _os;
	}

	std::istream& operator>>(std::istream& _is, BigInteger& _value)
	{
		std::string s;
		_is >> s;
		_value = BigInteger(s);
		return _is;
	}

	std::string BigInteger::ToString() const
	{
		if (m_digit.empty()) return "0";

		std::string data = "";
		if (m_isNegative) data = "-";

		data += std::to_string(m_digit.back());

		for (int i = m_digit.size() - 2; i >= 0; i--)
		{
			std::string block = std::to_string(m_digit[i]);
			while (block.size() < 4) block = '0' + block;
			data += block;
		}
		return data;
	}

	int BigInteger::ToInt() const
	{
		long long value = ToLongLong();
		if (value > std::numeric_limits<int>::max() ||
			value < std::numeric_limits<int>::min())
			throw std::overflow_error("BigInteger: value exceeds int range");
		return static_cast<int>(value);
	}

	long long BigInteger::ToLongLong() const
	{
		// check if zero
		if (m_digit.size() == 1 && m_digit[0] == 0)
			return 0;

		// 자료형 내에 들어오는지 검사
		//	음수의 경우 LLONG_MAX + 1 까지 (LLONG_MIN)
		//	양수의 경우 LLONG_MAX 까지
		unsigned long long ulimit = std::numeric_limits<long long>::max();
		unsigned long long limit = m_isNegative ? (ulimit + 1) : ulimit;
		unsigned long long value = 0;

		for (int i = m_digit.size() - 1; i >= 0; i--)
		{
			unsigned int digit = static_cast<unsigned int>(m_digit[i]);

			// Normalize가 에러가 발생한 경우 방어 동작 넣기
			if (digit > static_cast<unsigned int>(m_base))
				throw std::logic_error("BigInteger: invalid digit state");

			// value에 값 넣기 전 테스트
			if (value > (limit - digit) / static_cast<unsigned long long>(m_base))
				throw std::overflow_error("BigInteger: value exceeds long long range");

			value = value * static_cast<unsigned long long>(m_base) + digit;
		}

		// 양수의 경우
		if (!m_isNegative)
			return static_cast<long long>(value);

		// 음수 min 값 처리
		if (value == ulimit + 1)
			return std::numeric_limits<long long>::min();

		return -static_cast<long long>(value);
	}

	bool BigInteger::TryToInt(int& _out) noexcept
	{
		try
		{
			_out = ToInt();
			return true;
		}
		catch (const std::exception&)
		{
			return false;
		}
	}

	bool BigInteger::TryToLongLong(long long& _out) noexcept
	{
		try
		{
			_out = ToLongLong();
			return true;
		}
		catch (const std::exception&)
		{
			return false;
		}
	}

	void BigInteger::MakeDigit(std::string _data)
	{
		m_digit.clear();

		// erase whitespace
		while (!_data.empty() && std::isspace(_data.front()) != 0)
			_data.erase(_data.begin());
		while (!_data.empty() && std::isspace(_data.back()) != 0)
			_data.pop_back();

		// if empty return 0
		if (_data.empty() || _data == "0")
		{
			m_digit.push_back(0);
			m_isNegative = false;
			return;
		}

		// check if data is negative
		if (_data[0] == '-' || _data[0] == '+')
		{
			if (_data[0] == '-')
				m_isNegative = true;

			_data.erase(0, 1);
			if (_data.empty())
				throw std::invalid_argument("BigInteger: sign only string");
		}

		for (char c : _data)
		{
			if (c < '0' || c > '9')
				throw std::invalid_argument("BigInteger: invalid charactor in string");
		}

		// erase unnecessary the front of '0'
		auto firstNonZero = _data.find_first_not_of('0');
		if (firstNonZero == std::string::npos)
		{
			m_isNegative = false;
			m_digit.push_back(0);
			return;
		}
		if (firstNonZero > 0)
			_data.erase(0, firstNonZero);

		// split string data
		int dataIndex = _data.size();
		while (dataIndex > 0)
		{
			int chunkSize = std::min(m_chunk, dataIndex);
			int start = dataIndex - chunkSize;
			if (start < 0)
				start = 0;

			std::string digit = _data.substr(start, chunkSize);
			int chunkValue = std::stoi(digit);

			m_digit.push_back(chunkValue);
			dataIndex -= chunkSize;
		}
	}

	void BigInteger::Normalize()
	{
		ll carry = 0;
		for (int i = 0; i < m_digit.size(); i++)
		{
			ll value = (ll)m_digit[i] + carry;

			carry = value / m_base;
			m_digit[i] = (int)(value % m_base);

			if (m_digit[i] < 0)
			{
				m_digit[i] += m_base;
				carry--;
			}
		}

		while (carry > 0)
		{
			m_digit.push_back(carry % m_base);
			carry /= m_base;
		}

		while (m_digit.size() > 1 && m_digit.back() == 0)
			m_digit.pop_back();
	}

	int BigInteger::CompareAbs(const BigInteger& _other) const
	{
		if (m_digit.size() > _other.m_digit.size()) return 1;
		if (m_digit.size() < _other.m_digit.size()) return -1;

		// 크기가 같을 때 사이즈 비교 넣기
		for (int i = m_digit.size() - 1; i >= 0; i--)
		{
			if (m_digit[i] > _other.m_digit[i]) return 1;
			if (m_digit[i] < _other.m_digit[i]) return -1;
		}

		// 완전 동일
		return 0;
	}

	int BigInteger::Compare(const BigInteger& _other) const
	{
		// 부호가 다른 경우 음수가 더 적다
		if (m_isNegative != _other.m_isNegative)
		{
			return m_isNegative ? -1 : 1;
		}

		// 둘 다 양수거나 음수
		int cmp = CompareAbs(_other);
		if (!m_isNegative)
		{
			// 둘 다 양수라면 그대로
			return cmp;
		}
		else
		{
			// 둘 다 음수라면 반대로 -> |a| < |b| 라면 a > b
			return -cmp;
		}
	}

	BigInteger BigInteger::AddAbs(const BigInteger& _a, const BigInteger& _b)
	{
		BigInteger result;
		result.m_digit.clear();
		int maxSize = std::max(_a.m_digit.size(), _b.m_digit.size());
		ll carry = 0;

		// 두 수의 최대 사이즈를 비교한 뒤, 각 항목에서 더하기.
		for (int i = 0; i < maxSize; i++)
		{
			ll value = carry;
			if (i < _a.m_digit.size()) value += _a.m_digit[i];
			if (i < _b.m_digit.size()) value += _b.m_digit[i];

			result.m_digit.push_back((int)(value % m_base));
			carry = value / m_base;
		}

		if (carry > 0)
			result.m_digit.push_back((int)carry);

		// 0 정리
		result.Normalize();
		return result;
	}

	BigInteger BigInteger::SubAbs(const BigInteger& _a, const BigInteger& _b)
	{
		BigInteger result;
		result.m_digit.clear();
		ll borrow = 0;

		for (int i = 0; i < _a.m_digit.size(); i++)
		{
			ll value = _a.m_digit[i] - borrow;
			if (i < _b.m_digit.size()) value -= _b.m_digit[i];

			if (value < 0)
			{
				value += m_base;
				borrow = 1;
			}
			else borrow = 0;

			result.m_digit.push_back((int)value);
		}

		// 0 정리
		result.Normalize();
		return result;
	}

	BigInteger BigInteger::MulSmall(const BigInteger& _a, int _factor)
	{
		BigInteger result;

		// 곱하는 수가 0일시, 초기값 0을 반환
		if (_factor == 0) return result;

		result.m_digit.clear();

		ll carry = 0;
		for (int i = 0; i < _a.m_digit.size(); i++)
		{
			ll value = (ll)_a.m_digit[i] * _factor + carry;
			result.m_digit.push_back(value % m_base);
			carry = value / m_base;
		}

		while (carry > 0)
		{
			result.m_digit.push_back((int)(carry % m_base));
			carry /= m_base;
		}
		result.Normalize();

		return result;
	}

	// 항상 양수만 받음
	void BigInteger::DivModAbs(const BigInteger& _a, const BigInteger& _b, BigInteger& _quotient, BigInteger& _remainder)
	{
		// 0으로 나누기 체크 (안전용, 보통 operator/에서 먼저 체크)
		if (_b.m_digit.size() == 1 && _b.m_digit[0] == 0)
		{
			throw std::runtime_error("Division by zero");
		}

		int cmp = _a.CompareAbs(_b);
		// |a| < |b| 몫 = 0, 나머지 = a
		if (cmp < 0)
		{
			_quotient.m_digit.clear();
			_quotient.m_digit.push_back(0);
			_quotient.m_isNegative = false;

			_remainder = _a;
			_remainder.m_isNegative = false;
			return;
		}
		// |a| = |b| 몫 = 1, 나머지 = 0
		if (cmp == 0)
		{
			_quotient.m_digit.clear();
			_quotient.m_digit.push_back(1);
			_quotient.m_isNegative = false;

			_remainder.m_digit.clear();
			_remainder.m_digit.push_back(0);
			_remainder.m_isNegative = false;
			return;
		}

		_remainder = _a;
		_remainder.m_isNegative = false;

		int n = _remainder.m_digit.size();
		int m = _b.m_digit.size();

		int maxShift = n - m;
		_quotient.m_digit.assign(maxShift + 1, 0);
		_quotient.m_isNegative = false;

		BigInteger shiftedDivisor;

		for (int shift = maxShift; shift >= 0; shift--)
		{
			// shiftedDivisor = _b * base^shift
			shiftedDivisor.m_digit.clear();
			shiftedDivisor.m_digit.reserve(m + shift);
			shiftedDivisor.m_digit.insert(shiftedDivisor.m_digit.end(), shift, 0);
			shiftedDivisor.m_digit.insert(shiftedDivisor.m_digit.end(),
				_b.m_digit.begin(), _b.m_digit.end());
			shiftedDivisor.m_isNegative = false;
			shiftedDivisor.Normalize();

			// remainder가 이 만큼도 안되면 이 자릿수의 몫은 0
			if (_remainder.CompareAbs(shiftedDivisor) < 0)
				continue;

			// quotient digit ∈ [1, m_base-1] 이진탐색
			int low = 1;
			int high = m_base - 1;
			int best = 0;

			while (low <= high)
			{
				int mid = (low + high) / 2;
				BigInteger prod = MulSmall(shiftedDivisor, mid);
				int comp = prod.CompareAbs(_remainder);

				if (comp <= 0)
				{
					best = mid;       // 아직 remainder보다 작거나 같음 → 더 키울 수 있음
					low = mid + 1;
				}
				else
				{
					high = mid - 1;   // 너무 큼 → 줄이기
				}
			}

			if (best > 0)
			{
				BigInteger sub = MulSmall(shiftedDivisor, best);
				_remainder = SubAbs(_remainder, sub);   // |remainder| >= |sub| 보장
				_remainder.m_isNegative = false;
				_quotient.m_digit[shift] = best;
			}
		}

		_quotient.Normalize();
		_remainder.Normalize();
	}
}