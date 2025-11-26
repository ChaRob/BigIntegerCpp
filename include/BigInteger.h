#pragma once
//////////////////////////
//		시스템 헤더		//
//////////////////////////
#include <vector>
#include <string>
#include <iosfwd>

namespace bigint
{
	class BigInteger
	{
		// 생성자, 소멸자 정의
	public:
		BigInteger(std::string _data = "");
		BigInteger(const char* _data);
		BigInteger(int _value);
		BigInteger(long long _value);
		BigInteger(unsigned int _value);
		BigInteger(unsigned long long _value);

		explicit BigInteger(double _value);		// 실수 -> 정수부 암시적 형변환 방지

		~BigInteger();

		// 공용 멤버 변수 정릐
	public:
		static const int m_base = 10000;	// 기수, 항상 10의 제곱으로 표현되야함
		static const int m_chunk = 4;		// 기수에 따른 사이즈

		// 클래스 멤버 변수 정의
	private:
		std::vector<int> m_digit;	// little endian 배열
		bool m_isNegative;			// 음수 데이터 체크

		// Operator 정의
	public:
		BigInteger operator+(const BigInteger& _other) const;
		BigInteger operator-(const BigInteger& _other) const;
		BigInteger operator*(const BigInteger& _other) const;
		BigInteger operator/(const BigInteger& _other) const;
		BigInteger operator%(const BigInteger& _other) const;

		bool operator==(const BigInteger& _other) const;
		bool operator!=(const BigInteger& _other) const;
		bool operator<(const BigInteger& _other) const;
		bool operator<=(const BigInteger& _other) const;
		bool operator>(const BigInteger& _other) const;
		bool operator>=(const BigInteger& _other) const;

		friend std::ostream& operator<<(std::ostream& _os, const BigInteger& _value);
		friend std::istream& operator>>(std::istream& _is, BigInteger& _value);

		// 외부 멤버 함수 정의
	public:
		// BigInteger 형 변환
		std::string ToString() const;
		int ToInt() const;
		long long ToLongLong() const;
		bool TryToInt(int& _out) noexcept;				// noexcept 키워드로 내부에서 예외 처리하지 않고 흘려보내기 (false)
		bool TryToLongLong(long long& _out) noexcept;

		// 내부 멤버 함수 정의
	private:
		void MakeDigit(std::string _data = "");
		void Normalize();
		int CompareAbs(const BigInteger& _other) const; // 1 : this 멤버, -1 : 매개변수 멤버, 0 : 완전 동일
		int Compare(const BigInteger& _other) const;	// CompareAbs와 동일, 부호 포함

		// 절대값 연산 헬퍼
		static BigInteger AddAbs(const BigInteger& _a, const BigInteger& _b);
		static BigInteger SubAbs(const BigInteger& _a, const BigInteger& _b); // |a| > |b| 가정
		static BigInteger MulSmall(const BigInteger& _a, int _factor);
		static void DivModAbs(const BigInteger& _a, const BigInteger& _b, BigInteger& _quotient, BigInteger& _remainder);
	};
}