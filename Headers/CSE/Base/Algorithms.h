/**
 * @file Algorithms.h
 * @brief CSpaceEngine 常用算法
 * @copyright Copyright (c) StellarDX Astronomy, 
 * license under GNU General Public License v3.0 or later
 *           @see https://www.gnu.org/licenses/
 */

namespace cse {

/**
 * @defgroup Algorithms 常用算法
 * @ingroup Base
 * @brief 一些GL函数
 * @{
 */

/**
 * @brief 计算输入值的绝对值
 * @param _Xx 输入值（浮点/整型/复数/浮点向量/整型向量）
 * @return 输入值的绝对值
 * @retval 浮点类型 返回 |x|
 * @retval 整型类型 返回 |x|
 * @retval 复数类型 返回复数的模 |a + bi| = sqrt(a² + b²)
 * @retval 向量类型 返回逐元素计算绝对值的向量
 */
_NODISCARD float64 abs(float64 _Xx) noexcept;

/// @see abs
_NODISCARD int64 abs(int64 _Xx) noexcept;

/// @see abs
_NODISCARD float64 abs(complex64 _Xx) noexcept;

/// @see abs
template<std::size_t N>
fvec<N> __cdecl abs(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE abs(_X[i]))
}

/// @see abs
template<std::size_t N>
ivec<N> __cdecl abs(ivec<N> _X)
{
    __stelcxx_array_math_function_body(i, i, _CSE abs(_X[i]))
}

/**
 * @brief 计算数值的符号
 * @param _Xx 输入值（浮点/整型/浮点向量/整型向量）
 * @return 符号标识
 * @retval -1 若 x < 0
 * @retval 0 若 x = 0
 * @retval +1 若 x > 0
 * @note 若输入为向量则返回逐元素符号标识的向量
 */
_NODISCARD float64 sgn(float64 _Xx) noexcept;

/// @see sgn
_NODISCARD int64 sgn(int64 _Xx) noexcept;

/// @see sgn
template<std::size_t N>
fvec<N> __cdecl sgn(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE sgn(_X[i]))
}

/// @see sgn
template<std::size_t N>
ivec<N> __cdecl sgn(ivec<N> _X)
{
    __stelcxx_array_math_function_body(i, i, _CSE sgn(_X[i]))
}

/**
 * @brief 向下取整函数
 * @param _Xx 输入值（浮点/浮点向量）
 * @return 不大于输入值的最大整数
 * @note 若输入为向量则返回逐元素向下取整的向量
 */
_NODISCARD float64 floor(float64 _Xx) noexcept;

/// @see floor
template<std::size_t N>
fvec<N> __cdecl floor(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE floor(_X[i]))
}

/**
 * @brief 向上取整函数
 * @param x 输入值（浮点/浮点向量）
 * @return 不小于输入值的最小整数
 * @note 若输入为向量则返回逐元素向上取整的向量
 */
_NODISCARD float64 ceil(float64 x) noexcept;

/// @see ceil
template<std::size_t N>
fvec<N> __cdecl ceil(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE ceil(_X[i]))
}

/**
 * @brief 获取数值的小数部分
 * @param _Xx 输入值（浮点/浮点向量）
 * @return 输入值的小数部分
 * @note 若输入为向量则返回各元素小数部分组成的向量
 */
_NODISCARD float64 FractionalPart(float64 _Xx) noexcept;

/// @see FractionalPart
template<std::size_t N>
fvec<N> __cdecl FractionalPart(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE FractionalPart(_X[i]))
}

/**
 * @brief 计算x - floor(x)的值
 * @param _Xx 输入值（浮点/浮点向量）
 * @return x - floor(x)的结果
 * @note 若输入为向量则返回各元素计算结果组成的向量
 * @attention 此函数与 @ref FractionalPart 行为不同，FractionalPart是严格提取小数部分
 */
_NODISCARD float64 fract(float64 _Xx) noexcept;

/// @see fract
template<std::size_t N>
fvec<N> __cdecl fract(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE fract(_X[i]))
}

/**
 * @brief 计算模运算结果
 * @param _Left 被模数（浮点/浮点向量）
 * @param _Right 模数（浮点/浮点向量）
 * @return x % y的结果
 * @note 若输入为向量则返回向量各元素对标量取模结果组成的向量
 */
