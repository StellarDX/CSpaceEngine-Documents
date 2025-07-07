/**
 * @file    AdvMath.h
 * @defgroup AdvMath 高等数学
 * @ingroup Base
 * @brief   CSpaceEngine高等数学库 (SciC++) - 高级数学计算库<br>
 *          CSpaceEngine Advanced Math Library (SciC++) - Advanced mathematical computing library
 *
 * @details 本模块提供解决高复杂度数学问题的工具集<br>
 *          This library provides tools for solving highly complex mathematical problems
 *
 * @subsection features 功能列表 (Features)
 * 1. 坐标转换 (非独立命名空间) / Coordinate transformation (non-dedicated namespace)
 * 2. std::vector的四则运算和动态矩阵 / Arithmetic operations for std::vector and dynamic matrices
 * 3. 特殊函数实现 / Special functions
 * 4. 导数计算 / Derivative calculation
 * 5. 积分计算 / Integration
 * 6. 常微分方程求解 / Ordinary differential equations
 * 7. 反函数计算 / Inverse functions
 *
 * @note <i>「从前有棵树，叫高数，上面挂了很多人；」<br>
 *       「旁边有座坟，叫微积分，里面葬了很多人。」</i>
 *
 * @copyright Copyright (c) StellarDX Astronomy, 
 * license under GNU General Public License v3.0 or later
 *           @see https://www.gnu.org/licenses/
 *
 * @subsection third_party 第三方代码声明 (Third-Party Code Attribution)
 * 本模块部分功能基于以下开源库实现：<br>
 * Portions of this module incorporate code from the following open-source libraries:
 *
 * @subsubsection boost <a href=https://www.boost.org/>Boost</a> 
 * 提供高斯积分, 二分搜索
 * @copyright Copyright John Maddock 2017, Nick Thompson 2017,
 * license under Boost Software License, Version 1.0
 *           @see https://www.boost.org/LICENSE_1_0.txt
 *
 * @subsubsection scipy <a href=https://scipy.org/>SciPy</a>
 * 提供一阶导函数, 提供一阶导函数，牛顿-科特斯积分，龙格-库塔微分方程，牛顿迭代
 * @copyright Copyright (c) 2001-2002 Enthought, Inc. 2003-2024, SciPy Developers,
 * license under BSD License
 *           @see https://github.com/scipy/scipy/blob/main/LICENSE.txt
 *
 * @subsubsection pynverse <a href=https://github.com/alvarosg/pynverse>Pynverse</a>
 * 提供反函数
 * @copyright Copyright (c) 2016 Alvaro Sanchez-Gonzalez,
 * license under MIT License
 *           @see https://opensource.org/license/mit/
 *
 * @note <i>「常言道，前人栽树后人乘凉，但是如果前人栽的树太多，后人也就太容易挂在上面了。」</i>
 */

namespace cse {

/**
 * @defgroup CoordCvt 坐标转换
 * @ingroup AdvMath
 * @brief 直角坐标与极坐标的转换
 * @{
 */

/**
 * @brief 将直角坐标(XY)转换为极坐标(r, θ)
 * @param[in] XY 直角坐标输入 (X: 水平分量, Y: 垂直分量)
 * @return 极坐标输出 (r: 距离, θ: 方位角[角度制, 范围-180°~180°])
 * @note
 * - 坐标系定义：  
 *   - 本初子午线方向：X轴正方向  
 *   - 角度方向：X轴逆时针旋转为正（数学标准）
 */
vec2 _cdecl XYToPolar(vec2 XY);

/**
 * @brief 将三维直角坐标(XYZ)转换为极坐标(Lon, Lat, Dist)
 * @param[in] XYZ 直角坐标输入 (X: 前方向分量, Y: 垂直分量, Z: 右方向分量)
 * @return 极坐标输出 (Lon: 经度[角度制, 范围-180°~180°],
 *                    Lat: 纬度[角度制, 范围-90°~90°],
 *                    Dist: 距离)
 * @note
 * - 坐标系定义：  
 *   - 赤道平面：X-Z平面
 *   - 春分点方向：Z轴负方向
 *   - 经度方向：由西向东为正（从北极俯视逆时针）
 */
vec3 _cdecl XYZToPolar(vec3 XYZ);

/**
 * @brief 将极坐标(r, θ)转换为直角坐标(XY)
 * @param[in] Polar 极坐标输入 (r: 距离, θ: 方位角[角度制])
 * @return 直角坐标输出 (X: 水平分量, Y: 垂直分量)
 * @note
 * - 坐标系定义：  
 *   - 本初子午线方向：X轴正方向  
 *   - 角度方向：X轴逆时针旋转为正（数学标准）
 */
vec2 _cdecl PolarToXY(vec2 Polar);

/**
 * @brief 将极坐标(Lon, Lat, Dist)转换为三维直角坐标(XYZ)
 * @param[in] Polar 极坐标输入 (Lon: 经度[角度制], Lat: 纬度[角度制], Dist: 空间距离)
 * @return 直角坐标输出 (X: 前方向分量, Y: 垂直分量, Z: 右方向分量)
 * @note
 * - 坐标系定义：  
 *   - 赤道平面：X-Z平面
 *   - 春分点方向：Z轴负方向
 *   - 经度方向：由西向东为正（从北极俯视逆时针）
 */
vec3 _cdecl PolarToXYZ(vec3 Polar);

/**@}*/


namespace SciCxx{

#include "./LinAlg/DynamicMatrix.inc"

/**
 * @defgroup Iterators 迭代器
 * @ingroup AdvMath
 * @brief 一些后续功能依赖的通用迭代器
 * @{
 */

/**
 * @brief 迭代型求解算法使用的通用迭代器(由SciPy翻译而来，并转写为抽象基类)
 * @note 此工具仅供内部使用
 */
class ElementwiseIterator
{
protected:
    /**
     * @brief 迭代器状态
     * @note 状态转移由具体实现控制
     */
    enum
    {
        InProgress = 1, ///< 进行中
        Finished   = 0, ///< 已完成
        ValueError = -3 ///< 数值错误
    }State;

    uint64     IterCount     = 0; ///< 当前迭代次数
    uint64     EvaluateCount = 0; ///< 函数评估次数

public:
    /**
     * @brief 预评估函数（纯虚函数）
     * @return DynamicMatrix<float64> 返回预评估结果矩阵
     * @note 派生类必须实现此方法，用于每次迭代前的预处理
     */
    virtual DynamicMatrix<float64> PreEvaluator() = 0;

    /**
     * @brief 后评估函数（纯虚函数）
     * @param[in] x   当前迭代点矩阵
     * @param[in] fx  目标函数值矩阵
     * @note 派生类必须实现此方法，用于每次迭代后的处理
     */
    virtual void PostEvaluator(DynamicMatrix<float64> x, DynamicMatrix<float64> fx) = 0;

        /**
     * @brief 终止条件检查（纯虚函数）
     * @retval true   满足终止条件
     * @retval false  未满足终止条件
     * @note 派生类应实现具体终止条件判断逻辑
     */
    virtual bool CheckTerminate() = 0;

    /**
     * @brief 迭代终止处理函数（纯虚函数）
     * @note 用于在迭代结束后执行的处理
     */
    virtual void Finalize() = 0;

    /**
     * @brief 执行主循环
     * @param[in] Function     目标函数对象
     * @param[in] MaxIterLog   最大迭代次数的对数
     */
    void Run(Function1D Function, float64 MaxIterLog);
};

/**@}*/


/**
 * @defgroup SpecFuncs 特殊函数
 * @ingroup AdvMath
 * @brief 一些常用的，或者主要功能依赖的特殊函数求解
 * @{
 */

/**
 * @brief 计算初等对称多项式的值
 * @details 初等对称多项式定义参见: https://en.wikipedia.org/wiki/Elementary_symmetric_polynomial\n
 * 对于给定的参数V = (x₁, x₂, ..., xₙ)，第k个初等对称多项式eₖ定义为：
 * \f[
 * e_k = \sum_{1\leq i_1 < i_2 < \cdots < i_k \leq n} x_{i_1}x_{i_2}\cdots x_{i_k}
 * \f]
 * 其中定义e₀ = 1
 *
 * 此处使用如下递推关系动态规划求解，时间复杂度\f$O(n^2)\f$，<b>可能还有一种基于FFT分治的算法速度更快</b>
 * \f[
 * {\begin{aligned}e_{1}&=\sum _{1\leq j\leq n}X_{j},\\e_{k}&=\sum _{1\leq j\leq n-k+1}X_{j}e_{k-1}\\\end{aligned}}
 * \f]
 * 
 * @par 示例
 * 输入\f$V = \left(2, 3, 4\right)\f$<br>
 * 计算过程：
 * \f[
 * \begin{align*}
 * e_0 &= 1 \\
 * e_1 &= 2 + 3 + 4 = 9 \\
 * e_2 &= 2 \times 3 + 2 \times 4 + 3 \times 4 = 26 \\
 * e_3 &= 2 \times 3 \times 4 = 24
 * \end{align*}
 * \f]
 * 结果为 \f$\left(1, 9, 26, 24\right)\f$
 * 
 * @param[in] V 输入参数向量 (x₁, x₂, ..., xₙ)
 * @return std::vector<float64> 所有基本对称多项式的值，从e₀到eₙ排列
 */
std::vector<float64> ElementarySymmetricPolynomial(std::vector<float64> V);

/**
 * @brief 生成范德蒙德矩阵
 * @details 对于给定参数V = (x₁, x₂, ..., xₙ)，生成的n×n范德蒙德矩阵为：
 * \f[
 * \begin{bmatrix}
 * 1      & 1      & \cdots & 1      \\
 * x_1    & x_2    & \cdots & x_n    \\
 * x_1^2  & x_2^2  & \cdots & x_n^2  \\
 * \vdots & \vdots & \ddots & \vdots \\
 * x_1^{n-1} & x_2^{n-1} & \cdots & x_n^{n-1}
 * \end{bmatrix}
 * \f]
 * 
 * @par 示例
 * 输入\f$V = \left(2, 3, 4, 5\right)\f$<br>
 * 输出矩阵：
 * \f[
 * \begin{bmatrix}
 * 1 &  1 &   1 &    1 \\
 * 2 &  3 &   4 &    5 \\
 * 4 &  9 &  16 &   25 \\
 * 8 & 27 &  64 &  125
 * \end{bmatrix}
 * \f]
 *
 * @param[in] V 范德蒙德矩阵的参数向量 (x₁, x₂, ..., xₙ)
 * @return DynamicMatrix<float64> 生成的范德蒙德矩阵
 */
DynamicMatrix<float64> Vandermonde(std::vector<float64> V);

/**
 * @brief 快速计算范德蒙德矩阵的逆矩阵
 * @details 对于给定的参数V = (x₁, x₂, ..., xₙ)，计算其对应的范德蒙德矩阵的逆矩阵
 * 
 * 范德蒙德矩阵\f[\mathbb{V}=\left[\begin{matrix}1&\cdots&1&\cdots&1\\\vdots&\ddots&\vdots&\ddots&\vdots\\{x_1}^{p-1}&\cdots&{x_q}^{p-1}&\cdots&{x_n}^{p-1}\\\vdots&\ddots&\vdots&\ddots&\vdots\\{x_1}^{n-1}&\cdots&{x_q}^{n-1}&\cdots&{x_n}^{n-1}\\\end{matrix}\right]\f]
 * 的逆矩阵可以通过如下初等变换的方法得到：
 * \f[\mathbb{V}^{- 1} = \begin{bmatrix} {\left( {- 1} \right)^{n - 1}\frac{e_{n - 1}\left( {x_{2},x_{3},\cdots,x_{n}} \right)}{\prod\limits_{i = 2}^{n}\left( {x_{1} - x_{i}} \right)}} & \cdots & {\left( {- 1} \right)^{n - q}\frac{e_{n - q}\left( {x_{2},x_{3},\cdots,x_{n}} \right)}{\prod\limits_{i = 2}^{n}\left( {x_{1} - x_{i}} \right)}} & \cdots & \frac{1}{\prod\limits_{i = 2}^{n}\left( {x_{1} - x_{i}} \right)} \\  \vdots & \ddots & \vdots & \ddots & \vdots \\ {\left( {- 1} \right)^{n - 1}\frac{e_{n - 1}\left( {x_{1},\cdots,x_{p - 1},x_{p + 1},\cdots,x_{n}} \right)}{\prod\limits_{i = 1,i \neq p}^{n}\left( {x_{p} - x_{i}} \right)}} & \cdots & {\left( {- 1} \right)^{n - q}\frac{e_{n - q}\left( {x_{1},\cdots,x_{p - 1},x_{p + 1},\cdots,x_{n}} \right)}{\prod\limits_{i = 1,i \neq p}^{n}\left( {x_{p} - x_{i}} \right)}} & \cdots & \frac{1}{\prod\limits_{i = 1,i \neq p}^{n}\left( {x_{p} - x_{i}} \right)} \\  \vdots & \ddots & \vdots & \ddots & \vdots \\ {\left( {- 1} \right)^{n - 1}\frac{e_{n - 1}\left( {x_{1},x_{2},\cdots,x_{n - 1}} \right)}{\prod\limits_{i = 1}^{n - 1}\left( {x_{n} - x_{i}} \right)}} & \cdots & {\left( {- 1} \right)^{n - q}\frac{e_{n - q}\left( {x_{1},x_{2},\cdots,x_{n - 1}} \right)}{\prod\limits_{i = 1}^{n - 1}\left( {x_{n} - x_{i}} \right)}} & \cdots & \frac{1}{\prod\limits_{i = 1}^{n - 1}\left( {x_{n} - x_{i}} \right)} \end{bmatrix}\f]
 * 其中\f$e_k\left(x_1,x_2,\cdots,x_{n}\right)\f$为关于\f$x_1,x_2,\cdots,x_{n}\f$的k次初等对称多项式。其定义参见 @ref ElementarySymmetricPolynomial
 *
 * @par 示例
 * 输入\f$V = \left(2, 3, 4, 5\right)\f$<br>
 * 对应范德蒙德矩阵：
 * \f[
 * \begin{bmatrix}
 * 1 &  1 &   1 &    1 \\
 * 2 &  3 &   4 &    5 \\
 * 4 &  9 &  16 &   25 \\
 * 8 & 27 &  64 &  125
 * \end{bmatrix}
 * \f]
 * 其逆矩阵为：
 * \f[
 * \begin{bmatrix}
 * 10 & -7.8333 &  2 & -0.1667 \\
 * -20 & 19 & -5.5 & 0.5 \\
 * 15 & -15.5 & 5 & -0.5 \\
 * -4 & 4.3333 & -1.5 & 0.1667
 * \end{bmatrix}
 * \f]
 *
 * @see 算法来源: https://zhuanlan.zhihu.com/p/678666109\n
 *
 * @param[in] V 范德蒙德矩阵的参数向量 (x₁, x₂, ..., xₙ)
 * @return DynamicMatrix<float64> 范德蒙德矩阵的逆矩阵
 */
DynamicMatrix<float64> InverseVandermonde(std::vector<float64> V);

/**
 * @class Polynomial
 * @brief 多项式类
 *
 * 表示多项式函数 \f$ P(x) = \sum_{k=0}^{n} a_k x^k \f$，其中系数按降幂排序
 */
class Polynomial
{
protected:
    std::vector<float64> Coefficients; ///< 多项式系数数组，按 \f$ [x^n \to x^0] \f$ 降幂排列

public:
    /// @brief 默认构造函数（零多项式）
    Polynomial() {}
    
