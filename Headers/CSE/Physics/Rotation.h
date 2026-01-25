/**
 * @file Rotation.h
 * @brief 自转工具系列
 * @author StellarDX Astronomy
 * @date 2026
 * @copyright Copyright (c) 2026 StellarDX Astronomy
 * 
 * @details
 * 本文件定义了自转计算相关的核心类和函数
 * 
 * @bug 自转跟踪器仍在施工阶段，可能部分数据的计算仍有问题。
 */

namespace cse {
namespace Rotation {

/// @defgroup Rotation 自转工具
/// @brief CSpaceEngine 自转工具系列
/// @ingroup Physics
/// @{

/**
 * @brief 经典物体自转参数
 * @details 用于描述简单自转模型的参数，适用于大多数天体的基本自转描述
 */
struct SimpleRotationalElems
{
    /** @brief 自转参数的参考历元 (J2000儒略日) */
    float64 RotationEpoch = _NoDataDbl;
    /** @brief 自转轴相对于黄道面的倾角 (单位: 角度) */
    Angle   Obliquity = _NoDataDbl;
    /** @brief 自转轴北端指向的赤经 (单位: 角度) */
    Angle   EqAscendNode = _NoDataDbl;
    /** @brief 参考历元时刻的自转相位偏移 (单位: 角度) */
    Angle   RotationOffset = _NoDataDbl;
    /** @brief 物体的自转周期 (单位: 秒) */
    float64 RotationPeriod = _NoDataDbl;
    /** @brief 自转周期的长期变化率 (单位: 秒) */
    float64 Precession = _NoDataDbl;

    /**
     * @brief 赋值运算符重载，从Object::SimpleRotationModel赋值
     * @param[in] P 源数据对象
     * @return 当前对象的引用
     */
    SimpleRotationalElems& operator=(const Object::SimpleRotationModel& P) noexcept;

    /**
     * @brief 类型转换运算符，转换为Object::SimpleRotationModel
     * @return Object::SimpleRotationModel类型的对象
     */
    operator Object::SimpleRotationModel();
};

/**
 * @brief IAU/IAG 建议案（WGCCRE）的复杂自转模型参数
 * @details 基于物体的北极方向（由赤经和赤纬定义）和本初子午线位置来描述旋转。
 *          支持长期项（线性速率）和周期项（三角函数级数）。
 */
struct WGCCREComplexRotationalElems
{
    /** @brief 自转参数的参考历元 (J2000儒略日) */
    float64 Epoch = _NoDataDbl;
    /** @brief 物体自转轴北极的赤经 (单位: 角度) */
    Angle   PoleRA = _NoDataDbl;
    /** @brief 物体自转轴北极赤经的变化率 (单位: 度/世纪) */
    Angle   PoleRARate = _NoDataDbl;
    /** @brief 物体自转轴北极的赤纬 (单位: 角度) */
    Angle   PoleDec = _NoDataDbl;
    /** @brief 物体自转轴北极赤纬的变化率 (单位: 度/世纪) */
    Angle   PoleDecRate = _NoDataDbl;
    /** @brief 参考历元时刻本初子午线的旋转相位 (单位: 角度) */
    Angle   PrimeMeridian = _NoDataDbl;
    /** @brief 物体的平均自转速率 (单位: 度/天) */
    Angle   RotationRate = _NoDataDbl;
    /** @brief 物体自转速率的变化率 (单位: 度/世纪^2) */
    Angle   RotationAccel = _NoDataDbl;

    /**
     * @brief 周期项时间单位枚举
     * @details 定义周期项计算中时间 `t` 的单位是天还是世纪。
     */
    enum PeriodicTermsMode : uint8_t
    {
        Diurnal = 0, ///< 时间单位为天 (Daily)
        Secular = 1  ///< 时间单位为世纪 (Secular)
    };
    /** @brief 周期项计算的时间单位 */
    PeriodicTermsMode PeriodicTermsUnit;

