/**
 * @file        DateTime.h
 * @brief       时间日期处理模块提供日期类和相关转换功能，部分算法来自Stellarium(GPLv2)
 * @note        此日期和时间功能类似Qt的同名功能，但更简单
 * 
 * @author      StellarDX Astronomy
 * @copyright   Copyright (C) StellarDX Astronomy.
 */

namespace cse {
namespace Epoch{

/**
 * @defgroup DateTime 日期和时间
 * @ingroup Base
 * @{
 */

extern const _REGEX_NS regex ISO8601TimeStringRegex;  ///< ISO8601时间格式正则
extern const _REGEX_NS regex SEDateTimeStringRegex;   ///< SE日期时间格式正则
extern const _REGEX_NS regex SEDateStringRegex;       ///< SE日期格式正则
extern const _REGEX_NS regex SETimeStringRegex;       ///< SE时间格式正则

/**
 * @class CSEDate
 * @brief 日期处理类
 * @note 提供日期存储、验证和转换功能
 */
class CSEDate
{
    int years = -1;         ///< 年份，允许所有有效值
    int months = -1;        ///< 月份，有效范围：1(一月)~12(十二月)
    int days = -1;          ///< 日期，有效范围取决于月份(1~28/29/30/31)

public:
    /**
     * @brief 默认构造函数
     */
    CSEDate() {}

    /**
     * @brief 构造函数（带参数）
     * @param y 年份
     * @param m 月份
     * @param d 日期
     */
    CSEDate(int y, int m, int d);

    /**
     * @brief 检查日期是否为空值
     * @return true-为空, false-非空
     */
    bool IsNull() const;

    /**
     * @brief 检查日期是否有效
     * @return true-有效, false-无效
     */
    bool IsValid() const;

    /// @brief 获取年份
    int year() const;
    /// @brief 获取月份
    int month() const;
    /// @brief 获取日期
    int day() const;

    /**
     * @brief 计算指定天数后的新日期
     * @param ndays 增加的天数（负数为减少）
     * @return 计算后的新日期对象
     */
    CSEDate AddDays(int ndays) const;

    /**
     * @brief 计算指定月数后的新日期
     * @param nmonths 增加的月数（负数为减少）
     * @return 计算后的新日期对象
     */
    CSEDate AddMonths(int nmonths) const;

    /**
     * @brief 计算指定年数后的新日期
     * @param nyears 增加的年数（负数为减少）
     * @return 计算后的新日期对象
     */
    CSEDate AddYears(int nyears) const;

        /**
     * @brief 提取日期分量
     * @param[out] year 年份输出指针（可空）
     * @param[out] month 月份输出指针（可空）
     * @param[out] day 日期输出指针（可空）
     */
    void GetDate(int* year, int* month, int* day) const;

    /**
     * @brief 设置日期
     * @param year 年份
     * @param month 月份
     * @param day 日期
     * @return true-设置成功, false-设置失败
     */
    bool SetDate(int year, int month, int day);

    /**
     * @brief 日期转字符串
     * @param _Fmt 格式字符串（默认"{}.{:02}.{:02}"）
     * @return 格式化后的日期字符串
     */
    ustring ToString(cstring _Fmt = "{}.{:02}.{:02}")const;

    /**
     * @brief 获取星期几
     * @return 星期数：0-星期日,1-星期一,...,6-星期六
     */
    int DayOfWeek() const;

    /**
     * @brief 转换为儒略日（已废弃）
     * @deprecated 建议使用 GetJDFromDate 替代
     * @note 1582-10-15前使用儒略历
     * @return 儒略日计数
     */
    int64 ToJulianDay() const;

    /**
     * @brief 获取当前系统日期
     * @return 当前日期对象
     */
    static CSEDate CurrentDate();

    /**
     * @brief 从儒略日转换到日期
     * @param JD 儒略日
     * @return 转换后的日期对象
     * @note 此函数来自Stellarium
     */
    static CSEDate FromJulianDay(const double JD);

