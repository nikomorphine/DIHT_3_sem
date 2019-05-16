//
//  main.cpp
//  bigInt
//
//  Created by Vladislav Terzi on 19/12/2018.
//  Copyright © 2018 Vladislav Terzi. All rights reserved.
//

#include <iostream>
#include <string>
#include <vector>
//constant for base digit number
const int BASE = 9;
const long BIG = 1000000000;
class BigInteger {
public:
    //constructors===========================================================================================
    BigInteger() : sign(1) {}
    
    BigInteger(int num) {
        number.push_back(::abs(num));
        sign = num < 0 ? false : true;
    }
    //casts==================================================================================================
    operator bool() const {
        for (auto i : number) {
            if (i != 0) {
                return true;
            }
        }
        return false;
    }
    operator int() const {
        return number[0];
    }
    //operators==============================================================================================
    //summatio
    BigInteger operator+(const BigInteger &newNum) const {
        if (sign == newNum.sign) {
            BigInteger res;
            res.sign = sign;
            if (newNum <= *this) {
                res = *this;
                long carry = 0;
                long carryOld = 0;
                int i = 0;
                for (; i < static_cast<int>(newNum.number.size()); ++i) {
                    carry = (carry + number[i] + newNum.number[i]) >= BIG ? 1 : 0;
                    if (!carry) {
                        res.number[i] = number[i] + newNum.number[i] + (int) carryOld;
                    } else {
                        res.number[i] = number[i] + newNum.number[i] + (int) carryOld - BIG;
                    }
                    carryOld = carry;
                }
                for (; i < static_cast<int>(number.size()); ++i) {
                    carry = (carry + number[i]) >= BIG ? 1 : 0;
                    if (!carry) {
                        res.number[i] = number[i] + (int) carryOld;
                    } else {
                        res.number[i] = number[i] + (int) carryOld - BIG;
                    }
                    carryOld = carry;
                }
                if (carry) {
                    res.number.push_back(1);
                }
            } else {
                res = *this;
                res = newNum + res;
            }
            return res;
        } else if (sign) {
            return *this - (-newNum);
        } else {
            return newNum - (-*this);
        }
    }
    void operator +=(BigInteger newNum) {
        *this + newNum;
    }
    //difference
    BigInteger operator-(const BigInteger &newNum) const {
        if (newNum == BigInteger(0)) {
            return *this;
        } else if (*this == BigInteger(0)) {
            return -newNum;
        }
        if (sign == newNum.sign) {
            BigInteger res;
            res.sign = sign;
            if (newNum <= *this) {
                res = *this;
                long borrow = 0;
                long borrowOld = 0;
                int i = 0;
                for (; i < static_cast<int>(newNum.number.size()); ++i) {
                    borrow = (borrow - newNum.number[i] + number[i]) < 0 ? -1 : 0;
                    if (!borrow) {
                        res.number[i] = - newNum.number[i] + number[i] + (int) borrowOld;
                    } else {
                        res.number[i] = newNum.number[i] - number[i] + (int) borrowOld;
                    }
                    borrowOld = borrow;
                }
                for (; i < static_cast<int>(number.size()); ++i) {
                    borrow = (borrow + number[i]) < 0 ? -1 : 0;
                    if (!borrow) {
                        res.number[i] = number[i] + (int) borrowOld;
                    } else {
                        res.number[i] = number[i] + (int) borrowOld + BIG;
                    }
                    borrowOld = borrow;
                }
            } else {
                res = *this;
                res = newNum - res;
                res = -res;
            }
            if (res.number.back() == 0 && res.number.size() != 1) {
                res.number.pop_back();
            }
            return res;
        } else if (sign) {
            return *this + (-newNum);
        } else {
            return - (newNum + (-*this));
        }
    }
    