    /**
     * @brief 通过系数向量构造多项式
     * @param[in] Coeffs 多项式系数向量，以降幂顺序排列
     */
    Polynomial(const std::vector<float64>& Coeffs) : Coefficients(Coeffs) {}

    /**
     * @brief 获取多项式最高次数
     * @return \f$ n \f$（对 \f$ P(x) = \sum_{k=0}^{n} a_k x^k \f$）
     */
    uint64 MaxPower()const {return Coefficients.size() - 1;}
    
    /// @brief 获取系数
    std::vector<float64> GetCoefficients()const {return Coefficients;}

    /**
     * @brief 多项式求值运算
     * @param[in] x 自变量值
     * @return \f$ P(x) \f$ 的计算结果
     */
    float64 operator()(float64 x)const;
    
    /**
     * @brief 求导函数
     * @return 导函数多项式 \f$ \frac{d}{dx}P(x) \f$
     * @note 多项式的导函数仍然是多项式
     */
    Polynomial Derivative()const;
};

/**
 * @brief 生成n次第一类勒让德多项式的系数
 * 
 * 勒让德多项式可以用罗德里格斯公式定义：
 * \f[
 * P_n(x) = \frac{1}{2^n n!} \left[ (x^2-1)^n \right]^{\left(n\right)}
 * \f]
 *
 * 根据以上定义可得到勒让德多项式的一般形式为：
 * \f[
 * P_n(x) = \sum_{k=0}^{\lfloor n/2 \rfloor} (-1)^k \frac{(2n-2k)!}{2^n  k!  (n-k)!  (n-2k)!}  x^{n-2k}
 * \f]
 * 前半部分即为第\f$ n-2k \f$项的系数
 * 
 * @param[in] n 勒让德多项式的次数
 * @return n次第一类勒让德多项式的系数，以降幂顺序排列，缺失项补0
 * 
 * @par 示例
 * 输入\f$n = 1, \cdots ,10\f$，结果如下：
 * |n|系数|对应多项式|
 * |-|-|-|
 * |0|\f$(1)\f$|\f$1\f$|
 * |1|\f$(1, 0)\f$|\f$x\f$|
 * |2|\f$(1.5, 0, -0.5)\f$|\f${\tfrac {1}{2}}\left(3x^{2}-1\right)\f$|
 * |3|\f$(2.5, 0, -1.5, 0)\f$|\f${\tfrac {1}{2}}\left(5x^{3}-3x\right)\f$|
 * |4|\f$(4.375, 0, -3.75, 0, 0.375)\f$|\f${\tfrac {1}{8}}\left(35x^{4}-30x^{2}+3\right)\f$|
 * |5|\f$(7.875, 0, -8.75, 0, 1.875, 0)\f$|\f${\tfrac {1}{8}}\left(63x^{5}-70x^{3}+15x\right)\f$|
 * |6|\f$(14.4375, 0, -19.6875, 0, 6.5625, 0, -0.3125)\f$|\f${\tfrac {1}{16}}\left(231x^{6}-315x^{4}+105x^{2}-5\right)\f$|
 * |7|\f$(26.8125, 0, -43.3125, 0, 19.6875, 0, -2.1875, 0)\f$|\f${\tfrac {1}{16}}\left(429x^{7}-693x^{5}+315x^{3}-35x\right)\f$|
 * |8|\f$(50.2734375, 0, -93.84375, 0, 54.140625, 0, -9.84375, 0, 0.2734375)\f$|\f${\tfrac {1}{128}}\left(6435x^{8}-12012x^{6}+6930x^{4}-1260x^{2}+35\right)\f$|
 * |9|\f$(94.9609375, 0, -201.09375, 0, 140.765625, 0, -36.09375, 0, 2.4609375, 0)\f$|\f${\tfrac {1}{128}}\left(12155x^{9}-25740x^{7}+18018x^{5}-4620x^{3}+315x\right)\f$|
 * |10|\f$(180.42578125, 0, -427.32421875, 0, 351.9140625, 0, -117.3046875, 0, 13.53515625, 0, -0.24609375)\f$|\f${\tfrac {1}{256}}\left(46189x^{10}-109395x^{8}+90090x^{6}-30030x^{4}+3465x^{2}-63\right)\f$|
 * 
 * @see https://en.wikipedia.org/wiki/Legendre_polynomials
 */
std::vector<float64> LegendrePolynomialCoefficients(uint64 n);

/**
 * @brief 生成斯蒂尔杰斯多项式系数
 *
 * 斯蒂尔杰斯多项式定义为：
 * \f[
 * \int_{-1}^1 K_{n+1}(x) \cdot P_n(x) \cdot x^k \, dx = 0
 * \f]
 * 其中\f$K_{n+1}(x)\f$为n次斯蒂尔杰斯多项式，\f$P_n(x)\f$为n次勒让德多项式。
 *
 * 根据定义可推导出：
 * \f[
 * K_{n+1}(x) = \sum_{i=1}^r \left( a_i \cdot P_{2i-1-q}(x) \right)
 * \f]
 * 其中系数\f$a_i\f$的递推公式为：
 * \f[
 * a_{r-n} = \sum_{i=0}^{n-1} \left( -a_{r-i} \cdot \frac{S(r-i,n)}{S(r-n,n)} \right)
 * \f]
 * 其中：
 * \f[
 * \frac{S(i,k)}{S(r-k,k)} = \frac{S(i-1,k)}{S(r-k,k)} \cdot \frac{(n-q+2(i+k-1))(n+q+2(k-i+1))(n-1-q+2(i-k))(2(k+i-1)-1-q-n)}{(n-q+2(i-k))(2(k+i-1)-q-n)(n+1+q+2(k-i))(n-1-q+2(i+k))}
 * \f]
 *
 * 根据上式可以得到1-5次斯蒂尔杰斯多项式分别为：
 * \f{align*}{
 * E_1(x) &= P_1(x) \\
 * E_2(x) &= P_2(x) - \frac{2}{5}P_0(x) \\
 * E_3(x) &= P_3(x) - \frac{9}{14}P_1(x) \\
 * E_4(x) &= P_4(x) - \frac{20}{27}P_2(x) + \frac{14}{891}P_0(x) \\
 * E_5(x) &= P_5(x) - \frac{35}{44}P_3(x) + \frac{135}{12584}P_1(x)
 * \f}
 *
 * @par 参考文献
 * Patterson T N L. The optimum addition of points to quadrature formulae[J]. 
 * Mathematics of Computation, 1968, 22(104): 847-856.
 * http://www.ams.org/journals/mcom/1968-22-104/S0025-5718-68-99866-9/S0025-5718-68-99866-9.pdf
 *
 * @param N 多项式次数（需满足 \f$ N \geq 1 \f$）
 * @return 系数向量（降幂排列），缺失项补零
 */
std::vector<float64> StieltjesPolynomialCoefficients(uint64 N);

/**
 * @brief 计算组合数C(n,k)
 * @param[in] n 总数
 * @param[in] k 选取数
 * @return 组合数值
 * @note 内部使用函数
 */
uint64 __Int_Comb(uint64 n, uint64 k);

/**
 * @brief 生成n阶不完全贝尔多项式的值
 *
 * 不完全贝尔多项式 \f$ B_{n,k}(x_1 \to x_{n-k+1}) \f$ 满足：
 * \f[
 * B_{n,k}(x_{1} \to x_{n-k+1})=n!\sum \prod _{i=1}^{n-k+1}{\frac {x_{i}^{j_{i}}}{(i!)^{j_{i}}j_{i}!}}
 * \f]
 * 其中求和满足 \f$ \sum_{i=1}^{n-k+1} j_i = k \f$ 和 \f$ \sum_{i=1}^{n-k+1} i \cdot  j_i = k \f$。
 * 
 * 根据以上定义，可以得到如下递推关系，使得求解可以使用动态规划以提高效率：
 * \f[
 * B_{n+1,k+1}(x_{1} \to x_{n-k+1})=\sum _{i=0}^{n-k} \left( {C_{n}^{i}} \cdot x_{i+1} \cdot B_{n-i,k}(x_{1} \to x_{n-k-i+1}) \right)
 * \f]
 * 输出矩阵为上三角矩阵（按列优先存储），元素 \f$ m[i][j] \f$ 对应 \f$ B_{i,j} \f$，
 * 无效位置用 NaN 填充。
 *
 * @par 示例
 * 输入 \f$ x = {1,2,3,4,5,6} \f$ 时输出矩阵：
 * \f[
 * \begin{bmatrix}
 * 1 & 0 & 0 & 0 & 0 & 0 & 0 \\
 * \text{NaN} & 1 & 2 & 3 & 4 & 5 & 6 \\
 * \text{NaN} & \text{NaN} & 1 & 6 & 24 & 80 & 240 \\
 * \text{NaN} & \text{NaN} & \text{NaN} & 1 & 12 & 90 & 540 \\
 * \text{NaN} & \text{NaN} & \text{NaN} & \text{NaN} & 1 & 20 & 240 \\
 * \text{NaN} & \text{NaN} & \text{NaN} & \text{NaN} & \text{NaN} & 1 & 30 \\
 * \text{NaN} & \text{NaN} & \text{NaN} & \text{NaN} & \text{NaN} & \text{NaN} & 1
 * \end{bmatrix}
 * \f]
 *
 * @param x 贝尔多项式参数序列 \f$ [x_1, x_2, \dots, x_m] \f$（长度决定最大阶数）
 * @return 下三角矩阵（行主序），矩阵大小 \f$ (m+1) \times (m+1) \f$
 */
DynamicMatrix<float64> BellPolynomialsTriangularArray(std::vector<float64> x);

/**@}*/


/**
 * @defgroup Derivatives 导数
 * @ingroup AdvMath
 * @brief 一些导数算法
 * @see https://en.wikipedia.org/wiki/Numerical_differentiation
 * @{
 */

/**
 * @brief 一元函数导函数的基类
 */
class DerivativeFunction
{
protected:
    Function1D OriginalFunction; ///< 原函数
    float64    DerivativeOrder;  ///< 导数阶数(支持非整数阶)

public:
    /**
     * @brief 计算x处的导数值
     * @param[in] x 求导点的x坐标
     * @return float64 导数值
     */
    virtual float64 operator()(float64 x)const = 0;
};

// ------------------------------------------------------------------------------------- //

/**
 * @brief 基于有限差分法的一元函数的数值导数 (此功能翻译自SciPy，并转化为独立的类)
 * @details 据SciPy的文档所说，此方法的实现受到了jacobi、numdifftools和DERIVEST的启发，但其实现更直接地遵循了泰勒级数理论（可以说是天真地遵循了泰勒级数理论）。
 * @see [SciPy官方文档](https://docs.scipy.org/doc/scipy/reference/generated/scipy.differentiate.derivative.html#scipy.differentiate.derivative)
 *
 * @par 示例：
 * 求ln(x)的导函数，已知ln'(x) = 1 / x
 * @code{.cpp}
 * float64 (*f)(float64) = cse::ln;
 * SciCxx::Adaptive1stOrderFDDerivativeFunction df(f);
 * cout << df(1) << ' ';
 * cout << df(2) << ' ';
 * cout << df(5) << '\n';
 * @endcode
 * 输出: 0.999999999998355 0.499999999999178 0.200000000000003
 */
class Adaptive1stOrderFDDerivativeFunction : public DerivativeFunction
{
public:
    using Mybase = DerivativeFunction; ///< 基类

