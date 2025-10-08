/**
 * @file Orbit.h
 * @brief 轨道工具系列
 * @author StellarDX Astronomy
 * @date 2025
 * @copyright Copyright (c) 2025 StellarDX Astronomy
 * 
 * @details
 * 本文件定义了轨道计算相关的核心类和函数，包括：
 * - 开普勒轨道元素结构体
 * - 卫星跟踪器接口和实现
 * - 开普勒方程求解器
 * - 轨道参数转换工具
 * 
 * @note
 * - 支持椭圆、抛物线和双曲线轨道计算
 * - 提供多种数值算法求解开普勒方程
 * - 包含轨道状态向量与开普勒元素的相互转换
 * 
 * @bug 轨道跟踪器仍在测试阶段，可能部分数据的计算仍有问题。
 * 
 * @todo 
 * - 实现等轴轨道跟踪器(EquinoctialSatelliteTracker)
 * - 优化高离心率轨道的计算精度
 */

namespace cse {
namespace Orbit {

/// @defgroup Orbit 轨道工具
/// @brief SpaceEngine 轨道工具系列
/// @ingroup Physics
/// @{

/**
 * @defgroup OrbitData 数据结构
 * @brief 三种用于存储物体空间状态的结构体
 * @{
 */

/**
 * @struct KeplerianOrbitElems
 * @brief 开普勒轨道根数
 */
struct KeplerianOrbitElems
{
    ustring   RefPlane        = _NoDataStr;     ///< 参考平面
    float64   Epoch           = _NoDataDbl;     ///< 历元
    float64   GravParam       = _NoDataDbl;     ///< 引力参数(G*M)
    float64   PericenterDist  = _NoDataDbl;     ///< 近心点距离
    float64   Period          = _NoDataDbl;     ///< 轨道周期
    float64   Eccentricity    = _NoDataDbl;     ///< 离心率
    Angle     Inclination     = _NoDataDbl;     ///< 轨道倾角
    Angle     AscendingNode   = _NoDataDbl;     ///< 升交点赤经
    Angle     ArgOfPericenter = _NoDataDbl;     ///< 近心点幅角
    Angle     MeanAnomaly     = _NoDataDbl;     ///< 平近点角

    /**
     * @brief 赋值操作符重载
     * @param[in] P 轨道参数对象
     * @return 开普勒轨道要素引用
     */
    KeplerianOrbitElems& operator=(const Object::OrbitParams& P) noexcept;
    
    /**
     * @brief 类型转换操作符
     * @return 轨道参数对象
     */
    operator Object::OrbitParams();
};

/**
 * @struct EquinoctialOrbitElems
 * @brief 春分点轨道根数
 */
struct EquinoctialOrbitElems
{
    ustring   RefPlane        = _NoDataStr;     ///< 参考平面
    float64   Epoch           = _NoDataDbl;     ///< 历元
    float64   GravParam       = _NoDataDbl;     ///< 引力参数(G*M)
    float64   PericenterDist  = _NoDataDbl;     ///< 近心点距离
    float64   Period          = _NoDataDbl;     ///< 轨道周期
    float64   EccentricityF   = _NoDataDbl;     ///< 离心率向量f分量
    float64   EccentricityG   = _NoDataDbl;     ///< 离心率向量g分量
    float64   InclinationH    = _NoDataDbl;     ///< 倾角向量h分量
    float64   InclinationK    = _NoDataDbl;     ///< 倾角向量k分量
    Angle     MeanLongitude   = _NoDataDbl;     ///< 平黄经
};

/**
 * @struct OrbitStateVectors
 * @brief 轨道状态向量
 */
struct OrbitStateVectors
{
    ustring   RefPlane        = _NoDataStr;       ///< 参考系
    float64   GravParam       = _NoDataDbl;       ///< 引力参数(G*M)
    float64   Time            = _NoDataDbl;       ///< 儒略日时间
    vec3      Position        = vec3(_NoDataDbl); ///< 位置矢量(米)
    vec3      Velocity        = vec3(_NoDataDbl); ///< 速度矢量(米/秒，SE中为米/天)
};

///@}

/**
 * @defgroup OrbitElements 轨道根数
 * @brief 轨道计算相关的基础元素和接口
 * @{
 */

/**
 * @brief 卫星轨道跟踪器接口
 * @details 定义卫星轨道跟踪的基本操作接口，是一切轨道工具的基础类
 * @interface SatelliteTracker
 */
struct SatelliteTracker
{
public:
    /**
     * @brief 增加毫秒数
     * @param[in] Ms 要增加的毫秒数
     */
    virtual void AddMsecs(int64 Ms) = 0;
    
    /**
     * @brief 增加秒数
     * @param[in] Sec 要增加的秒数
     */
    virtual void AddSeconds(int64 Sec) = 0;
    
    /**
     * @brief 增加小时数
     * @param[in] Hrs 要增加的小时数
     */
    virtual void AddHours(int64 Hrs) = 0;
    
    /**
     * @brief 增加天数
     * @param[in] Days 要增加的天数
     */
    virtual void AddDays(int64 Days) = 0;
    
    /**
     * @brief 增加年数
     * @param[in] Years 要增加的年数
     */
    virtual void AddYears(int64 Years) = 0;
    
    /**
     * @brief 增加世纪数
     * @param[in] Centuries 要增加的世纪数
     */
    virtual void AddCenturies(int64 Centuries) = 0;

    /**
     * @brief 设置到当前日期
     */
    virtual void ToCurrentDate() = 0;
    
    /**
     * @brief 设置特定日期时间
     * @param[in] DateTime 要设置的日期时间
     */
    virtual void SetDate(CSEDateTime DateTime) = 0;
    
    /**
     * @brief 设置儒略日
     * @param[in] JD 儒略日数值
     */
    virtual void SetDate(float64 JD) = 0;
    
    /**
     * @brief 移动
     * @param[in] MeanAnomalyOffset 平近点角偏移量
     */
    virtual void Move(Angle MeanAnomalyOffset) = 0;
    
    /**
     * @brief 重置轨道跟踪器状态
     */
    virtual void Reset() = 0;