    /**
     * @brief 检查当前年份是否为闰年
     * @return true-闰年, false-平年
     */
    bool IsLeap();

    /**
     * @brief 静态闰年检查
     * @param year 待检查年份
     * @return true-闰年, false-平年
     */
    static bool IsLeap(int year);

    /**
     * @brief 日期有效性检查
     * @param year 年份
     * @param month 月份
     * @param day 日期
     * @return true-有效, false-无效
     */
    static bool IsValid(int year, int month, int day);
};

/**
 * @brief 重构的gmtime函数（线程更氨醛）
 * @param timer 时间戳
 * @return 协调世界时(UTC)时间结构
 */
tm gmtime_remake(time_t timer);

/**
 * @brief 重构的localtime函数（线程更氨醛）
 * @param timer 时间戳
 * @return 本地时间结构
 */
tm localtime_remake(time_t timer);

/**
 * @class CSETime
 * @brief 时间类，用于表示和操作时间（时、分、秒、毫秒）
 */
class CSETime
{
    int hours = -1;         /*!< 小时，有效范围：0-23 */
    int minutes = -1;       /*!< 分钟，有效范围：0-59 */
    int seconds = -1;       /*!< 秒，有效范围：0-59 */
    int msecs = -1;         /*!< 毫秒，有效范围：0-999 */

public:
    /// @brief 默认构造函数（初始化为无效时间）
    CSETime() {}

    /**
     * @brief 构造函数（指定时/分/秒/毫秒）
     * @param h 小时值（0-23）
     * @param m 分钟值（0-59）
     * @param s 秒值（0-59），默认0
     * @param ms 毫秒值（0-999），默认0
     */
    CSETime(int h, int m, int s = 0, int ms = 0);
    
    /**
     * @brief 构造函数（指定时/分/带小数秒）
     * @param h 小时值（0-23）
     * @param m 分钟值（0-59）
     * @param s 秒值（小数形式，如30.5表示30秒500毫秒）
     */
    CSETime(int h, int m, double s = 0);

    /**
     * @brief 判断是否为Null值（全初始化为-1）
     * @return true-是Null值, false-非Null值
     */
    bool IsNull() const;
    
    /**
     * @brief 判断时间是否有效
     * @return true-时间有效, false-时间无效
     */
    bool IsValid() const;

    /// @brief 获取小时值
    int hour() const;
    /// @brief 获取分钟值
    int minute() const;
    /// @brief 获取秒值
    int second() const;
    /// @brief 获取毫秒值
    int msec() const;

    /**
     * @brief 增加指定毫秒数
     * @param ms 要增加的毫秒数（可正可负）
     * @param[out] AddDays 可选输出参数，返回跨越的天数变化
     * @return 增加毫秒后的新时间对象
     */
    CSETime AddMSecs(int ms, int* AddDays = nullptr) const;
    
    /**
     * @brief 增加指定秒数
     * @param s 要增加的秒数（可正可负）
     * @param[out] AddDays 可选输出参数，返回跨越的天数变化
     * @return 增加秒数后的新时间对象
     */
    CSETime AddSecs(int s, int* AddDays = nullptr) const;
    
    /**
     * @brief 计算到目标时间的毫秒差
     * @param right 目标时间对象
     * @return 当前时间到目标时间的毫秒数（负值表示当前时间在目标时间之前）
     */
    int MSecsTo(CSETime right);

    /**
     * @brief 设置时分秒毫秒
     * @param h 小时值（0-23）
     * @param m 分钟值（0-59）
     * @param s 秒值（0-59）
     * @param ms 毫秒值（0-999），默认0
     * @return true-设置成功, false-参数无效
     */
    bool SetHMS(int h, int m, int s, int ms = 0);
    
