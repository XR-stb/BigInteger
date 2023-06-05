import random

# 随机生成指定长度的数字子字符串
def generate_number(length):
    return ''.join(random.choices('0123456789', k=length))

# 生成随机符号，用于表示正数或负数
def generate_sign():
    return random.choice(['', '-'])

# 构造随机的数字子字符串和符号
def generate_random_number():
    length = random.randint(1, 100)
    number = generate_number(length)
    sign = generate_sign()
    return sign + number

# 大数相加
def add_big_numbers(num1, num2):
    return str(int(num1) + int(num2))

# 大数相减
def sub_big_numbers(num1, num2):
    return str(int(num1) - int(num2))

def genAddCase(n):
    for i in range(n):
        # 生成随机的数字子字符串和符号，并计算它们的和
        num1 = generate_random_number()
        num2 = generate_random_number()
        result = add_big_numbers(num1, num2)

        print(f"num1 = \"{num1}\";")
        print(f"num2 = \"{num2}\";")
        print(f"assert(num1 + num2 == \"{result}\");\n")

def genSubCase(n):
    for i in range(n):
        # 生成随机的数字子字符串和符号，并计算它们的和
        num1 = generate_random_number()
        num2 = generate_random_number()
        result = sub_big_numbers(num1, num2)

        print(f"num1 = \"{num1}\";")
        print(f"num2 = \"{num2}\";")
        print(f"assert(num1 - num2 == \"{result}\");\n")

#genAddCase(10)
genSubCase(10)
