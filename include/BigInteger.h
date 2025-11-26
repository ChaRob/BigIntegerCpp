#pragma once
//////////////////////////
//		시스템 헤더		//
//////////////////////////
#include <vector>
#include <string>
#include <iostream>

class BigInteger
{
	// 생성자, 소멸자 정의
public:
	BigInteger(std::string _data = "");
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
	std::string ToString() const;

	// 내부 멤버 함수 정의
private:
	void MakeDigit(std::string _data = "");
	void Normalize();
	int CompareAbs(const BigInteger& _other) const; // 1 : this 멤버, -1 : 매개변수 멤버, 0 : 완전 동일
	int Compare(const BigInteger& _other) const; // CompareAbs와 동일, 부호 포함

	// 절대값 연산 헬퍼
	static BigInteger AddAbs(const BigInteger& _a, const BigInteger& _b);
	static BigInteger SubAbs(const BigInteger& _a, const BigInteger& _b); // |a| > |b| 가정
	static BigInteger MulSmall(const BigInteger& _a, int _factor);
	static void DivModAbs(const BigInteger& _a, const BigInteger& _b, BigInteger& _quotient, BigInteger& _remainder);
};