    /**
     * @brief 获取开普勒轨道根数
     * @return 开普勒轨道根数
     */
    virtual KeplerianOrbitElems KeplerianElems()const = 0;
    
    /**
     * @brief 获取春分点轨道根数
     * @return 春分点轨道根数
     */
    virtual EquinoctialOrbitElems EquinoctialElems()const = 0;
    
    /**
     * @brief 获取轨道状态向量
     * @param[in] AxisMapper 坐标轴映射矩阵
     * @return 轨道状态向量
     */
    virtual OrbitStateVectors StateVectors(mat3 AxisMapper)const = 0;
};

/**
 * @brief 基于开普勒轨道根数的卫星跟踪器
 * @details 天体轨道跟踪器，根据轨道六根数计算物体的实时位置和速度
 * 使用的参数如下：
 *  1. 近地点距离(P)：物体距离中心最近时的距离。（至于为何不用半长轴，是因为当轨道为抛物线时半长轴恒为inf）
 *  2. 轨道离心率(e)：物体轨道的离心率，表示轨道的椭圆程度。
 *  3. 轨道倾角(i)：物体轨道与赤道面的夹角。
 *  4. 升交点赤经(Ω)：物体升交点在赤道面上的投影(位置)。
 *  5. 近地点幅角(ω)：物体近地点(最近点)的投影(位置)。
 *  6. 平近点角(M)：物体在升交点处的当地平交角。
 *
 * @par 编码过程简述
 *  1. 先计算当前状态下物体的平均角速度，用于后续计算当前时刻下物体的平近点角。具体可以解万有引力方程得到：
 *  \f[ G \cdot \frac{M \cdot m}{R^2} = m \cdot R \cdot \omega^2 \f] [a]
 * 
 *    其中G为万有引力常数，M为中心天体质量，m为物体质量，R为两个天体之间的距离，ω为物体的角速度。
 *    化简以后可以得到如下两个式：
 *    - 角速度与周期的转换：\f[ \omega = \frac{360}{T} \f] (T为周期) [b]
 *    - 联立[a]和[b]得：\f[ \omega = \sqrt{\frac{G \cdot M}{R^3}} \f]
 *
 *    其中椭圆和双曲线轨道中R = a，抛物线轨道中R = p
 *    @note 这个数据只是在没有任何干扰的情况下得到的，实际上一个物体的轨道可能会受到其他物体的影响而发生变化
 *
 *  2. 轨道六根数中唯一一个随时间变化的量是平近点角(M)，但是真正确定物体位置需要用到真近点角，所以此处需要进行一些转换，公式如下：[1][3][4]
 *
 *    - 平近点角计算偏近点角(E)：
 *        - 椭圆轨道：\f[ M = E - e \cdot \sin(E) \f] [c1]
 *        - 抛物线轨道：\f[ M = \frac{1}{2} \cdot E + \frac{1}{6} \cdot E^3 \f] [c2]
 *        - 双曲线轨道：\f[ M = e \cdot \sinh(E) - E \f] [c3]
 *    - 偏近点角计算真近点角(φ)：
 *        - 椭圆轨道：\f[ \varphi = \arctan\left(\frac{\sqrt{1 - e^2} \cdot \sin(E)}{\cos(E) - e}\right) \f] [d1]
 *        - 抛物线轨道：\f[ \varphi = 2 \cdot \arctan(E) \f] [d2]
 *        - 双曲线轨道：\f[ \varphi = 2 \cdot \arctan\left(\sqrt{\frac{e + 1}{e - 1}} \cdot \tanh\left(\frac{E}{2}\right)\right) \f] [d3]
 *    
 *    @note 式[c1]和式[c3]无法直接得到精确解，此处使用数值算法以获取近似值。
 *
 *  3.按照@蓝羽提出的模型[2]，轨道坐标系经过三次方向余弦矩阵变换即可变为中心天体惯性系。代入剩下的根数即可得到位置与速度。
 *
 * @par 参考文献
 * [1] 范伟,王贵文,刘发发. GPS卫星轨道位置计算方法的研究 [J]. 山西师范大学学报(自然科学版), 2015, 29 (04): 63-68. DOI:10.16207/j.cnki.1009-4490.2015.04.015.<br>
 * [2] 轨道六根数 - 卫星百科(灰机Wiki) https://sat.huijiwiki.com/wiki/%E8%BD%A8%E9%81%93%E5%85%AD%E6%A0%B9%E6%95%B0<br>
 * [3] 开普勒方程 - 卫星百科(灰机Wiki)https://sat.huijiwiki.com/wiki/%E5%BC%80%E6%99%AE%E5%8B%92%E6%96%B9%E7%A8%8B<br>
 * [4] 偏近点角 - 卫星百科(灰机Wiki)https://sat.huijiwiki.com/wiki/%E5%81%8F%E8%BF%91%E7%82%B9%E8%A7%92<br>
 *
 * @class KeplerianSatelliteTracker
 */
class KeplerianSatelliteTracker : public SatelliteTracker
{
public:
    using Mybase    = SatelliteTracker;   ///< 基类类型别名
    using BaseType  = KeplerianOrbitElems; ///< 基础数据类型别名

protected:
    BaseType InitialState;   ///< 初始轨道状态
    BaseType CurrentState;   ///< 当前轨道状态
    Angle    AngularVelocity; ///< 角速度

    /**
     * @brief 检查轨道参数有效性
     * @param[in] InitElems 初始轨道要素
     * @return 验证后的轨道要素
     */
    BaseType CheckParams(const BaseType& InitElems);

public:
    /**
     * @brief 构造函数
     * @param[in] InitElems 初始开普勒轨道要素
     */
    KeplerianSatelliteTracker(const BaseType& InitElems);
    
    /**
     * @brief 构造函数
     * @param[in] InitState 初始轨道状态向量
     */
    KeplerianSatelliteTracker(const OrbitStateVectors& InitState);

    void AddMsecs(int64 Ms)override;
    void AddSeconds(int64 Sec)override;
    void AddHours(int64 Hrs)override;
    void AddDays(int64 Days)override;
    void AddYears(int64 Years)override;
    void AddCenturies(int64 Centuries)override;