    /**
     * @brief 自转模型中的周期项参数结构体
     * @details 描述一个具体的周期性扰动项，用于修正北极位置和本初子午线。
     */
    struct PeriodicTermsElems
    {
        /** @brief 赤经 (RA) 扰动项的振幅比例因子 (A for RA: Σ(A * sin(δ * t^2 + ω * t + φ))) */
        float64 PoleRAAmpScale;
        /** @brief 赤纬 (Dec) 扰动项的振幅比例因子 (A for Dec: Σ(A * cos(δ * t^2 + ω * t + φ))) */
        float64 PoleDecAmpScale;
        /** @brief 本初子午线 (PM) 扰动项的振幅比例因子 (A for PM: Σ(A * sin(δ * t^2 + ω * t + φ))) */
        float64 PrimeMerAmpScale;
        /** @brief 扰动项的初始相位 (φ in the trigonometric functions) (单位: 角度) */
        Angle   Phase;
        /** @brief 扰动项的频率 (ω for linear term in argument) (单位: 角度/单位时间) */
        Angle   Frequency;
        /** @brief 扰动项的频率变化率 (δ for quadratic term in argument) (单位: 角度/单位时间^2) */
        Angle   FrequencyRate;
    };

    /** @brief 存储所有周期项参数的向量 */
    std::vector<PeriodicTermsElems> PeriodicTerms;

    /**
     * @brief 赋值运算符重载，从Object::IAURotationModel赋值
     * @param[in] P 源数据对象
     * @return 当前对象的引用
     */
    WGCCREComplexRotationalElems& operator=(const Object::IAURotationModel& P) noexcept;

    /**
     * @brief 类型转换运算符，转换为Object::IAURotationModel
     * @return Object::IAURotationModel类型的对象
     */
    operator Object::IAURotationModel(); // NOLINT(google-explicit-constructor)
};

/**
 * @brief 自转跟踪器接口
 * @details 定义了所有自转跟踪器实现必须遵循的基础接口，包括时间推进、日期设置和状态查询。
 */
struct RotationTracker
{
public:
    /**
     * @brief 推进指定的毫秒数
     * @param[in] Ms 要增加的毫秒数
     */
    virtual void AddMsecs(int64 Ms) = 0;

    /**
     * @brief 推进指定的秒数
     * @param[in] Sec 要增加的秒数
     */
    virtual void AddSeconds(int64 Sec) = 0;

    /**
     * @brief 推进指定的小时数
     * @param[in] Hrs 要增加的小时数
     */
    virtual void AddHours(int64 Hrs) = 0;

    /**
     * @brief 推进指定的天数
     * @param[in] Days 要增加的天数
     */
    virtual void AddDays(int64 Days) = 0;

    /**
     * @brief 推进指定的年数
     * @param[in] Years 要增加的年数
     */
    virtual void AddYears(int64 Years) = 0;

    /**
     * @brief 推进指定的世纪数
     * @param[in] Centuries 要增加的世纪数
     */
    virtual void AddCenturies(int64 Centuries) = 0;

    /**
     * @brief 将内部日期设置为当前系统日期
     */
    virtual void ToCurrentDate() = 0;

    /**
     * @brief 设置内部日期为指定的日期时间
     * @param[in] DateTime 目标日期时间
     */
    virtual void SetDate(CSEDateTime DateTime) = 0;

    /**
     * @brief 设置内部日期为指定的儒略日
     * @param[in] JD 目标儒略日
     */
    virtual void SetDate(float64 JD) = 0;

    /**
     * @brief 将内部日期和状态重置为初始状态
     */
    virtual void Reset() = 0;

    /**
     * @brief 获取物体自转轴北极在当前时刻的赤道坐标
     * @param[out] RA 指向接收赤经的Sexagesimal对象指针
     * @param[out] Dec 指向接收赤纬的Sexagesimal对象指针
     */
    virtual void NorthPolePos(Sexagesimal* RA, Sexagesimal* Dec) const = 0;

