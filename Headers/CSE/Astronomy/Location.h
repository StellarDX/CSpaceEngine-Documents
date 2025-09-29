/**
 * @file        Location.h
 * @defgroup    Locations 天文数据结构
 * @ingroup     Astronomy
 * @brief       天文相关数据结构定义
 * @details     定义了天体位置、行星系统、深空天体等相关数据结构
 */

namespace cse {

/**
 * @struct   Sexagesimal
 * @ingroup  Locations
 * @brief    六十进制角度表示结构体
 * @details  用于表示度分秒格式的角度，支持正负号
 */
struct Sexagesimal
{
    bool     Negative;  ///< 符号
    uint16_t Degrees;   ///< 度数
    uint16_t Minutes;   ///< 分数
    float64  Seconds;   ///< 秒数

    /// @brief 默认构造函数
    Sexagesimal() {}
    
    /// @brief 参数化构造函数
    /// @param[in] Neg  符号（0为正，1为负）
    /// @param[in] Deg  度数
    /// @param[in] Min  分数
    /// @param[in] Sec  秒数
    Sexagesimal(bool Neg, uint16_t Deg, uint16_t Min, float64 Sec)
        : Negative(Neg), Degrees(Deg), Minutes(Min), Seconds(Sec) {}
        
    /// @brief 从十进制角度构造
    /// @param[in] Decimal 十进制角度
    Sexagesimal(Angle Decimal);
    
    /// @brief 转换为浮点数
    operator float64();
    
    /// @brief 转换为角度类型
    operator Angle();
}__declspec(packed);

/**
 * @brief 输出运算符重载
 * @ingroup Locations
 */
std::ostream& operator<<(std::ostream& os, const Sexagesimal& Angle);
/// @ingroup  Locations
std::wostream& operator<<(std::wostream& os, const Sexagesimal& Ang);

/**
 * @brief 从秒数创建六十进制角度
 * @ingroup Locations
 * @param[in] TS 总秒数
 * @return 六十进制角度结构体
 */
Sexagesimal __Create_Sexagesimal_From_Seconds(float64 TS);

/**
 * @brief 24小时制转360度制
 * @ingroup Locations
 * @param[in] n 是否为负
 * @param[in] d 小时数
 * @param[in] m 分钟数
 * @param[in] s 秒数
 * @return 转换后的六十进制角度
 */
Sexagesimal __Convert_24_to_360(bool n, float64 d, float64 m, float64 s);

/**
 * @brief 360度制转24小时制
 * @ingroup Locations
 * @param[in] Ang 六十进制角度
 * @return 转换后的二十四小时制角度
 */
Sexagesimal __Convert_360_to_24(Sexagesimal Ang);

/**
 * @class  Location
 * @ingroup Locations
 * @brief  天体位置类
 * @details 使用极坐标表示天体在空间中的位置
 */
class Location : public SEObject
{
public:
    ustring                        Type = _NoDataStr;       ///< 天体类型
    ustringlist                    Name;                    ///< 天体名称列表
    Sexagesimal                    RA;                      ///< 赤经(转换为360度制存储)
    Sexagesimal                    Dec;                     ///< 赤纬
    float64                        Dist;                    ///< 距离(单位：秒差距)
    std::map<char, float64>        AppMagn;                 ///< 各波段视星等
};

/**
 * @struct StellarSystem
 * @ingroup Locations
 * @brief  行星系统结构体
 * @details 表示一个天体及其子系统的层次结构
 */
struct StellarSystem
{
    ObjectPtr_t PObject;                                   ///< 指向天体对象的指针
    std::vector<std::shared_ptr<StellarSystem>> PSubSystem; ///< 子系统指针数组
};

namespace SystemBuilder
{
    /// @brief 索引树类型定义
    /// @ingroup Locations
    using IndexTreeType = std::multimap<uint64, uint64>;

    /**
     * @brief 对天体列表进行索引排序
     * @ingroup Locations
     * @param[in] List 天体对象列表
     * @param[out] Barycenter 质心索引指针
     * @return 排序后的索引树
     */
    IndexTreeType __Sort_Indices(std::vector<Object> List, uint64* Barycenter);