    /**
     * @brief 转换为字符串
     * @param _Fmt 格式化字符串，默认格式"{:02}:{:02}:{:02}.{:03}"
     * @return 格式化后的时间字符串
     */
    ustring ToString(cstring _Fmt = "{:02}:{:02}:{:02}.{:03}");

    /**
     * @brief 静态方法：验证时间分量有效性
     * @param h 小时值
     * @param m 分钟值
     * @param s 秒值
     * @param ms 毫秒值，默认0
     * @return true-参数有效, false-参数无效
     */
    static bool IsValid(int h, int m, int s, int ms = 0);
    
    /**
     * @brief 静态方法：获取系统当前时间
     * @return 包含当前系统时间的CSETime对象
     */
    static CSETime CurrentTime();
};

/**
 * @struct CSETimezone
 * @brief 时区信息结构体
 */
struct CSETimezone
{
    long         Bias;          ///< 与UTC的标准时差（分钟）
    std::wstring StandardName;  ///< 标准时间名称
    CSEDateTime  StandardDate;  ///< 标准时间切换日期
    long         StandardBias;  ///< 标准时间的附加时差（分钟）
    std::wstring DaylightName;  ///< 夏令时名称
    CSEDateTime  DaylightDate;  ///< 夏令时切换日期
    long         DaylightBias;  ///< 夏令时的附加时差（分钟）
};

/**@}*/

} // Epoch

/**
 * @class CSEDateTime
 * @ingroup DateTime
 * @brief 日期时间处理类，支持时区转换和天文计算
 * @details 封装日期和时间操作，支持儒略日转换、时区偏移和日历计算
 * @note 日期范围支持负年份（天文历法计算需要）
 */
class CSEDateTime
{
    double OffsetSecs = 0;          ///< 相对于UTC的时区偏移秒数
    Epoch::CSEDate _Date;            ///< 日期部分
    Epoch::CSETime _Time;            ///< 时间部分

public:
    /**
     * @brief 默认构造函数
     * @return 生成空日期时间对象
     */
    CSEDateTime() {}

    /**
     * @brief 构造函数（指定时区）
     * @param Date   日期对象
     * @param Time   时间对象
     * @param TimeZone 时区对象
     */
    CSEDateTime(const Epoch::CSEDate& Date, const Epoch::CSETime& Time, const Epoch::CSETimezone& TimeZone);

    /**
     * @brief 构造函数（指定偏移秒数）
     * @param Date       日期对象
     * @param Time       时间对象
     * @param OffsetSec  时区偏移秒数（默认0）
     */
    CSEDateTime(const Epoch::CSEDate& Date, const Epoch::CSETime& Time, const double& OffsetSec = 0.0);

    /**
     * @brief 构造函数（仅日期）
     * @param Date 日期对象
     */
    CSEDateTime(const Epoch::CSEDate& Date);

    Epoch::CSEDate date() const { return _Date; }  ///< 获取日期部分
    Epoch::CSETime time() const { return _Time; }  ///< 获取时间部分
    long TimeZone();                              ///< 获取时区信息

    CSEDateTime AddDays(int ndays) const;    ///< 增加指定天数
    CSEDateTime AddMSecs(int msecs) const;  ///< 增加指定毫秒数
    CSEDateTime AddMonths(int nmonths) const;///< 增加指定月数
    CSEDateTime AddSecs(int s) const;       ///< 增加指定秒数
    CSEDateTime AddYears(int nyears) const; ///< 增加指定年数

    bool IsNull() const;   ///< 检查是否为空日期时间
    bool IsValid() const;  ///< 检查日期时间是否有效

    float64 OffsetFromUTC() const;            ///< 获取UTC偏移秒数
    void SetDate(const Epoch::CSEDate& date);  ///< 设置日期部分
    void SetTime(const Epoch::CSETime& time);  ///< 设置时间部分
    void SetOffsetFromUTC(int OffsetSeconds); ///< 设置UTC偏移秒数
    void SetTimeZone(const Epoch::CSETimezone& ToZone); ///< 设置时区