    void operator -=(BigInteger newNum) {
        *this - newNum;
    }
    //multiplication
    //base conversion
    static std::vector<int> convertBase(const std::vector<int>& num, int oldDigits, int newDigits) {
        std::vector<long long> p(std::max(oldDigits, newDigits) + 1);
        p[0] = 1;
        for (int i = 1; i < static_cast<int>(p.size()); i++) {
            p[i] = p[i - 1] * 10;
        }
        std::vector<int> res;
        long long cur = 0;
        int curDigits = 0;
        for (int i = 0; i < static_cast<int>(num.size()); i++) {
            cur += num[i] * p[curDigits];
            curDigits += oldDigits;
            while (curDigits >= newDigits) {
                res.push_back(int(cur % p[newDigits]));
                cur /= p[newDigits];
                curDigits -= newDigits;
            }
        }
        res.push_back((int) cur);
        while (!res.empty() && !res.back())
            res.pop_back();
        return res;
    }
    typedef std::vector<long long> vll;
    //karatsuba multiplication
    static vll karatsubaMultiply(const vll& num1, const vll& num2) {
        int n = static_cast<int>(num1.size());
        vll res(n + n);
        if (n <= 32) {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    res[i + j] += num1[i] * num2[j];
                }
            }
            return res;
        }
        
        int k = n >> 1;
        vll num1L(num1.begin(), num1.begin() + k);
        vll num1R(num1.begin() + k, num1.end());
        vll num2L(num2.begin(), num2.begin() + k);
        vll num2R(num2.begin() + k, num2.end());
        
        vll LL = karatsubaMultiply(num1L, num2L);
        vll RR = karatsubaMultiply(num1R, num2R);
        
        for (int i = 0; i < k; i++) {
            num1L[i] += num1L[i];
        }
        for (int i = 0; i < k; i++) {
            num2R[i] += num2L[i];
        }
        
        vll r = karatsubaMultiply(num1R, num2R);
        for (int i = 0; i < static_cast<int>(LL.size()); i++) {
            r[i] -= LL[i];
        }
        for (int i = 0; i < static_cast<int>(RR.size()); i++) {
            r[i] -= RR[i];
        }
        for (int i = 0; i < (int) r.size(); i++) {
            res[i + k] += r[i];
        }
        for (int i = 0; i < static_cast<int>(LL.size()); i++) {
            res[i] += LL[i];
        }
        for (int i = 0; i < static_cast<int>(RR.size()); i++) {
            res[i + n] += RR[i];
        }
        return res;
    }
    //operator *
    BigInteger operator*(const BigInteger& newNum) const {
        if ((number.size() == 1 and number[0] == 0) or
            (newNum.number.size() == 1 and newNum.number[0] == 0)) {
            BigInteger res;
            res.number.push_back(0);
            return res;
        }
        std::vector<int> firstNum6 = convertBase(this->number, BASE, 6);
        std::vector<int> secondNum6 = convertBase(newNum.number, BASE, 6);
        vll firstNum(firstNum6.begin(), firstNum6.end());
        vll secondNum(secondNum6.begin(), secondNum6.end());
        while (firstNum.size() < secondNum.size()) {
            firstNum.push_back(0);
        }
        while (secondNum.size() < firstNum.size()) {
            secondNum.push_back(0);
        }
        while (firstNum.size() & (firstNum.size() - 1)) {
            firstNum.push_back(0);
            secondNum.push_back(0);
        }
        vll mult = karatsubaMultiply(firstNum, secondNum);
        BigInteger res;
        res.sign = sign * newNum.sign;
        for (int i = 0, carry = 0; i < (int) mult.size(); i++) {
            long long cur = mult[i] + carry;
            res.number.push_back((int) (cur % 1000000));
            carry = (int) (cur / 1000000);
        }
        res.number = convertBase(res.number, 6, BASE);
        return res;
    }
    //division
    void operator/=(int div) {
        if (div < 0) {
            sign = 1 - sign;
            div = -div;
        }
        for (int i = static_cast<int>(number.size()) - 1, rem = 0; i >= 0; --i) {
            long long cur = number[i] + rem * (long long) BIG;
            number[i] = (int) (cur / div);
            rem = (int) (cur % div);
        }
        while (number.back() == 0) {
            number.pop_back();
        }
    }
    BigInteger devideUnsigned(BigInteger num1, BigInteger num2) {
        BigInteger left = BigInteger(0);
        BigInteger right = num1;
        if (num2 > num1) {
            return left;
        }
        while (left + BigInteger(1) < right) {
            BigInteger temp = right - left;
            std::cout << temp << std::endl;
            temp /= 2;
            BigInteger mid = left + temp;
            if (num2 * mid <= num1) {
                left = mid;
            } else {
                right = mid;
            }
        }
        return left;
    }
    BigInteger divide(BigInteger num1, BigInteger num2) {
        BigInteger res;
        if (num1.sign == 1 and num2.sign == 0) {
            res = divide(num1, -num2);
            res.sign = 0;
            return res;
        } else if (num1.sign == 0) {
            if (num2.sign == 0) {
                res = devideUnsigned(-num1, -num2);
                return res;
            }
            else {
                res = divide(-num1, num2);
                return res;
            }
        }
        return devideUnsigned(num1, num2);
    }
    BigInteger operator/(const BigInteger& newNum) {
        BigInteger temp = *this;
        return divide(temp, newNum);
    }
    BigInteger operator%(const BigInteger& newNum) {
        BigInteger temp = *this;
        return temp - divide(temp, newNum) * newNum;
    }
    //unary minus
    BigInteger operator-() const {
        BigInteger newNum;
        newNum.number = number;
        newNum.sign = 1 - sign;
        return newNum;
    }
    //prefix operator ++
    BigInteger operator++() {
        BigInteger newNum;
        newNum.number.push_back(1);
        return (*this + newNum);
    }
    //postfix operator ++
    BigInteger operator++(int) {
        BigInteger result(*this);
        ++(*this);
        return result;
    }
    //prefix operator --
    BigInteger operator--() {
        BigInteger newNum;
        newNum.number.push_back(1);
        return (*this - newNum);
    }
    //postfix operator --
    BigInteger operator--(int) {
        BigInteger result(*this);
        --(*this);
        return result;
    }
    //operator = with int
    void operator=(int num) {
        sign = 1;
        if (num < 0) {
            sign = 0;
        }
        number.clear();
        number.push_back(::abs(num));
    }
    //operator = with big integer
    void operator=(BigInteger num) {
        number = num.number;
        sign = num.sign;
    }
    //opreator <
    bool operator<(const BigInteger &newNum) const {
        if (sign != newNum.sign) {
            return sign < newNum.sign;
        }
        if (static_cast<int>(number.size()) != static_cast<int>(newNum.number.size())) {
            return number.size() < newNum.number.size();
        }
        for (int i = 0; i < static_cast<int>(number.size()); ++i) {
            if (number[i] != newNum.number[i]) {
                return number[i] < newNum.number[i];
            }
        }
        return false;
    }
    //operator >
    bool operator>(const BigInteger &newNum) const {
        return newNum < *this;
    }
    //operator <=
    bool operator<=(const BigInteger &newNum) const {
        return !(newNum > *this);
    }
    //operator >=
    bool operator>=(const BigInteger &newNum) const {
        return !(*this < newNum);
    }
    //operator ==
    bool operator==(const BigInteger &newNum) const {
        return !(*this < newNum) and !(newNum < *this);
    }
    //operator !=
    bool operator!=(const BigInteger &newNum) const {
        return *this < newNum or newNum < *this;
    }
    //in/out streams=========================================================================================
    //function to read a number from a string
    void read(const std::string &temp) {
        sign = 1;
        number.clear();
        int pos = 0;
        if (temp[pos] == '-') {
            sign = 0;
            ++pos;
        }
        for (int i = static_cast<int>(temp.size()) - 1; i >= pos; i -= BASE) {
            int tempNum = 0;
            for (int j = std::max(pos, i - BASE + 1); j <= i; j++) {
                tempNum = tempNum * 10 + temp[j] - '0';
            }
            number.push_back(tempNum);
        }
    }
    //input stream
    friend std::istream& operator>>(std::istream &stream, BigInteger &number) {
        std::string temp;
        stream >> temp;
        number.read(temp);
        return stream;
    }
    //output stream
    friend std::ostream& operator<<(std::ostream &stream, BigInteger &num) {
        stream << num.toString();
        return stream;
    }
    //function toString to convert big int into a string=====================================================
    std::string toString() {
        std::string numStr;
        if (number.size() == 1 && number[0] == 0) {
            numStr = '0';
            return numStr;
        }
        if (static_cast<int>(this->number.size()) == 1) {
            numStr = std::to_string(number[0]);
            if (!sign) {
                numStr = '-' + numStr;
            }
            return numStr;
        }
        for (int i = 0; i < static_cast<int>(this->number.size()) - 1; ++i) {
            std::string temp = std::to_string(number[i]);
            while (temp.size() < 9) {
                temp = '0' + temp;
            }
            numStr = temp + numStr;
        }
        numStr = std::to_string(number[static_cast<int>(this->number.size()) - 1]) + numStr;
        if (!sign) {
            numStr = '-' + numStr;
        }
        return numStr;
    }
private:
    //each element is a decimal digit of a big integer
    std::vector<int> number;
    //sign means positive big integer or 0; !sign means negative big integer
    bool sign;
};

int main() {
    BigInteger bigint1, bigint2;
    std::cin >> bigint1 >> bigint2;
    BigInteger c = bigint1 - bigint2;
    std::cout << c;
    
    return 0;
}