    void ToCurrentDate()override;
    void SetDate(CSEDateTime DateTime)override;
    void SetDate(float64 JD)override;
    void Move(Angle MeanAnomalyOffset)override;
    void Reset()override;

    KeplerianOrbitElems KeplerianElems()const override;
    EquinoctialOrbitElems EquinoctialElems()const override;
    
    /**
     * @brief 获取轨道状态向量
     * @param[in] AxisMapper 坐标轴映射矩阵，默认为标准映射
     * @return 轨道状态向量
     */
    OrbitStateVectors StateVectors(mat3 AxisMapper
        = {1, 0, 0, 0, 0, -1, 0, 1, 0})const override;

    /**
     * @brief 将状态向量转换为开普勒轨道要素
     * @param[in] State 轨道状态向量
     * @param[in] AxisMapper 坐标轴映射矩阵，默认为标准映射
     * @return 开普勒轨道要素
     */
    static KeplerianOrbitElems StateVectorstoKeplerianElements
        (OrbitStateVectors State, mat3 AxisMapper
        = {1, 0, 0, 0, 0, 1, 0, -1, 0});
};

/**
 * @brief 基于春分点轨道根数实现的卫星轨道跟踪器
 * @class EquinoctialSatelliteTracker
 * @todo 待实现
 */
class EquinoctialSatelliteTracker : public SatelliteTracker
{
    // TODO...
};

///@}

/**
 * @defgroup KeplerianEquations 开普勒方程（内部使用）
 * @brief 开普勒方程求解工具集
 * @{
 */

namespace KE {

/**
 * @page KEHistory 一些碎碎念
 * @ingroup KeplerianEquations
 * @details
 * <b>丹霞</b>：开普勒方程是航天动力学基础方程，也是开普勒定律的数学描述，其定义为M = E - e * sin(E)。它看似简单但实则是一个超越方程，这意味着无法使用初等或者解析的方式精确求解这个方程的逆，所以实际计算的时候，数值算法还是逃不了的_(:з」∠)_。当然这个问题也是一个困扰了学界200年之久的问题，直到像牛顿迭代这样的数值算法的出现，这个问题才得以解决。牛顿迭代法的实现非常简单，仅需要函数的导函数和一个初值就可以快速收敛。不过问题同样出现在这个初值的选择上，因为它是影响牛顿迭代速度的一个关键因子。学界对于这个初始值的算法可以说五花八门，例如2006年美国海军天文台在文献[1]中就给出了一种三阶初值估计算法，但是由此算法得到的初值接牛顿迭代仍然可能出现收敛慢的情况，故可以认为它是一种"不太稳定"的算法。2021年Richard J. Mathar在文献[2]中也提出了一种更好的初值确定方法，而且实验的结果也算是比较好看的了。本研究最初尝试先使用幂级数展开到前几项以确定一个初始值然后再接牛顿迭代，但效果依然不理想，直到看到了文献[3]。文献[3]中提到了三种算法，分别是增强型牛顿迭代，增强型Markley算法和分段五次多项式拟合，并且在近抛物线轨道的近日点附近会自动切换到二分法以保证求解精度，并且除了增强型牛顿迭代以外，另外两种算法都没有循环结构，所以速度也非常快。经后续实验，此方法在64位浮点下精度可达1-2个ULP，可以认为是开普勒方程反函数的"正解"。此处的开普勒方程求解算法使用文献[3]中的实现。<br>
 * 对于双曲线轨道的开普勒方程，其定义为M = e * sinh(E) - E，Virginia Raposo-Pulido和Jesus Pelaez给出了一种四倍精度的HKE–SDG算法[4]，此算法使用多项式拟合接牛顿迭代实现，经实验仅需3次迭代就能在4倍精度下到达大约1-2个ulp的精度。另外2024年3月初，广东工业大学的吴柏生老师等人在SCI发表了一种新的快速求解算法，这里称它为"吴柏生算法"[5]。吴柏生算法的原理大致是分段帕德逼近接一次施罗德迭代，仅需要评估不超过三个超越函数，所以它的速度可能是很快的。不过由于吴柏生算法并未开源，故目前无法验证其准确性。所以本文仍然使用HKE–SDG算法计算双曲开普勒方程。<br>
 * 抛物线这种情况反而是最简单的，定义为M = (1 / 2) * E + (1 / 6) * E^3，直接就是一个多项式[6]。此方程可使用三次方程求根公式（如范盛金算法）求解，结果必然为1实2虚，其中唯一实根就是要得到的解。
 * 
 * @par 参考文献：
 * [1] Murison M A .A Practical Method for Solving the Kepler Equation[J].  2006. DOI:10.13140/2.1.5019.6808.<br>
 * [2] Mathar R J .Improved First Estimates to the Solution of Kepler's Equation[J].  2021. DOI:10.48550/arXiv.2108.03215.<br>
 * [3] Tommasini D , Olivieri D N .Two fast and accurate routines for solving the elliptic Kepler equation for all values of the eccentricity and mean anomaly[J].天文学与天体物理, 2022, 658:A196.DOI:10.1051/0004-6361/202141423.<br>
 * [4] Raposo-Pulido V ,J. Peláez.An efficient code to solve the Kepler equation. Hyperbolic case[J].Astronomy and Astrophysics, 2018, 619.DOI:10.1051/0004-6361/201833563.<br>
 * [5] Wu B , Zhou Y , Lim C W ,et al.A new method for solving the hyperbolic Kepler equation[J].Applied Mathematical Modelling, 2024, 127(000):7. DOI:10.1016/j.apm.2023.12.017.<br>
 * [6] 平近点角 - 卫星百科(灰机Wiki) https://sat.huijiwiki.com/wiki/%E5%B9%B3%E8%BF%91%E7%82%B9%E8%A7%92<br>
 */

/**
 * @brief 椭圆开普勒方程
 * @ingroup KeplerianEquations
 * @param Eccentricity 离心率
 * @param EccentricAnomaly 偏近点角
 * @return 平近点角
 */
Angle __Elliptical_Keplerian_Equation(float64 Eccentricity, Angle EccentricAnomaly);

/**
 * @brief 抛物线开普勒方程
 * @ingroup KeplerianEquations
 * @param EccentricAnomaly 偏近点角
 * @return 平近点角
 */
Angle __Parabolic_Keplerian_Equation(Angle EccentricAnomaly);

/**
 * @brief 双曲开普勒方程
 * @ingroup KeplerianEquations
 * @param Eccentricity 离心率
 * @param EccentricAnomaly 偏近点角
 * @return 平近点角
 */
Angle __Hyperbolic_Keplerian_Equation(float64 Eccentricity, Angle EccentricAnomaly);

/**
 * @brief 椭圆开普勒方程的反函数
 * @ingroup KeplerianEquations
 */
class __Elliptical_Inverse_Keplerian_Equation
{
protected:
    float64 Eccentricity; /**< 离心率 */
public:
    /**
     * @brief 构造函数
     * @param e 离心率
     */
    __Elliptical_Inverse_Keplerian_Equation(float64 e);
    