    /**
     * @brief 差分方向
     */
    enum DirectionType
    {
        Center = 0, Forward = 1, Backward = -1
    };

    /**
     * @brief 计算过程用到的迭代器
     * @details 实现自适应步长调整和误差控制的迭代计算过程
     */
    class Iterator : public ElementwiseIterator
    {
    public:
        using Mybase = ElementwiseIterator; ///< 基类
        friend class Adaptive1stOrderFDDerivativeFunction;

        /**
         * @brief 迭代状态
         */
        enum StateType
        {
            ErrorIncrease = 2 ///< 误差增大
        };

    protected:
        float64                Input;             ///< 输入值x
        float64                Output;            ///< 输出导数值
        DynamicMatrix<float64> Intermediates;     ///< 中间计算结果
        float64                Error;             ///< 当前误差估计
        float64                Step;              ///< 当前步长
        float64                LastOutput;        ///< 上一次迭代的输出
        float64                LastError;         ///< 上一次迭代的误差
        float64                StepFactor;        ///< 步长调整因子（每次迭代步长减小系数）
        float64                AbsoluteTolerence; ///< 绝对误差
        float64                RelativeTolerence; ///< 相对误差
        uint64                 Terms;             ///< 项数
        DirectionType          Direction;         ///< 差分方向
        std::vector<float64>   CentralWeights;    ///< 中心差分权重系数
        std::vector<float64>   ForwardWeights;    ///< 前向差分权重系数

    public:
        /**
         * @brief 计算权重系数
         * @param[in] n 权重项数
         * @return std::vector<float64> 权重系数向量
         */
        std::vector<float64> DerivativeWeight(uint64 n);

        /**
         * @brief 获取采样点
         * @return DynamicMatrix<float64> 采样点
         */
        DynamicMatrix<float64> PreEvaluator() override;

        /**
         * @brief 求解主函数
         * @param[in] x 采样点矩阵
         * @param[in] fx 采样点函数值矩阵
         */
        void PostEvaluator(DynamicMatrix<float64> x, DynamicMatrix<float64> fx) override;

        /**
         * @brief 终止检测
         * @return bool 是否满足终止条件
         */
        bool CheckTerminate() override;

        /**
         * @brief 最终处理函数（空实现）
         */
        void Finalize() override {}
    };

protected:
    // 一些附加选项，懒得加getter和setter了
    float64       AbsoluteTolerence = 300;    ///< 绝对误差容限的负对数，默认 -log(0x1p-2022) ≈ 307.65
    float64       RelativeTolerence = 7.5;    ///< 相对误差容限的负对数，默认 -log(sqrt(0x1p-52)) ≈ 7.83
    uint64        FDMOrder          = 8;      ///< 有限差分阶数（必须为偶数）
    float64       InitialStepSize   = 0.5;    ///< 初始步长
    float64       StepFactor        = 2;      ///< 步长调整因子（每次迭代步长减小系数）
    float64       MaxIteration      = 1;      ///< 最大迭代次数的对数（向下取整）
    DirectionType Direction         = Center; ///< 差分方向（默认中心差分）

public:
    /**
     * @brief 构造函数
     * @param[in] Function 原函数
     */
    Adaptive1stOrderFDDerivativeFunction(Function1D Function)
    {
        OriginalFunction = Function;
        DerivativeOrder = 1;
    }

    /**
     * @brief 计算x处的导数值
     * @param[in] x 求导点的x坐标
     * @return float64 导数值
     */
    float64 operator()(float64 x)const override;
};

/**
 * @page fractional_derivative 一些碎碎念
 * <b>丹灵</b>：莱布尼茨于1695年9月30日在致洛必达的信中提过这样一个问题，大致意思是整数阶的导数概念是否能够被推广到非整数阶。洛必达当时收到信也好奇，并在回应中提到了这样一个问题，就是一个函数的半阶导函数会是什么样的。莱布尼茨最后认为，这是个悖论，但总有一天，我们会得出有意义的结论。目前，我们将幂，阶乘等看似离散的定义成功的推广到了全体实数甚至是复数域上，当然导数与积分也不例外。先是柯西推出了迭代积分公式，将这个公式继续向实数域推广，即为黎曼-刘维尔积分，再根据这一定义又可以定义黎曼-刘维尔导数和卡普托导数。使用黎曼-刘维尔积分生成不定积分的方式很简单，只需要一个定积分算法就能完成。但是数值计算黎曼-刘维尔导数就有些复杂了，其中有一个办法就是使用格林沃尔德-莱特尼科夫算法将原来的导数化为有限差分。然而，据薛定宇的文章所说，相比整数阶的导函数仅需要求导点附近的几个点就能算，非整数阶导需要从t0开始的所有函数值才能计算，也就是说尽管能使用有限差分计算但是计算结果仍然为“递推函数”，使得要获取一个确定点的函数值变得难上加难。所以本文中的求高阶导的方法是从定义下手，先用二项式差分求整数阶导，然后套黎曼-刘维尔导数或卡普托导数得到非整数阶的导函数。
 * 
 * @subsubsection 参考文献
 * [1] 薛定宇.分数阶微积分学与分数阶控制[M].科学出版社,2018.<br>
 * [2] Das S. L’Hopital’s question-"What is ½ derivative of f (x) = x i.e.
 *     d1/2 f(x) / dx1/2?"[A]. 2013.<br>
 * [3] Wikipedia Contributors. Riemann–Liouville integral — Wikipedia, The
 *     Free Encyclopedia[A/OL]. (2025).
 *      https://en.wikipedia.org/wiki/Riemann%E2%80%93Liouville_integral<br>
 * [4] Wikipedia Contributors. Caputo fractional derivative — Wikipedia,
 *     The Free Encyclopedia[A/OL]. (2025).
 *     https://en.wikipedia.org/wiki/Caputo_fractional_derivative<br>
 */

/**
 * @class RiemannLiouvilleBinomialFDDerivativeFunction
 * @brief 黎曼-刘维尔/二项式差分导数，基于黎曼-刘维尔导数和有限差分得到。因导数算法的权重生成部分使用了二项式定理而得名。严格来说此方法也是有限差分的一个子集，但是目前还无法将此类与上述类合并。
 * @details 此导函数一次性封装了三种算法，分别是二项式差分，黎曼-刘维尔导数和卡普托导数。其中二项式差分用于求整数阶的导数，剩下两种则将导数扩展到非整数阶。
 *
 * 黎曼-刘维尔导数和卡普托导数的共同点在于，它们都是一个函数的n阶不定积分的ceil(n)阶导，只是黎曼-刘维尔算法是先积分后求导，而卡普托算法则是先求导后积分。尽管它们的顺序不同，但是在某些情况下仍然会得到截然不同的结果。相比之下，卡普托算法的优点是积分初值可以被保持下来，而黎曼-刘维尔算法则需要额外引入这个分数阶初值条件。
 *
 * @warning 注：此方法目前正在优化中，精度较低且延迟极高，慎用。
 */
typedef class RiemannLiouvilleBinomialFDDerivativeFunction : public DerivativeFunction
{
public:
    using Mybase        = DerivativeFunction; ///< 基类
    using DefaultEngine = DefaultIntegratingFunction; ///< 默认积分引擎
    using EnginePtr     = std::shared_ptr<DefiniteIntegratingFunction>; ///< 积分引擎指针

    /**
     * @class Iterator
     * @brief 导数计算的迭代器
     */
    class Iterator : public ElementwiseIterator
    {
    public:
        using Mybase = ElementwiseIterator; ///< 基类
        friend class RiemannLiouvilleBinomialFDDerivativeFunction;

        enum StateType /// 迭代状态
        {
            ErrorIncrease = 2 ///< 误差增长
        };

    protected:
        float64                DerivativeOrder;      ///< 导数阶数
        float64                Input;                ///< 当前输入
        float64                Output;               ///< 当前输出
        float64                Error;                ///< 当前误差估计
        float64                Step;                 ///< 当前步长
        float64                LastOutput;           ///< 上一步输出值
        float64                LastError;            ///< 上一步误差估计
        float64                StepFactor;           ///< 步长调整因子
        float64                AbsoluteTolerence;    ///< 绝对误差容限
        float64                RelativeTolerence;    ///< 相对误差容限
        uint64                 Terms;                ///< 项数
        std::vector<float64>   Weights;              ///< 导数权重向量

    public:
        /**
         * @brief 计算n阶导数的权重系数
         * @param n 导数阶数
         * @return 权重系数
         */
        std::vector<float64> DerivativeWeight(uint64 n);

        /// @brief 获取采样点（前处理）
        DynamicMatrix<float64> PreEvaluator() override;

        /**
         * @brief 求解主函数（后处理）
         * @param x 输入值矩阵
         * @param fx 函数值矩阵
         */
        void PostEvaluator(DynamicMatrix<float64> x, DynamicMatrix<float64> fx) override; // 求解主函数
        /// @brief 终止检测
        bool CheckTerminate() override; 
        /// @brief 最终处理（空实现）
        void Finalize() override {} 
    };

protected:
    float64 AbsoluteTolerence = 300;  ///< 绝对误差负对数（默认 \f$-\log(2^{-2022})\f$ ≈ 307.65)
    float64 RelativeTolerence = 7.5;  ///< 相对误差负对数（默认 \f$-\log(\sqrt{2^{-52}})\f$ ≈ 7.83)
    float64 InitialStepSize   = 3;    ///< 初始步长负对数
    float64 StepFactor        = 10;   ///< 步长减小系数
    float64 MaxIteration      = 3;    ///< 最大迭代次数对数

    EnginePtr Engine;        ///< 定积分计算引擎
    float64 InitValue = 0;   ///< 积分基数（通常为0）

    /**
     * @brief 内联积分
     * @param Func 被积函数
     * @param x 积分上限
     * @return 积分结果
     */
    float64 IntegralInline(Function1D Func, float64 x)const;

    /**
     * @brief 内联导数
     * @param Func 目标函数
     * @param x 求导点
     * @return 导数值
     */
    float64 DerivativeInline(Function1D Func, float64 x)const;

public:
    /**
     * @brief 构造函数
     * @param Function 原函数
     * @param DerivOrder 导数阶数（默认1阶）
     * @param a 积分基数（默认0）
     * @param IFunc 积分引擎（默认使用DefaultEngine）
     */
    RiemannLiouvilleBinomialFDDerivativeFunction(Function1D Function, float64 DerivOrder = 1,
        float64 a = 0, EnginePtr IFunc = std::make_shared<DefaultEngine>(DefaultEngine()))
        : InitValue(a), Engine(IFunc)
    {
        OriginalFunction = Function;
        DerivativeOrder = DerivOrder;
    }

    /**
     * @brief 二项式差分导数
     * @details
     * 二项式差分求导的定义如下，用于求整数阶导数：
     * \f[
     *  f^{(n)}(x)=\lim _{h \to 0} \frac{1}{h^{n}} \sum_{j=1}^{n} \left( (-1)^{j} \cdot C_n^j \cdot f(x-j h) \right)
     * \f]
     * 其中h为步长
     * @param[in] x 求导点
     * @return 导数值
     */
    float64 Binomial(float64 x)const;

    /**
     * @brief 黎曼-刘维尔导数
     * @details 黎曼-刘维尔导数的定义为：
     * \f[
     *   _{C}^{RL}f^{(\alpha)}(x) = \frac{1}{\Gamma(n-\alpha)} \left( \int_{C}^{x} \frac{f(t)}{(x-t)^{\alpha-n+1}} dt \right)^{(n)}
     * \f]
     * 其中\f$\alpha\f$为导函数的阶数，\f$n = \lceil \alpha \rceil\f$，i为积分初值，一般置0。
     * @param[in] x 求导点
     * @return 导数值
     */
    float64 RiemannLiouville(float64 x)const;

    /**
     * @brief 卡普托导数计算
     * @details 卡普托导数的定义为：
     * \f[
     *   _{C}^{Caputo}f^{(\alpha)}(x) = \frac{1}{\Gamma(n-\alpha)} \int_{C}^{x} \frac{f^{(n)}(t)}{(x-t)^{\alpha-n+1}} dt
     * \f]
     * 其中\f$\alpha\f$为导函数的阶数，\f$n = \lceil \alpha \rceil\f$，i为积分初值，一般置0。
     * @param[in] x 求导点
     * @return 导数值
     */
    float64 Caputo(float64 x)const;

    /// @brief 计算导数值
    float64 operator()(float64 x)const override;
}RiemannLiouvilleDerivativeFunction;

/// @brief 默认导数函数类型（使用一阶自适应有限差分）
using DefaultDerivativeFunction = Adaptive1stOrderFDDerivativeFunction;

/**@}*/


/**
 * @defgroup Integrations 积分
 * @ingroup AdvMath
 * @brief 一些积分算法
 * @note 「每日一积，成为积佬」
 * @{
 */

/**
 * @brief 一元函数的定积分
 */
typedef class DefiniteIntegratingFunction
{
protected:
    /**
     * @brief 计算定积分
     * @param f 被积函数，形式为float64(float64)的一元函数
     * @param a 积分下限
     * @param b 积分上限
     * @return float64 积分结果值
     * @note 子类必须实现此方法以提供具体积分算法
     */
    virtual float64 Run(Function1D f, float64 a, float64 b)const = 0;
    
public:
    /**
     * @brief 运算符重载，提供函数式调用接口
     * @param f 被积函数
     * @param a 积分下限
     * @param b 积分上限
     * @return float64 积分结果值
     * @note 实际调用protected的Run方法
     */
    float64 operator()(Function1D f, float64 a, float64 b)const;
}IntegralFunction;

/**
 * @brief 基于采样点的定积分
 * @details 通过函数采样点计算定积分的基类
 */
class SampleBasedIntegratingFunction : public DefiniteIntegratingFunction
{
public:
    using Mybase = DefiniteIntegratingFunction;  ///< 基类
    using Mybase::operator();                    ///< 继承基类运算符

protected:
    /**
     * @brief 定积分计算
     * @param f 被积函数
     * @param a 积分下限
     * @param b 积分上限
     * @return float64 积分结果值
     * @details 通过采样点计算积分值，调用GetSamplesFromFunction获取采样点
     */
    float64 Run(Function1D f, float64 a, float64 b)const final;
    