    /**
     * @brief 广度优先构建行星系统
     * @ingroup Locations
     * @param[in] List 天体对象列表
     * @param[in] Indices 索引树
     * @param[in] Barycenter 质心索引
     * @return 构建完成的系统根指针
     */
    std::shared_ptr<StellarSystem> __BFS_BuildSystem(std::vector<Object> List, IndexTreeType Indices, uint64 Barycenter);
}

/**
 * @brief 从一组物体重建行星系统
 * @ingroup Locations
 * @param[in] List 物体数组
 * @return 系统根指针
 */
std::shared_ptr<StellarSystem> MakeSystem(std::vector<Object> List);

/**
 * @struct __Flux_Type  
 * @ingroup Locations
 * @brief 测光系统波长参数结构体
 */
struct __Flux_Type
{
    char    PhotometricLetter;   ///< 测光系统字母标识
    float64 EffectiveWavelength; ///< 有效波长 λeff
    float64 FWHM;                ///< 半高全宽 Δλ
};

/// @brief 测光系统波长参数表
/// @ingroup Locations
extern const __Flux_Type __Photometric_Wavelengths_Table[9];  

/**
 * @class  StarBarycenter
 * @ingroup Locations
 * @brief  恒星质心类
 * @details 表示恒星系统的质心位置和相关属性
 */
class StarBarycenter : public Location
{
public:
    Epoch::CSEDate                 DateUpdated;   ///< 数据更新日期
    ustring                        CenterOf = _NoDataStr;     ///< 所属中心天体
    bool                           ForceAppMagn = 0;          ///< 是否强制计算视星等
    std::shared_ptr<StellarSystem> System;        ///< 行星系统指针
};

/**
 * @struct EulerAnglesType
 * @ingroup Locations
 * @brief  欧拉角结构体
 * @details 用于描述三维空间中的旋转姿态
 */
struct EulerAnglesType
{
    float64 Yaw = _NoDataDbl;   ///< 偏航角
    float64 Pitch = _NoDataDbl; ///< 俯仰角  
    float64 Roll = _NoDataDbl;  ///< 滚转角
};

/**
 * @class  DSO
 * @ingroup Locations
 * @brief  深空天体基类
 * @details 所有深空天体的基类，包含位置、姿态等通用属性
 */
class DSO : public Location
{
public:
    ustring                        Type = _NoDataStr;       ///< 深空天体类型
    EulerAnglesType                EulerAngles;             ///< 欧拉角姿态
    vec4                           Axis = vec4(_NoDataDbl); ///< 旋转轴
    vec4                           Quat = vec4(_NoDataDbl); ///< 四元数姿态
    float64                        AbsMagn = _NoDataDbl;    ///< 绝对星等
    float64                        Radius = _NoDataDbl;     ///< 半径
    float64                        Luminosity = _NoDataDbl; ///< 光度
    float64                        Age = _NoDataDbl;        ///< 年龄
};

/**
 * @class  Galaxy
 * @ingroup Locations
 * @brief  星系类
 * @details 继承自深空天体基类，包含星系特有的属性
 */
class Galaxy : public DSO
{
public:
    float64                        ModelBright = _NoDataDbl; ///< 模型亮度
    bool                           SolFade = 0;              ///< 太阳渐隐标志
};

/**
 * @class  Cluster
 * @ingroup Locations
 * @brief  星团类
 * @details 继承自深空天体基类，包含星团特有的属性
 */
class Cluster : public DSO
{
public:
    ustring                        Galaxy = _NoDataStr;      ///< 所属星系
    ustring                        CenPow = _NoDataStr;      ///< 中心功率
    uint64                         NStars = _NoDataInt;      ///< 恒星数量
    float64                        MaxStarAppMagn = _NoDataDbl; ///< 最亮恒星视星等
    float64                        FeH = _NoDataDbl;         ///< 金属丰度
    vec3                           Color = vec3(_NoDataDbl); ///< 颜色
};

/**
 * @class  Nebula
 * @ingroup Locations
 * @brief  星云类
 * @details 继承自深空天体基类，包含星云特有的属性
 */
class Nebula : public DSO
{
public:
    ustring                        Galaxy = _NoDataStr;      ///< 所属星系
    vec3                           Randomize = vec3(_NoDataDbl); ///< 随机化参数
    bool                           GenerateStars = 0;        ///< 是否生成恒星标志
};

/**
 * @brief 从键值对获取位置信息
 * @ingroup Locations
 * @param[in] KeyValue 键值对数据
 * @return 位置对象
 */
Location GetLocationFromKeyValue(_SC SCSTable::SCKeyValue KeyValue);

/// @brief 模板特化：从表中获取位置对象
/// @ingroup Locations
template<> Location GetObject(_SC SharedTablePointer Table, ustring Name);

/// @brief 模板特化：将位置对象转换为表
/// @ingroup Locations
template<> _SC SCSTable MakeTable(Location Loc, int Fl, std::streamsize Prec);

/**
 * @brief 从键值对获取恒星质心信息
 * @ingroup Locations
 * @param[in] KeyValue 键值对数据
 * @return 恒星质心对象
 */
StarBarycenter GetStarBarycenterFromKeyValue(_SC SCSTable::SCKeyValue KeyValue);

/// @brief 模板特化：从表中获取恒星质心对象
/// @ingroup Locations
template<> StarBarycenter GetObject(_SC SharedTablePointer Table, ustring Name);

/// @brief 模板特化：将恒星质心对象转换为表
/// @ingroup Locations
template<> _SC SCSTable MakeTable(StarBarycenter Bar, int Fl, std::streamsize Prec);

/**
 * @brief 从键值对获取深空天体信息
 * @ingroup Locations
 * @param[in] KeyValue 键值对数据
 * @return 深空天体对象
 */
DSO GetDSOFromKeyValue(_SC SCSTable::SCKeyValue KeyValue);

/// @brief 模板特化：从表中获取深空天体对象
/// @ingroup Locations
template<> DSO GetObject(_SC SharedTablePointer Table, ustring Name);

/// @brief 模板特化：将深空天体对象转换为表
/// @ingroup Locations
template<> _SC SCSTable MakeTable(DSO Obj, int Fl, std::streamsize Prec);

/**
 * @brief 从键值对获取星系信息
 * @ingroup Locations
 * @param[in] KeyValue 键值对数据
 * @return 星系对象
 */
Galaxy GetGalaxyFromKeyValue(_SC SCSTable::SCKeyValue KeyValue);

/// @brief 模板特化：从表中获取星系对象
/// @ingroup Locations
template<> Galaxy GetObject(_SC SharedTablePointer Table, ustring Name);

/// @brief 模板特化：将星系对象转换为表
/// @ingroup Locations
template<> _SC SCSTable MakeTable(Galaxy Obj, int Fl, std::streamsize Prec);

/**
 * @brief 从键值对获取星团信息
 * @ingroup Locations
 * @param[in] KeyValue 键值对数据
 * @return 星团对象
 */
Cluster GetClusterFromKeyValue(_SC SCSTable::SCKeyValue KeyValue);

/// @brief 模板特化：从表中获取星团对象
/// @ingroup Locations
template<> Cluster GetObject(_SC SharedTablePointer Table, ustring Name);

/// @brief 模板特化：将星团对象转换为表
/// @ingroup Locations
template<> _SC SCSTable MakeTable(Cluster Obj, int Fl, std::streamsize Prec);

/**
 * @brief 从键值对获取星云信息
 * @ingroup Locations
 * @param[in] KeyValue 键值对数据
 * @return 星云对象
 */
Nebula GetNebulaFromKeyValue(_SC SCSTable::SCKeyValue KeyValue);

/// @brief 模板特化：从表中获取星云对象
/// @ingroup Locations
template<> Nebula GetObject(_SC SharedTablePointer Table, ustring Name);

/// @brief 模板特化：将星云对象转换为表
/// @ingroup Locations
template<> _SC SCSTable MakeTable(Nebula Obj, int Fl, std::streamsize Prec);

}