    /**
     * @brief 函数调用运算符
     * @param MeanAnomaly 平近点角
     * @return 偏近点角
     */
    virtual Angle operator()(Angle MeanAnomaly)const = 0;
};

/**
 * @brief 抛物线开普勒方程的反函数
 * @ingroup KeplerianEquations
 */
class __Parabolic_Inverse_Keplerian_Equation
{
public:
    /**
     * @brief 默认构造函数
     */
    __Parabolic_Inverse_Keplerian_Equation() {}
    
    /**
     * @brief 函数调用运算符
     * @param MeanAnomaly 平近点角
     * @return 偏近点角
     */
    virtual Angle operator()(Angle MeanAnomaly)const = 0;
};

/**
 * @brief 双曲开普勒方程的反函数
 * @ingroup KeplerianEquations
 */
class __Hyperbolic_Inverse_Keplerian_Equation
{
protected:
    float64 Eccentricity; /**< 离心率 */
public:
    /**
     * @brief 构造函数
     * @param e 离心率
     */
    __Hyperbolic_Inverse_Keplerian_Equation(float64 e);
    
    /**
     * @brief 函数调用运算符
     * @param MeanAnomaly 平近点角
     * @return 偏近点角
     */
    virtual Angle operator()(Angle MeanAnomaly)const = 0;
};

/**
 * @brief 椭圆开普勒方程求解工具
 * @ingroup KeplerianEquations
 */
class __Enhanced_Inverse_Keplerian_Equation_Solver
    : public __Elliptical_Inverse_Keplerian_Equation
{
public:
    using Mybase = __Elliptical_Inverse_Keplerian_Equation; /**< 基类类型定义 */

protected:
    float64 AbsoluteTolerence = 14.522878745280337562704972096745; /**< 绝对容差，对应3E-15 */
    float64 RelativeTolerence = 15.657577319177793764036061134032; /**< 相对容差，对应2.2E-16 */

    constexpr static const float64 EBoundary = 0.99;  /**< 离心率边界值 */
    constexpr static const float64 MBoundary = 0.0045; /**< 平近点角边界值 */

    /**
     * @brief 运行求解算法
     * @param MRad 平近点角（弧度）
     * @param AbsTol 绝对容差
     * @param RelTol 相对容差
     * @return 偏近点角（弧度）
     */
    virtual float64 Run(float64 MRad, float64 AbsTol, float64 RelTol)const = 0;
    
    /**
     * @brief 边界处理函数
     * @param MRad 平近点角（弧度）
     * @param AbsTol 绝对容差
     * @param RelTol 相对容差
     * @return 偏近点角（弧度）
     */
    virtual float64 BoundaryHandler(float64 MRad, float64 AbsTol, float64 RelTol)const;

public:
    /**
     * @brief 构造函数
     * @param e 离心率
     */
    __Enhanced_Inverse_Keplerian_Equation_Solver(float64 e) : Mybase(e) {}
    
    /**
     * @brief 函数调用运算符
     * @param MeanAnomaly 平近点角
     * @return 偏近点角
     */
    Angle operator()(Angle MeanAnomaly)const final;
};

/**
 * @brief 牛顿迭代法求解椭圆开普勒方程
 * @ingroup KeplerianEquations
 */
class __Newton_Inverse_Keplerian_Equation
    : public __Enhanced_Inverse_Keplerian_Equation_Solver
{
public:
    using Mybase = __Enhanced_Inverse_Keplerian_Equation_Solver; /**< 基类类型定义 */
    
protected:
    /**
     * @brief 运行牛顿迭代算法
     * @param MRad 平近点角（弧度）
     * @param AbsTol 绝对容差
     * @param RelTol 相对容差
     * @return 偏近点角（弧度）
     */
    float64 Run(float64 MRad, float64 AbsTol, float64 RelTol)const override;
    
public:
    /**
     * @brief 构造函数
     * @param e 离心率
     */
    __Newton_Inverse_Keplerian_Equation(float64 e) : Mybase(e) {}
};

/**
 * @brief Markley算法求解椭圆开普勒方程
 * @ingroup KeplerianEquations
 */
class __Markley_Inverse_Keplerian_Equation
    : public __Enhanced_Inverse_Keplerian_Equation_Solver
{
public:
    using Mybase = __Enhanced_Inverse_Keplerian_Equation_Solver; /**< 基类类型定义 */
    
protected:
    /**
     * @brief 运行Markley算法
     * @param MRad 平近点角（弧度）
     * @param AbsTol 绝对容差
     * @param RelTol 相对容差
     * @return 偏近点角（弧度）
     */
    float64 Run(float64 MRad, float64 AbsTol, float64 RelTol)const override;
    
public:
    /**
     * @brief 构造函数
     * @param e 离心率
     */
    __Markley_Inverse_Keplerian_Equation(float64 e) : Mybase(e) {}
};

/**
 * @brief 分段五次多项式拟合求解椭圆开普勒方程
 * @ingroup KeplerianEquations
 */
class __Piecewise_Quintic_Inverse_Keplerian_Equation
    : public __Enhanced_Inverse_Keplerian_Equation_Solver
{
public:
    using Mybase = __Enhanced_Inverse_Keplerian_Equation_Solver; /**< 基类类型定义 */

protected:
    std::vector<int64>             BlockBoundaries; /**< 块边界索引 */
    std::vector<Angle>             Breakpoints;     /**< 断点角度 */
    SciCxx::DynamicMatrix<float64> Coefficients;    /**< 多项式系数矩阵 */

    /**
     * @brief 获取第一组系数
     * @param Eccentricity 离心率
     * @param Tolerence 容差
     * @param Grid 网格点输出参数
     */
    static void GetCoefficients1(float64 Eccentricity, float64 Tolerence,
        std::vector<Angle>* Grid/*, uint64* n*/);
        
    /**
     * @brief 获取第二组系数
     * @param Eccentricity 离心率
     * @param Grid 网格点
     * @param kvec 索引向量输出参数
     * @param bp 断点输出参数
     * @param coeffs 系数矩阵输出参数
     */
    static void GetCoefficients2(float64 Eccentricity,
        const std::vector<Angle>& Grid, //uint64 n,
        std::vector<int64>* kvec, std::vector<Angle>* bp,
        SciCxx::DynamicMatrix<float64>* coeffs);

    /**
     * @brief 查找区间索引
     * @param MRad 平近点角（弧度）
     * @return 区间索引
     */
    uint64 FindInterval(float64 MRad)const;
    
    /**
     * @brief 边界处理函数
     * @param MRad 平近点角（弧度）
     * @param AbsTol 绝对容差
     * @param RelTol 相对容差
     * @return 偏近点角（弧度）
     */
    float64 BoundaryHandler(float64 MRad, float64 AbsTol, float64 RelTol)const override;
    
    /**
     * @brief 运行分段五次多项式算法
     * @param MRad 平近点角（弧度）
     * @param AbsTol 绝对容差
     * @param RelTol 相对容差
     * @return 偏近点角（弧度）
     */
    float64 Run(float64 MRad, float64 AbsTol, float64 RelTol)const override;

public:
    /**
     * @brief 构造函数
     * @param e 离心率
     */
    __Piecewise_Quintic_Inverse_Keplerian_Equation(float64 e);

    /**
     * @brief 获取系数
     * @param Eccentricity 离心率
     * @param Tolerence 容差
     * @param kvec 索引向量输出参数
     * @param bp 断点输出参数
     * @param coeffs 系数矩阵输出参数
     */
    static void GetCoefficients(float64 Eccentricity, float64 Tolerence,
        /*uint64* n,*/ std::vector<int64>* kvec, std::vector<Angle>* bp,
        SciCxx::DynamicMatrix<float64>* coeffs);
};

/**
 * @brief 抛物线开普勒方程求解工具
 * @ingroup KeplerianEquations
 */
class __Polynomial_Parabolic_Inverse_Keplerian_Equation
    : public __Parabolic_Inverse_Keplerian_Equation
{
public:
    /**
     * @brief 函数调用运算符
     * @param MeanAnomaly 平近点角
     * @return 偏近点角
     */
    Angle operator()(Angle MeanAnomaly)const;
};

/**
 * @brief 双曲开普勒方程求解工具（HKE–SDG算法）
 * @ingroup KeplerianEquations
 */
class __SDGH_Equacion_Inversa_de_Keplerh
    : public __Hyperbolic_Inverse_Keplerian_Equation
{
public:
    using Mybase     = __Hyperbolic_Inverse_Keplerian_Equation; /**< 基类类型定义 */
    using STableType = float64; /**< 分段表类型 */
    using PTableType = std::function<float64(float64, float64)>; /**< 多项式表类型 */

    constexpr static const uint64 SegmentTableSize  = 51; /**< 分段表大小 */
    constexpr static const uint64 SegmentTableBound = 26; /**< 分段表边界 */
    constexpr static const uint64 PolynomTableSize  = 50; /**< 多项式表大小 */
    constexpr static const uint64 PolynomTableBound = 26; /**< 多项式表边界 */

    static const STableType SegmentCoeffsTable[SegmentTableSize]; /**< 分段系数表 */
    static const PTableType TablaPolinomios[PolynomTableSize];    /**< 多项式表 */

protected:
    float64 AbsoluteTolerence = 15.65; /**< 绝对容差 */
    float64 RelativeTolerence = 15.65; /**< 相对容差 */
    float64 MaxIterations     = 1.69897; /**< 最大迭代次数 */

    float64 SegmentTable[SegmentTableSize]; /**< 分段表 */

    /**
     * @brief 奇异角初始化估计器
     * @param MRad 平近点角（弧度）
     * @return 初始估计值
     */
    float64 SingularCornerInitEstimator(float64 MRad)const;
    
    /**
     * @brief 第二种奇异角初始化估计器
     * @param MRad 平近点角（弧度）
     * @return 初始估计值
     */
    float64 SingularCornerInitEstimatorDOS(float64 MRad)const;
    
    /**
     * @brief 第三种奇异角初始化估计器
     * @param MRad 平近点角（弧度）
     * @return 初始估计值
     */
    float64 SingularCornerInitEstimatorTRES(float64 MRad)const;

    /**
     * @brief 牛顿法初始值估计
     * @param MeanAnomaly 平近点角
     * @return 初始估计值
     */
    float64 NewtonInitValue(Angle MeanAnomaly)const;
    
    /**
     * @brief 运行求解算法
     * @param MeanAnomaly 平近点角
     * @param NumberOfIters 迭代次数输出参数
     * @param Residual 残差输出参数
     * @return 偏近点角
     */
    Angle Run(Angle MeanAnomaly, uint64* NumberOfIters, float64* Residual)const;

public:
    /**
     * @brief 构造函数
     * @param e 离心率
     */
    __SDGH_Equacion_Inversa_de_Keplerh(float64 e);

    /**
     * @brief 函数调用运算符
     * @param MeanAnomaly 平近点角
     * @return 偏近点角
     */
    Angle operator()(Angle MeanAnomaly)const override;
    
    /**
     * @brief 函数调用运算符（带输出参数）
     * @param MeanAnomaly 平近点角
     * @param NumberOfIters 迭代次数输出参数
     * @param Residual 残差输出参数
     * @return 偏近点角
     */
    Angle operator()(Angle MeanAnomaly, uint64* NumberOfIters, float64* Residual)const;

    /**
     * @brief 获取分段表
     * @param Eccentricity 离心率
     * @param SegTable 分段表输出参数
     */
    static void GetSegments(float64 Eccentricity, float64* SegTable);
    
    /**
     * @brief 向量化双曲开普勒方程求解
     * @param Eccentricity 离心率
     * @param MRad 平近点角（弧度）
     * @param Init 初始值
     * @return 包含4个结果的向量
     */
    static vec4 VectorizedHKE(float64 Eccentricity, float64 MRad, float64 Init);
};

/**
 * @brief 默认椭圆开普勒方程求解器
 * @ingroup KeplerianEquations
 */
using DefaultEllipticalIKE = KE::__Newton_Inverse_Keplerian_Equation; 
/**
 * @brief 默认抛物线开普勒方程求解器
 * @ingroup KeplerianEquations
 */
using DefaultParabolicIKE  = KE::__Polynomial_Parabolic_Inverse_Keplerian_Equation;
/**
 * @brief 默认双曲开普勒方程求解器
 * @ingroup KeplerianEquations
 */
using DefaultHyperbolicIKE = KE::__SDGH_Equacion_Inversa_de_Keplerh; 

}

///@}

/**
 * @defgroup TLE 两行根数
 * @brief    两行根数(Two-Line Element Set)数据解析类
 * @{
 */

/**
 * @struct SpacecraftBasicData
 * @brief 航天器基础数据结构体
 * @details 包含卫星编号、分类、国际标识符等基本信息
 */
struct SpacecraftBasicData
{
    uint32_t     CatalogNumber;  ///< 卫星目录编号
    char         Classification; ///< 密级分类 (U: 未分类, C: 已分类, S: 秘密)