    /**
     * @brief 基于采样点计算积分
     * @param Samples 采样点集合，每个点为vec2(x,f(x))
     * @return float64 积分结果值
     * @note 子类必须实现此方法以提供具体积分算法
     */
    virtual float64 Run(std::vector<vec2> Samples)const = 0;

public:
    /**
     * @brief 对一元函数进行固定步长采样
     * @param f 被采样函数
     * @param a 采样区间下限
     * @param b 采样区间上限
     * @param Samples 采样点数量
     * @return std::vector<vec2> 采样点集合
     * @note 采样点均匀分布在[a,b]区间
     */
    static std::vector<vec2> GetEvenlySpacedSamplesFromFunction(Function1D f, float64 a, float64 b, uint64 Samples);
    
    /**
     * @brief 对参数曲线进行采样
     * @param x x坐标函数x(t)
     * @param y y坐标函数y(t)
     * @param a 参数下限
     * @param b 参数上限
     * @param Samples 采样点数量
     * @return std::vector<vec2> 采样点集合
     * @note 用于参数曲线 \f$(x(t),y(t))\f$ 的采样
     */
    static std::vector<vec2> GetSamplesFromParametricCurve(Function1D x, Function1D y, float64 a, float64 b, uint64 Samples);

    /**
     * @brief 获取函数采样点
     * @param f 被采样函数
     * @param a 采样区间下限
     * @param b 采样区间上限
     * @param Samples 采样点数量(0表示使用默认值)
     * @return std::vector<vec2> 采样点集合
     * @note 默认实现调用GetEvenlySpacedSamplesFromFunction
     */
    virtual std::vector<vec2> GetSamplesFromFunction(Function1D f, float64 a, float64 b, uint64 Samples = 0)const;
    
    /**
     * @brief 基于采样点计算积分
     * @param Samples 采样点集合
     * @return float64 积分结果值
     */
    float64 operator()(std::vector<vec2> Samples)const;
};

/**
 * @brief 一元函数的不定积分
 */
typedef class IndefiniteIntegratingFunction
{
protected:
    Function1D OriginalFunction;  ///< 被积函数f(x)
    float64    IntegralOrder;     ///< 积分阶数(支持分数阶积分)
    
public:
    /**
     * @brief 计算不定积分值
     * @param x 积分上限变量值
     * @return float64 积分结果值 \f$F(x)\f$
     */
    virtual float64 operator()(float64 x)const = 0;
}AntiderivativeFunction;

// ------------------------------------------------------------------------------------- //

#if defined __GNUG__ && !defined(__clang__)
#define __Tbl_FpType __float128 ///< GCC支持的四倍精度浮点(15位指数+112位尾数)
#else
#define __Tbl_FpType long double ///< 当前编译器支持的最大精度浮点(通常为15位指数+64位尾数)
#endif

/// 高斯-7点系数表
extern const __Tbl_FpType __Gaussian07_Table[8];
/// 克朗罗德-15点系数表
extern const __Tbl_FpType __Kronrod15_Table[16];
/// 高斯-10点系数表
extern const __Tbl_FpType __Gaussian10_Table[10];
/// 克朗罗德-21点系数表
extern const __Tbl_FpType __Kronrod21_Table[22];
/// 高斯-15点系数表
extern const __Tbl_FpType __Gaussian15_Table[16];
/// 克朗罗德-31点系数表
extern const __Tbl_FpType __Kronrod31_Table[32];
/// 高斯-20点系数表
extern const __Tbl_FpType __Gaussian20_Table[20];
/// 克朗罗德-41点系数表
extern const __Tbl_FpType __Kronrod41_Table[42];
/// 高斯-25点系数表
extern const __Tbl_FpType __Gaussian25_Table[26];
/// 克朗罗德-51点系数表
extern const __Tbl_FpType __Kronrod51_Table[52];
/// 高斯-30点系数表
extern const __Tbl_FpType __Gaussian30_Table[30];
/// 克朗罗德-61点系数表
extern const __Tbl_FpType __Kronrod61_Table[62];

#undef __Tbl_FpType
#undef __Tbl_Fp

/**
 * @class GaussKronrodQuadrature
 * @brief 高斯-克朗罗德积分 (实为高斯积分和高斯-克朗罗德积分两种方法的合并)
 * @details 此积分定义为：
 * \f[
 * \int_a^b f(x)dx \approx \sum_{i=1}^n w_i f(x_i)
 * \f]
 * 其中\f$w_i\f$为权重，\f$x_i\f$为积分点
 *
 * @par 示例：
 * 计算exp(-t^2)从0到+inf的积分：
 * \code{.cpp}
 * auto f1 = [](double t) { return exp(-t*t); };
 * GaussKronrodQuadrature IntegralFunc;
 * cout << IntegralFunc(f1, 0, std::numeric_limits<float64>::infinity()) << '\n';
 * \endcode
 * 输出：0.886226925452758 (sqrt(pi)/2)
 *
 * @note 高斯-克朗罗德积分通过高阶(Kronrod)和低阶(Gauss)结果的差值估计误差
 */
class GaussKronrodQuadrature : public DefiniteIntegratingFunction
{
public:
    using Mybase = DefiniteIntegratingFunction; ///< 基类

protected:
    std::vector<float64> GaussCoefficients;     ///< 高斯积分系数
    std::vector<float64> KronrodCoefficients;   ///< 克朗罗德积分系数

    uint64  Order     = 21;    ///< 默认积分阶数(21点Kronrod+10点Gauss)
    float64 Tolerence = 14;    ///< 默认误差的负对数
    uint64  MaxLevels = 15;    ///< 最大递归深度

    /**
     * @brief 获取特殊积分阶数的节点和权重
     * @param[in] N 积分点数
     * @param[out] GaussCoeffs 高斯系数输出
     * @param[out] KronrodCoeffs 克朗罗德系数输出(可选)
     * @return true-找到预定义系数 false-需要计算
     */
    static bool GetNodesAndWeightsSpecialCases(uint64 N, std::vector<float64>* GaussCoeffs, std::vector<float64>* KronrodCoeffs = nullptr);

    /// @brief 高斯积分
    float64 GIntegrate(Function1D f, float64* pL1)const;
    
    /**
     * @brief 自适应积分核心算法
     * @param[in] f 被积函数
     * @param[in] a 积分下限
     * @param[in] b 积分上限
     * @param[in] Level 当前递归深度
     * @param[in] Tol 容差要求
     * @param[out] LastErr 上一级误差估计
     * @param[out] L1 L1范数估计
     * @return 积分结果
     */
    float64 GKAdaptiveIntegrate(Function1D f, float64 a, float64 b, uint64 Level, float64 Tol, float64* LastErr, float64* L1)const;
    
    /// @brief 非自适应积分实现
    float64 GKNonAdaptiveIntegrate(Function1D f, float64* Error, float64* pL1)const;
    
    /// @brief 计算积分
    float64 Run(Function1D f, float64 a, float64 b)const override;

public:
    bool GaussOnly = false; ///< 标记是否仅使用高斯积分(默认false)

    /// @brief 默认构造函数(使用21点规则)
    GaussKronrodQuadrature() : GaussKronrodQuadrature(21) {}
    
    /**
     * @brief 指定积分阶数的构造函数
     * @param[in] N 积分点数(若为非标准点数则计算)
     */
    GaussKronrodQuadrature(uint64 N) : Order(N)
    {
        GetNodesAndWeights(N, &GaussCoefficients, &KronrodCoefficients);
    }

    /**
    * @brief 获取高斯-克朗罗德积分法的节点和权重
    * @param[in] N 积分点数
    * @param[out] GaussCoeffs 高斯系数输出数组，格式为[x0,w0,x1,w1,...]
    * @param[out] KronrodCoeffs 克朗罗德系数输出数组，格式为[x0,w0,x1,w1,...]（可选）
    * @note 对于预定义的N值，直接调用 @ref GetNodesAndWeightsSpecialCases 查表；否则动态计算
    * @details
    * 高斯节点和克朗罗德节点分别为勒让德多项式和斯蒂尔杰斯多项式的解，参见 @ref LegendrePolynomialCoefficients 和 @ref StieltjesPolynomialCoefficients<br>
    * 高斯权重可使用如下公式计算：
    * \f[ w_i^{G} = \frac{2}{(1-x_i^2)[P_n'(x_i)]^2} \f]
    *
    * 克朗罗德权重可使用如下公式计算：
    * - 对于偶数节点：
    * \f[ w_i^{K} = w_i^{G} + \frac{2}{(n+1)P_n'(x_i)E_{n+1}(x_i)} \f]
    * - 对于奇数节点：
    * \f[ w_i^{K} = \frac{2}{(n+1)P_n(x_i)E_{n+1}'(x_i)} \f]
    * 
    * 其中\f$P_n\f$为勒让德多项式，\f$E_{n+1}\f$为斯蒂尔杰斯多项式
    *
    * @par 参考文献
    * - [1] Szeg G. Inequalities for the zeros of Legendre polynomials and related
    *       functions[J]. Transactions of the American Mathematical Society, 1936,
    *       39:1-17. DOI:10.1090/S0002-9947-1936-1501831-2.
    * - [2] Tricomi F G. Sugli zeri dei polinomi sferici ed ultrasferici[J]. 
    *       Annali di Matematica Pura ed Applicata, 1950, 31(1): 93-97.
    *
    * @exception std::logic_error 当多项式求解失败时抛出
    */
    static void GetNodesAndWeights(uint64 N, std::vector<float64>* GaussCoeffs, std::vector<float64>* KronrodCoeffs = nullptr);

    /**
     * @brief 高斯积分
     * @param[in] f 被积函数
     * @param[in] a 积分下限
     * @param[in] b 积分上限
     * @param[out] L1Norm L1范数输出(可选)
     * @return 积分结果
     */
    float64 GaussIntegrate(Function1D f, float64 a, float64 b, float64* L1Norm = nullptr)const;