    /**
     * @brief 转换为UTC时间
     * @return UTC时间对象
     */
    CSEDateTime ToUTC() const;

    static CSEDateTime CurrentDateTime(Epoch::CSETimezone* ExportTimezone = nullptr); ///< 获取当前时间（指定时区）
    static CSEDateTime CurrentDateTimeUTC(); ///< 获取当前UTC时间

    /**
     * @brief 转换为字符串
     * @param _Fmt 格式字符串（默认ISO8601格式）
     * @return 格式化后的时间字符串
     */
    ustring ToString(cstring _Fmt = "{}-{:02}-{:02}T{:02}:{:02}:{:02}.{:03}{:+03}:{:02}")const;
};

/**
 * @defgroup StellariumFuncs 天文历法
 * @ingroup DateTime
 * @brief 一些Stellarium移植来的高级功能
 *
 * @par 参考文献
 * [1]	Chereau F, Wolf A, Zotti G, 等. Stellarium[CP/OL]. (2025-06). https://github.com/Stellarium/stellarium.
 * 
 * @see 原始代码位置：Stellarium/stellarium/src/core/StelUtils.cpp
 * @copyright Copyright (C) 2002 Fabien Chereau, Licenced under GPL-2.0
 * @{
 */

/**
 * @brief 将儒略日(JD)转换为CSEDateTime对象
 * @param JD 儒略日数值（含小数部分）
 * @return 对应的CSEDateTime对象
 * @subsubsection 一些碎碎念
 * 丹霞：ISO 8601和一些天文历法定义公元前1年为0年，公元前2年为-1年，但是实际历法中不存在所谓0年，公元前1年后直接是公元1年。这种差异可能会给计算机编程带来问题，例如Qt中的QDateTime就没有0年。
 * 而CSEDateTime在设计时沿用了ISO 8601标准，也就是支持0年的表示方式，我认为现在必须考虑一下这个问题了。
 * @see https://en.wikipedia.org/wiki/Year_zero
 */
CSEDateTime JDToDateTime(const double& JD);

/**
 * @brief 从儒略日提取年月日
 * @param jd 儒略日数值
 * @param[out] yy 年（输出参数）
 * @param[out] mm 月（输出参数）
 * @param[out] dd 日（输出参数）
 */
void GetDateFromJulianDay(const double jd, int *yy, int *mm, int *dd);

/**
 * @brief 从儒略日提取时分秒
 * @param julianDay 儒略日数值
 * @param[out] hour 时（输出参数）
 * @param[out] minute 分（输出参数）
 * @param[out] second 秒（输出参数）
 * @param[out] millis 毫秒（输出参数）
 * @param[out] wrapDay 是否跨日标志（可选输出）
 */
void GetTimeFromJulianDay(const double julianDay, int *hour, int *minute, int *second, int *millis, bool *wrapDay = nullptr);

/**
 * @brief 从儒略日完整提取日期时间
 * @param julianDay 儒略日数值
 * @param[out] year 年（输出参数）
 * @param[out] month 月（输出参数）
 * @param[out] day 日（输出参数）
 * @param[out] hour 时（输出参数）
 * @param[out] minute 分（输出参数）
 * @param[out] second 秒（输出参数）
 * @param[out] millis 毫秒（输出参数）
 */
void GetDateTimeFromJulianDay(const double julianDay, int *year, int *month, int *day, int *hour, int *minute, int *second, int *millis);

/**
 * @brief 从儒略日计算小时数（十进制格式）
 * @param julianDay 儒略日数值
 * @return 小时数（含小数部分）
 */
double GetHoursFromJulianDay(const double julianDay);

/**
 * @brief 将儒略日格式化为ISO8601日期字符串
 * @param jd 儒略日数值
 * @param addMS 是否包含毫秒（默认包含）
 * @return ISO8601格式的UTC日期字符串
 */
ustring JulianDayToISO8601String(const double jd, bool addMS = true);

/**
 * @brief 计算日期对应的星期几
 * @param JD 儒略日数值
 * @return 星期几（0=周日, 1=周一, ...）
 */
int GetDayOfWeek(double JD);

/**
 * @brief 计算日期对应的星期几
 * @param year 年
 * @param month 月
 * @param day 日
 * @return 星期几（0=周日, 1=周一, ...）
 */
int GetDayOfWeek(int year, int month, int day);

/**
 * @brief 获取当前系统时间对应的儒略日
 * @return 当前儒略日数值
 */
double GetJDFromSystem();

/**
 * @brief 从贝塞尔历元计算儒略日
 * @param Epoch 贝塞尔历元（年份表示）
 * @return 对应的儒略日数值
 */
double GetJDFromBesEpoch(const double Epoch);

/**
 * @brief 将时间转换为儒略日小数部分
 * @param Time 时间对象
 * @return 儒略日小数部分（注：儒略日从12:00开始）
 */
double TimeToJDFract(const _TIME CSETime& Time);

/**
 * @brief 将儒略日小数部分转换为时间对象
 * @param jd 儒略日小数部分
 * @return 对应的时间对象
 */
_TIME CSETime JDFractToTime(const double jd);

/**
 * @brief 从日历日期计算儒略日
 * @param[out] newjd 计算结果儒略日（输出参数）
 * @param y 年
 * @param m 月（1-12）
 * @param d 日
 * @param h 时
 * @param min 分
 * @param s 秒
 * @return 操作是否成功
 * @note 1582年10月15日前使用儒略历
 */
bool GetJDFromDate(double* newjd, const int y, const int m, const int d, const int h, const int min, const double s);

/**
 * @brief 获取指定年份月份的天数
 * @param month 月（1-12）
 * @param year 年
 * @return 该月天数
 */
int NumOfDaysInMonthInYear(const int month, const int year);

/**
 * @brief 判断是否为闰年
 * @param year 待判断年份
 * @return 是否为闰年
 * @note 自动处理1582年儒略历到格里历的转换
 */
bool IsLeapYear(const int year);

/**
 * @brief 计算日期在年内的天数
 * @param year 年
 * @param month 月
 * @param day 日
 * @return 年内天数（1月1日为1）
 * @see Meeus, Astronomical Algorithms 2nd ed., 1998, ch.7, p.65
 */
int DayInYear(const int year, const int month, const int day);

/**
 * @brief 计算日期对应的年份小数表示
 * @param year 年
 * @param month 月
 * @param day 日（可含小数部分）
 * @return 年份小数表示（如YYYY.ddd）
 * @note 负年份处理：年份值会减小（如-501年对应-500.5）
 */
double YearFraction(const int year, const int month, const double day);

/**
 * @brief 从年内天数和年份反推日期
 * @param day 年内天数
 * @param year 年
 * @return 包含年月日的整型向量
 */
ivec3 DateFromDayYear(const int day, const int year);

/**
 * @brief 从ISO8601字符串解析儒略日
 * @param iso8601Date ISO8601格式日期字符串
 * @param[out] jd 计算结果儒略日（输出参数）
 * @return 操作状态码
 */
int GetJulianDayFromISO8601String(const std::string& iso8601Date, double* jd);

/**
 * @brief 解析ISO8601日期字符串
 * @param iso8601Date ISO8601格式日期字符串
 * @param[out] y 年（输出参数）
 * @param[out] m 月（输出参数）
 * @param[out] d 日（输出参数）
 * @param[out] h 时（输出参数）
 * @param[out] min 分（输出参数）
 * @param[out] s 秒（输出参数）
 * @param[out] offsec 时区偏移秒数（输出参数）
 * @return 操作状态码
 */
int GetDateTimeFromISO8601String(const std::string& iso8601Date, int* y, int* m, int* d, int* h, int* min, double* s, int* offsec);

/**@}*/
}