    /**
     * @struct COSPAR_ID
     * @brief 国际卫星标识符结构体
     */
    struct COSPAR_ID
    {
        int32_t  LaunchYear;     ///< 发射年份的后两位数字
        uint32_t LaunchNumber;   ///< 当年发射序号
        char     LaunchPiece[3]; ///< 发射部件标识
    } IntDesignator;             ///< 国际卫星标识符

    float64      D1MeanMotion;   ///< 平运动一阶导数；弹道系数 (转/天²) (当前存储为度/秒)
    float64      D2MeanMotion;   ///< 平运动二阶导数 (转/天³) (当前存储为度/秒³)
    float64      BSTAR;          ///< B*阻力项或辐射压力系数 (1/地球半径) (当前存储为1/米)
    uint32_t     EphemerisType;  ///< 星历类型 (通常为零；仅用于未分发的TLE数据)
    uint32_t     ElementSet;     ///< 根数集编号，生成新TLE时递增
    uint32_t     RevolutionNum;  ///< 历元时刻的圈数 (圈数)
};

/**
 * @class TLE
 * @brief 两行根数集解析类
 * @details 提供NORAD和NASA标准双线轨道元素集的解析功能
 * 
 * 此类用于解析和处理标准的双线轨道元素集格式，与NORAD和NASA使用的格式兼容。
 * 包含卫星名称、轨道参数等信息的提取和验证功能。
 */
class TLE // Two-line element set
{
public:
    /// @name 数据长度常量
    /// @{
    static const auto TitleLength      = 24; ///< 卫星名称长度，与NORAD SATCAT一致
    static const auto DataLength       = 69; ///< 第1行和第2行数据标准长度
    /// @}