    /**
     * @brief 执行高斯-克朗罗德自适应积分
     * @param[in] f 被积函数
     * @param[in] a 积分下限
     * @param[in] b 积分上限
     * @param[out] LastError 误差估计(可选)
     * @param[out] L1Norm L1范数估计(可选)
     * @return 积分结果
     */
    float64 GaussKronrodIntegrate(Function1D f, float64 a, float64 b, float64* LastError = nullptr, float64* L1Norm = nullptr)const;
};

/**
 * @struct __Newton_Cotes_Param_Table_Type
 * @brief 牛顿-科特斯积分参数表类型
 * @details 存储牛顿-科特斯积分所需的权重和误差系数
 */
struct __Newton_Cotes_Param_Table_Type
{
    int64 Scale;            ///< 权重系数分母
    const int64* Weights;   ///< 权重系数数组指针
    int64 ErrorCoeff1;      ///< 误差系数1
    int64 ErrorCoeff2;      ///< 误差系数2
};

/**
 * @var __Newton_Cotes_Table
 * @brief 牛顿-科特斯积分预计算参数表
 * @details 包含1-14阶牛顿-科特斯积分的预计算参数
 */
extern const __Newton_Cotes_Param_Table_Type __Newton_Cotes_Table[14];

/**
 * @class NewtonCotesFormulae
 * @brief 牛顿-科特斯积分
 * @details 此积分定义为如下，梯形、辛普森等均为该积分的子集：
 * \f[
 * \int _{a}^{b}f(x)\,dx\approx \sum _{i=0}^{n} \left( w_{i} f(x_{i}) \right)
 * \f]
 * 
 * 其中：
 * - \f$n\f$ 为积分阶数
 * - \f$x_i\f$ 为等距或非等距采样点
 * - \f$w_i\f$ 为权重系数，可通过求解以下线性系统得到：
 * \f[
 * \begin{bmatrix}
 * 1      & 1      & \cdots & 1      \\
 * x_0    & x_1    & \cdots & x_n    \\
 * x_0^2  & x_1^2  & \cdots & x_n^2  \\
 * \vdots & \vdots & \ddots & \vdots \\
 * x_0^{n} & x_1^{n} & \cdots & x_n^{n}
 * \end{bmatrix} 
 * \begin{bmatrix}
 * w_0 \\ w_1 \\ w_2 \\ \vdots \\ w_n
 * \end{bmatrix} = 
 * \begin{bmatrix}
 * n \\ \frac{n^2}{2} \\ \frac{n^3}{3} \\ \vdots \\ \frac{n^{n+1}}{n+1}
 * \end{bmatrix}
 * \f]
 * 
 * @par 参考文献
 * Keesling J. Closed Newton-Cotes Integration[R/OL]. https://people.clas.ufl.edu/kees/files/NewtonCotes.pdf.
 * 
 * @warning 使用此方法计算封闭图形的面积(即曲线积分)时阶数不能太高，最好直接使用梯形或辛普森，要不然结果会很怪
 */
class NewtonCotesFormulae : public SampleBasedIntegratingFunction
{
public:
    using Mybase = SampleBasedIntegratingFunction;  ///< 基类

    /**
     * @struct Block
     * @brief 积分块数据结构
     * @details 包含积分计算所需的所有数据和参数
     */
    struct Block
    {
        std::vector<float64> Samples;   ///< 采样点函数值集合
        float64              BStep;     ///< 采样步长
        std::vector<float64> Weights;   ///< 权重系数向量
        float64              ErrorCoeff;///< 误差系数

        /**
         * @brief 执行当前块的积分计算
         * @return 当前块的积分近似值
         * 
         * 计算公式：
         * \f[
         * I = \text{BStep} \times \sum_{i=0}^{n} w_i \cdot y_i
         * \f]
         */
        float64 Integrate();

        /**
         * @brief 估计当前块积分误差
         * @return 误差估计值
         * @note 当前为待实现状态
         */
        float64 ErrorEstimate();
    };

protected:
    uint64 Level;  ///< 积分阶数(0=龙贝格,1=梯形,2=辛普森,...)

    /**
     * @brief 创建等距采样积分块
     * @param[in] N 采样点数量
     * @return Block 构造的积分块
     */
    Block CreateBlock(uint64 N)const;

    /**
     * @brief 创建自定义节点积分块
     * @param[in] Samples 采样点集合(包含x,y坐标)
     * @return Block 构造的积分块
     */
    Block CreateBlock(std::vector<vec2> Samples)const;

    /**
     * @brief 单次积分实现
     * @param[in] Samples 采样点集合
     * @param[in] L 积分阶数
     * @return float64 积分结果
     */
    float64 SingleIntegrateImpl(std::vector<vec2> Samples, uint64 L)const;

    /**
     * @brief 辛普森积分实现
     * @param[in] Samples 采样点集合
     * @return 积分结果
     */
    static float64 SimpsonImpl(std::vector<vec2> Samples);

    /**
     * @brief 积分计算
     * @param[in] Samples 采样点集合
     * @return float64 积分结果
     * @details 根据Level自动选择适当的积分方法
     */
    float64 Run(std::vector<vec2> Samples) const override;

    /**
     * @brief 从采样点计算权重和误差系数
     * @param[in] SamplePos 采样点位置
     * @param[in] IsEqual 是否为等距节点
     * @param[out] WeightOut 输出的权重系数
     * @param[out] ErrorOut 输出的误差系数
     */
    static void GetWeightsErrorsFromSamples(std::vector<float64> SamplePos, bool IsEqual,
        std::vector<float64>* WeightOut, float64* ErrorOut);

public:
    /**
     * @brief 默认构造函数(1阶梯形法则)
     */
    NewtonCotesFormulae() : NewtonCotesFormulae(1) {}

    /**
     * @brief 构造函数
     * @param[in] N 积分阶数(0=龙贝格,1=梯形,2=辛普森,...)
     */
    explicit NewtonCotesFormulae(uint64 N) : Level(N) {}

    /**
     * @brief 获取等距节点的积分参数
     * @param[in] N 节点数量
     * @param[out] Weight 输出的权重系数
     * @param[out] Error 输出的误差系数
     * @exception std::logic_error 当N为0时抛出
     */
    static void GetEvenlySizedParameters(uint64 N, std::vector<float64>* Weight, float64* Error);

    /**
     * @brief 获取预定义特殊情况的积分参数
     * @param[in] N 节点数量(1-14)
     * @param[out] Weight 输出的权重系数
     * @param[out] Error 输出的误差系数
     */
    static void GetSpecialCaseParameters(uint64 N, std::vector<float64>* Weight, float64* Error);

    /**
     * @brief 从采样点获取积分参数
     * @param[in] SamplePos 采样点位置
     * @param[out] Weight 输出的权重系数
     * @param[out] Error 输出的误差系数
     * @exception std::logic_error 当采样点少于2个时抛出
     */
    static void GetParametersFromSamples(std::vector<float64> SamplePos,
        std::vector<float64>* Weight, float64* Error);

    /**
     * @brief 判断采样点是否为等距分布
     * @param[in] Samples 采样点集合
     * @return bool true-等距, false-非等距
     */
    static bool IsEvenlySized(std::vector<vec2> Samples);

    /**
     * @brief 梯形积分
     * @details
     * 梯形积分的定义如下：
     * \f[
     * \int _{a}^{b}f(x)\,dx\approx \sum _{k=1}^{N}{\frac {f(x_{k-1})+f(x_{k})}{2}}\Delta x_{k}
     * \f]
     * @param[in] Samples 采样点集合
     * @return float64 积分结果
     * @exception std::logic_error 当采样点少于2个时抛出
     */
    static float64 Trapezoidal(std::vector<vec2> Samples);

    /**
     * @brief 辛普森积分，可自动处理偶数个采样点的情况
     * @details
     * 辛普森积分定义如下，适用于等距采样：
     * \f[
     * {\begin{aligned}\int _{a}^{b}f(x)\,dx&\approx {\frac {1}{3}}h\sum _{i=1}^{n/2}{\big [}f(x_{2i-2})+4f(x_{2i-1})+f(x_{2i}){\big ]}\\&={\frac {1}{3}}h{\big [}f(x_{0})+4f(x_{1})+2f(x_{2})+4f(x_{3})+2f(x_{4})+\dots +2f(x_{n-2})+4f(x_{n-1})+f(x_{n}){\big ]}\\&={\frac {1}{3}}h\left[f(x_{0})+4\sum _{i=1}^{n/2}f(x_{2i-1})+2\sum _{i=1}^{n/2-1}f(x_{2i})+f(x_{n})\right].\end{aligned}}
     * \f]
     * 
     * 如果样本点非等距则使用以下算法：
     * \f[
     * \int _{a}^{b}f(x)\,dx\approx \sum _{i=0}^{N/2-1}{\frac {h_{2i}+h_{2i+1}}{6}}\left[\left(2-{\frac {h_{2i+1}}{h_{2i}}}\right)f_{2i}+{\frac {(h_{2i}+h_{2i+1})^{2}}{h_{2i}h_{2i+1}}}f_{2i+1}+\left(2-{\frac {h_{2i}}{h_{2i+1}}}\right)f_{2i+2}\right]
     * \f]
     *
     * 如果样本点个数为偶数，则正常积分到倒数第二个点，最后一个点按如下方法处理后相加：
     * \f[
     * \alpha f_{N}+\beta f_{N-1}-\eta f_{N-2}
     * \f]
     * 其中：
     * \f[
     * {\begin{aligned}\alpha &={\frac {2h_{N-1}^{2}+3h_{N-1}h_{N-2}}{6(h_{N-2}+h_{N-1})}},\\[1ex]\beta &={\frac {h_{N-1}^{2}+3h_{N-1}h_{N-2}}{6h_{N-2}}},\\[1ex]\eta &={\frac {h_{N-1}^{3}}{6h_{N-2}(h_{N-2}+h_{N-1})}}.\end{aligned}}
     * \f]
     * 
     * @param[in] Samples 采样点集合
     * @return float64 积分结果
     * @exception std::logic_error 当采样点少于3个时抛出
     */
    static float64 Simpson(std::vector<vec2> Samples);

    /**
     * @brief 龙贝格积分
     * @param[in] Samples 等距采样点集合(数量需为2^N+1)
     * @param[out] RichardsonExtrapolationTable 理查森外推表(可选输出)
     * @return 积分近似值
     * @exception std::logic_error 当采样点非等距或数量不符合要求时抛出
     */
    static float64 Romberg(std::vector<vec2> Samples,
        DynamicMatrix<float64>* RichardsonExtrapolationTable = nullptr);

    /**
     * @brief 单次积分
     * @param[in] Samples 采样点集合
     * @return float64 积分结果
     * @exception std::logic_error 当采样点数量与阶数不匹配时抛出
     */
    float64 SingleIntegrate(std::vector<vec2> Samples)const;

    /**
     * @brief 复合积分(适用于等距采样)
     * @param[in] Samples 采样点集合
     * @return float64 积分结果
     * @exception std::logic_error 当采样点非等距时抛出
     */
    float64 CompositeIntegrate(std::vector<vec2> Samples)const;

    /**
     * @brief 离散积分(适用于非等距采样)
     * @param[in] Samples 采样点集合
     * @return float64 积分结果
     */
    float64 DiscreteIntegrate(std::vector<vec2> Samples)const;
};

/// 默认积分函数类型别名(使用高斯-克朗罗德求积法)
using DefaultIntegratingFunction = GaussKronrodQuadrature;

// ------------------------------------------------------------------------------------- //

/**
 * @brief 黎曼-刘维尔积分，可计算非整数阶积分
 *
 * 此广义不定积分的定义为：
 * \f[
 * \int^{(\alpha)} f(x) dx = \frac{1}{\Gamma(\alpha)} \int_{C}^{x} (x-t)^{\alpha-1} f(t) dt
 * \f]
 * 其中：
 *   - \f$\alpha\f$ 为积分阶数（可为非整数）
 *   - \f$c\f$ 为积分基数（初始条件点）
 *   - \f$F(c)\f$ 为初始条件值（确定积分常数）
 *   - \f$\Gamma\f$ 为伽马函数
 *
 * 因为一个函数的不定积分有无限多个，故此处需要一个辅助点以确定最终得到的函数<br>
 * 例如：被积函数为f(x) = x，初始条件F(1) = 0<br>
 * 此时f(x)的一阶不定积分为\f$ F1(x) = \frac{1}{2}  x^2 - \frac{1}{2} \f$
 * 二阶不定积分为 \f$ F2(x) = \frac{1}{6}  x^3 - \frac{1}{2}  x + \frac{1}{3} \f$
 * 更高阶的不定积分以此类推。
 */
class RiemannLiouvilleIntegratingFunction : public IndefiniteIntegratingFunction
{
public:
    using Mybase        = IndefiniteIntegratingFunction; ///< 基类
    using DefaultEngine = DefaultIntegratingFunction;    ///< 默认积分引擎
    using EnginePtr     = std::shared_ptr<DefiniteIntegratingFunction>; ///< 积分引擎指针

protected:
    EnginePtr Engine;      ///< 定积分引擎
    vec2      InitValue;   ///< 积分基数(c)和初始值(F(c))的组合

public:
    /**
     * @brief 构造函数
     * @param f         被积函数
     * @param Order     积分阶数α（默认1阶）
     * @param a         积分基数(c)和初始值(F(c))（默认(0,0)）
     * @param IFunc     定积分计算引擎（默认使用DefaultEngine）
     */
    RiemannLiouvilleIntegratingFunction(
        Function1D f, 
        float64 Order = 1,
        vec2 a = vec2(0, 0), 
        EnginePtr IFunc = std::make_shared<DefaultEngine>(DefaultEngine())
    ) : InitValue(a), Engine(IFunc)
    {
        OriginalFunction = f;
        IntegralOrder = Order;
    }

