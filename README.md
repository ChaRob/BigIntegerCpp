# BigInteger (C++ Arbitrary Precision Integer Library)

C++로 구현된 임의 정밀도 정수(Big Integer) 연산 라이브러리입니다.  
기본 정수 타입(`int`, `long long`)으로 표현할 수 없는 매우 큰 수를 다루기 위해 설계되었습니다.

본 라이브러리는 다음 목표를 가지고 배포됩니다.

- 직관적인 사용 방식 (`BigInteger a("1234")`)
- C++ 기본 연산자 연동
- 큰 수 연산에서도 안정적인 동작
- 쉽게 확장 가능한 구조

---

## ?? Features

현재 라이브러리가 제공하는 기능은 다음과 같습니다.

- 문자열 기반 생성자 (`BigInteger("12345678901234567890")`)
- 사칙연산 ( `+`, `-`, `*`, `/`, `%` )
- 비교 연산 ( `==`, `!=`, `<`, `<=`, `>`, `>=` )
- 부호 판별
- 스트림 연산자 ( `<<`, `>>` )
- 증감, 대입 연산자 ( `+=`, `-=`, `++`, `--`, `=`)
- string 및 int, long long 변환 함수 제공
- 내부 구조  
  - base = 10^9
  - little-endian vector 저장 방식
- 10^100 까지 연산속도 보장
- BigInteger 용 수학 함수 (Pow, ModPow, Gcd, Lcm)
- 곱셈 연산 Karatsuba 방식 적용
- Windows / x64 / MSVC Release 빌드
- Division 시 Knuth 알고리즘은 구현되어 있으나 비활성화됨.

---

## ?? Example

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
## ?? Benchmark

### Test Environment

```Text
Compiler : MSVC (Visual Studio)
Build    : Release x64
Optimization : /O2
Base : 10^9
Karatsuba : Enabled
Knuth Division : Disabled
```

모든 테스트는 랜덤 BigInteger를 사용하여 수행

### Arithmetic Benchmark

#### Addition / Subtraction

| Digits | Add    | Sub    |
| ------ | ------ | ------ |
| 100    | < 1 ms | < 1 ms |
| 1,000  | < 1 ms | < 1 ms |
| 10,000 | < 1 ms | < 1 ms |

#### Multiplication (Karatsuba)

| Digits | Time   |
| ------ | ------ |
| 500    | < 1 ms |
| 1,000  | < 1 ms |
| 2,000  | < 1 ms |
| 5,000  | 1 ms   |
| 10,000 | 5 ms   |

Karatsuba multiplication automatically activates for large numbers.

#### Division

| Dividend / Divisor | Time   |
| ------------------ | ------ |
| 200 / 100 digits   | 2 ms   |
| 300 / 150 digits   | 3 ms   |
| 500 / 250 digits   | 7 ms   |
| 1000 / 500 digits  | 22 ms  |
| 5000 / 2500 digits | 167 ms |

Number Theory Benchmark

#### GCD

| Digits | Time   |
| ------ | ------ |
| 100    | 11 ms  |
| 500    | 111 ms |
| 1000   | 313 ms |

#### Modular Exponentiation

| Base       | Exp | Mod        | Time   |
| ---------- | --- | ---------- | ------ |
| 50 digits  | 20  | 50 digits  | 48 ms  |
| 100 digits | 30  | 100 digits | 172 ms |
| 200 digits | 50  | 200 digits | 379 ms |


#### Stability Tests

Division stress tests executed successfully.

```
StressDivTest passed (1000 / 500 digits, 1000 iterations)
StressDivTest passed (5000 / 2500 digits, 200 iterations)
```

---

## ??? How to Build

? 현재 버전은 Window x64 MSVC Release 빌드에서만 사용 가능합니다. 추후 확장 예정입니다.

1. Static Library (BigInteger.lib)와 Header (BigInteger.h) 파일을 다운로드 합니다.
2. BigInteger를 사용하려는 C++ 프로젝트에서 다음과 같이 설정합니다.
    - include 경로에 BigInteger.h 가 있는 폴더 추가
    - 링커 입력에 Library를 추가
    
---

## ?? Usage Notes

- namespace bigint를 지정하여 사용하여 주십시오.
- 내부 기수(base)는 10^9이며 정수는 vector<int>로 저장됩니다.
- int, long long과의 연산을 지원하지만 float, double과는 사용하지 않는 것을 권장합니다.
- FFT 기반 곱셈이 업데이트될 예정입니다.

---