    /// @name 第1行数据字段位置
    /// @{
    static const auto L1LineNumber     = 0;  ///< 行号位置
    static const auto L1CatalogNumber  = 2;  ///< 目录编号位置
    static const auto L1Classification = 7;  ///< 密级分类位置
    static const auto L1COSPARIDYD     = 9;  ///< COSPAR标识年份位置
    static const auto L1COSPARIDP      = 14; ///< COSPAR标识序号位置
    static const auto L1EpochI         = 18; ///< 历元整数部分位置
    static const auto L1EpochF         = 24; ///< 历元小数部分位置
    static const auto L1D1MeanMotion   = 33; ///< 平运动一阶导数位置
    static const auto L1D2MeanMotionM  = 44; ///< 平运动二阶导数尾数位置
    static const auto L1D2MeanMotionE  = 50; ///< 平运动二阶导数指数位置
    static const auto L1BSTARM         = 53; ///< B*阻力项尾数位置
    static const auto L1BSTARE         = 59; ///< B*阻力项指数位置
    static const auto L1EphemerisType  = 62; ///< 星历类型位置
    static const auto L1ElementSet     = 64; ///< 根数集编号位置
    static const auto L1Checksum       = 68; ///< 校验和位置
    /// @}

    /// @name 第2行数据字段位置  
    /// @{
    static const auto L2LineNumber     = 0;  ///< 行号位置
    static const auto L2CatalogNumber  = 2;  ///< 目录编号位置
    static const auto L2Inclination    = 8;  ///< 轨道倾角位置
    static const auto L2AscendingNode  = 17; ///< 升交点赤经位置
    static const auto L2Eccentricity   = 26; ///< 偏心率位置
    static const auto L2ArgOfPericen   = 34; ///< 近地点幅角位置
    static const auto L2MeanAnomaly    = 43; ///< 平近点角位置
    static const auto L2MeanMotionI    = 52; ///< 平运动整数部分位置
    static const auto L2MeanMotionF    = 55; ///< 平运动小数部分位置
    static const auto L2Revolutions    = 63; ///< 圈数位置
    static const auto L2Checksum       = 68; ///< 校验和位置
    /// @}

    /**
     * @enum SatelliteClassification
     * @brief 卫星密级分类枚举
     */
    enum SatelliteClassification : char
    {
        Unclassified = 'U', ///< 未分类
        Classified   = 'C', ///< 已分类  
        Secret       = 'S'  ///< 秘密
    };

protected:
    char Title[TitleLength + 1]; ///< 卫星名称（防御性编程，预留结束符）
    char Line1[DataLength + 1];  ///< 第1行数据（防御性编程，预留结束符）
    char Line2[DataLength + 1];  ///< 第2行数据（防御性编程，预留结束符）

public:
    /**
     * @brief 默认构造函数
     */
    TLE();
    