_NODISCARD float64 mod(float64 _Left, float64 _Right) noexcept;

/// @see mod
template<std::size_t N>
fvec<N> __cdecl mod(fvec<N> _Left, float64 _Right)
{
    __stelcxx_array_math_function_body(f, i, _CSE mod(_Left[i], _Right))
}

/// @see mod
template<std::size_t N>
fvec<N> __cdecl mod(fvec<N> _Left, fvec<N> _Right)
{
    __stelcxx_array_math_function_body(f, i, _CSE mod(_Left[i], _Right[i]))
}

/**
 * @brief 返回参数或序列中的最小值
 * 
 * @details
 * 此函数提供多种重载形式：
 * - 基础类型比较（浮点/整型）
 * - 向量与标量逐元素比较
 * - 双向量逐元素比较
 * - 迭代器范围查找
 * - 初始化列表查找
 * 
 * @tparam N    向量维度（模板重载）
 * @tparam iterator  随机访问迭代器类型（范围重载）
 * @tparam genType  初始化列表元素类型（列表重载）
 * 
 * @param _Left  第一个比较值/向量
 * @param _Right 第二个比较值/标量
 * @param begin  起始迭代器（迭代器重载）
 * @param end    结束迭代器（迭代器重载）
 * @param _Ilist 初始化列表（列表重载）
 * 
 * @return 最小值
 * @retval 基础类型重载: 两者中最小值
 * @retval 向量重载: 逐元素最小值组成的向量
 * @retval 迭代器重载: 指向最小元素的迭代器（空范围返回end）
 * @retval 列表重载: 列表中的最小值
 * 
 * @note 
 * - 迭代器要求满足 random_access_iterator
 * 
 * @exception noexcept 所有重载均不抛出异常
 */
_NODISCARD constexpr float64 min(float64 _Left, float64 _Right) noexcept(noexcept(_Left < _Right));
/// @see min
_NODISCARD constexpr int64 min(int64 _Left, int64 _Right) noexcept(noexcept(_Left < _Right));
/// @see min
_NODISCARD constexpr uint64 min(uint64 _Left, uint64 _Right) noexcept(noexcept(_Left < _Right));

/// @see min
template<std::size_t N>
fvec<N> __cdecl min(fvec<N> _Left, float64 _Right)
{
    __stelcxx_array_math_function_body(f, i, _CSE min(_Left[i], _Right))
}

/// @see min
template<std::size_t N>
fvec<N> __cdecl min(fvec<N> _Left, fvec<N> _Right)
{
    __stelcxx_array_math_function_body(f, i, _CSE min(_Left[i], _Right[i]))
}

/// @see min
template<std::size_t N>
ivec<N> __cdecl min(ivec<N> _Left, int64 _Right)
{
    __stelcxx_array_math_function_body(i, i, _CSE min(_Left[i], _Right))
}

/// @see min
template<std::size_t N>
ivec<N> __cdecl min(ivec<N> _Left, ivec<N> _Right)
{
    __stelcxx_array_math_function_body(i, i, _CSE min(_Left[i], _Right[i]))
}

/// @see min
template<std::size_t N>
uvec<N> __cdecl min(uvec<N> _Left, uint64 _Right)
{
    __stelcxx_array_math_function_body(u, i, _CSE min(_Left[i], _Right))
}

/// @see min
template<std::size_t N>
uvec<N> __cdecl min(uvec<N> _Left, uvec<N> _Right)
{
    __stelcxx_array_math_function_body(u, i, _CSE min(_Left[i], _Right[i]))
}

/// @see min
template<typename iterator> requires std::random_access_iterator<iterator>
_NODISCARD constexpr iterator min(iterator begin, iterator end) noexcept
{
    if (begin >= end){return end;}
    auto It = begin;
    while (begin != end)
    {
        It = *begin < *It ? begin : It;
        ++begin;
    }
    return It;
}

/// @see min
template<typename genType>
_NODISCARD constexpr genType min(std::initializer_list<genType> _Ilist) noexcept
{
    return *(_CSE min(_Ilist.begin(), _Ilist.end()));
}