    /**
     * @brief 获取物体在当前时刻的自转相位
     * @return 当前自转相位 (Angle类型)
     */
    virtual Angle RotationPhase() const = 0;
};

/**
 * @brief 基于IAU自转模型的自转跟踪器实现类
 * @details 实现了RotationTracker接口，使用WGCCRE推荐的IAU自转模型进行精确的自转状态跟踪。
 * @note「是非成败转头空，青山依旧在，几度夕阳红。」
 */
class IAU_WGCCRERotationTracker : public RotationTracker
{
public:
    using Mybase = RotationTracker; ///< 父类别名
    using BaseType = WGCCREComplexRotationalElems; ///< 使用的参数类型别名

    // 默认黄赤交角以地球的为准，转经典参数会用到
    /// @brief 默认黄道北极的赤经 (以地球为例)
    static const Sexagesimal DefaultNorthEclipticPoleRA;
    /// @brief 默认黄道北极的赤纬 (以地球为例)
    static const Sexagesimal DefaultNorthEclipticPoleDec;

protected:
    /// @brief 当前实例使用的黄道北极赤经
    Sexagesimal NorthEclipticPoleRA;
    /// @brief 当前实例使用的黄道北极赤纬
    Sexagesimal NorthEclipticPoleDec;
    /// @brief 初始状态参数
    BaseType InitialState;
    /// @brief 当前计算出的状态参数
    BaseType CurrentState;

    /**
     * @brief 检查并验证输入的初始化参数
     * @param[in] InitElems 待检查的初始参数
     * @return 验证后可能被调整的参数
     */
    BaseType CheckParams(const BaseType& InitElems);

public:
    /**
     * @brief 构造函数
     * @param[in] InitElems 初始化用的自转模型参数
     */
    explicit IAU_WGCCRERotationTracker(const BaseType& InitElems);

    // --- RotationTracker 接口实现 ---
    void AddMsecs(int64 Ms) override;
    void AddSeconds(int64 Sec) override;
    void AddHours(int64 Hrs) override;
    void AddDays(int64 Days) override;
    void AddYears(int64 Years) override;
    void AddCenturies(int64 Centuries) override;

    void ToCurrentDate() override;
    void SetDate(CSEDateTime DateTime) override;
    void SetDate(float64 JD) override;
    void Reset() override;

    void NorthPolePos(Sexagesimal* RA, Sexagesimal* Dec) const override;
    Angle RotationPhase() const override;
};

/**
 * @brief 计算会合自转周期
 * @details 考虑了轨道运动影响的自转周期，即从外部观察者角度看，物体连续两次面对同一恒星的时间间隔。
 * @param[in] RotationPeriod 物体的恒星自转周期 (单位: 秒)
 * @param[in] OrbitalPeriod 物体的轨道周期 (单位:秒)
 * @return 会合自转周期 (单位: 秒)
 */
float64 SynodicRotationPeriod(float64 RotationPeriod, float64 OrbitalPeriod);

/**
 * @brief 计算恒星自转周期
 * @details 考虑了岁差效应修正后的自转周期。
 * @param[in] RotationPeriod 输入的基准自转周期 (单位: 秒)
 * @param[in] Precession 自转周期的长期变化率 (单位: 秒)
 * @return 修正后的恒星自转周期 (单位: 秒)
 */
float64 StellarRotationPeriod(float64 RotationPeriod, float64 Precession = 0.0);

/**
 * @brief 计算回归年
 * @details 考虑了岁差效应修正后的轨道周期。
 * @param[in] SiderealOrbitalPeriod 输入的基准恒星轨道周期 (单位: 秒)
 * @param[in] Precession 岁差率对周期的影响 (单位: 秒)
 * @return 修正后的回归年 (单位: 秒)
 */
float64 TropicalOrbitalPeriod(float64 SiderealOrbitalPeriod, float64 Precession = 0.0);

/// @}

}

}