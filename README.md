# BigInteger (C++ Arbitrary Precision Integer Library)

C++로 구현된 임의 정밀도 정수(Big Integer) 연산 라이브러리입니다.  
기본 정수 타입(`int`, `long long`)으로 표현할 수 없는 매우 큰 수를 다루기 위해 설계되었습니다.

본 라이브러리는 다음 목표를 가지고 배포됩니다.

- 직관적인 사용 방식 (`BigInteger a("1234")`)
- C++ 기본 연산자 연동
- 큰 수 연산에서도 안정적인 동작
- 쉽게 확장 가능한 구조

---

## 🔧 Features

현재 라이브러리가 제공하는 기능은 다음과 같습니다.

- 문자열 기반 생성자 (`BigInteger("12345678901234567890")`)
- 사칙연산 ( `+`, `-`, `*`, `/`, `%` )
- 비교 연산 ( `==`, `!=`, `<`, `<=`, `>`, `>=` )
- 부호 판별
- 스트림 연산자 ( `<<`, `>>` )
- 증감, 대입 연산자 ( `+=`, `-=`, `++`, `--`, `=`)
- string 및 int, long long 변환 함수 제공
- 내부 구조  
  - base = 10^4
  - little-endian vector 저장 방식
- 10^100 까지 연산속도 보장
- Windows / x64 / MSVC Release 빌드

---

## 📘 Example

```cpp
#include "BigInteger.h"
#include <iostream>

int main()
{
    bigint::BigInteger a("12345678901234567890");
    bigint::BigInteger b("9876543210");

    bigint::BigInteger sum = a + b;
    bigint::BigInteger mul = a * b;

    std::cout << "Sum : " << sum << std::endl;
    std::cout << "Mul : " << mul << std::endl;

    return 0;
}
```

---

## 🏗️ How to Build

❗ 현재 버전은 Window x64 MSVC Release 빌드에서만 사용 가능합니다. 추후 확장 예정입니다.

1. Static Library (BigInteger.lib)와 Header (BigInteger.h) 파일을 다운로드 합니다.
2. BigInteger를 사용하려는 C++ 프로젝트에서 다음과 같이 설정합니다.
    - include 경로에 BigInteger.h 가 있는 폴더 추가
    - 링커 입력에 Library를 추가
    
---

## 🔌 Usage Notes

- namespace bigint를 지정하여 사용하여 주십시오.
- 내부 기수(base)는 10^4이며 정수는 vector<int>로 저장됩니다.
- 연산 속도는 기수 변경과 알고리즘 개선을 통해 향후 더 최적화될 예정입니다.
- Karatsuba, FFT 기반 곱셈, 모듈러 연산 확장 등이 업데이트될 수 있습니다.

---