/**
 * @brief 返回参数或序列中的最大值
 * 
 * @details
 * 此函数提供多种重载形式：
 * - 基础类型比较（浮点/整型）
 * - 向量与标量逐元素比较
 * - 双向量逐元素比较
 * - 迭代器范围查找
 * - 初始化列表查找
 * 
 * @tparam N    向量维度（模板重载）
 * @tparam iterator  随机访问迭代器类型（范围重载）
 * @tparam genType  初始化列表元素类型（列表重载）
 * 
 * @param _Left  第一个比较值/向量
 * @param _Right 第二个比较值/标量
 * @param begin  起始迭代器（迭代器重载）
 * @param end    结束迭代器（迭代器重载）
 * @param _Ilist 初始化列表（列表重载）
 * 
 * @return 最大值
 * @retval 基础类型重载: 两者中最大值
 * @retval 向量重载: 逐元素最大值组成的向量
 * @retval 迭代器重载: 指向最大元素的迭代器（空范围返回end）
 * @retval 列表重载: 列表中的最大值
 * 
 * @note 
 * - 迭代器要求满足 random_access_iterator
 * 
 * @exception noexcept 所有重载均不抛出异常
 */
_NODISCARD constexpr float64 max(float64 _Left, float64 _Right) noexcept(noexcept(_Right < _Left));
/// @see max
_NODISCARD constexpr int64 max(int64 _Left, int64 _Right) noexcept(noexcept(_Right < _Left));
/// @see max
_NODISCARD constexpr uint64 max(uint64 _Left, uint64 _Right) noexcept(noexcept(_Right < _Left));

/// @see max
template<std::size_t N>
fvec<N> __cdecl max(fvec<N> _Left, float64 _Right)
{
    __stelcxx_array_math_function_body(f, i, _CSE max(_Left[i], _Right))
}

/// @see max
template<std::size_t N>
fvec<N> __cdecl max(fvec<N> _Left, fvec<N> _Right)
{
    __stelcxx_array_math_function_body(f, i, _CSE max(_Left[i], _Right[i]))
}

/// @see max
template<std::size_t N>
ivec<N> __cdecl max(ivec<N> _Left, int64 _Right)
{
    __stelcxx_array_math_function_body(i, i, _CSE max(_Left[i], _Right))
}

/// @see max
template<std::size_t N>
ivec<N> __cdecl max(ivec<N> _Left, ivec<N> _Right)
{
    __stelcxx_array_math_function_body(i, i, _CSE max(_Left[i], _Right[i]))
}

/// @see max
template<std::size_t N>
uvec<N> __cdecl max(uvec<N> _Left, uint64 _Right)
{
    __stelcxx_array_math_function_body(u, i, _CSE max(_Left[i], _Right))
}

/// @see max
template<std::size_t N>
uvec<N> __cdecl max(uvec<N> _Left, uvec<N> _Right)
{
    __stelcxx_array_math_function_body(u, i, _CSE max(_Left[i], _Right[i]))
}

/// @see max
template<typename iterator> requires std::random_access_iterator<iterator>
_NODISCARD constexpr iterator max(iterator begin, iterator end)noexcept
{
    if (begin >= end){return end;}

    auto It = begin;
    while (begin != end)
    {
        It = *begin > *It ? begin : It;
        ++begin;
    }

    return It;
}

/// @see max
template<typename genType>
_NODISCARD constexpr genType max(std::initializer_list<genType> _Ilist)noexcept
{
    return *(_CSE max(_Ilist.begin(), _Ilist.end()));
}

/**
 * @brief 将输入值约束在指定范围内
 * 
 * @details 将输入值x约束在[MinVal, MaxVal]范围内，返回约束后的值。
 * 若x小于MinVal则返回MinVal，若x大于MaxVal则返回MaxVal。
 *
 * @tparam N 向量维度
 *
 * @param x 输入值（浮点/整型/无符号整型/浮点向量/整型向量/无符号整型向量）
 * @param MinVal 范围下限（浮点/整型/无符号整型/浮点向量/整型向量/无符号整型向量）
 * @param MaxVal 范围上限（浮点/整型/无符号整型/浮点向量/整型向量/无符号整型向量）
 * @return 约束后的值，若输入向量则将各元素约束在指定范围内
 */