    /**
     * @brief 计算分数阶积分在点x处的值
     * @param x  积分上限
     * @return 积分的值
     */
    float64 operator()(float64 x)const override;
};

/**@}*/


/**
 * @defgroup ODEs 微分方程
 * @ingroup AdvMath
 * @brief 一些微分方程算法
 * @{
 */

/**
 * @brief 常微分方程(ODE)求解器，此功能译自Scipy
 * 以下是来自SciPy的原始功能描述：
 *
 * 本类对给定初始值的常微分方程组进行数值积分：
 * \f[
 *   \begin{cases}
 *   \mathbf{y}' = f(x, \mathbf{y}) \\
 *   \mathbf{y}(x_0) = \mathbf{y}_0
 *   \end{cases}
 * \f]
 * 其中：
 * - \f$x\f$ : 一维自变量
 * - \f$\mathbf{y}(x)\f$ : \f$N\f$维向量值函数
 * - \f$f(x, \mathbf{y})\f$ : 待求解的微分方程
 *
 * 目标是在已知初始条件\f$\mathbf{y}(x_0)=\mathbf{y}_0\f$时，近似计算满足微分方程组的\f$\mathbf{y}(x)\f$
 *
 * @par 示例
 * 洛特卡-沃尔泰拉方程的定义如下：
 * \f[
 *   \begin{cases}
 *   x'(t) = \alpha x - \beta x y \\
 *   y'(t) = -\gamma y + \delta x y
 *   \end{cases}
 * \f]
 * 以α = 1.5, β = 1, γ = 3, δ = 1为例，可以使用以下方法生成此函数：
 * @code
 * float64 a = 1.5, b = 1, c = 3, d = 1;
 * auto lotkavolterra = [a, b, c, d](float64 t, std::vector<float64> z)
 * {
 *     float64 x = z[0], y = z[1];
 *     return std::vector<float64>{a*x - b*x*y, -c*y + d*x*y};
 * };
 * auto ODE = CreateODEFunction<...>(lotkavolterra, {10, 5}, 0, 15);
 * cout << '[';
 * for (int i = 0; i <= 300; ++i)
 * {
 *     cout << (*ODE)((15. / 300.) * i) << ", ";
 * }
 * cout << "]\n";
 * @endcode
 * 将输出的数组使用matplotlib打印出来，即可看到函数图像
 */
class OrdinaryDifferentialEquation
{
public:
    /// 微分方程函数类型：f(t, y) -> y'(t)
    using Fty        = std::vector<float64>(float64 Scalar, std::vector<float64> Coeffs);
    
    /// 状态向量类型（多分量值）
    using ValueArray = std::vector<float64>;
    
    /// 状态存储容器（时间点 -> 状态向量）
    using StateType  = std::map<float64, ValueArray>;

    /**
     * @brief 函数块
     * 
     * 在[First, Last]区间内连续取值
     */
    struct DenseOutput
    {
    public:
        /// 状态向量类型
        using ValueArray = std::vector<float64>;

    protected:
        float64 First;  ///< 区间起始点
        float64 Last;   ///< 区间结束点

    public:
        DenseOutput() {}
        
        /**
         * @brief 构造函数
         * @param[in] Fi 区间起始点
         * @param[in] La 区间结束点
         */
        DenseOutput(float64 Fi, float64 La) : First(Fi), Last(La) {}

        float64 first()const {return First;}  ///< 获取区间起始点
        float64 last()const {return Last;}    ///< 获取区间结束点
        float64 size()const {return Last - First;}  ///< 获取区间长度

        /**
         * @brief 插值计算算子
         * @param[in] _Xx 目标时间点
         * @return 对应时间点的状态向量
         */
        virtual ValueArray operator()(float64 _Xx)const = 0;
    };

    /// 求解器状态码
    enum StateCode
    {
        Processing = -1,  ///< 计算中
        Succeeded  = 0,   ///< 成功
        Failed     = 1    ///< 失败
    };

protected:
    std::function<Fty>   Invoker;      ///< 微分方程 f(t,y)
    enum StateCode       State;        ///< 当前求解状态
    float64              EndPoint;     ///< 积分终点
    bool                 Direction;    ///< 积分方向（0:正向，1:负向）
    StateType            StateBuffer;  ///< 状态缓存（时间点->状态）

    /**
     * @brief 单步求解（纯虚函数）
     * @return 状态码（成功/失败）
     */
    virtual int Run() = 0;

public:
    OrdinaryDifferentialEquation() {}
    
    /**
     * @brief 构造函数
     * @param[in] Right 微分方程函数
     */
    OrdinaryDifferentialEquation(std::function<Fty> Right) : Invoker(Right) {}

    float64 CurrentPoint()const;      ///< 获取当前积分点
    float64 PrevPoint()const;         ///< 获取上一积分点
    StateType Solutions()const {return StateBuffer;}  ///< 获取全部求解结果
    enum StateCode CurrentState()const {return State;}  ///< 获取当前状态
    
    /// 获取当前步长
    constexpr float64 size()const {return abs(CurrentPoint() - PrevPoint());}

    /**
     * @brief 初始化求解器（纯虚函数）
     * @param[in] InitState 初始状态向量
     * @param[in] First 积分起点
     * @param[in] Last 积分终点
     */
    virtual void Init(ValueArray InitState, float64 First, float64 Last) = 0;
    
    virtual void Clear() = 0;  ///< 重置所有状态
    
    /**
     * @brief 调用计算
     * @exception 可能抛出数值计算异常
     */
    void __cdecl InvokeRun() noexcept(0);
    
    virtual void SaveDenseOutput() = 0;  ///< 保存状态

    /**
     * @brief 计算参数x处的值
     * @param[in] x 参数
     * @return 对应的所有函数的值
     */
    virtual ValueArray operator()(float64 x)const = 0;
};

/// @brief RK23方法C系数表 \f$c_i\f$
extern const uint64  __RK23_C_Table[3];   
/// @brief RK23方法A系数表 \f$a_{ij}\f$
extern const uint64  __RK23_A_Table[9];   
/// @brief RK23方法B系数表 \f$b_i\f$
extern const uint64  __RK23_B_Table[3];   
/// @brief RK23方法误差估计系数表 \f$\hat{b}_i - b_i\f$
extern const uint64  __RK23_E_Table[4];  
/// @brief RK23方法插值系数表
extern const uint64  __RK23_P_Table[12];  
/// @brief RK45方法C系数表 \f$c_i\f$
extern const float64 __RK45_C_Table[6];   
/// @brief RK45方法A系数表 \f$a_{ij}\f$
extern const float64 __RK45_A_Table[30];  
/// @brief RK45方法B系数表 \f$b_i\f$
extern const float64 __RK45_B_Table[6];   
/// @brief RK45方法误差估计系数表 \f$\hat{b}_i - b_i\f$
extern const float64 __RK45_E_Table[7];   
/// @brief RK45方法插值系数表
extern const float64 __RK45_P_Table[28];  

/**
 * @class RungeKuttaODEEngine
 * @brief 龙格-库塔微分方程求解引擎
 */
class RungeKuttaODEEngine : public OrdinaryDifferentialEquation
{
public:
    using Mybase     = OrdinaryDifferentialEquation;  ///< 基类
    using ValueArray = Mybase::ValueArray;            ///< 值类型
    using StateType  = DynamicMatrix<float64>;        ///< 状态矩阵类型

    /**
     * @struct DenseOutput
     * @brief 函数块保存
     */
    struct DenseOutput : public Mybase::DenseOutput
    {
        using Mybase   = Mybase::DenseOutput;   ///< 基类
        using QTblType = DynamicMatrix<float64>; ///< 系数矩阵类型

        uint64     DenseOutputOrder; ///< 阶数 \f$p_d\f$
        QTblType   QTable;           ///< 系数矩阵 \f$Q \in \mathbb{R}^{(s+1) \times p_d}\f$
        ValueArray Base;             ///< 基础解向量 \f$y(t_0)\f$

        DenseOutput() = default;  ///< 默认构造函数

        /**
         * @brief 参数化构造函数
         * @param[in] DenseOutputOrder 插值阶数
         * @param[in] First 区间起始点 \f$t_0\f$
         * @param[in] Last 区间结束点 \f$t_1\f$
         * @param[in] BaseValue 基础解向量 \f$y(t_0)\f$
         * @param[in] QTbl 龙格-库塔系数矩阵 \f$Q\f$
         */
        DenseOutput(uint64 DenseOutputOrder, float64 First, float64 Last,
            ValueArray BaseValue, QTblType QTbl)
            : DenseOutputOrder(DenseOutputOrder), Mybase(First, Last),
            Base(std::move(BaseValue)), QTable(std::move(QTbl)) {}

        /**
         * @brief 计算给定参数的函数值
         * @param[in] _Xx 参数 \f$t \in [t_0, t_1]\f$
         * @return 函数值 \f$y(t)\f$
         */
        ValueArray operator()(float64 _Xx) const override;

        friend struct RungeKuttaODEEngine;
    };

    constexpr static const float64 MinFactor  = 0.2;    ///< 最小缩放因子 \f$\alpha_{\min}\f$
    constexpr static const float64 MaxFactor  = 10.;    ///< 最大缩放因子 \f$\alpha_{\max}\f$
    constexpr static const float64 FactorSafe = 0.9;    ///< 安全因子 \f$\beta\f$

protected:
    uint32_t ErrorEsitmatorOrder; ///< 误差估计阶数 \f$q\f$
    uint32_t StepTakenOrder;      ///< 实际阶数 \f$p\f$
    uint32_t NStages;             ///< 计算级数 \f$s\f$
    uint64   DenseOutputOrder;    ///< 连续输出阶数 \f$p_d\f$
    uint64   EquationCount;       ///< 微分方程数量 \f$n\f$

    const float64* CTable; ///< \f$c_i\f$ 系数数组
    const float64* ATable; ///< \f$a_{ij}\f$ 系数数组
    const float64* BTable; ///< \f$b_i\f$ 系数数组
    const float64* ETable; ///< 误差估计系数 \f$e_i\f$ 数组
    const float64* PTable; ///< 连续输出系数数组

    std::map<float64, DenseOutput> Interpolants; ///< 函数块存储
    ValueArray     CurrentFx;  ///< 当前导数值 \f$f(t_n, y_n)\f$
    StateType      KTable;     ///< 阶段导数矩阵 \f$K\f$

    float64        RelTolerNLog = 3;  ///< 相对误差负对数 \f$-\log(\epsilon_{rel})\f$
    float64        AbsTolerNLog = 6;  ///< 绝对误差负对数 \f$-\log(\epsilon_{abs})\f$
    float64        MaxStep = __Float64::FromBytes(POS_INF_DOUBLE); ///< 最大允许步长 \f$h_{max}\f$
    float64        AbsStep;           ///< 当前绝对步长 \f$|h|\f$
    const float64  ErrExponent;       ///< 误差指数 \f$\frac{1}{q+1}\f$

    /**
     * @brief 计算向量的RMS范数
     * @param[in] Input 输入向量 \f$\mathbf{v}\f$
     * @return RMS范数值 \f$\|\mathbf{v}\|_{RMS} = \sqrt{\frac{1}{n}\sum_{i=1}^n v_i^2}\f$
     */
    float64 RMSNorm(std::vector<float64> Input);

    /// @brief 设置初始步长(自适应策略)
    void SetInitStep();

    /**
     * @brief 构造函数
     * @param[in] Right 微分方程 \f$f(t,y)\f$
     * @param[in] ErrorEsitmatorOrder 误差估计阶数 \f$q\f$
     * @param[in] StepTakenOrder 实际阶数 \f$p\f$
     * @param[in] NStages 计算级数 \f$s\f$
     * @param[in] DenseOutputOrder 连续输出阶数 \f$p_d\f$
     * @param[in] EquationCount 微分方程数量 \f$n\f$
     */
    RungeKuttaODEEngine(std::function<Fty> Right,
        uint32_t ErrorEsitmatorOrder, uint32_t StepTakenOrder, uint32_t NStages,
        uint64 DenseOutputOrder, uint64 EquationCount) : 
        Mybase(std::move(Right)),
        ErrorEsitmatorOrder(ErrorEsitmatorOrder), 
        StepTakenOrder(StepTakenOrder), 
        NStages(NStages),
        DenseOutputOrder(DenseOutputOrder), 
        EquationCount(EquationCount),
        CurrentFx(EquationCount), 
        KTable({NStages + 1, EquationCount}),
        ErrExponent(1.0 / (ErrorEsitmatorOrder + 1)) {}

