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
        }else {
            int zero = 0;
            for(int i = 0; i < str.size(); i++) {
                if(str[i] == '0') zero ++;
            }
            if(zero == str.size()) value = "0";
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

        if (value.size() < other.value.size()) {
            isSmaller = true;
        } else if (value.size() == other.value.size()) {
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
        int i = a.value.size() - 1;
        int j = b.value.size() - 1;

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
        while (result.size() > 1 && result.back() == '0') {
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

    BigInteger operator/(const BigInteger& other) const {
        // cout << "--------begin-------\n";
        if(value.size() < other.value.size()) return BigInteger("0");

        //相除后的运算符是一个同或的结果
        bool isNeg = this->isNegative != other.isNegative;
        BigInteger A = BigInteger(false, *this), B = BigInteger(false, other);
        int len = B.value.size();
        //这里使用的算法为：取等长前缀做减法，直到A 小于 B
        std::string result;
        while(A >= B) {
            int TempLen = len;
            BigInteger tmpA = BigInteger(A.value.substr(0, TempLen));
            if(tmpA < B){
                while(TempLen < A.value.size()) TempLen++;
                
                tmpA = BigInteger(A.value.substr(0, TempLen));
                if(tmpA < B) break;
            }
            //cout << A << " " << tmpA << std::endl;
            int cnt = 0;
            while(tmpA >= B){
                tmpA = tmpA - B;
                // cout << "tmpA is " << tmpA << std::endl;
                cnt++;
            }
            result += std::to_string(cnt);
            // cout << "check: " <<  A.value.substr(TempLen) << endl;
            // if(tmpA == "0") tmpA = "1";
            A.value = tmpA.value + A.value.substr(TempLen);
            // cout << "new A is " << A << std::endl;
            //if(A < B) result += "0";
        }
        if(result == "") result += "0";
        else {
            BigInteger TmpA(result);
            
            //todo: Test 1002 / -555 -10 == -1
            while((TmpA * B).value.size() < this->value.size()) {
                result += "0";
                TmpA = result;
                //cout << TmpA << "----" << endl;
                if(TmpA > *this){
                    //cout << "TempA is:" << TmpA << "  A is : " << A << endl;
                    result.pop_back();
                    TmpA = result;
                    break;
                }
                // cout << (TmpA) << "---###---------" << *this << std::endl;
            }
        }
        // cout << isNeg << " " << result << std::endl;
        BigInteger tmp = BigInteger(isNeg, result);
        if(tmp == "-0") tmp.isNegative = false;
        //cout << std::boolalpha << tmp.isNegative << "-----\n";
        
        // cout << tmp << std::endl;
        // cout << std::boolalpha << (tmp == "-10") << std::endl;
        //cout << "--------end-------\n";
        return tmp;  
    }

    friend std::ostream& operator<<(std::ostream& os, const BigInteger& obj) {
        if (obj.isNegative) {
            os << '-';
        }
        os << obj.value;
        return os;
    }

};

const int TestCOUNT = 10;

void randomTest(const std::string& operation, int min= -10000, int max = 10000) {
    std::cout << "***********************Running " << operation << " Tests***********************" << std::endl;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(min, max);

    for (int i = 0; i < TestCOUNT; ++i) {
        int num1 = dist(gen);
        int num2 = dist(gen);

        BigInteger a(std::to_string(num1));
        BigInteger b(std::to_string(num2));
        BigInteger expected;
        BigInteger result;

        if (operation == "+") {
            expected = BigInteger(std::to_string(num1 + num2));
            result = a + b;
        } else if (operation == "-") {
            expected = BigInteger(std::to_string(num1 - num2));
            result = a - b;
        } else if (operation == "*") {
            expected = BigInteger(std::to_string(num1 * num2));
            result = a * b;
        } else if (operation == "/") {
            expected = BigInteger(std::to_string(num1 / num2));
            result = a / b;
        }

        std::cout << "Test " << num1 << " " << operation << " " << num2
                  << " " << result << " == " << expected << std::endl;
        assert(result == expected);

        std::cout << "Test " << (i + 1) << " passed" << std::endl;
    }
}

void runAdditionTests() {
    randomTest("+");

    BigInteger num1, num2;
    num1 = "964793941351798875130890128898086485681241334814868066116469822595";
    num2 = "542060529704217132357214772959828385120983424339263541090375634996";
    assert(num1 + num2 == "1506854471056016007488104901857914870802224759154131607206845457591");

    num1 = "-11407631912626230252367469223167656290390890475827609315553696289483";
    num2 = "-622358735778";
    assert(num1 + num2 == "-11407631912626230252367469223167656290390890475827609316176055025261");

    num1 = "-1451727640372150342694657990205152075821407831153281802";
    num2 = "-20726915010027835065";
    assert(num1 + num2 == "-1451727640372150342694657990205152096548322841181116867");

    num1 = "378105201894439826001386678007";
    num2 = "46408267178";
    assert(num1 + num2 == "378105201894439826047794945185");

    num1 = "-401352663034326201321730";
    num2 = "906092740496950206492169757841878889190256238852327468200435470298517938447068207122565641389436";
    assert(num1 + num2 == "906092740496950206492169757841878889190256238852327468200435470298517938045715544088239440067706");

    num1 = "6280938082901588107969672670114844942864465723767937359114430528151000202";
    num2 = "-169737372801639219359121260";
    assert(num1 + num2 == "6280938082901588107969672670114844942864465723598199986312791308791878942");

    num1 = "4891993881875355817054354217645127142317488740323";
    num2 = "78172579890945836844989520331474240546484714755034487986357748";
    assert(num1 + num2 == "78172579890950728838871395687291294900702359882176805475098071");

    num1 = "39332";
    num2 = "730286864164573765164462259628494101475536081003775088852448656483617445622702226058959588304602717";
    assert(num1 + num2 == "730286864164573765164462259628494101475536081003775088852448656483617445622702226058959588304642049");

    num1 = "9281779";
    num2 = "7861247922119448629890175529175201413181683174369126395290950494";
    assert(num1 + num2 == "7861247922119448629890175529175201413181683174369126395300232273");

    num1 = "-4081508731122637188726229073162319284277332977975461002284920767";
    num2 = "04";
    assert(num1 + num2 == "-4081508731122637188726229073162319284277332977975461002284920763");

    num1 = "-976748496616784459049811566313746991893353615270030752";
    num2 = "-399679388240147850845582962266568406378550013568919477066893025039289680561857555668529767384";
    assert(num1 + num2 == "-399679388240147850845582962266568406379526762065536261525942836605603427553750909283799798136");

    std::cout << "***********************Test Addition Successful!***********************" << std::endl;
}

void runSubtractionTests() {
    randomTest("-");

    BigInteger num1, num2;

    num1 = "-874445435842639838877693744091";
    num2 = "5138076149581085788513466024062932782543840542790049273152971609173775463977827312928469158";
    assert(num1 - num2 == "-5138076149581085788513466024062932782543840542790049273152972483619211306617666190622213249");

    num1 = "95107282417738793026415";
    num2 = "-551943879271254850848339988828957171877079443312791787898379298668051621195056762280837302274165652";
    assert(num1 - num2 == "551943879271254850848339988828957171877079443312791787898379298668051621195151869563255041067192067");

    num1 = "-18053639289474007213946034769270700006";
    num2 = "90301";
    assert(num1 - num2 == "-18053639289474007213946034769270790307");

    num1 = "-914888188594569201372815243932724814565686868629006852684208690321806429023522259063956061046848109";
    num2 = "-3383521451328587219733622448444374516871359709823475933160";
    assert(num1 - num2 == "-914888188594569201372815243932724814565683485107555524096988956699357984649005387704246237570914949");

    num1 = "-42609821895384021940801857867165684333030100418";
    num2 = "-404684079137531034032947833286888441832";
    assert(num1 - num2 == "-42609821490699942803270823834217851046141658586");

    num1 = "-74734087249527717679669988232963";
    num2 = "703200302772919215100142709534800513421235492";
    assert(num1 - num2 == "-703200302772993949187392237252480183409468455");

    num1 = "31960914815173225005825990289225841362813372427221";
    num2 = "-35787783495697426203735035198071182";
    assert(num1 - num2 == "31960914815173260793609485986652045097848570498403");

    num1 = "-9239576966760532705989515966946785580507742051130201336277257576761702";
    num2 = "1730689";
    assert(num1 - num2 == "-9239576966760532705989515966946785580507742051130201336277257578492391");

    num1 = "-70492912131149135406529794980741969445";
    num2 = "-82593732358753368234964";
    assert(num1 - num2 == "-70492912131149052812797436227373734481");

    num1 = "-363899055377972592318728868698204746174122392672797079322650154129027636943";
    num2 = "820316728203887494464493549398804948818046";
    assert(num1 - num2 == "-363899055377972592318728868698205566490850596560291543816199552933976454989");


    std::cout << "***********************Test Subtraction Successful!***********************" << std::endl;
}


void runMultiplicationTests() {
    randomTest("*");

    BigInteger num1, num2;

    num1 = "0";
    num2 = "1234567";
    assert(num1 * num2 == "0");

    num1 = "1234567";
    num2 = "0";
    assert(num1 * num2 == "0");

    num1 = "0";
    num2 = "-1234567";
    assert(num1 * num2 == "0");

    num1 = "-1234567";
    num2 = "0";
    assert(num1 * num2 == "0");

    num1 = "978589317935403580524741174006";
    num2 = "-90509077484724206257019681411625431895685544824051050469665209069711802436024518652";
    assert(num1 * num2 == "-88571216402738854083626273911168938903957813772995872304578859791453281026294738966838506705211410784851524559912");

    num1 = "-73071485952769591912309438439999474011325061530286013582323466";
    num2 = "121317184954072601754213";
    assert(num1 * num2 == "-8864826976201066608635868352543931222793459676245158085581760956590025991919994262258");

    num1 = "60804275239255660091407521140471330257740738985236447470529001738773972617036767931574009404";
    num2 = "698373";
    assert(num1 * num2 == "42464064111664693105016544761434384326089173107336533529335750531312795578477818730677135669499692");

    num1 = "-39145567455071476420441778577455981197905856232846263299378522675706306604909891590083502905";
    num2 = "9914996271547053655901905643312309824769165645878654098655256517";
    assert(num1 * num2 == "-388128155364627374535796170428524388793701479708777343194586491343756747866924313930482748539840657691497658103128699160268069983013655376838862542689681885");

    num1 = "89365465149240356961050629455";
    num2 = "-31325287012035009142033311801148622811245857555859573188474284711326963870071447";
    assert(num1 * num2 == "-2799398844763966203840525804194721925146808772135370562605512739246192876821780437648117091361338975172671385");

    num1 = "5269690501652";
    num2 = "68860622";
    assert(num1 * num2 == "362874165691248747544");

    num1 = "-9038654635725550761920";
    num2 = "952145506193893843716638573378750544974339628663581697525517372372718328177003144066006834693247";
    assert(num1 * num2 == "-8606114393444689696685044625449835745337305127328008910465897200565497572693742709752158658141122585433457850328754240");

    num1 = "283";
    num2 = "-780255";
    assert(num1 * num2 == "-220812165");

    num1 = "8830048976534505543863302735441876679967664";
    num2 = "-126719111539546392836988099128";
    assert(num1 * num2 == "-1118935961157133477206060265931263209228052561225694825363414687866596992");

    num1 = "-043404966031021468501077010162774131316915743553182850020639910857056210836268107";
    num2 = "-34210694718998487258508014329846922904959283934028057206414099";
    assert(num1 * num2 == "1484914042175774882132346762639900872820735303932911221194455290535464777327314166521134838110758802767085513534606456668701664400416828840593");
    std::cout << "***********************Test Multiplication Successful!***********************" << std::endl;
}

void runDivisionTests() {
    randomTest("/");

    BigInteger num1, num2;


    num1 = "1000";
    num2 = "10";
    assert(num1 / num2 == "100");

    num1 = "5897";
    num2 = "-551";
    assert(num1 / num2 == "-10");
    
    num1 = "0";
    num2 = "-378789";
    assert(num1 / num2 == "0");

    num1 = "2249";
    num2 = "-36";
    assert(num1 / num2 == "-62");

    num1 = "8870";
    num2 = "577";
    assert(num1 / num2 == "15");

    num1 = "3765815919";
    num2 = "-3062162";
    assert(num1 / num2 == "-1229");

    num1 = "-2693";
    num2 = "22";
    assert(num1 / num2 == "-122");

    num1 = "-1";
    num2 = "-595833026";
    assert(num1 / num2 == "0"); 

    num1 = "10784";
    num2 = "-4335287";
    assert(num1 / num2 == "0");

    num1 = "553069";
    num2 = "-93414309";
    assert(num1 / num2 == "0");

    num1 = "602273537";
    num2 = "-51";
    assert(num1 / num2 == "-11809285");

    std::cout << "***********************Test Division Successful!***********************" << std::endl;
}



int main() {
    runAdditionTests();

    runSubtractionTests();

    runMultiplicationTests();

    runDivisionTests();

    return 0;
}