float64 clamp(float64 x, float64 MinVal, float64 MaxVal);
/// @see clamp
int64 clamp(int64 x, int64 MinVal, int64 MaxVal);
/// @see clamp
uint64 clamp(uint64 x, uint64 MinVal, uint64 MaxVal);

/// @see clamp
template<std::size_t N>
fvec<N> __cdecl clamp(fvec<N> x, float64 MinVal, float64 MaxVal)
{
    __stelcxx_array_math_function_body(f, i, _CSE clamp(x[i], MinVal, MaxVal));
}

/// @see clamp
template<std::size_t N>
fvec<N> __cdecl clamp(fvec<N> x, fvec<N> MinVal, fvec<N> MaxVal)
{
    __stelcxx_array_math_function_body(f, i, _CSE clamp(x[i], MinVal[i], MaxVal[i]));
}

/// @see clamp
template<std::size_t N>
ivec<N> __cdecl clamp(ivec<N> x, int64 MinVal, int64 MaxVal)
{
    __stelcxx_array_math_function_body(i, i, _CSE clamp(x[i], MinVal, MaxVal));
}

/// @see clamp
template<std::size_t N>
ivec<N> __cdecl clamp(ivec<N> x, ivec<N> MinVal, ivec<N> MaxVal)
{
    __stelcxx_array_math_function_body(i, i, _CSE clamp(x[i], MinVal[i], MaxVal[i]));
}

/// @see clamp
template<std::size_t N>
uvec<N> __cdecl clamp(uvec<N> x, uint64 MinVal, uint64 MaxVal)
{
    __stelcxx_array_math_function_body(u, i, _CSE clamp(x[i], MinVal, MaxVal));
}

/// @see clamp
template<std::size_t N>
uvec<N> __cdecl clamp(uvec<N> x, uvec<N> MinVal, uvec<N> MaxVal)
{
    __stelcxx_array_math_function_body(u, i, _CSE clamp(x[i], MinVal[i], MaxVal[i]));
}

/**
 * @brief 线性混合函数
 * 
 * 在x和y之间执行线性混合，使用a作为权重参数。
 * 根据参数类型不同，有以下行为：
 * - 当a为浮点时：返回x*(1-a) + y*a
 * - 当a为布尔时：返回a ? y : x
 * 
 * @tparam N 向量维度
 * @param x 混合范围的起始值（浮点/整型/无符号整型/布尔/浮点向量/整型向量/无符号整型向量/布尔向量）
 * @param y 混合范围的结束值（浮点/整型/无符号整型/布尔/浮点向量/整型向量/无符号整型向量/布尔向量）
 * @param a 用于在x和y之间混合的标量权重(0.0-1.0)（浮点/布尔）
 * @return 混合结果
 * 
 */
float64 mix(float64 x, float64 y, float64 a);

/// @see mix
float64 mix(float64 x, float64 y, bool a);

/// @see mix
int64 mix(int64 x, int64 y, bool a);

/// @see mix
uint64 mix(uint64 x, uint64 y, bool a);

/// @see mix
bool mix(bool x, bool y, bool a);

/// @see mix
template<std::size_t N>
fvec<N> __cdecl mix(fvec<N> x, fvec<N> y, float64 a)
{
    __stelcxx_array_math_function_body(f, i, _CSE mix(x[i], y[i], a));
}

/// @see mix
template<std::size_t N>
fvec<N> __cdecl mix(fvec<N> x, fvec<N> y, fvec<N> a)
{
    __stelcxx_array_math_function_body(f, i, _CSE mix(x[i], y[i], a[i]));
}

/// @see mix
template<std::size_t N>
fvec<N> __cdecl mix(fvec<N> x, fvec<N> y, bvec<N> a)
{
    __stelcxx_array_math_function_body(f, i, _CSE mix(x[i], y[i], a[i]));
}

/// @see mix
template<std::size_t N>
ivec<N> __cdecl mix(ivec<N> x, ivec<N> y, bvec<N> a)
{
    __stelcxx_array_math_function_body(i, i, _CSE mix(x[i], y[i], a[i]));
}