    /// @brief 执行求解过程(核心算法)
    int Run() override;

public:
    /**
     * @brief 初始化求解器（自动步长）
     * @param[in] InitState 初始状态 \f$y(t_0)\f$
     * @param[in] First 积分起点 \f$t_0\f$
     * @param[in] Last 积分终点 \f$t_{end}\f$
     */
    void Init(ValueArray InitState, float64 First, float64 Last) override;

    /**
     * @brief 初始化求解器（指定步长）
     * @param[in] InitState 初始状态 \f$y(t_0)\f$
     * @param[in] First 积分起点 \f$t_0\f$
     * @param[in] Last 积分终点 \f$t_{end}\f$
     * @param[in] InitStep 初始步长 \f$h_0\f$
     */
    virtual void Init(ValueArray InitState, float64 First, float64 Last, float64 InitStep);
    
    /// @brief 清除求解器状态
    void Clear() override;
    
    /// @brief 保存函数块
    void SaveDenseOutput() override;
    
    /**
     * @brief 计算指定参数的函数值
     * @param[in] _Xx 参数 \f$t\f$
     * @return 函数值 \f$y(t)\f$
     */
    ValueArray operator()(float64 _Xx) const override;
};

/**
 * @class RungeKutta2ndOrderODEEngine
 * @brief 2阶龙格-库塔算法(阶数23方法)
 * 
 * 此方法也称博加茨基-尚平算法:
 * - 误差估计阶数: 2
 * - 实际阶数: 3
 * - 计算级数: 3
 * - 连续输出阶数: 3
 *
 * @see https://en.wikipedia.org/wiki/Bogacki%E2%80%93Shampine_method
 */
typedef class RungeKutta2ndOrderODEEngine : public RungeKuttaODEEngine
{
public:
    using Mybase = RungeKuttaODEEngine;  ///< 基类

    /**
     * @brief 构造函数
     * @param[in] Function 微分方程
     * @param[in] EquationCount 方程数量
     */
    RungeKutta2ndOrderODEEngine(std::function<Fty> Function, uint64 EquationCount) :
        Mybase(std::move(Function), 2, 3, 3, 3, EquationCount) {}

    /**
     * @brief 初始化求解器
     * @param[in] InitState 初始状态
     * @param[in] First 积分起点
     * @param[in] Last 积分终点
     * @param[in] InitStep 初始步长（可选）
     * 
     * 加载RK23方法的Butcher表系数
     */
    void Init(ValueArray InitState, float64 First, float64 Last,
        float64 InitStep = __Float64::FromBytes(BIG_NAN_DOUBLE)) override
    {
        CTable = reinterpret_cast<const float64*>(__RK23_C_Table);
        ATable = reinterpret_cast<const float64*>(__RK23_A_Table);
        BTable = reinterpret_cast<const float64*>(__RK23_B_Table);
        ETable = reinterpret_cast<const float64*>(__RK23_E_Table);
        PTable = reinterpret_cast<const float64*>(__RK23_P_Table);
        Mybase::Init(std::move(InitState), First, Last, InitStep);
    }
}BogackiShampineODEEngine;

/**
 * @class RungeKutta4thOrderODEEngine
 * @brief 4阶龙格-库塔算法(阶数45方法)
 * 
 * 此方法也称多尔曼-普林斯算法:
 * - 误差估计阶数: 4
 * - 实际阶数: 5
 * - 计算级数: 6
 * - 稠密输出阶数: 4
 *
 * @see https://en.wikipedia.org/wiki/Dormand%E2%80%93Prince_method
 */
typedef class RungeKutta4thOrderODEEngine : public RungeKuttaODEEngine
{
public:
    using Mybase = RungeKuttaODEEngine;  ///< 基类

    /**
     * @brief 构造函数
     * @param[in] Function 微分方程
     * @param[in] EquationCount 方程数量
     */
    RungeKutta4thOrderODEEngine(std::function<Fty> Function, uint64 EquationCount) :
        Mybase(std::move(Function), 4, 5, 6, 4, EquationCount) {}

    /**
     * @brief 初始化求解器
     * @param[in] InitState 初始状态
     * @param[in] First 积分起点
     * @param[in] Last 积分终点
     * @param[in] InitStep 初始步长（可选）
     * 
     * 加载RK45方法的Butcher表系数
     */
    void Init(ValueArray InitState, float64 First, float64 Last,
        float64 InitStep = __Float64::FromBytes(BIG_NAN_DOUBLE)) override
    {
        CTable = __RK45_C_Table;
        ATable = __RK45_A_Table;
        BTable = __RK45_B_Table;
        ETable = __RK45_E_Table;
        PTable = __RK45_P_Table;
        Mybase::Init(std::move(InitState), First, Last, InitStep);
    }
}DormandPrinceODEEngine, RungeKuttaDPODEEngine, DOPRIODEEngine;

/// @brief 默认ODE求解引擎(使用4阶龙格-库塔方法)
using DefaultODEEngine = RungeKutta4thOrderODEEngine;

// ------------------------------------------------------------------------------------- //

/**
 * @brief 快速创建常微分方程（ODE）
 * 
 * 创建指定的 ODE 求解引擎对象，初始化求解器并进行全区间积分计算，
 * 最终返回包含求解结果的智能指针。求解过程满足：
 * \f[
 * y'(t) = f(t, y),\quad y(t_0) = y_0
 * \f]
 * 
 * @tparam Engine       ODE 求解引擎类型，需满足：Engine 派生自 OrdinaryDifferentialEquation
 * @param Func          常微分方程 \f$ f(t, y) \f$，原型为 OrdinaryDifferentialEquation::Fty，注意函数返回的数组长度要和输入数组的长度相同
 * @param Coeffs        初值条件数组 \f$ y(t_0) \f$，数组长度决定方程个数
 * @param First         积分起始点 \f$ t_0 \f$
 * @param Last          积分终止点 \f$ t_{\text{end}} \f$
 * @return std::shared_ptr<OrdinaryDifferentialEquation> 包含求解结果的 ODE 对象指针
 */
template<typename Engine> requires std::is_base_of_v<OrdinaryDifferentialEquation, Engine>
std::shared_ptr<OrdinaryDifferentialEquation>
CreateODEFunction(std::function<OrdinaryDifferentialEquation::Fty> Func,
    OrdinaryDifferentialEquation::ValueArray Coeffs, float64 First, float64 Last)
{
    OrdinaryDifferentialEquation* Eng = new Engine(Func, Coeffs.size());
    Eng->Init(Coeffs, First, Last);
    while (Eng->CurrentState() == Eng->Processing)
    {
        Eng->InvokeRun();
        Eng->SaveDenseOutput();
    }
    return std::shared_ptr<OrdinaryDifferentialEquation>(Eng);
}

/**@}*/


/**
 * @defgroup Optimizations 数值优化
 * @ingroup AdvMath
 * @brief 一些数值优化算法
 * @{
 */

/**
 * @brief 一维函数极小值求解器基类
 * 
 * 此功能由 SciPy 的 minimize_scalar 函数转写而成，用于求解函数在给定区间内的极小值点。
 * 子类需实现具体的优化算法。
 */
class MinimizerBase
{
public:
    /**
     * @brief 执行极小值求解操作
     * 
     * @param[in] Func 目标函数，需满足一维函数接口要求
     * @return vec2 包含两个元素的向量：
     *         - vec2.x: 极小值点坐标
     *         - vec2.y: 函数在极小值点处的取值
     */
    virtual vec2 operator()(Function1D Func)const = 0;
};

// ------------------------------------------------------------------------------------- //

/**
 * @brief 布伦特无约束极小值优化算法
 * 
 * 此功能译自 SciPy 的 brent 优化实现，Boost 库中有类似功能。
 * 基于黄金分割搜索和抛物线插值法，求解函数在无约束条件下的局部极小值。
 * 
 * 算法原理详见Boost库的文档：https://www.boost.org/doc/libs/latest/libs/math/doc/html/math_toolkit/brent_minima.html
 */
class BrentUnboundedMinimizer : public MinimizerBase
{
public:
    using Mybase = MinimizerBase;  ///< 基类

    /// 黄金分割比例常数 \f$ \phi = \frac{\sqrt{5}-1}{2} \approx 0.618 \f$
    constexpr static const float64 GoldenRatio  = 0.61803398874989484820458683436564;
    
    /// 黄金分割比例相关常数 \f$ 1 + \phi \f$
    constexpr static const float64 GoldenRatio2 = 1. + GoldenRatio;
    
    /// 黄金分割比例共轭值 \f$ 1 - \phi \f$
    constexpr static const float64 GRatioConj   = 1. - GoldenRatio;

    /**
     * @brief 区间包围结构体
     * 
     * 包含三个有序点和对应函数值：
     * - First: 左边界点 \f$ (x_a, f(x_a)) \f$
     * - Centre: 中间点 \f$ (x_b, f(x_b)) \f$，满足 \f$ f(x_b) < f(x_a) \f$ 且 \f$ f(x_b) < f(x_c) \f$
     * - Last: 右边界点 \f$ (x_c, f(x_c)) \f$
     */
    struct BracketType
    {
        vec2 First;   ///< 区间左端点及函数值
        vec2 Centre;  ///< 区间中心点及函数值（当前最优点）
        vec2 Last;    ///< 区间右端点及函数值
    };

protected:
    float64 Tolerence    = 7.83;  ///< 收敛容差的负对数（默认值对应 \f$ \epsilon = 10^{-7.83} \approx 1.48 \times 10^{-8} \f$)
    float64 MaxIter      = 2.7;   ///< 最大迭代次数的对数（默认 \f$ \lfloor 10^{2.7} \rfloor = 500 \f$ 次）
    float64 MinTolerence = 11;    ///< 最小容差的负对数（\f$ \epsilon_{\text{min}} = 10^{-11} \f$）

public:
    BrentUnboundedMinimizer() {}  ///< 默认构造函数

    /**
     * @brief 为目标函数生成有效区间包围结构
     * 
     * 在目标函数 func 附近搜索有效区间 \f$ (x_a, x_b, x_c) \f$，满足：
     * 1. \f$ x_a < x_b < x_c \f$
     * 2. \f$ f(x_b) < f(x_a) \f$ 且 \f$ f(x_b) < f(x_c) \f$
     * 
     * @note 此函数译自SciPy的bracket函数
     * 
     * @param[in] Function 待求解函数
     * @param[in] InitStart 初始搜索起点（默认 0.0）
     * @param[in] InitEnd 初始搜索终点（默认 1.0）
     * @param[out] FCalls 函数调用次数计数器（可选）
     * @param[in] MaxIter 最大迭代次数的对数（默认 3，对应 1000 次）
     * @param[in] AbsoluteTolerence 绝对容差的负对数（默认 21，对应 \f$ 10^{-21} \f$)
     * @param[in] GrowLimit 区间扩展因子上限（默认 110）
     * @return BracketType 生成的区间包围结构
     */
    static BracketType CreateBracket(Function1D Function, float64 InitStart = 0, float64 InitEnd = 1,
        uint64* FCalls = nullptr, float64 MaxIter = 3,
        float64 AbsoluteTolerence = 21, float64 GrowLimit = 110)noexcept(0);

    /**
     * @brief 从预定义点集生成区间包围结构
     * 
     * @param[in] Function 目标函数对象
     * @param[in] Points 初始点集（至少需包含 3 个点）
     * @param[out] FCalls 函数调用次数计数器（可选）
     * @return BracketType 生成的区间包围结构
     */
    static BracketType CreateBracketFromArray(Function1D Function,
        std::vector<float64> Points = {}, uint64* FCalls = nullptr);

    /**
     * @brief 求解极小值
     * 
     * @param[in] Func 目标函数对象
     * @param[in] Points 可选初始点集
     * @return vec2 极小值点坐标及函数值
     */
    vec2 Run(Function1D Func, std::vector<float64> Points = {})const;