    /**
     * @brief 参数化构造函数
     * @param Name 卫星名称
     * @param L1 第1行数据
     * @param L2 第2行数据
     */
    TLE(char const* Name, char const* L1, char const* L2);
    
    /**
     * @brief 数据数组构造函数
     * @param Data 包含名称、第1行、第2行的数据数组
     */
    TLE(char const* const* Data) : TLE(Data[0], Data[1], Data[2]) {}

    /**
     * @brief 验证TLE数据有效性
     * @return true 数据有效，false 数据无效
     */
    bool IsValid() const;

    /**
     * @brief 获取TLE数据
     * @param Title 输出卫星名称
     * @param L1 输出第1行数据  
     * @param L2 输出第2行数据
     */
    void Get(void* Title, void* L1, void* L2) const;

    /**
     * @brief 获取卫星名称
     * @return 卫星名称字符串
     */
    ustring SatelliteName() const;
    
    /**
     * @brief 获取航天器基础数据
     * @return SpacecraftBasicData结构体
     */
    SpacecraftBasicData BasicData() const;
    
    /**
     * @brief 获取开普勒轨道根数
     * @return KeplerianOrbitElems轨道根数
     */
    KeplerianOrbitElems OrbitElems() const;

    /**
     * @brief 将TLE转换为字符串
     * @param Delim 分隔符，默认为换行符
     * @return 格式化后的字符串
     */
    std::string ToString(char Delim = '\n') const;
    
    /**
     * @brief 从字符串解析TLE数据
     * @param Data 输入字符串数据
     * @param Delim 分隔符，默认为换行符
     * @return 解析后的TLE对象
     */
    static TLE FromString(char const* Data, char Delim = '\n');
    
    /**
     * @brief 验证数据行校验和
     * @param Line 数据行
     * @param Size 数据行长度
     * @param Checksum 校验和位置
     * @return 校验结果
     */
    static int VerifyLine(const char* Line, int Size, int Checksum);
};

/**
 * @class OEM
 * @brief 轨道星历消息
 *
 * @details 
 * @par 功能描述
 * 实现CCSDS 502.0-B-3标准的轨道星历消息数据结构，用于高精度轨道数据存储。
 *
 * @par 参考文献
 * [1] Orbit Data Messages[S/OL]. CCSDS 502.0-B-3. 2023. https://ccsds.org/wp-content/uploads/gravity_forms/5-448e85c647331d9cbaf66c096458bdd5/2025/01//502x0b3e1.pdf<br>
 * [2] Sease B. oem[C]. Github. https://github.com/bradsease/oem<br>
 * [3] 刘泽康. 中国空间站OEM来啦，快来和我们一起追"星"吧！[EB/OL]. (2023-09-13). https://www.cmse.gov.cn/xwzx/202309/t20230913_54312.html<br>
 *
 * @todo 目前只支持导入和导出，完整功能待实现
 */
class OEM
{
public:
    /// @name 格式化字符串常量
    /// @{
    constexpr static const cstring KeyValueFmtString = "{} = {}";                    ///< 键值对格式化字符串
    constexpr static const cstring SimplifiedISO8601String = 
        "{}-{:02}-{:02}T{:02}:{:02}:{:02}.{:03}";                                   ///< 简化ISO8601时间格式
    constexpr static const cstring EphemerisFmtString = 
        "{} {:.13g} {:.13g} {:.13g} {:.13g} {:.13g} {:.13g}";                       ///< 星历数据格式化字符串
    constexpr static const cstring EphemerisFmtStringWithAccel = 
        "{} {:.13g} {:.13g} {:.13g} {:.13g} {:.13g} {:.13g} {:.13g} {:.13g} {:.13g}"; ///< 带加速度的星历数据格式化字符串
    constexpr static const cstring CovarianceMatFmtString = "{:.8g}";               ///< 协方差矩阵格式化字符串
    /// @}

    /// @name 文件头信息
    /// @{
    std::string  OEMVersion;        ///< OEM文件版本号
    std::string  Classification;    ///< 文件分类级别
    CSEDateTime  CreationDate;      ///< 文件创建时间
    std::string  Originator;        ///< 文件创建者
    std::string  MessageID;         ///< 消息标识符
    /// @}

    /**
     * @brief 数据值类型定义
     */
    struct ValueType
    {
        /**
         * @brief 元数据类型定义
         */
        struct MetadataType
        {
            std::string  ObjectName;           ///< 目标物体名称
            std::string  ObjectID;             ///< 目标物体标识符
            std::string  CenterName;           ///< 中心天体名称
            std::string  RefFrame;             ///< 参考坐标系
            CSEDateTime  RefFrameEpoch;        ///< 参考坐标系历元
            std::string  TimeSystem;           ///< 时间系统
            CSEDateTime  StartTime;            ///< 数据开始时间
            CSEDateTime  UseableStartTime;     ///< 有效开始时间
            CSEDateTime  UseableStopTime;      ///< 有效结束时间
            CSEDateTime  StopTime;             ///< 数据结束时间
            std::string  Interpolation;        ///< 插值方法
            uint64       InterpolaDegrees = 0; ///< 插值阶数
        };
        
        MetadataType MetaData;  ///< 元数据实例

        /**
         * @brief 星历数据类型定义
         */
        struct EphemerisType
        {
            CSEDateTime  Epoch;        ///< 历元时间
            vec3         Position;     ///< 位置矢量 (km)
            vec3         Velocity;     ///< 速度矢量 (km/s)
            vec3         Acceleration; ///< 加速度矢量 (km/s²)
        };
        
        std::vector<EphemerisType> Ephemeris;  ///< 星历数据序列

        /**
         * @brief 协方差矩阵类型定义
         */
        struct CovarianceMatrixType
        {
            CSEDateTime  Epoch;        ///< 历元时间
            std::string  RefFrame;     ///< 参考坐标系
            matrix<6, 6> Data;         ///< 6x6协方差矩阵数据
        };
        