/// @see mix
template<std::size_t N>
uvec<N> __cdecl mix(uvec<N> x, uvec<N> y, bvec<N> a)
{
    __stelcxx_array_math_function_body(u, i, _CSE mix(x[i], y[i], a[i]));
}

/// @see mix
template<std::size_t N>
bvec<N> __cdecl mix(bvec<N> x, bvec<N> y, bvec<N> a)
{
    __stelcxx_array_math_function_body(b, i, _CSE mix(x[i], y[i], a[i]));
}

/**
 * @brief 生成阶跃函数（通过比较x和edge，若为向量则逐个元素比较）
 * @tparam N 向量维度
 * @param edge 指定阶跃函数的边缘位置（浮点/浮点向量）
 * @param x 用于生成阶跃函数的输入值（浮点/浮点向量）
 * @return 返回阶跃函数结果
 */
float64 step(float64 edge, float64 x);

/// @see step
template<std::size_t N>
fvec<N> __cdecl step(float64 edge, fvec<N> x)
{
    __stelcxx_array_math_function_body(f, i, _CSE step(edge, x[i]));
}

/// @see step
template<std::size_t N>
fvec<N> __cdecl step(fvec<N> edge, fvec<N> x)
{
    __stelcxx_array_math_function_body(f, i, _CSE step(edge[i], x[i]));
}

/**
 * @brief 执行0到1之间的平滑Hermite插值（当x在edge0和edge1之间时）
 * @see https://en.wikipedia.org/wiki/Smoothstep
 * @tparam N 向量维度
 * @param edge0 Hermite函数的下边缘值（浮点/浮点向量）
 * @param edge1 Hermite函数的上边缘值（浮点/浮点向量）
 * @param x 输入值（浮点/浮点向量）
 * @return 返回插值结果
 */
float64 smoothstep(float64 edge0, float64 edge1, float64 x);

/**
 * @brief GLSL原始版本的平滑插值函数
 * @see smoothstep
 */
float64 __Smoothstep_GL(float64 edge0, float64 edge1, float64 x);

/**
 * @brief AMD提供的平滑插值函数版本
 * @see smoothstep
 */
float64 __Smoothstep_AMD(float64 edge0, float64 edge1, float64 x);

/**
 * @brief Ken Perlin改进版本的平滑插值函数
 * @see smoothstep
 */
float64 __Smoothstep_KP(float64 edge0, float64 edge1, float64 x);

/**
 * @brief 平滑插值函数的反函数
 * @see smoothstep
 */
float64 inverse_smoothstep(float64 x);

/// @see smoothstep
template<std::size_t N>
fvec<N> __cdecl smoothstep(float64 edge0, float64 edge1, fvec<N> x)
{
    __stelcxx_array_math_function_body(f, i, _CSE smoothstep(edge0, edge1, x[i]));
}

/// @see smoothstep
template<std::size_t N>
fvec<N> __cdecl smoothstep(fvec<N> edge0, fvec<N> edge1, fvec<N> x)
{
    __stelcxx_array_math_function_body(f, i, _CSE smoothstep(edge0[i], edge1[i], x[i]));
}

// -------------------------------------------------------------------
// 无穷和NaN判断函数

/**
 * @brief 判断浮点数是否为无穷大
 * @tparam N 向量维度
 * @param[in] _X 待检测的浮点数
 * @return 返回1表示是无穷大，0表示不是
 * @exception 无异常抛出
 */
int64 isinf(float64 _X) throw();

/**
 * @brief 判断浮点数是否为NaN
 * @tparam N 向量维度
 * @param[in] _X 待检测的浮点数
 * @return 返回1表示是NaN，0表示不是
 * @exception 无异常抛出
 */
int64 isnan(float64 _X) throw();

/// @see isinf
template<std::size_t N>
ivec<N> __cdecl isinf(fvec<N> _X)
{
    __stelcxx_array_math_function_body(i, i, _CSE isinf(_X[i]))
}

/// @see isnan
template<std::size_t N>
ivec<N> __cdecl isnan(fvec<N> _X)
{
    __stelcxx_array_math_function_body(i, i, _CSE isnan(_X[i]))
}

/**@}*/

}