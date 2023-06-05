#pragma once

#include <iostream>
#include <string>
#include <algorithm>
#include <cassert>
#include <random>

using namespace std;

class BigInteger {
private:
    string value;
    bool isNegative;

public:
    BigInteger() : value("0"), isNegative(false) {}

    BigInteger(bool isNeg, string &&val) : value(val), isNegative(isNeg) {}
    BigInteger(string &&val, bool isNeg) : value(val), isNegative(isNeg) {}

    BigInteger(const BigInteger &other) : value(other.value), isNegative(other.isNegative) {}
    BigInteger(BigInteger &other) : value(other.value), isNegative(other.isNegative) {}

    BigInteger(bool isNeg, BigInteger other) : value(other.value), isNegative(isNeg) {}
    BigInteger(BigInteger other, bool isNeg) : value(other.value), isNegative(isNeg) {}

    BigInteger(const std::string str) : value(str), isNegative(false) {
        // TODO:合法值检验
        // eg:空字符串，不合法字符等
        if (value[0] == '-') {
            value = value.substr(1);
            isNegative = true;
        }
    }

    std::string getValue() const{
        return string(isNegative ? "-" : "") + value;
    }

    BigInteger& operator=(const std::string& str) {
        BigInteger result(str);
        *this = result;
        return *this;
    }

    BigInteger& operator=(const char * str) {
        std::string tmpString = std::string(str);
        BigInteger result(tmpString);
        result.swap(*this);
        return *this;
    }

    BigInteger& operator=(const BigInteger &other) {
        this->isNegative = other.isNegative;
        this->value = other.value;
        return *this;
    }

    bool operator==(const BigInteger& other) const {
        return value == other.value && isNegative == other.isNegative;
    }

    bool operator==(const std::string& str) const {
        return (isNegative ? "-" : "") + value == str;
    }

    bool operator==(const char * str) const {
        return (isNegative ? "-" : "") + value == std::string(str);
    }

    bool operator!=(const BigInteger& other) const {
        return !(*this == other);
    }

    bool operator<(const BigInteger& other) const {
        if (isNegative != other.isNegative) {
            return isNegative;
        }

        bool isSmaller = false;

        if (value.length() < other.value.length()) {
            isSmaller = true;
        } else if (value.length() == other.value.length()) {
            isSmaller = value < other.value;// 小心字符串比较
        }

        return isNegative ? !isSmaller : isSmaller;
    }

    bool operator>(const BigInteger& other) const {
        return other < *this;
    }

    bool operator<=(const BigInteger& other) const {
        return *this < other || *this == other;
    }

    bool operator>=(const BigInteger& other) const {
        return *this > other || *this == other;
    }

    void swap(BigInteger& other) {
        std::swap(value, other.value);
        std::swap(isNegative, other.isNegative);
    }

    //正数+正数正常写
    //正数-正数正常写

    //负数+正数或者正数+负数可以改为正数-正数

    //负数-正数可以改为0-(正数+正数)
    
    //负数-负数 == -（正数+正数）
    //正数-负数 == 正数+正数
    BigInteger operator+(const BigInteger& other) const {
        //负数+正数 == 正数-负数
        if (isNegative && !other.isNegative) {
            BigInteger neg = *this;
            neg.isNegative = false;
            return other - neg;
        }
        // 正数+负数 == 正数-正数
        if (!isNegative && other.isNegative) {
            BigInteger neg = other;
            neg.isNegative = false;
            return *this - neg;
        }
        //负数+负数 == -（正数+正数）
        if (isNegative && other.isNegative) {
            BigInteger a = *this, b = other;
            a.isNegative = false;
            b.isNegative = false;
            return -(a + b);
        }

        //到这里就是两个正数了
        std::string result;
        int carry = 0;
        int i = value.size() - 1;
        int j = other.value.size() - 1;

        while (i >= 0 || j >= 0 || carry > 0) {
            int digit1 = (i >= 0) ? (value[i] - '0') : 0;
            int digit2 = (j >= 0) ? (other.value[j] - '0') : 0;
            int sum = digit1 + digit2 + carry;

            carry = sum / 10;
            int digit = sum % 10;
            result.push_back(digit + '0');

            --i;
            --j;
        }

        std::reverse(result.begin(), result.end());

        BigInteger resultNum(result);
        if (isNegative && other.isNegative) {
            resultNum.isNegative = true;
        }

        return resultNum;
    }

    BigInteger operator-() const {
        BigInteger result = *this;
        result.isNegative = !result.isNegative;
        return result;
    }

    BigInteger operator-(const BigInteger& other) const {
        //负数 - 正数 == -(正数+正数)
        if (isNegative && !other.isNegative) {
            BigInteger neg = *this;
            neg.isNegative = false;
            return -(neg + other);
        }
        //正数 - 负数 == 正数+正数
        if (!isNegative && other.isNegative) {
            BigInteger neg = other;
            neg.isNegative = false;
            return *this + neg;
        }
        //负数 - 负数 == 负数+正数
        if (isNegative && other.isNegative) {
            BigInteger neg = other;
            neg.isNegative = false;
            return *this + neg;
        }

        //到这里说明全是正数了，让大数在前
        bool isNeg = false;
        BigInteger a = *this, b = other;
        if(a < b){
            isNeg = true;//大数跑前面去了，所以要取反
            a.swap(b);
        }

        std::string result;
        int borrow = 0;
        int i = a.value.length() - 1;
        int j = b.value.length() - 1;

        while (i >= 0 || j >= 0) {
            int diff = borrow;
            if (i >= 0) {
                diff += a.value[i] - '0';
                i--;
            }
            if (j >= 0) {
                diff -= b.value[j] - '0';
                j--;
            }

            if (diff < 0) {
                diff += 10;
                borrow = -1;
            } else {
                borrow = 0;
            }

            result.push_back(diff + '0');
        }
        
        // 去除结果中的前导零,这里的值还是逆序的
        // 单独的值0 不用去
        while (result.length() > 1 && result.back() == '0') {
            result.pop_back();
        }
        std::reverse(result.begin(), result.end());
        
        return BigInteger(isNeg, result);
    }

    BigInteger operator*(const BigInteger& other) const {
        //相乘后的运算符是一个同或的结果
        bool isNeg = this->isNegative != other.isNegative;

        std::string num1 = this->value;
        std::string num2 = other.value;
        // 获取两个数字的长度
        int len1 = num1.size();
        int len2 = num2.size();

        // 创建一个结果字符串，初始全为0
        std::string result(len1 + len2, '0');

        // 从右往左逐位相乘
        for (int i = len1 - 1; i >= 0; i--) {
            int carry = 0;
            for (int j = len2 - 1; j >= 0; j--) {
                int product = (num1[i] - '0') * (num2[j] - '0') + (result[i + j + 1] - '0') + carry;
                carry = product / 10;
                result[i + j + 1] = '0' + (product % 10);
            }
            result[i] += carry;
        }

        // 移除结果字符串前面多余的0
        result.erase(0, result.find_first_not_of('0'));

        // 如果结果字符串为空，说明乘积为0
        if (result.empty()) {
            return BigInteger("0");
        }

        return BigInteger(isNeg, result);
    }


    friend std::ostream& operator<<(std::ostream& os, const BigInteger& obj) {
        if (obj.isNegative) {
            os << '-';
        }
        os << obj.value;
        return os;
    }

};