        std::vector<CovarianceMatrixType> CovarianceMatrices;  ///< 协方差矩阵序列
    };

    using ValueSet = std::vector<ValueType>;  ///< 数据集类型定义
    ValueSet Data;  ///< 轨道星历数据集

    /**
     * @brief 插值工具映射表
     * @details 存储Interpolation字段对应的插值工具或函数，通过指针侧载调用
     * @note 值类型目前未实现，暂时使用"能通过编译"的占位符
     */
    static const std::map<std::string, void*> InterpolationTools;

protected:
    /// @name 数据解析保护方法
    /// @{
    
    /**
     * @brief 解析注释行
     * @param Line 输入行字符串
     * @return 如果是注释行返回true，否则返回false
     */
    static bool ParseComment(std::string Line);
    
    /**
     * @brief 移除字符串中的空白字符
     * @param Line 待处理的字符串引用
     */
    static void RemoveWhiteSpace(std::string& Line);
    
    /**
     * @brief 解析键值对
     * @param Line 输入行字符串
     * @return 键值对，first为键，second为值
     */
    static std::pair<std::string, std::string> ParseKeyValue(std::string Line);
    
    /**
     * @brief 解析原始数据行
     * @param Line 输入行字符串
     * @return 分割后的数据字符串向量
     */
    static std::vector<std::string> ParseRawData(std::string Line);
    
    /**
     * @brief 解析星历数据
     * @param Line 输入行字符串
     * @return 解析后的星历数据
     */
    static ValueType::EphemerisType ParseEphemeris(std::string Line);
    
    /**
     * @brief 传输头信息
     * @param Buf 缓冲区数据
     * @param out 输出OEM对象指针
     */
    static void TransferHeader(std::map<std::string, std::string> Buf, OEM* out);
    
    /**
     * @brief 传输元数据
     * @param Buf 缓冲区数据
     * @param out 输出OEM对象指针
     */
    static void TransferMetaData(std::map<std::string, std::string> Buf, OEM* out);
    
    /**
     * @brief 传输星历数据
     * @param Buf 缓冲区星历数据
     * @param out 输出OEM对象指针
     */
    static void TransferEphemeris(std::vector<ValueType::EphemerisType> Buf, OEM* out);
    
    /**
     * @brief 传输协方差矩阵数据
     * @param Buf 缓冲区协方差矩阵数据
     * @param out 输出OEM对象指针
     */
    static void TransferCovarianceMatrices(
        std::vector<ValueType::CovarianceMatrixType> Buf, OEM* out);
    /// @}

    /// @name 数据导出保护方法
    /// @{
    
    /**
     * @brief 导出键值对
     * @param fout 输出流
     * @param Key 键名
     * @param Value 键值
     * @param Optional 是否为可选字段，默认为0
     * @param Fmt 格式化字符串，默认为KeyValueFmtString
     */
    static void ExportKeyValue(std::ostream& fout, std::string Key, std::string Value,
                              bool Optional = 0, cstring Fmt = KeyValueFmtString);
    
    /**
     * @brief 导出星历数据
     * @param fout 输出流
     * @param Eph 星历数据向量
     * @param Fmt 格式化字符串，默认为EphemerisFmtString
     */
    static void ExportEphemeris(std::ostream& fout, 
                               std::vector<ValueType::EphemerisType> Eph,
                               cstring Fmt = EphemerisFmtString);
    
    /**
     * @brief 导出协方差矩阵
     * @param fout 输出流
     * @param Mat 协方差矩阵向量
     * @param KVFmt 键值对格式化字符串，默认为KeyValueFmtString
     * @param MatFmt 矩阵数据格式化字符串，默认为CovarianceMatFmtString
     */
    static void ExportCovarianceMatrix(std::ostream& fout, 
                                      std::vector<ValueType::CovarianceMatrixType> Mat,
                                      cstring KVFmt = KeyValueFmtString, 
                                      cstring MatFmt = CovarianceMatFmtString);
    /// @}

public:
    /// @name 静态构造方法
    /// @{
    
    /**
     * @brief 从输入流导入OEM数据
     * @param fin 输入流
     * @param out 输出OEM对象指针
     */
    static void Import(std::istream& fin, OEM* out);
    
    /**
     * @brief 从字符串构造OEM对象
     * @param Src 源字符串
     * @return 构造的OEM对象
     */
    static OEM FromString(std::string Src);
    
    /**
     * @brief 从文件构造OEM对象
     * @param Path 文件路径
     * @return 构造的OEM对象
     */
    static OEM FromFile(std::filesystem::path Path);
    /// @}

    /// @name 数据导出方法
    /// @{
    
    /**
     * @brief 导出OEM数据到输出流
     * @param fout 输出流
     * @param KVFmt 键值对格式化字符串，默认为KeyValueFmtString
     * @param EphFmt 星历数据格式化字符串，默认为EphemerisFmtString
     * @param CMFmt 协方差矩阵格式化字符串，默认为CovarianceMatFmtString
     */
    void Export(std::ostream& fout, 
                cstring KVFmt = KeyValueFmtString,
                cstring EphFmt = EphemerisFmtString, 
                cstring CMFmt = CovarianceMatFmtString) const;
    
    /**
     * @brief 将OEM对象转换为字符串
     * @return 表示OEM数据的字符串
     */
    std::string ToString() const;
    
    /**
     * @brief 将OEM对象保存到文件
     * @param Path 文件路径
     */
    void ToFile(std::filesystem::path Path) const;
    /// @}

    /// @name 轨道状态计算运算符（待实现）
    /// @{
    
    /**
     * @brief 根据时间计算轨道状态向量
     * @param time 日期时间
     * @return 轨道状态向量
     * @todo 待实现
     */
    OrbitStateVectors operator()(CSEDateTime time);
    
    /**
     * @brief 根据时间偏移计算轨道状态向量
     * @param timeOffset 时间偏移量
     * @return 轨道状态向量
     * @todo 待实现
     */
    OrbitStateVectors operator()(float64 timeOffset);
    /// @}
};
///@}

///@}

}

}