    /// 实现基类的函数调用接口
    vec2 operator()(Function1D Func)const override;
};

/**@}*/


/**
 * @defgroup InvFuncs 反函数
 * @ingroup AdvMath
 * @brief 一些反函数/方程求解算法
 * @{
 */

/**
 * @class InverseFunction
 * @brief 反函数基类
 */
class InverseFunction
{
protected:
    Function1D OriginalFunction; ///< 原函数

public:
    /**
     * @brief 函数调用运算符
     * @param x 输入值
     * @return 反函数计算结果
     */
    virtual float64 operator()(float64 x)const = 0;
};

/**
 * @class BracketingRootFindingEngine
 * @brief 包围法求根引擎
 * 
 * 使用包围法(如二分法)寻找函数根的基类
 */
class BracketingRootFindingEngine : public InverseFunction
{
public:
    using Mybase = InverseFunction; ///< 基类别名

protected:
    float64 First; ///< 搜索区间起始点
    float64 Last;  ///< 搜索区间终点
};

/**
 * @class IterativeRootFindingEngine
 * @brief 迭代法求根引擎
 * 
 * 使用迭代法(如牛顿法)寻找函数根的基类
 */
class IterativeRootFindingEngine : public InverseFunction
{
public:
    using Mybase = InverseFunction; ///< 基类别名

protected:
    float64 ReferencePoint; ///< 迭代参考点
};

// ------------------------------------------------------------------------------------- //

/**
 * @page InverseFuncDisclaimer 一些碎碎念
 * <b>丹霞</b>：Pynverse的作者Alvaro Sanchez Gonzalez(英)在Pynverse的免责声明中表示，它的算法尽管性能很好，但是在某些特定条件下仍会掉精度，并且它的精度是不可控的，所以也不能说是一种完全通用的方法。既然如此就会产生一个问题，就是对一个函数数值求反函数是否有一种精度可控且普适性的方法呢？答案是目前没有，甚至来说在一个开放区间内数值计算任意函数的反函数本身仍是数学界的一个尚未解决的难题。本质上来说，Pynverse的算法为一种可以“自适应”的布伦特算法——一种混合了二分法、割线法和逆二次插值的函数求解算法。<br>
 *
 * Pynverse库的免责声明：https://github.com/alvarosg/pynverse/blob/master/README.md
 */

/// 一个-inf到+inf的范围
static const vec2 __Whole_Line =
    {__Float64::FromBytes(NEG_INF_DOUBLE), __Float64::FromBytes(POS_INF_DOUBLE)};

/**
 * @class BrentInverseFunction
 * @brief 布伦特反函数，译自pynverse，并转写为独立的类。
 *
 * 此方法的原理就是通过求一个与f(x)-y相关的损失函数的极小值以确定反函数的值。可选用如下函数作为损失函数：
 *   \f[ L_1(x) = (f(x) - y)^2 \f]
 *   \f[ L_2(x) = |f(x) - y| \f]
 * 其中f(x)为原函数，y为反函数的参数。这里最好是使用L_1作为损失函数，因为它是光滑的，易于求导，而L_2可能会出现部分点不可导的情况。损失函数的极小值就是反函数值
 *
 * @par 示例
 * 求\f$ y = e^x \f$在\f$ y = 10 \f$处的反函数值(即\f$ \ln(10) \f$)
 * - 先定义损失函数：\f$ L(x) = (e^x - 10)^2 \f$
 * - 求导得：\f$ \nabla L(x) = 2(e^x - 10)e^x \f$
 * - 令导数为零：\f$ x = \ln(10) \f$
 */
class BrentInverseFunction : public InverseFunction
{
public:
    using Mybase = InverseFunction; ///< 基类

protected:
    vec2    Domain;     ///< 定义域
    bvec2   OpenDomain; ///< 区间类型(0=闭，1=开)
    vec2    Range;      ///< 值域

    /**
     * @brief 检查参数有效性
     */
    void CheckParameters();

    /**
     * @brief 获取参考点
     * @param Domain 定义域区间
     * @return 参考点坐标
     */
    vec2 GetReferencePoints(vec2 Domain)const;

    /**
     * @brief 获取参考值
     * @param Domain 定义域区间
     * @param Trend 函数趋势
     * @return 参考值
     */
    vec2 GetReferenceValues(vec2 Domain, int Trend)const;

public:
    /**
     * @brief 构造函数
     * @param Func 原函数对象
     * @param Domain 原函数定义域，默认为(-inf, +inf)
     * @param OpenDomain 区间开闭标志，默认为闭区间
     * @param Range 原函数值域，默认为(-inf, +inf)
     * @note 反函数的值域=原函数定义域，反函数定义域=原函数值域
     */
    BrentInverseFunction(Function1D Func, vec2 Domain = __Whole_Line,
        bvec2 OpenDomain = {0, 0}, vec2 Range = __Whole_Line)
        : Domain(Domain), OpenDomain(OpenDomain), Range(Range)
    {
        OriginalFunction = Func;
        CheckParameters();
    }

    /**
     * @brief 计算反函数值
     * @param x 输入值
     * @return 反函数计算结果
     */
    float64 operator()(float64 x)const override;
};

/**
 * @brief 二分搜索反函数（二分法求根算法）
 * @details 
 * 最简单的寻根算法，通过不断二分区间逼近方程的根。虽然计算量较大但稳定可靠。\n
 * @note「一尺之棰，日取其半，万世不竭。」
 */
class BisectionRootFindingEngine : public BracketingRootFindingEngine
{
public:
    using Mybase = BracketingRootFindingEngine;  ///< 基类类型别名

protected:
    float64 AbsoluteTolernece = 11.7;  ///< 绝对误差的负对数（默认值对应 \f$2 \times 10^{-12}\f$）
    float64 RelativeTolerence = 15;    ///< 相对误差的负对数（默认值对应 \f$2^{-50} \approx 8.88 \times 10^{-16}\f$）
    float64 MaxIteration      = 2;     ///< 最大迭代次数的对数（默认值对应 \f$10^2=100\f$次迭代）

public:
    /**
     * @brief 构造函数
     * @param[in] Func   待求解的一元函数
     * @param[in] Domain 求解区间 [a,b]
     * @note 会自动确保 Domain[0] <= Domain[1]
     */
    BisectionRootFindingEngine(Function1D Func, vec2 Domain)
    {
        OriginalFunction = Func;
        First = min(Domain[0], Domain[1]);
        Last = max(Domain[0], Domain[1]);
    }

    /**
     * @brief 执行二分法求根运算
     * @param[in] x            反函数的参数
     * @param[out] IterCount   迭代次数计数器
     * @param[out] FCallCount  函数调用次数计数器
     * @return float64         求得的根近似值
     */
    float64 Run(float64 x = 0, uint64* IterCount = nullptr, uint64* FCallCount = nullptr)const;

    /**
     * @brief 重载函数调用运算符
     * @param[in] x 反函数的参数
     * @return float64 求得的根近似值
     * @see Run()
     */
    float64 operator()(float64 x) const override;
};

/**
 * @class HouseholderIteratorGroup
 * @brief 豪斯霍尔德迭代族，牛顿迭代和哈雷迭代均属于此类的子集，以美国数学家阿尔斯通·斯科特·豪斯霍尔德的名字命名。
 * 
 * 此迭代族的定义为：
 * \f[
 * x_{n+1} = x_n + d \cdot \frac{(1/f(x_n))^{(d-1)}}{(1/f(x_n))^{(d)}}
 * \f]
 * 其中\f$d\f$为迭代阶数，d阶迭代算法具有d+1阶的收敛性。
 *
 * 设\f$g(x) = 1/f(x)\f$，则上式可转化为求\f$g(x)\f$的n阶导函数问题。\f$g(x)\f$可以看作\f$\frac{1}{x}\f$和\f$f(x)\f$组成的复合函数，其导数可以套用Faa di Bruno求导公式计算，Faa di Bruno公式定义如下：
 * \f[
 * f(g(x))^{(n)}=\sum {\frac {n!}{\prod_{i=1}^{n} m_{i}!}}\cdot f^{(\sum_{k=1}^{n} m_{k})}(g(x))\cdot \prod _{j=1}^{n}\left({\frac {g^{(j)}(x)}{j!}}\right)^{m_{j}}
 * \f]
 * 其中\f$m_n\f$满足约束条件：\f[ \sum_{i=1}^n (i \cdot m_i) = n \f]
 * 
 * 由于上式中\f$ j > n - k + 1 \f$时\f$ m_j \f$必为0，此时再设\f$ k = \sum_{k=1}^{n} m_{k} \f$，得：
 * \f[
 * f(g(x))^{(n)}=\sum _{k=0}^{n}f^{(k)}(g(x))\cdot B_{n,k}\left(g'(x) \to g^{(n-k+1)}(x)\right)
 * \f]
 * 其中\f$B_{n,k}\f$为不完全贝尔多项式。
 *
 * \f$ \frac{1}{x} \f$的n阶导函数为\f$\frac{(-1^n \cdot n!)}{x^{n + 1}}\f$，将\f$g(x)\f$代入Faa di Bruno公式，得：
 * \f[
 * g^{(n)}(x) = \sum_{k=1}^n \frac{(-1)^k k!}{f(x)^{k+1}} \cdot B_{n,k}\left(f'(x) \to f^{(n-k+1)}(x)\right)
 * \f]
 *
 * 此公式即为\f$\frac{1}{f(x)}\f$的n阶导函数的公式。

 * 根据以上的公式，可得阶数为1时，得到的结果即为牛顿迭代，阶数为2时，得到的结果即为哈雷迭代。
 *
 * @note 
 * 「你越是努力盯着上面的公式，那些符号便越是在你眼前模糊成一道道虚影」<br>
 * 「别问为什么人在梦中还会犯困，这大概就是知识的力量吧。」<br>
 * 「云里看的不仅仅是花，还有......」<br>
 */
class HouseholderIteratorGroup : public IterativeRootFindingEngine
{
public:
    using Mybase = IterativeRootFindingEngine;  ///< 基类

protected:
    std::vector<Function1D> DerivativeFunctions;  ///< 存储函数的各阶导函数
    float64 AbsoluteTolerence = 7.83;            ///< 绝对误差的负对数，默认1.48e-8
    float64 RelativeTolerence = __Float64::FromBytes(POS_INF_DOUBLE);  ///< 相对误差的负对数，默认0
    float64 MaxIteration = 1.7;                   ///< 最大迭代次数的对数

public:
    /**
     * @brief 构造函数
     * @param[in] Functions 函数及其导函数，至少需要原函数和一阶导函数
     * @param[in] RefX 参考点
     * @throw std::logic_error 当输入函数不足时抛出异常
     */
    HouseholderIteratorGroup(std::vector<Function1D> Functions, float64 RefX)
    {
        if (Functions.size() <= 1)
        {
            throw std::logic_error(
                "Original function and its derivative is required.");
        }
        OriginalFunction = Functions[0];
        DerivativeFunctions.insert(
            DerivativeFunctions.begin(), Functions.begin() + 1, Functions.end());
        ReferencePoint = RefX;
    }

    /**
     * @brief 获取迭代阶数
     * @return 当前设置的迭代阶数
     */
    uint64 Order() const { return DerivativeFunctions.size(); }

    /**
     * @brief 执行迭代
     * @param[in] x 初始猜测值，默认为0
     * @param[out] IterCount 迭代次数指针，可为null
     * @param[out] FCallCount 函数调用次数指针，可为null
     * @return 求得的根近似值
     */
    float64 Run(float64 x = 0, uint64* IterCount = nullptr, uint64* FCallCount = nullptr) const;

    /**
     * @brief 重载函数调用运算符
     * @param[in] x 初始猜测值
     * @return 求得的根近似值
     */
    float64 operator()(float64 x) const override;

    /**
     * @brief 静态牛顿迭代法
     * @param[in] f 目标函数
     * @param[in] df 一阶导函数
     * @param[in] x 初始猜测值，默认为0
     * @param[out] IterCount 迭代次数指针，可为null
     * @param[out] FCallCount 函数调用次数指针，可为null
     * @param[in] MaxIter 最大迭代次数的对数
     * @param[in] AbsTol 绝对误差的负对数
     * @param[in] RelTol 相对误差的负对数
     * @return 求得的根近似值
     */
    static float64 Newton(Function1D f, Function1D df, float64 x = 0,
        uint64* IterCount = nullptr, uint64* FCallCount = nullptr, float64 MaxIter = 1.7,
        float64 AbsTol = 7.83, float64 RelTol = __Float64::FromBytes(POS_INF_DOUBLE));

    /**
     * @brief 静态哈雷迭代法
     * @param[in] f 目标函数
     * @param[in] df 一阶导函数
     * @param[in] d2f 二阶导函数
     * @param[in] x 初始猜测值，默认为0
     * @param[out] IterCount 迭代次数指针，可为null
     * @param[out] FCallCount 函数调用次数指针，可为null
     * @param[in] MaxIter 最大迭代次数的对数
     * @param[in] AbsTol 绝对误差的负对数
     * @param[in] RelTol 相对误差的负对数
     * @return 求得的根近似值
     */
    static float64 Halley(Function1D f, Function1D df, Function1D d2f, float64 x = 0,
        uint64* IterCount = nullptr, uint64* FCallCount = nullptr, float64 MaxIter = 1.7,
        float64 AbsTol = 7.83, float64 RelTol = __Float64::FromBytes(POS_INF_DOUBLE));
};

/**@}*/
    
} // SciCxx

} // CSE