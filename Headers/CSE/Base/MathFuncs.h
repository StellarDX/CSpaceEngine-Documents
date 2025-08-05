/**
 * @file MathFuncs.h
 * @defgroup MathFuncs 数学函数
 * @ingroup Base
 * @brief CSpaceEngine数学函数库
 * @details 包含常用数学函数的声明，包括指数、对数、幂、三角函数等
 * 
 * @copyright 注：本文件及相关源文件中的数学函数来自多个数学库，如glibc（以LGPL2.1许可发布）、IBM Accurate Mathematical Library（以GPL2.0或LGPL2.1许可发布？），Cephes Mathematical Library（一个商业产品）（现在是开源的，但许可证仍不明确，其netlib官网上显示的是MIT许可证，包含该库的其他一些项目显示它是根据BSD许可证发布的。本数学库使用的相关函数函数为glibc的版本，使用LGPL2.1许可证发布。），或者其他一些开源项目。无论哪种情况，它们的版权都归作者所有。您在这里看到的内容或相关源文件可以自由使用，但不附带任何支持或保证。
 *
 * @note <i>「宇宙之大，粒子之微。火箭之速，化工之巧。星球之变，生命之谜。日用其繁，无不数学。」</i>
 */

/**
 * @defgroup MathFuncsExp 指数函数
 * @ingroup MathFuncs
 * @brief 指数函数，包含exp
 * @details
 * exp函数的实现来自<a href=https://www.gnu.org/software/libc/>glibc</a>
 *
 * **以下内容为AI智能解析，仅供参考**
 * 
 * # exp函数原理
 * 
 * ## 基本原理
 * 
 * 指数函数 `exp(x)` 的计算基于以下数学原理：
 * 
 * 1. **参数分解** ：将输入x分解为 `x = k*ln2/N + r`，其中k是整数，r是余项，N是预先选定的常数(这里是128=2^7)。
 * 
 * 数学表达式：
 * \f[
 * e^x = e^{k \cdot \frac{\ln 2}{N} + r} = 2^{k/N} \cdot e^r
 * \f]
 * 
 * 2. **查表法**：预先计算并存储 `2^{i/N}` 的值(i=0,...,N-1)，存储在`__Exp64f_table`中。
 * 
 * 3. **多项式近似**：对余项r使用泰勒级数展开近似计算`e^r`。
 * 
 * ## 详细步骤
 * 
 * ### 1. 输入处理
 * 
 * ```cpp
 * if (abstop - 0x3C9 >= 0x408 - 0x3C9) // 0x063
 * ```
 * 这部分处理特殊情况：
 * - 极小值直接返回1+x避免下溢
 * - 极大值(+inf/-inf)返回相应极限值
 * - 非规格化数特殊处理
 * 
 * ### 2. 参数分解
 * 
 * ```cpp
 * __Float64 InvLn2N = 1.4426950408889634073599246810019 * N; // N / ln2
 * z = InvLn2N * x;
 * ```
 * 将x分解为：
 * \f[
 * x = \frac{k}{N} \ln 2 + r
 * \f]
 * 其中k是整数，r是余项。
 * 
 * ### 3. 整数k提取
 * 
 * ```cpp
 * __Float64 Shift = 0x1.8p52;
 * kd = z + Shift;
 * ki = kd.Bytes;
 * kd -= Shift; // Round z to integer
 * ```
 * 将浮点数z四舍五入到最近的整数k。
 * 
 * ### 4. 余项r计算
 * 
 * ```cpp
 * __Float64 NegLn2N = -0.69314718055994530941723212145818 / N; // -ln2 / N
 * r = x + kd * NegLn2hiN + kd * NegLn2loN;
 * ```
 * 使用高精度计算余项r，避免精度损失。
 * 
 * ### 5. 查表获取2^(k/N)
 * 
 * ```cpp
 * idx = 2 * (ki % N);
 * top = ki << (52 - 7);
 * tail = __Float64::FromBytes(__Exp64f_table[idx]);
 * sbits = __Exp64f_table[idx + 1] + top;
 * ```
 * 查表获取2^(k/N)的近似值，分为尾数(tail)和指数部分(sbits)。
 * 
 * ### 6. 多项式近似计算e^r
 * 
 * ```cpp
 * tmp = tail + r * (C1 + r * (C2 + r * (C3 + r * (C4 + r * C5))));
 * ```
 * 使用5阶泰勒多项式近似计算e^r：
 * \f[
 * e^r \approx 1 + r + \frac{r^2}{2!} + \frac{r^3}{3!} + \frac{r^4}{4!} + \frac{r^5}{5!}
 * \f]
 * 
 * ### 7. 结果组合
 * 
 * 最终结果组合为：
 * \f[
 * e^x = 2^{k/N} \times e^r \approx \text{scale} \times (1 + \text{tmp})
 * \f]
 * 
 * ## 数学公式总结
 * 
 * 1. 参数分解：
 * \f[
 * x = k \cdot \frac{\ln 2}{N} + r
 * \f]
 * 
 * 2. 指数函数分解：
 * \f[
 * e^x = 2^{k/N} \cdot e^r
 * \f]
 * 
 * 3. 泰勒展开：
 * \f[
 * e^r \approx \sum_{n=0}^{5} \frac{r^n}{n!} = 1 + r + \frac{r^2}{2} + \frac{r^3}{6} + \frac{r^4}{24} + \frac{r^5}{120}
 * \f]
 * 
 * 4. 最终结果：
 * \f[
 * e^x \approx \text{scale} \times (1 + \text{tmp})
 * \f]
 * 
 * ## 实现技巧
 * 
 * 1. **高精度计算** ：使用分离的高低位计算余项r，减少舍入误差
 * 2. **查表优化** ：预先计算并存储2^(i/N)的值，加速计算
 * 3. **特殊处理** ：对极大/极小值进行特殊处理，保证数值稳定性
 * 4. **多项式近似** ：使用5阶泰勒多项式平衡精度和性能
 * 
 * 这个实现综合运用了数值分析中的多种技巧，在保证精度的同时实现了高性能的指数函数计算。
 *
 * @{
 */

/**
 * @brief 计算自然指数函数（e^x）
 * @param _X 指数值
 * @return e 的 _X 次幂
 */
float64 __cdecl exp(float64 _X);

/**
 * @brief 计算复数的自然指数函数（e^z）
 * @param _X 复数指数值
 * @return e 的 _X 次幂
 */
complex64 __cdecl expc(complex64 _X);

/**
 * @brief 计算浮点向量每个元素的自然指数
 * @tparam N 向量维度
 * @param _X 输入向量
 * @return 各元素指数结果向量
 */
template<std::size_t N>
fvec<N> __cdecl exp(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE exp(_X[i]))
}

/**@}*/

/**
 * @defgroup MathFuncsLog 对数函数
 * @ingroup MathFuncs
 * @brief 对数函数分为lb，ln，log三种，其中lb是以2为底的对数，ln是以e为底的对数，log是自定义底数(默认10)。它们的实现如下：（lb函数已弃用）
 * @details
 * ln和log函数的实现均来自<a href=https://www.gnu.org/software/libc/>glibc</a>，由Stephen L. Moshier所作。
 * 
 * **以下内容为AI智能解析，仅供参考**
 *
 * # ln函数的实现原理
 * 
 * ## 核心思想
 * 
 * 该实现基于以下数学原理：
 * 
 * 1. **对数分解** ：`ln(x) = ln(t) + ln(x/t)`，其中 `t` 是一个接近 `x` 的预定义值
 * 2. **查表法** ：预先计算 `ln(t) - (t-1)` 的值存储在表中
 * 3. **泰勒展开** ：对于 `ln(1+z)` 使用泰勒级数展开，其中 `z = (x/t - 1)`
 * 
 * ## 关键公式
 * 
 * 1. 对数分解公式：
 * \f[
 * \ln(x) = \ln(t) + \ln\left(\frac{x}{t}\right) = \ln(t) + \ln(1 + z) \quad \text{其中} \quad z = \frac{x}{t} - 1
 * \f]
 * 至于为何这里要“强行”凑一个 `1+x` ，是因为 `ln(x)` 在 `x=0` 处的值为 `-inf` ，收敛半径是0，所以直接展开个🔨。因此我们经常见的形式是 `ln(x+1)` 在 `x=0` 处的泰勒展开，这个就相当于 `ln(x)` 在 `x=1` 处的泰勒展开， `ln(x+1)` 的收敛半径是1展开才有意义。
 * 
 * 2. 泰勒级数展开（对于小的 `z`）：
 * \f[
 * \ln(1+z) = z - \frac{z^2}{2} + \frac{z^3}{3} - \frac{z^4}{4} + \cdots
 * \f]
 * 
 * 3. 最终组合公式：
 * \f[
 * \ln(x) = (t-1) + [\ln(t) - (t-1)] + z - \frac{z^2}{2} + \cdots + e \cdot \ln(2)
 * \f]
 * 其中 `e` 是指数部分。
 * 
 * ## 实现细节
 * 
 * ### 1. 查表设计
 * 
 * 查表 `__Ln128f_table` 存储的是 `ln(t) - (t-1)` 的值，其中：
 * \f[
 * t = 0.5 + \frac{k+26}{128}, \quad k = 0, \ldots, 91
 * \f]
 * 
 * 这样设计的目的是：
 * - 覆盖区间 `[0.703125, 1.40625]`（约 `0.5+26/128` 到 `0.5+117/128`）
 * - 提供足够高的精度（表中使用长双精度）
 * 
 * ### 2. 参数分解
 * 
 * 函数首先处理输入 `x`：
 * 1. 检查特殊值（0、负数、无穷大等）
 * 2. 分解 `x` 为指数部分和尾数部分
 * 3. 对于接近1的值（`0.9921875 < x < 1.0078125`），直接使用泰勒展开
 * 4. 其他情况通过查表找到最接近的 `t` 值
 * 
 * ### 3. 泰勒级数展开
 * 
 * 对于小的 `z`，使用15阶泰勒展开计算 `ln(1+z)`：
 * \f[
 * \ln(1+z) \approx z - \frac{z^2}{2} + \frac{z^3}{3} - \cdots + \frac{z^{15}}{15}
 * \f]
 * 
 * ### 4. 结果组合
 * 
 * 最终结果由多个部分组成：
 * 1. `(t-1)`：线性近似部分
 * 2. `__Ln128f_table[k-26]`：`ln(t)-(t-1)` 的高精度修正
 * 3. 泰勒展开部分：`ln(1+z)` 的计算结果
 * 4. 指数部分：`e * ln(2)`（分为高精度和低精度两部分 `ln2a` 和 `ln2b`）
 * 
 * ## 数学推导
 * 
 * 完整的计算过程可以表示为：
 * \f[
 * \ln(x) = \ln(t \cdot \frac{x}{t}) = \ln(t) + \ln\left(1 + \frac{x-t}{t}\right)
 * \f]
 * 
 * 将 `ln(t)` 表示为：
 * \f[
 * \ln(t) = (t-1) + [\ln(t) - (t-1)]
 * \f]
 * 
 * 然后使用泰勒展开 `ln(1+z)`：
 * \f[
 * \ln(1+z) \approx z - \frac{z^2}{2} + \frac{z^3}{3} - \cdots
 * \f]
 * 
 * 最终组合所有部分：
 * \f[
 * \ln(x) \approx (t-1) + [\ln(t)-(t-1)] + \left(z - \frac{z^2}{2} + \cdots\right) + e \cdot \ln(2)
 * \f]
 * 
 * ## 精度考虑
 * 
 * 1. **表项设计** ：存储 `ln(t)-(t-1)` 而不是直接存储 `ln(t)` 是为了减少数值计算时的舍入误差
 * 2. **泰勒展开** ：使用高阶展开（15项）确保小范围内的精度
 * 3. **双精度处理** ：`ln(2)` 分为高精度和低精度两部分存储，减少舍入误差
 * 
 * ## 适用范围
 * 
 * 该实现适用于所有正有限浮点数，特别优化了：
 * 1. 接近1的输入值（直接泰勒展开）
 * 2. 其他值通过查表+泰勒展开的组合方法
 * 
 * 这种设计在保证精度的同时，通过查表减少了需要计算的泰勒级数项数，提高了计算效率。
 * 
 * ---
 * 
 * **以下内容为AI智能解析，仅供参考**
 *
 * # log函数的实现原理
 * 
 * ## 1. 输入处理和特殊情况检查
 * 
 * ```cpp
 * hx = x.Bytes;
 * if ((hx & 0x7FFFFFFFFFFFFFFF) == 0) { return __Float64::FromBytes(NEG_INF_DOUBLE); }
 * if (hx >= 0x7FF0000000000000) { return __Float64::FromBytes(POS_INF_DOUBLE); }
 * if (hx < 0) { return __Float64::FromBytes(BIG_NAN_DOUBLE); }
 * if (x == 1.0L) { return 0.0L; }
 * ```
 * 
 * 这部分处理特殊情况：
 * - 输入为0时返回负无穷（-∞）
 * - 输入为正无穷（+∞）时返回正无穷
 * - 输入为负数时返回NaN（非数字）
 * - 输入为1时直接返回0（因为ln(1)=0）
 * 
 * ## 2. 分解浮点数
 * 
 * ```cpp
 * int e;
 * x = frexp(x, &e);
 * ```
 * 
 * 使用`frexp`函数将浮点数分解为尾数x（范围[0.5, 1)）和指数e：
 * 
 * \f[ x = m \times 2^e \f]
 * 
 * 其中 \f$ 0.5 \leq m < 1 \f$
 * 
 * ## 3. 主要计算逻辑
 * 
 * 函数根据指数e的大小分为两种计算路径：
 * 
 * ### 3.1 当 |e| > 2 时（大范围情况）
 * 
 * ```cpp
 * if ((e > 2) || (e < -2))
 * ```
 * 
 * #### 3.1.1 调整x到接近1的范围
 * 
 * 根据x的值调整到最优区间：
 * - 如果x < √0.5 ≈ 0.7071，调整e和x：
 * ```cpp
 * e -= 1;
 * z = x - 0.5L;
 * y = 0.5L * z + 0.5L;
 * ```
 * - 否则：
 * ```cpp
 * z = x - 0.5L;
 * z -= 0.5L;
 * y = 0.5L * x + 0.5L;
 * ```
 * 
 * #### 3.1.2 有理函数近似
 * 
 * 计算：
 * \f[ \log(x) \approx x \cdot \frac{P(z)}{Q(z)} \f]
 * 
 * 其中：
 * - P(z)和Q(z)是预先计算的多项式系数
 * - 使用`neval`和`deval`函数高效计算多项式值
 * 
 * #### 3.1.3 合并结果
 * 
 * 将多项式近似结果与指数部分合并：
 * ```cpp
 * z = y * L10EB;
 * z += x * L10EB;
 * z += e * L102B;
 * z += y * L10EA;
 * z += x * L10EA;
 * z += e * L102A;
 * ```
 * 
 * 这实际上是计算：
 * \f[ \log(x) = (y + x) \cdot \text{L10EA} + e \cdot \text{L102A} + \text{高阶修正项} \f]
 * 
 * 其中：
 * - L102A和L102B是与log(2)相关的常数
 * - L10EA和L10EB是与log(e)相关的常数
 * 
 * ### 3.2 当 |e| ≤ 2 时（小范围情况）
 * 
 * #### 3.2.1 调整x到接近1的范围
 * 
 * ```cpp
 * if (x < SQRTH)
 * {
 *     e -= 1;
 *     x = 2.0 * x - 1.0L;
 * } 
 * else 
 * {
 *     x = x - 1.0L;
 * }
 * ```
 * 
 * #### 3.2.2 有理函数近似
 * 
 * 计算：
 * \f[ \log(1 + x) \approx x - \frac{x^2}{2} + x \cdot \frac{P(x)}{Q(x)} \f]
 * 
 * 其中P(x)和Q(x)是不同的多项式系数集合。
 * 
 * #### 3.2.3 合并结果
 * 
 * 与3.1.3类似的方式合并结果。
 * 
 * ## 4. 数学原理
 * 
 * 该实现基于以下数学原理：
 * 
 * 1. **浮点数分解** ：任何正浮点数x可以表示为：
 * \f[ x = m \times 2^e \f]
 * 其中 \f$ 0.5 \leq m < 1 \f$
 * 
 * 2. **对数性质** ：
 * \f[ \log(x) = \log(m) + e \cdot \log(2) \f]
 * 
 * 3. **有理函数近似** ：对于接近1的值，使用多项式有理近似来计算log(m)或log(1+x)。
 * 
 * 4. **范围缩减** ：通过调整区间，确保近似在最优范围内进行，提高精度。
 * 
 * ## 5. 关键常数
 * 
 * 代码中使用了几个重要常数：
 * 
 * - `L102A` 和 `L102B`：log(2)的高精度表示，拆分为高位和低位部分
 * \f[ \log(2) \approx 0.3125 - 1.14700043360188047862611052755069732318101185 \times 10^{-2} \f]
 * 
 * - `L10EA` 和 `L10EB`：log(e)的高精度表示
 * \f[ \log(e) \approx 0.5 - 6.570551809674817234887108108339491770560299 \times 10^{-2} \f]
 * 
 * - `SQRTH`：sqrt(0.5) ≈ 0.7071，用于区间判断
 * 
 * ## 6. 多项式求值优化
 * 
 * 使用`neval`和`deval`函数高效计算多项式值，采用秦九韶写法：
 * 
 * 对于多项式：
 * \f[ P(x) = p_0 + p_1 x + p_2 x^2 + \cdots + p_n x^n \f]
 * 
 * 秦九韶写法计算：
 * \f[ P(x) = p_0 + x(p_1 + x(p_2 + \cdots + x(p_{n-1} + x p_n)\cdots)) \f]
 * 
 * 这种方法减少了乘法和加法次数，提高了数值稳定性。
 * 
 * ## 总结
 * 
 * 这个对数函数实现结合了：
 * 1. 范围缩减技术
 * 2. 高精度有理函数近似
 * 3. 多项式求值优化
 * 4. 特殊值处理
 * 
 * 通过将输入分解为尾数和指数，并在最优区间内进行多项式近似，实现了高精度的对数计算。
 * 
 * @{
 */

/**
 * @brief 计算对数函数
 * @param _X 真数值（必须大于0）
 * @param _Base 底数（必须大于0且不等于1）
 * @return 满足 _Base^y = _X 的 y 值
 */
float64 __cdecl log(float64 _X, float64 _Base);

/**
 * @brief [已废弃] 二进制对数
 * @deprecated 此功能已废弃
 */
float64 __cdecl lb(float64 _X);

/**
 * @brief 计算自然对数（ln(x)）
 * @param _X 真数值（必须大于0）
 * @return 满足 e^y = _X 的 y 值
 */
float64 __cdecl ln(float64 _X);

/**
 * @brief 计算常用对数（log10(x)）
 * @param _X 真数值（必须大于0）
 * @return 满足 10^y = _X 的 y 值
 */
float64 __cdecl log(float64 _X);

/**
 * @brief [已废弃] 复数的二进制对数
 * @deprecated 此功能已废弃
 */
complex64 __cdecl lbc(complex64 _X, int64 _K = 0);

/**
 * @brief 计算复数的自然对数（ln(z)）
 * @param _X 复数真数值
 * @param _K 分支选择偏移量（默认为0）
 * @return 复数的自然对数
 */
complex64 __cdecl lnc(complex64 _X, int64 _K = 0);

/**
 * @brief 计算复数的常用对数（log10(z)）
 * @param _X 复数真数值
 * @param _K 分支选择偏移量（默认为0）
 * @return 复数的常用对数
 */
complex64 __cdecl logc(complex64 _X, int64 _K = 0);

/**
 * @brief 计算复数的任意底数对数
 * @param _X 复数真数值
 * @param _Base 复数底数值
 * @param _K 分支选择偏移量（默认为0）
 * @return 复数的任意底数对数
 */
complex64 __cdecl logc(complex64 _X, complex64 _Base, int64 _K = 0);

// 向量版本的重载函数使用统一注释
/**
 * @brief 计算向量各元素的自然对数
 * @tparam N 向量维度
 * @param _X 输入向量
 * @return 各元素自然对数结果向量
 */
template<std::size_t N>
fvec<N> __cdecl ln(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE ln(_X[i]))
}

/**
 * @brief 计算向量各元素的常用对数
 * @tparam N 向量维度
 * @param _X 输入向量
 * @return 各元素常用对数结果向量
 */
template<std::size_t N>
fvec<N> __cdecl log(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE log(_X[i]))
}

/**
 * @brief 计算向量各元素的任意底数对数（标量底数）
 * @tparam N 向量维度
 * @param _X 输入向量
 * @param _Base 对数底数（标量）
 * @return 各元素对数结果向量
 */
template<std::size_t N>
fvec<N> __cdecl log(fvec<N> _X, float64 _Base)
{
    __stelcxx_array_math_function_body(f, i, _CSE log(_X[i], _Base))
}

/**
 * @brief 计算向量各元素的任意底数对数（向量底数）
 * @tparam N 向量维度
 * @param _X 真数值向量
 * @param _Base 底数向量
 * @return 各元素对数结果向量
 */
template<std::size_t N>
fvec<N> __cdecl log(fvec<N> _X, fvec<N> _Base)
{
    __stelcxx_array_math_function_body(f, i, _CSE log(_X[i], _Base[i]))
}

/**@}*/

/**
 * @defgroup MathFuncsPow 幂函数
 * @ingroup MathFuncs
 * @brief 幂函数包含pow，sqrt，cbrt和yroot四个函数
 * @details
 * pow，cbrt函数的实现来自<a href=https://www.gnu.org/software/libc/>glibc</a>，
 * sqrt函数的实现来自<a href=https://github.com/dreal-deps/mathlib>IBM Accurate Mathematical Library</a>
 *
 * **以下内容为AI智能解析，仅供参考**
 * 
 * # pow 函数原理
 * 
 * ## 1. 基本思路
 * 
 * `pow(x,y)` 函数的核心数学原理是基于对数恒等式：
 * 
 * \f[ x^y = e^{y \cdot \ln(x)} \f]
 * 
 * 因此，实现 `pow` 函数的关键在于：
 * 1. 精确计算自然对数 \f$\ln(x)\f$
 * 2. 精确计算指数函数 \f$e^{y \cdot \ln(x)}\f$
 * 
 * ## 2. 对数计算 (ln_inline)
 * 
 * 对数计算部分使用了查表法和泰勒级数展开相结合的方法：
 * 
 * ### 2.1 数学原理
 * 
 * 对于任意正数 \f$x\f$，可以表示为：
 * \f[ x = 2^k \cdot z \f]
 * 其中 \f$z \in [0x1.69555p-1, 0x1.69555p0)\f$，然后：
 * 
 * \f[ \ln(x) = k \cdot \ln2 + \ln(c) + \ln\left(\frac{z}{c}\right) \f]
 * 
 * 其中 \f$c\f$ 是预先计算的表中值，\f$\ln(z/c)\f$ 用泰勒级数展开计算。
 * 
 * ### 2.2 实现细节
 * 
 * 1. **范围缩减** ：将输入 \f$x\f$ 分解为 \f$2^k \cdot z\f$ 的形式
 * 2. **查表** ：使用预先计算的 `__Pow64f_ln_table` 表获取 \f$1/c\f$, \f$\ln(c)\f$ 和 \f$\ln(c)\f$ 的尾部
 * 3. **泰勒展开** ：计算 \f$r = z/c - 1\f$，然后用多项式近似 \f$\ln(1+r)\f$
 * 
 * 泰勒级数展开使用以下多项式：
 * \f[ \ln(1+r) \approx r - \frac{r^2}{2} + \frac{r^3}{3} - \frac{r^4}{4} + \frac{r^5}{5} - \frac{r^6}{6} + \frac{r^7}{7} \f]
 * 
 * ## 3. 指数计算 (exp_inline)
 * 
 * 指数计算部分也使用了查表法和泰勒级数展开：
 * 
 * ### 3.1 数学原理
 * 
 * \f[ e^x = 2^{k/N} \cdot e^r \f]
 * 其中 \f$N = 128\f$ (因为表有128项)，\f$x = \frac{\ln2}{N}k + r\f$
 * 
 * ### 3.2 实现细节
 * 
 * 1. **范围缩减** ：将输入 \f$x\f$ 分解为 \f$k \cdot \frac{\ln2}{N} + r\f$ 的形式
 * 2. **查表** ：使用 `__Exp64f_table` 获取 \f$2^{k/N}\f$ 的近似值和尾部
 * 3. **泰勒展开** ：计算 \f$e^r\f$ 的近似值
 * 
 * 泰勒级数展开使用以下多项式：
 * \f[ e^r \approx 1 + r + \frac{r^2}{2!} + \frac{r^3}{3!} + \frac{r^4}{4!} + \frac{r^5}{5!} \f]
 * 
 * ## 4. 特殊处理
 * 
 * 代码中还包含大量特殊情况的处理：
 * 
 * 1. **异常输入** ：处理 \f$x\f$ 或 \f$y\f$ 为 NaN、Inf、0 等情况
 * 2. **负x处理** ：当 \f$x\f$ 为负数时，检查 \f$y\f$ 是否为整数
 * 3. **下溢/上溢** ：处理结果过小或过大的情况
 * 4. **精度优化** ：使用双精度算法减少舍入误差
 * 
 * ## 5. 精度保证
 * 
 * 实现中采用了多种技术来保证高精度：
 * 
 * 1. **误差补偿** ：计算对数时分离高位和低位部分
 * 2. **精确的表格值** ：预先计算的表格包含高精度值
 * 3. **双精度算法** ：在某些关键步骤使用更高精度的计算
 * 
 * ## 6. 性能优化
 * 
 * 1. **查表法** ：减少复杂计算的需要
 * 2. **流水线友好** ：计算过程设计为适合现代CPU的流水线执行
 * 3. **分支预测** ：特殊情况的检查放在前面
 * 
 * 这个实现综合运用了数值分析中的多种技术，在保证精度的同时追求高性能，是数学函数库中典型的优化实现。
 *
 * @subsubsection 一些碎碎念
 * <b>丹霞</b>：早期版本的glibc中数学函数使用的是IBM设计的函数，其中一些函数在输入特定值的时候会产生高到离谱的延迟，一个典型的例子就是这个pow函数。根据网络上的一些反馈，此函数在输入(x -> 1, 1.5)的时候就会出现一定的延迟，且当x越接近1延迟越高，而输入x = 1.000000000000002时，延迟高达~777毫秒，这显然是无法接受的。另外据说IBM设计的exp函数在输入[-0.00000000033301145180213807, -0.00000000033301145180369239]以内的值时也会产生比正常情况高上百倍的延迟。所以目前版本的glibc除平方根和弧度三角函数外已经换用了自行设计的数学函数。<br>
 * 
 * 附：IBM数学函数库中的upow函数在最坏情况下（输入0.9999999999999940，0.4166666666666666）时可能的调用堆栈（自行推演，并非实际情况）：
 * ```
 * #0 upow(x=0x3FEFFFFFFFFFFFCA, y=0x3FDAAAAAAAAAAAA9) at upow.c:55
 * #1 power1(x=0x3FEFFFFFFFFFFFCA, y=0x3FDAAAAAAAAAAAA9) at upow.cc:136
 * #2 slowpow(x=0x3FEFFFFFFFFFFFCA, y=0x3FDAAAAAAAAAAAA9, z=lgg2(x, &aa, &error)) at slowpow.c:43
 * #3... 很多乱七八糟的函数，勉强看出来其中ln(x)的计算用的牛顿迭代，exp(x)属实没看出来是什么情况，
 * 总之所有这些函数基本都会产生一点延迟，而每次计算都会调用非常多次，结果就越堆越高，最后总体延迟就堆的特别高了。
 * ```
 *
 * 参考：
 * 1. 菲利普·马尔基诺写给glibc客服的信：https://sourceware.org/legacy-ml/libc-help/2009-01/msg00003.html
 * 2. 莱昂内尔·柴兰与Debian客服奥雷利安·雅诺的对话：https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=585430
 * 3. 阿什瓦尼·巴特与安德鲁·哈里的对话：https://gcc.gnu.org/legacy-ml/gcc-help/2006-05/msg00185.html，https://gcc.gnu.org/legacy-ml/gcc-help/2006-05/msg00189.html
 * 4. 杰森·萨默斯的测试与看法：https://entropymine.com/imageworsener/slowpow/
 * 5. IBM当年独领风骚的数学函数库代码(大奋警告！！！)：https://github.com/dreal-deps/mathlib （IBM：屮艸芔茻！）
 *
 * -----
 * **以下内容为AI智能解析，仅供参考**
 * 
 * # sqrt函数原理
 * 
 * ## 1. 核心算法概述
 * 
 * 该函数采用了 **查表法结合泰勒级数展开** 的方法来计算平方根，主要步骤如下：
 * 
 * 1. 通过查表获得初始近似值
 * 2. 使用泰勒级数展开进行精化
 * 3. 执行牛顿迭代进行最终修正
 * 4. 处理边界情况和特殊输入
 * 
 * ## 2. 数学原理
 * 
 * ### 2.1 平方根计算的基本公式
 * 
 * 计算 sqrt(x) 可以转化为计算 x 的平方根的倒数再乘以 x：
 * 
 * \f[ \sqrt{x} = x \times \frac{1}{\sqrt{x}} \f]
 * 
 * 但实际实现中使用了更高效的方法。
 * 
 * ### 2.2 泰勒级数展开
 * 
 * 函数中使用了反平方根(1/sqrt(1+x))的泰勒级数展开：
 * 
 * \f[ \frac{1}{\sqrt{1+x}} = 1 - \frac{1}{2}x + \frac{3}{8}x^2 - \frac{5}{16}x^3 + \frac{35}{128}x^4 - \frac{63}{256}x^5 + \cdots \f]
 * 
 * 代码中使用了前四项：
 * 
 * ```c
 * static const float64
 *     rt0 = 1.0000,
 *     rt1 = 0.5000,
 *     rt2 = 0.3750,
 *     rt3 = 0.3125;
 * ```
 * 
 * ### 2.3 查表法初始近似
 * 
 * 函数使用了一个预先计算的 128 项查找表 `__IBM_inroot_table`，表中存储了各种区间内的反平方根初始近似值。
 * 
 * ## 3. 实现步骤详解
 * 
 * ### 3.1 输入处理
 * 
 * ```c
 * __Float64 x = _X;
 * a = x;
 * k = a.parts.msw;  // 获取x的高32位(包含指数和部分尾数)
 * a.parts.msw = (k & 0x001fffff) | 0x3fe00000;  // 将指数部分设为1022(0x3fe)
 * ```
 * 
 * 这一步将输入x的指数部分归一化到接近1的范围，便于后续计算。
 * 
 * ### 3.2 查表获取初始近似值
 * 
 * ```c
 * t = __IBM_inroot_table[(k & 0x001fffff) >> 14];
 * ```
 * 
 * 使用x的尾数部分的高7位作为索引，从表中获取反平方根的初始近似值t。
 * 
 * ### 3.3 泰勒级数精化
 * 
 * ```c
 * y = 1.0 - t * (t * s);
 * t = t * (rt0 + y * (rt1 + y * (rt2 + y * rt3)));
 * ```
 * 
 * 这里计算了 `y = 1 - t²s`，然后使用泰勒级数对t进行精化。
 * 
 * ### 3.4 指数调整
 * 
 * ```c
 * c.parts.msw = 0x20000000 + ((k & 0x7fe00000) >> 1);
 * ```
 * 
 * 这一步调整结果的指数部分，因为平方根的指数是原数指数的一半。
 * 
 * ### 3.5 高精度计算
 * 
 * ```c
 * y = t * s;
 * hy = (y + big) - big;  // 获取y的高精度部分
 * del = 0.5 * t * ((s - hy * hy) - (y - hy) * (y + hy));  // 修正项
 * res = y + del;
 * ```
 * 
 * 这里使用了 Dekker 算法的高精度技巧，hy 是 y 的高精度部分，del 是修正项。
 * 
 * ### 3.6 结果验证和调整
 * 
 * ```c
 * if (res == (res + 1.002 * ((y - res) + del)))
 * {
 *     ret = res * c;
 * } 
 * else 
 * {
 *     // 更精确的计算
 *     res1 = res + 1.5 * ((y - res) + del);
 *     EMULV(res, res1, z, zz);  // 高精度乘法
 *     res = ((((z - s) + zz) < 0) ? max(res, res1) : min(res, res1));
 *     ret = res * c;
 * }
 * ```
 * 
 * 这里检查是否需要更精确的计算，如果需要则使用高精度乘法进行修正。
 * 
 * ### 3.7 特殊情况处理
 * 
 * ```c
 * // 处理NaN, Inf, 负数等情况
 * if ((k & 0x7ff00000) == 0x7ff00000) return x * x + x;
 * if (x == 0) return x;
 * if (k < 0) return __Float64::FromBytes(S_NAN_DOUBLE);
 * // 处理次正规数
 * return 0x1p-256 * __IBM_SQRT64F(x * 0x1p512);
 * ```
 * 
 * ## 4. 关键数学公式总结
 * 
 * 1. 泰勒级数展开：
 * \f[ \frac{1}{\sqrt{1+x}} \approx 1 - \frac{1}{2}x + \frac{3}{8}x^2 - \frac{5}{16}x^3 \f]
 * 
 * 2. 高精度修正项：
 * \f[ \text{del} = \frac{1}{2}t \times \left[ (s - h_y^2) - (y - h_y)(y + h_y) \right] \f]
 * 
 * 3. 最终结果：
 * \f[ \text{result} = (y + \text{del}) \times 2^{\lfloor e/2 \rfloor} \f]
 * 其中e是原始数的指数部分。
 * 
 * 这个实现结合了多种数值计算技术，包括查表法、多项式近似和高精度算法，能够在各种输入情况下提供正确舍入的平方根结果。
 *
 * ---
 * 
 * **以下内容为AI智能解析，仅供参考**
 *
 * # cbrt 函数原理
 * 
 * ## 1. 预处理阶段
 * 
 * ```c
 * if (!std::isfinite(x)) {return x + x;}
 * if (x == 0) {return x;}
 * ```
 * 
 * 首先处理特殊值：
 * - 非有限数（NaN或无穷大）直接返回原值
 * - 0的立方根还是0
 * 
 * ## 2. 符号处理
 * 
 * ```c
 * if (x > 0) { sign = 1; }
 * else { sign = -1; x = -x; }
 * ```
 * 
 * 保存原始数的符号，并转为正数处理，最后再恢复符号。
 * 
 * ## 3. 规范化处理
 * 
 * ```c
 * z = x;
 * x = ::frexp(x, &e);
 * ```
 * 
 * 使用`frexp`函数将浮点数分解为尾数x∈[0.5,1)和指数e：
 * \f[ x = m \times 2^e \f]
 * 
 * ## 4. 多项式近似计算
 * 
 * ```c
 * x = ((((1.3584464340920900529734e-1 * x
 *         - 6.3986917220457538402318e-1) * x
 *     + 1.2875551670318751538055e0) * x
 *     - 1.4897083391357284957891e0) * x
 *     + 1.3304961236013647092521e0) * x
 *     + 3.7568280825958912391243e-1;
 * ```
 * 
 * 这是一个5次多项式近似，用于计算[0.5,1)区间内的立方根，其形式为：
 * \f[ P(x) = ((((a_5 \cdot x + a_4) \cdot x + a_3) \cdot x + a_2) \cdot x + a_1) \cdot x + a_0 \f]
 * 
 * ## 5. 指数调整
 * 
 * 立方根的指数部分需要除以3：
 * 
 * ```c
 * if (e >= 0) 
 * {
 *     rem = e;
 *     e /= 3;
 *     rem -= 3 * e;
 *     if (rem == 1) {x *= CBRT2;}
 *     else if (rem == 2) {x *= CBRT4;}
 * }
 * else 
 * {
 *     e = -e;
 *     rem = e;
 *     e /= 3;
 *     rem -= 3 * e;
 *     if (rem == 1) {x *= CBRT2I;}
 *     else if (rem == 2) {x *= CBRT4I;}
 *     e = -e;
 * }
 * ```
 * 
 * 其中：
 * - CBRT2 = \f$ 2^{1/3} \f$
 * - CBRT4 = \f$ 4^{1/3} = 2^{2/3} \f$
 * - CBRT2I = \f$ 2^{-1/3} \f$
 * - CBRT4I = \f$ 4^{-1/3} = 2^{-2/3} \f$
 * 
 * 处理原理：
 * \f[ \sqrt[3]{m \times 2^e} = \sqrt[3]{m} \times 2^{e/3} \f]
 * 对于不能整除3的指数，用预先计算的常数进行补偿。
 * 
 * ## 6. 恢复指数
 * 
 * ```c
 * x = ::ldexp(x, e);
 * ```
 * 
 * 使用`ldexp`函数将结果乘以2^e，恢复正确的数量级。
 * 
 * ## 7. 牛顿迭代法精化
 * 
 * ```c
 * for (int i = 0; i < 3; ++i) 
 * {
 *     x -= (x - (z / (x * x))) / 3.;
 * }
 * ```
 * 
 * 执行3次牛顿迭代以提高精度，迭代公式为：
 * \f[ x_{n+1} = x_n - \frac{x_n - \frac{z}{x_n^2}}{3} = \frac{2x_n + \frac{z}{x_n^2}}{3} \f]
 * 
 * 这实际上是求解\f$ f(x) = x^3 - z = 0 \f$的牛顿迭代法。
 * 
 * ## 8. 恢复符号
 * 
 * ```c
 * if (sign < 0) {x = -x;}
 * ```
 * 
 * 根据原始数的符号调整最终结果的符号。
 * 
 * ## 数学原理总结
 * 
 * 1. 将输入x分解为：\f$ x = \text{sign} \times m \times 2^e \f$，其中\f$ m \in [0.5, 1) \f$
 * 2. 使用多项式近似计算\f$ \sqrt[3]{m} \f$
 * 3. 处理指数部分：\f$ \sqrt[3]{2^e} = 2^{\lfloor e/3 \rfloor} \times \text{补偿因子} \f$
 * 4. 通过牛顿迭代法提高结果精度
 * 5. 组合各部分结果并恢复符号
 * 
 * 这种方法结合了多项式近似的快速性和牛顿迭代的高精度，能够在保证效率的同时获得较高的计算精度。
 *
 * @{
 */

/**
 * @brief 计算幂函数（x^y）
 * @param _X 底数值
 * @param _Power 指数值
 * @return _X 的 _Power 次幂
 */
float64 __cdecl pow(float64 _X, float64 _Power);

/**
 * @brief 计算复数的幂函数（z^w）
 * @param _X 复数底数值
 * @param _Power 复数指数值
 * @param _K 分支选择偏移量（默认为0）
 * @return 复数的幂结果
 */
complex64 __cdecl powc(complex64 _X, complex64 _Power, int64 _K = 0);

/**
 * @brief 计算向量各元素的标量指数幂
 * @tparam N 向量维度
 * @param _X 底数向量
 * @param _Power 指数值（标量）
 * @return 各元素幂结果向量
 */
template<std::size_t N>
fvec<N> __cdecl pow(fvec<N> _X, float64 _Power)
{
    __stelcxx_array_math_function_body(f, i, _CSE pow(_X[i], _Power))
}

/**
 * @brief 计算向量各元素的向量指数幂
 * @tparam N 向量维度
 * @param _X 底数向量
 * @param _Power 指数向量
 * @return 各元素幂结果向量
 */
template<std::size_t N>
fvec<N> __cdecl pow(fvec<N> _X, fvec<N> _Power)
{
    __stelcxx_array_math_function_body(f, i, _CSE pow(_X[i], _Power[i]))
}

/**
 * @brief 计算矩阵的整数次幂
 * @tparam _Size 矩阵维度
 * @param _A 输入矩阵
 * @param _Power 幂指数（正整数）
 * @return 矩阵的 _Power 次幂
 * @note 时间复杂度为 O(n^4)，大数值输入可能导致高延迟
 */
template<std::size_t _Size>
matrix<_Size, _Size> __cdecl pow(matrix<_Size, _Size> _A, uint64 _Power)
{
    assert(_Power > 0);
    matrix<_Size, _Size> m0;
    for (size_t i = 0; i < _Power; i++) {m0 = _A * _A;}
    return m0;
}

/**
 * @brief 计算平方根（√x）
 * @param _X 被开方数（必须 ≥ 0）
 * @return _X 的平方根
 */
float64 __cdecl sqrt(float64 _X);

/**
 * @brief 计算复数的平方根
 * @param _X 复数被开方数
 * @return 包含两个平方根的数组
 */
std::array<complex64, 2> __cdecl sqrtc(complex64 _X);

/**
 * @brief 计算向量各元素的平方根
 * @tparam N 向量维度
 * @param _X 输入向量
 * @return 各元素平方根结果向量
 */
template<std::size_t N>
fvec<N> __cdecl sqrt(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE sqrt(_X[i]))
}

/**
 * @brief 计算平方根倒数（1/√x）
 * @param _X 被开方数（必须 > 0）
 * @return _X 的平方根倒数
 */
float64 __cdecl inversesqrt(float64 _X);

/**
 * @brief 计算复数的平方根倒数
 * @param _X 复数被开方数
 * @return 包含两个平方根倒数的数组
 */
std::array<complex64, 2> __cdecl inversesqrtc(complex64 _X);

/**
 * @brief 计算向量各元素的平方根倒数
 * @tparam N 向量维度
 * @param _X 输入向量
 * @return 各元素平方根倒数结果向量
 */
template<std::size_t N>
fvec<N> __cdecl inversesqrt(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE inversesqrt(_X[i]))
}

/**
 * @brief 计算立方根（³√x）
 * @param _X 被开方数
 * @return _X 的立方根
 */
float64 __cdecl cbrt(float64 _X);

/**
 * @brief 计算复数的立方根
 * @param _X 复数被开方数
 * @return 包含三个立方根的数组
 */
std::array<complex64, 3> __cdecl cbrtc(complex64 _X);

/**
 * @brief 计算向量各元素的立方根
 * @tparam N 向量维度
 * @param _X 输入向量
 * @return 各元素立方根结果向量
 */
template<std::size_t N>
fvec<N> __cdecl cbrt(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE cbrt(_X[i]))
}

/**
 * @brief 计算任意次方根（ʸ√x）
 * @param _X 被开方数
 * @param _Expo 根指数
 * @return _X 的 _Expo 次方根
 */
float64 __cdecl yroot(float64 _X, float64 _Expo);

/**
 * @brief 计算复数的任意次方根
 * @param _X 复数被开方数
 * @param _Expo 复数根指数
 * @param _K 分支选择偏移量（默认为0）
 * @return 复数根的结果
 */
complex64 __cdecl yrootc(complex64 _X, complex64 _Expo, int64 _K = 0);

/**
 * @brief 计算向量各元素的标量指数方根
 * @tparam N 向量维度
 * @param _X 输入向量
 * @param _Expo 根指数（标量）
 * @return 各元素方根结果向量
 */
template<std::size_t N>
fvec<N> __cdecl yroot(fvec<N> _X, float64 _Expo)
{
    __stelcxx_array_math_function_body(f, i, _CSE yroot(_X[i], _Expo))
}

/**
 * @brief 计算向量各元素的向量指数方根
 * @tparam N 向量维度
 * @param _X 输入向量
 * @param _Expo 根指数向量
 * @return 各元素方根结果向量
 */
template<std::size_t N>
fvec<N> __cdecl yroot(fvec<N> _X, fvec<N> _Expo)
{
    __stelcxx_array_math_function_body(f, i, _CSE yroot(_X[i], _Expo[i]))
}

/**@}*/

/**
 * @defgroup MathFuncsTri 三角函数
 * @ingroup MathFuncs
 * @brief 三角函数包含sin，cos，tan，ctg，sec，csc六个函数及其反函数，同时包含基于角度和弧度的。
 * @details
 * 角度sin和cos函数为基于<a href=https://opencv.org/>OpenCV</a>的快速一倍精度算法和<a href=https://www.gnu.org/software/libc/>glibc</a>的四倍精度算法整理而来的自研算法，tan函数则由glibc的四倍精度算法改编而来。
 * 弧度三角函数均使用<a href=https://github.com/dreal-deps/mathlib>IBM Accurate Mathematical Library</a>的算法。
 * 角度反三角函数基于<a href=https://www.gnu.org/software/libc/>glibc</a>提供的四倍精度算法改编，其中arctan函数为角度/弧度双兼容。
 * 弧度arcsin和arccos使用<a href=https://github.com/dreal-deps/mathlib>IBM Accurate Mathematical Library</a>的算法。
 *
 * **以下内容为AI智能解析，仅供参考**
 * 
 * # sin/cos函数原理
 * 
 * ## 1. 基本原理
 * 
 * 代码使用了两种不同的方法来计算sin/cos函数：
 * 
 * 1. **切比雪夫多项式逼近** ：对于小角度（|x| ≤ 8.4375°），直接使用切比雪夫多项式逼近。
 * 2. **角度分解法** ：对于较大角度（8.4375° < |x| ≤ 45°），将角度分解为高位h和低位l，利用预计算表和三角恒等式计算。
 * 
 * 对于超过45°的角度，先用诱导公式将其缩减到0-360°范围内，再利用周期性性质转换为小角度计算。
 * 
 * ## 2. 预计算表
 * 
 * 预计算表`__SinCos128F_Table`存储了33个角度（n*(360/256)，n=0..32）的正弦和余弦值的高精度值。每个角度对应4个条目：
 * 
 * - 正弦值的高113位
 * - 正弦值的低113位（补偿精度）
 * - 余弦值的高113位
 * - 余弦值的低113位
 * 
 * 这种设计允许在计算时保持高精度，通过将高位和低位部分组合起来。
 * 
 * ## 3. 小角度计算（|x| ≤ SINCOS_BOUNDARY）
 * 
 * 对于小角度，直接使用切比雪夫多项式逼近：
 * 
 * ### 正弦逼近公式：
 * \f[
 * \sin(x) \approx x(\text{SIN}_0 + x^2 (\text{SIN}_1 + x^2 (\text{SIN}_2 + \cdots + x^2 \text{SIN}_8)))
 * \f]
 * 
 * ### 余弦逼近公式：
 * \f[
 * \cos(x) \approx \text{COS}_0 + x^2 (\text{COS}_1 + x^2 (\text{COS}_2 + ... + x^2 \text{COS}_8))
 * \f]
 * 
 * 这些系数(SIN0-SIN8, COS0-COS8)是预先计算好的切比雪夫多项式系数，能够在小角度范围内提供高精度逼近。
 * 
 * ## 4. 中等角度计算（SINCOS_BOUNDARY < |x| ≤ 45°）
 * 
 * 对于中等角度，采用角度分解法：
 * 
 * 1. 将角度x分解为整数部分h和小数部分l：
 * ```
 * h = round(x * N/360)
 * l = x - h
 * ```
 * 其中N=132是预计算表的大小。
 * 
 * 2. 从预计算表中查找sin(h)和cos(h)的高低位值。
 * 
 * 3. 使用低阶切比雪夫多项式计算sin(l)和cos(l)：
 * ```
 * sin(l) ≈ l*(SSIN0 + l²*(SSIN1 + l²*(SSIN2 + ... + l²*SSIN5)))
 * cos(l) ≈ 1 + l²*(SCOS1 + l²*(SCOS2 + ... + l²*SCOS5))
 * ```

 * 4. 使用三角恒等式组合结果：
 * ```
 * sin(x) = sin(h+l) = sin(h)cos(l) + cos(h)sin(l)
 * cos(x) = cos(h+l) = cos(h)cos(l) - sin(h)sin(l)
 * ```
 * 
 * ## 5. 大角度计算（|x| > 45°）
 * 
 * 对于超过45°的角度，代码首先使用模运算将角度缩减到0-360°范围内，然后利用三角函数的周期性性质：
 * 
 * - 0-45°：直接计算
 * - 45-135°：利用sin(x)=cos(90°-x)或cos(x)=sin(90°-x)
 * - 135-225°：利用sin(x)=sin(180°-x)或cos(x)=-cos(180°-x)
 * - 225-315°：利用sin(x)=-cos(270°-x)或cos(x)=-sin(270°-x)
 * - 315-360°：利用sin(x)=-sin(360°-x)或cos(x)=cos(360°-x)
 * 
 * ## 6. 实现细节
 * 
 * - 使用`__Float64`类型表示高精度浮点数
 * - 通过查表法快速获取已知角度的sin/cos值
 * - 使用多项式逼近在局部范围内获得高精度结果
 * - 处理了各种边界条件（极小值、NaN、Inf等）
 * - 考虑了符号处理（sin是奇函数，cos是偶函数）
 * 
 * ## 7. 数学公式表示
 * 
 * ### 小角度逼近：
 * \f[
 * \sin(x) \approx \sum_{k=0}^{8} \text{SIN}_k \cdot x^{2k+1}
 * \f]
 * \f[
 * \cos(x) \approx \sum_{k=0}^{8} \text{COS}_k \cdot x^{2k}
 * \f]
 * 
 * ### 中等角度分解：
 * \f[
 * x = h + l
 * \f]
 * \f[
 * \sin(x) = \sin(h)\cos(l) + \cos(h)\sin(l)
 * \f]
 * \f[
 * \cos(x) = \cos(h)\cos(l) - \sin(h)\sin(l)
 * \f]
 * 
 * ### 大角度缩减：
 * 对于任意角度x，先计算：
 * \f[
 * x' = x \mod 360°
 * \f]
 * 然后根据x'所在区间使用适当的三角恒等式转换为小角度计算。
 * 
 * 这种方法结合了查表法的高效性和多项式逼近的精确性，能够在广泛的输入范围内提供高精度的三角函数计算结果。
 *
 * ---
 *
 * **以下内容为AI智能解析，仅供参考**
 *
 * # tan/ctg函数原理
 * 
 * ## 1. 核心原理
 * 
 * ### 1.1 帕德逼近方法
 * 
 * 帕德逼近是一种有理函数逼近方法，比泰勒级数能提供更好的近似效果。对于函数\f$f(x)\f$，其\f$[m/n]\f$阶帕德逼近定义为：
 * 
 * \f[
 * R(x) = \frac{P_m(x)}{Q_n(x)} = \frac{\sum_{j=0}^m a_j x^j}{1 + \sum_{k=1}^n b_k x^k}
 * \f]
 * 
 * 在代码中，正切函数采用了以下形式的帕德逼近：
 * 
 * \f[
 * \tan(x) \approx x + \frac{x^3}{3} + x^5 \frac{T(x^2)}{U(x^2)}
 * \f]
 * 
 * 其中\f$T\f$和\f$U\f$是多项式：
 * 
 * \f[
 * T(x^2) = T_0 + T_1 x^2 + T_2 x^4 + T_3 x^6 + T_4 x^8
 * \f]
 * 
 * \f[
 * U(x^2) = U_0 + U_1 x^2 + U_2 x^4 + U_3 x^6 + U_4 x^8 + U_5 x^{10}
 * \f]
 * 
 * ### 1.2 角度转换技巧
 * 
 * 当\f$x\f$接近45度时，正切函数的曲率增大，直接逼近效果不佳。代码采用了以下恒等式：
 * 
 * 对于\f$x \geq 38.6^\circ\f$，设\f$y = 45 - x\f$，则有：
 * 
 * \f[
 * \tan(x) = \tan(45 - y) = \frac{1 - \tan(y)}{1 + \tan(y)} = 1 - 2 \left( \frac{\tan(y) - \tan^2(y)}{1 + \tan(y)} \right)
 * \f]
 * 
 * 这样可以将大角度转换为小角度计算，提高精度。
 * 
 * ## 2. 关键实现细节
 * 
 * ### 2.1 系数计算
 * 
 * 分子多项式\f$T\f$和分母多项式\f$U\f$的系数是通过解线性方程组得到的，基于正切函数的泰勒展开系数：
 * 
 * 泰勒展开系数为：
 * \f[
 * a_0 = \frac{2}{15}, a_1 = \frac{17}{315}, a_2 = \frac{62}{2835}, a_3 = \frac{1382}{155925}, 
 * \f]
 * \f[
 * a_4 = \frac{21844}{6081075}, a_5 = \frac{929569}{638512875}, \ldots
 * \f]
 * 
 * 分母系数\f$U_n\f$通过解以下线性系统得到：
 * \f[
 * \begin{bmatrix}
 * a_5 & a_6 & a_7 & a_8 & a_9 \\
 * a_4 & a_5 & a_6 & a_7 & a_8 \\
 * a_3 & a_4 & a_5 & a_6 & a_7 \\
 * a_2 & a_3 & a_4 & a_5 & a_6 \\
 * a_1 & a_2 & a_3 & a_4 & a_5 \\
 * \end{bmatrix}
 * \begin{bmatrix}
 * U_0 \\
 * U_1 \\
 * U_2 \\
 * U_3 \\
 * U_4 \\
 * \end{bmatrix}
 * =
 * \begin{bmatrix}
 * -a_0 \\
 * -a_1 \\
 * -a_2 \\
 * -a_3 \\
 * -a_4 \\
 * \end{bmatrix}
 * \f]
 *
 * 分子系数\f$T_n\f$通过如下方式得到：
 * \f[
 * \begin{align}
 * T_0 & = a_0 U_0 \\
 * ​T_1 ​&= a_0 U_1 ​+ a_1​ U_0 \\
 * ​T_2 ​&= a_0 U_2 ​+ a_1​ U_1 ​+ a_2 ​U_0 \\
 * ​T_3 ​&= a_0 U_3 ​+ a_1 ​U_2 ​+ a_2​ U_1 ​+ a_3 ​U_0 \\
 * ​T_4 ​&= a_0 U_4 ​+ a_1​ U_3 ​+ a_2​ U_2 ​+ a_3 U_1 ​+ a_4 U_0​​
 * \end{align}
 * \f]
 * 
 * ### 2.2 多项式计算优化
 * 
 * 为避免"大数吃小数"问题，代码没有使用传统的秦九韶算法，而是直接计算各项之和：
 * 
 * ```cpp
 * T = T4.x * x8 + T3.x * x6 + T2.x * x4 + T1.x * x2 + T0.x;
 * U = U5.x * x10 + U4.x * x8 + U3.x * x6 + U2.x * x4 + U1.x * x2 + U0.x;
 * ```
 * 
 * ### 2.3 高精度处理
 * 
 * 对于余切计算，代码使用了额外的浮点截断修正来提高精度：
 * 
 * ```cpp
 * __Float64 x1 = H1.x * x;
 * __Float64 u1(w);
 * u1.parts.lsw = 0;  // 截断低位
 * __Float64 v = r - (u1 - x1);  // 修正项
 * __Float64 z = 1.0 / w;
 * __Float64 z1(z);
 * z1.parts.lsw = 0;  // 截断低位
 * __Float64 s = 1.0 - z1 * u1;
 * return z1 + z * (s - z1 * v);
 * ```
 * 
 * ## 3. 范围缩减处理
 * 
 * 对于超出\f$[-45^\circ, 45^\circ]\f$范围的输入，代码使用周期性将角度缩减到\f$[0^\circ, 180^\circ]\f$内，并利用三角函数的性质转换为基本范围内的计算：
 * 
 * - \f$\tan(x) = \cot(90^\circ - x)\f$
 * - \f$\tan(x) = -\cot(x - 90^\circ)\f$
 * - \f$\tan(x) = -\tan(180^\circ - x)\f$
 * 
 * 类似地，余切函数也做了相应的处理。
 * 
 * ## 4. 误差分析
 * 
 * 该实现的理论误差约为\f$O(x^{25})\f$，相当于23阶泰勒展开的精度。通过帕德逼近和角度转换技巧，在保持高精度的同时减少了计算量。
 * 
 * ## 5. 特殊处理
 * 
 * - 对于非常小的\f$x\f$ (\f$|x| < 2^{-57}\f$)，直接返回\f$x\f$或\f$1/x\f$以避免除零问题
 * - 对于\f$\tan(90^\circ)\f$、\f$\cot(0^\circ)\f$等特殊情况返回NaN
 * - 对于无穷大和NaN输入也返回NaN
 * 
 * 这种实现方式在保持高精度的同时，也考虑了各种边界情况和数值稳定性问题。
 *
 * ---
 *
 * **以下内容为AI智能解析，仅供参考**
 *
 * # 弧度 sin/cos 函数原理
 * 
 * ## 1. 基本原理
 * 
 * 正弦和余弦函数的计算通常采用以下步骤：
 * 1. **范围缩减** ：将任意输入角度缩减到[-π/4, π/4]区间
 * 2. **多项式近似** ：在缩减后的区间内使用多项式近似计算函数值
 * 3. **结果调整** ：根据原始角度所在的象限调整最终结果
 * 
 * ## 2. 关键组件
 * 
 * ### 2.1 多项式近似系数
 * 
 * 代码中定义了两组多项式系数：
 * 
 * **泰勒级数系数** ：
 * ```c
 * static const float64 s1 = -0x1.5555555555555p-03; // -1/6 = -0.16666666666666666
 * static const float64 s2 = +0x1.1111111110ECEp-07; // 1/120 ≈ 0.0083333333333323288
 * static const float64 s3 = -0x1.A01A019DB08B8p-13; // -1/5040 ≈ -0.00019841269834414642
 * static const float64 s4 = +0x1.71DE27B9A7ED9p-19; // 1/362880 ≈ 2.755729806860771e-06
 * static const float64 s5 = -0x1.ADDFFC2FCDF59p-26; // -1/39916800 ≈ -2.5022014848318398e-08
 * ```
 * 
 * 这些系数对应于sin函数的泰勒展开式：
 * $$
 * \sin(x) \approx x - \frac{x^3}{3!} + \frac{x^5}{5!} - \frac{x^7}{7!} + \frac{x^9}{9!} - \frac{x^{11}}{11!}
 * $$
 * 
 * **另一组多项式系数** ：
 * ```c
 * static const float64
 *     sn3 = -1.66666666666666666666666666666667E-01,  // -1/6
 *     sn5 = +8.33333333333333333333333366666667E-03,  // 1/120
 *     cs2 = +5.00000000000000000000000000000000E-01,  // 1/2
 *     cs4 = -4.16666666666666666666666633333333E-02,  // -1/24
 *     cs6 = +1.38888888888888888888888888888889E-03;  // 1/720
 * ```
 * 
 * ### 2.2 预计算表
 * 
 * 代码使用了`__IBM_sincostab`预计算表，包含440个双精度浮点数，用于快速查找sin和cos的近似值。这种查表法可以加速计算。
 * 
 * ## 3. 核心算法
 * 
 * ### 3.1 范围缩减
 * 
 * `__IBM_branred`和`__IBM_reduce_sincos`函数负责将输入角度缩减到[-π/4, π/4]区间。
 * 
 * **数学原理** ：
 * \f[
 * x = n \cdot \frac{\pi}{2} + r, \quad |r| \leq \frac{\pi}{4}
 * \f]
 * 
 * 其中n是整数，r是余数。然后利用周期性：
 * - sin(x)取决于n mod 4
 * - cos(x) = sin(x + π/2)
 * 
 * ### 3.2 多项式计算
 * 
 * `__IBM_TAYLOR_SIN`和`__IBM_Kernal_sin/cos`函数使用多项式近似计算缩减后的角度值。
 * 
 * **sin多项式** ：
 * \f[
 * \sin(x) \approx x + x \cdot x^2 \cdot (s_1 + x^2 \cdot (s_2 + x^2 \cdot (s_3 + x^2 \cdot (s_4 + x^2 \cdot s_5))))
 * \f]
 * 
 * **cos多项式** ：
 * \f[
 * \cos(x) \approx 1 - \frac{x^2}{2!} + \frac{x^4}{4!} - \frac{x^6}{6!}
 * \f]
 * 
 * ### 3.3 查表法
 * 
 * `__IBM_SINCOS_TABLE_LOOKUP`函数通过查表获取sin和cos的近似值，然后与多项式计算结果结合，提高精度。
 * 
 * ## 4. 实现细节
 * 
 * 1. **多精度计算** ：使用双精度浮点数的高位和低位部分(a和aa)来提高精度
 * 2. **特殊处理小角度** ：当|x| < 0.126时直接使用泰勒级数
 * 3. **象限处理** ：根据n mod 4的结果确定最终结果的符号和使用sin还是cos
 * 4. **大数处理** ：对于极大数(>2^1024)有特殊处理
 * 
 * ## 5. 数学公式总结
 * 
 * 1. 范围缩减公式：
 * \f[
 * x = n \cdot \frac{\pi}{2} + r, \quad |r| \leq \frac{\pi}{4}
 * \f]
 * 
 * 2. sin多项式近似：
 * \f[
 * \sin(r) \approx r - \frac{r^3}{6} + \frac{r^5}{120} - \frac{r^7}{5040} + \frac{r^9}{362880} - \frac{r^{11}}{39916800}
 * \f]
 * 
 * 3. cos多项式近似：
 * \f[
 * \cos(r) \approx 1 - \frac{r^2}{2} + \frac{r^4}{24} - \frac{r^6}{720}
 * \f]
 * 
 * 4. 最终结果调整：
 * - 当n mod 4 = 0: sin(x) = sin(r)
 * - 当n mod 4 = 1: sin(x) = cos(r)
 * - 当n mod 4 = 2: sin(x) = -sin(r)
 * - 当n mod 4 = 3: sin(x) = -cos(r)
 * 
 * 这种实现结合了多项式近似和查表法，在保证精度的同时提高了计算效率，是数学函数库中常见的高效实现方式。
 * 
 * ---
 *
 * # 弧度 tan 函数原理
 * 
 * ## 1. 算法概述
 * 
 * 该实现采用了分段多项式近似和查表相结合的方法，根据输入值的大小将计算分为多个区间，每个区间采用不同的优化策略：
 * 
 * 1. 极小值区间：直接返回x
 * 2. 小值区间：使用多项式近似
 * 3. 中等值区间：查表+多项式修正
 * 4. 大值区间：范围缩减+查表/多项式
 * 
 * ## 2. 数学原理
 * 
 * ### 2.1 泰勒展开
 * 
 * 对于小值区间，使用泰勒级数展开近似：
 * 
 * \f[
 * \tan(x) \approx x + \frac{x^3}{3} + \frac{2x^5}{15} + \frac{17x^7}{315} + \cdots
 * \f]
 * 
 * 代码中使用了5阶多项式近似：
 * ```c
 * t2 = d9 + x2 * d11;
 * t2 = d7 + x2 * t2;
 * t2 = d5 + x2 * t2;
 * t2 = d3 + x2 * t2;
 * t2 *= x * x2;
 * ```
 * 
 * ### 2.2 查表法
 * 
 * 对于中等值区间，使用了预计算的查表法。表中存储了186个点的数据，每个点包含：
 * 
 * 1. `xi`：参考点x值
 * 2. `Fi`：tan(xi)的值
 * 3. `Gi`：cot(xi)的值
 * 4. `FFi`：修正项
 * 
 * 计算时先找到最近的参考点，然后计算差值z = x - xi，再用多项式修正：
 * 
 * \f[
 * \tan(x) \approx F_i + \frac{(G_i + F_i)P(z)}{G_i - P(z)}
 * \f]
 * 
 * 其中$P(z)$是修正多项式：
 * \f[
 * P(z) = z + z^3(e_0 + e_1 z^2)
 * \f]
 * 
 * ### 2.3 范围缩减
 * 
 * 对于大值输入，使用范围缩减技术将输入值缩减到[-π/2, π/2]区间内。这利用了tan函数的周期性：
 * 
 * \f[
 * \tan(x) = \tan(x \mod \pi)
 * \f]
 * 
 * 代码中使用了三种不同的范围缩减算法，根据输入值大小选择：
 * 
 * 1. 算法I：适用于0.787 < |x| ≤ 25
 * 2. 算法II：适用于25 < |x| ≤ 1e8
 * 3. 算法III：适用于1e8 < |x| < 2^1024
 * 
 * ## 3. 关键代码段解析
 * 
 * ### 3.1 极小值区间处理 (|x| ≤ 1.259e-8)
 * 
 * ```c
 * if (w <= g1) { return x; }
 * ```
 * 
 * 直接返回x，因为tan(x) ≈ x当x趋近于0时。
 * 
 * ### 3.2 小值区间处理 (1.259e-8 < |x| ≤ 0.0608)
 * 
 * ```c
 * x2 = x * x;
 * t2 = d9 + x2 * d11;
 * t2 = d7 + x2 * t2;
 * t2 = d5 + x2 * t2;
 * t2 = d3 + x2 * t2;
 * t2 *= x * x2;
 * return x + t2;
 * ```
 * 
 * 这是秦九韶方法计算多项式，等价于：
 * \f[
 * x + \frac{x^3}{3} + \frac{2x^5}{15} + \frac{17x^7}{315} + \frac{62x^9}{2835} + \frac{1382x^{11}}{155925}
 * \f]
 * 
 * ### 3.3 中等值区间处理 (0.0608 < |x| ≤ 0.787)
 * 
 * ```c
 * i = ((int)(mfftnhf + 256 * w));
 * z = w - __IBM_TANF64_XFGTBL[i][0].x;
 * z2 = z * z;
 * s = (x < 0.0) ? -1 : 1;
 * pz = z + z * z2 * (e0 + z2 * e1);
 * fi = __IBM_TANF64_XFGTBL[i][1];
 * gi = __IBM_TANF64_XFGTBL[i][2];
 * t2 = pz * (gi + fi) / (gi - pz);
 * y = fi + t2;
 * return (s * y);
 * ```
 * 
 * 这里使用了查表加多项式修正的方法。
 * 
 * ### 3.4 大值处理 (|x| > 0.787)
 * 
 * 对大值先进行范围缩减，然后根据缩减后的y值大小选择不同的计算方法：
 * 
 * 1. 如果|y| ≤ 0.0608，使用多项式近似
 * 2. 如果0.0608 < |y| ≤ 0.787，使用查表法
 * 
 * ## 4. 误差分析
 * 
 * 代码注释中给出了各区间最大误差：
 * - 多项式近似区间：0.504 ULP
 * - 查表法区间：0.60-0.62 ULP
 * 
 * ## 5. 辅助函数
 * 
 * 代码中使用了几个重要的辅助函数：
 * 1. `EADD`：精确加法，计算和及误差
 * 2. `MUL12`：精确乘法，计算积及误差
 * 3. `DIV2`：精确除法，计算商及误差
 * 
 * 这些函数用于保证大值范围缩减时的精度。
 * 
 * ## 6. 总结
 * 
 * IBM的这个tan函数实现综合运用了：
 * - 泰勒展开多项式近似
 * - 查表法加多项式修正
 * - 多级范围缩减技术
 * - 精确的浮点运算方法
 * 
 * 通过分段处理和多种算法组合，在保证精度的同时覆盖了完整的双精度浮点输入范围。
 *
 * ---
 *
 * **以下内容为AI智能解析，仅供参考**
 *  
 * # arcsin函数原理
 * 
 * ## 基本思路
 * 
 * arcsin函数计算基于以下数学原理：
 * 
 * 1. 对于\f$|x| < 2^{-57}\f$的小输入值，使用线性近似：\f$\arcsin(x) \approx x\f$（弧度），然后转换为角度。
 * 
 * 2. 对于0.5 ≤ |x| < 0.625的中间范围，使用雷米兹算法(Remez algorithm)生成的有理多项式近似。
 * 
 * 3. 对于|x| ≥ 0.625的大输入值，使用变换公式：\f$\arcsin(x) = 90 - 2\arcsin(\sqrt{\frac{1-x}{2}})\f$。
 * 
 * ## 数学公式
 * 
 * ### 小输入值(|x| < 0x1p-57)
 * 
 * 直接线性近似：
 * \f[
 * \arcsin(x) \approx x \times \frac{180}{\pi}
 * \f]
 * 
 * ### 中等输入值(0.5 ≤ |x| < 0.625)
 * 
 * 使用有理多项式近似：
 * \f[
 * \arcsin(0.5625 + t) \approx \arcsin(0.5625) + \frac{P(t)}{Q(t)}, \quad -0.0625 \leq t \leq 0.0625
 * \f]
 * 
 * 其中P(t)和Q(t)是10次多项式：
 * \f[
 * \begin{aligned}
 * P(t) &= \sum_{i=0}^{10} rS_i \cdot t^i \\
 * Q(t) &= \sum_{i=0}^{10} sS_i \cdot t^i
 * \end{aligned}
 * \f]
 * 
 * ### 大输入值(|x| ≥ 0.625)
 * 
 * 使用变换公式：
 * \f[
 * \arcsin(x) = 90^\circ - 2 \cdot \arcsin\left(\sqrt{\frac{1-x}{2}}\right)
 * \f]
 * 
 * 对于接近1的输入值(0.975 ≤ |x| < 1)，直接使用上述变换。
 * 
 * 对于其他大输入值(0.625 ≤ |x| < 0.975)，使用更精确的计算：
 * \f[
 * \arcsin(x) = 45^\circ - \left[2s \cdot \frac{P(t)}{Q(t)} + 2c - R2DM2 \cdot w\right]
 * \f]
 * 其中：
 * \f[
 * t = \frac{1-x}{2}, \quad s = \sqrt{t}, \quad w = \text{truncated } s, \quad c = \frac{(t-w^2) \times 180}{(s+w) \times \pi}
 * \f]
 * 
 * ## 实现细节
 * 
 * 1. **特殊值处理** ：
 *  - 输入为NaN时返回NaN
 *  - 输入为±1时返回±90°
 * 
 * 2. **角度转换** ：
 *  - 使用常数R2D(180/π)和R2DM2(2×180/π)进行弧度到角度的转换
 * 
 * 3. **多项式计算** ：
 *  - 使用秦九韶方法高效计算多项式值，减少运算次数
 * 
 * 4. **精度优化** ：
 *  - 对于关键点(如0.5625)使用高精度常量(拆分为高位和低位部分)
 *  - 使用修正项提高接近特殊角度(如30°、60°)的计算精度
 * 
 * 这个实现通过分段处理和有理多项式近似，在保证高效率的同时达到了很高的计算精度(约35位有效数字)。
 *
 * ---
 *
 * **以下内容为AI智能解析，仅供参考**
 *
 * # arccos函数原理
 * 
 * ## 基本数学原理
 * 
 * 反余弦函数arccos(x)定义域为[-1, 1]，值域为[0, 180]。该实现使用了多种近似方法来计算不同区间内的arccos值。
 * 
 * ### 核心公式
 * 
 * 1. 对于小值(|x| < 0.4375)，使用反正弦近似：
 * \f[
 * \arcsin(x) = x + \frac{x^3 p_S(x^2)}{q_S(x^2)}
 * \f]
 * 然后利用关系：
 * \f[
 * \arccos(x) = 90° - \arcsin(x)
 * \f]
 * 
 * 2. 对于中间值(0.4375 ≤ |x| < 0.625)，使用多项式近似：
 * \f[
 * \arccos(x) = \arccos(c) + (x-c) \cdot \frac{r(x-c)}{s(x-c)}
 * \f]
 * 其中c是区间的中点(0.4375或0.5625)
 * 
 * 3. 对于大值(0.625 ≤ |x| ≤ 1)，使用变换：
 * \f[
 * \arccos(x) = 2 \arcsin\left(\sqrt{\frac{1-x}{2}}\right)
 * \f]
 * 
 * ## 实现细节解析
 * 
 * ### 预处理
 * 
 * 1. **异常处理** ：
 *  - 如果输入是NaN，返回NaN
 *  - 如果|x| > 1，返回NaN
 *  - 如果x = ±1，直接返回0°或180°
 *  - 如果|x|非常小(<2^-106)，直接返回90°
 * 
 * 2. **常数定义** ：
 *  - 弧度到角度转换因子：57.2957795130823208768... (180/π)
 *  - 多个预先计算的多项式系数(pS0-9, qS0-9等)
 * 
 * ### 区间处理
 * 
 * 1. **小值区间(|x| < 0.4375)** ：
 *  - 使用高阶多项式近似arcsin(x)
 *  - 计算z = x²
 *  - 计算分子p和分母q的秦九韶形式多项式
 *  - 结果转换为角度：90° - (R2D*x + x*p/q)
 * 
 * 2. **中间区间1(0.4375 ≤ |x| < 0.5)** ：
 *  - 使用以0.4375为中心的近似
 *  - 预先存储arccos(0.4375)和其补角值
 *  - 计算t = x - 0.4375
 *  - 计算p和q的秦九韶形式多项式
 *  - 根据x的符号选择结果公式
 * 
 * 3. **中间区间2(0.5 ≤ |x| < 0.625)** ：
 *  - 类似处理，但以0.5625为中心
 *  - 使用不同的多项式系数
 * 
 * 4. **大值区间(0.625 ≤ |x| ≤ 1)** ：
 *  - 使用变换z = (1-|x|)/2
 *  - 计算s = √z
 *  - 使用arcsin近似计算arcsin(s)
 *  - 最终结果根据x符号调整后乘以2
 * 
 * ### 精度优化
 * 
 * 1. **多项式系数** ：
 *  - 使用雷米兹算法求得最优多项式系数
 *  - 系数已转换为角度制，减少计算步骤
 * 
 * 2. **秦九韶方法** ：
 *  - 多项式计算采用秦九韶形式，提高数值稳定性
 * 
 * 3. **特殊处理** ：
 *  - 对于边界值和特殊值直接返回精确结果
 *  - 不同区间使用不同的近似方法，保证各区间精度
 * 
 * ## 数学公式总结
 * 
 * 1. 小值区间：
 * \f[
 * \arccos(x) \approx 90° - \left(\frac{180}{\pi}x + x \cdot \frac{\sum_{i=0}^9 pS_i \cdot x^{2i}}{\sum_{i=0}^9 qS_i \cdot x^{2i}}\right)
 * \f]
 * 
 * 2. 中间区间(以0.4375为例)：
 * \f[
 * \arccos(x) \approx \arccos(0.4375) + (x-0.4375) \cdot \frac{\sum_{i=0}^{10} P_i \cdot (x-0.4375)^i}{\sum_{i=0}^{10} Q_i \cdot (x-0.4375)^i}
 * \f]
 * 
 * 3. 大值区间：
 * \f[
 * \arccos(x) \approx 2 \cdot \left(\frac{180}{\pi}\sqrt{\frac{1-x}{2}} + \sqrt{\frac{1-x}{2}} \cdot \frac{p_S\left(\frac{1-x}{2}\right)}{q_S\left(\frac{1-x}{2}\right)}\right)
 * \f]
 * 
 * 这个实现通过分段处理和精心设计的多项式近似，在保证效率的同时达到了极高的计算精度(相对误差约1.9×10⁻³⁵)。
 *
 * ---
 *
 * **以下内容为AI智能解析，仅供参考**
 * 
 * # arctan函数原理
 * 
 * ## 基本原理
 * 
 * arctan函数计算基于以下数学原理：
 * 
 * 1. **查表法** ：预先计算并存储一系列已知点的arctan值
 * 2. **多项式逼近** ：对于表中点之间的值，使用多项式逼近计算剩余部分
 * 3. **范围缩减** ：通过数学变换将大参数转换为小参数计算
 * 
 * ### 核心公式
 * 
 * arctan函数的计算主要基于以下数学性质：
 * 
 * 1. **加法公式** ：
 * \f[
 * \arctan x = \arctan u + \arctan\left(\frac{x - u}{1 + x u}\right)
 * \f]
 * 其中u是接近x的值。
 * 
 * 2. **多项式逼近** ：
 * 对于小的t值，arctan(t)可以用多项式逼近：
 * \f[
 * \arctan(t) \approx t \cdot \frac{P(t^2)}{Q(t^2)}
 * \f]
 * 其中P和Q是预先计算的多项式系数。
 * 
 * ## 代码解析
 * 
 * ### 预计算表
 * 
 * 代码中定义了两个预计算表：
 * ```cpp
 * const __Float64 __Arctan128F_table_deg[84]  // 以度为单位的arctan值表
 * const __Float64 __Arctan128F_table_rad[84]  // 以弧度为单位的arctan值表
 * ```
 * 
 * 表中存储了从0到10.25(步长0.125)的x值对应的arctan(x)值，以及π/2的值。
 * 
 * ### 主要计算步骤
 * 
 * 1. **处理特殊值** ：
 *  - 处理NaN和无穷大情况
 *  - 处理非常小的x值(直接返回x)
 *  - 处理非常大的x值(返回±π/2)
 * 
 * 2. **范围缩减** ：
 *  - 对于x ≥ 10.25，使用恒等变换：
 *     \f[
 *     \arctan(x) = \frac{\pi}{2} - \arctan\left(\frac{1}{x}\right)
 *     \f]
 *  - 对于其他x值，找到最近的表中点u = k*0.125，然后计算：
 *     \f[
 *     t = \frac{x - u}{1 + x u}
 *     \f]
 * 
 * 3. **多项式逼近**：
 * 使用预先定义的p0-p4和q0-q4系数计算小值t的arctan：
 * \f[
 * \arctan(t) \approx t \cdot \frac{p_4 t^4 + p_3 t^3 + p_2 t^2 + p_1 t + p_0}{t^4 + q_4 t^3 + q_3 t^2 + q_2 t + q_1 t + q_0}
 * \f]
 * 
 * 4. **组合结果** ：
 * \[
 * \arctan(x) = \arctan(u) + \arctan(t)
 * \]
 * 其中arctan(u)从表中查找，arctan(t)用多项式逼近计算。
 * 
 * ## 数学推导
 * 
 * ### 加法公式推导
 * 
 * 从tan的加法公式出发：
 * \f[
 * \tan(A + B) = \frac{\tan A + \tan B}{1 - \tan A \tan B}
 * \f]
 * 
 * 设：
 * \f[
 * x = \tan(A + B), \quad u = \tan A
 * \f]
 * 
 * 则：
 * \f[
 * \begin{aligned}
 * x &= \frac{u + \tan B}{1 - u \tan B} \\
 * &\Rightarrow x(1 - u \tan B) = u + \tan B \\
 * &\Rightarrow x - x u \tan B = u + \tan B \\
 * &\Rightarrow \tan B = \frac{x - u}{1 + x u}
 * \end{aligned}
 * \f]
 * 
 * 因此：
 * \f[
 * \arctan x = A + B = \arctan u + \arctan\left(\frac{x - u}{1 + x u}\right)
 * \f]
 * 
 * ### 多项式逼近
 * 
 * 对于小的t值，arctan(t)可以用泰勒展开：
 * \f[
 * \arctan(t) = t - \frac{t^3}{3} + \frac{t^5}{5} - \frac{t^7}{7} + \cdots
 * \f]
 * 
 * 代码中使用的是更高效的有理多项式逼近（疑似雷米兹算法得到），形式为：
 * \f[
 * \arctan(t) \approx t \cdot \frac{P(t^2)}{Q(t^2)}
 * \f]
 * 
 * 这种形式可以更精确地逼近arctan函数，同时计算效率较高。
 * 
 * ## 实现优化
 * 
 * 1. **非对称舍入** ：`k = int32_t(8.0 * x + 0.25)`使用非对称舍入来避免t为负数时的抵消误差
 * 2. **高精度系数** ：使用高精度的多项式系数p0-p4和q0-q4
 * 3. **双表支持** ：同时支持弧度和角度两种单位输出
 * 4. **特殊值处理** ：正确处理NaN、无穷大等特殊情况
 * 
 * ## 误差分析
 * 
 * 该算法的误差主要来自：
 * 1. 表查找的量化误差(最大步长0.125)
 * 2. 多项式逼近的截断误差
 * 3. 浮点运算的舍入误差
 * 
 * 由于使用了高精度的逼近多项式和合理的范围缩减技术，整体精度可以达到很高水平。
 *
 * ---
 *
 * **以下内容为AI智能解析，仅供参考**
 *
 * # 弧度 arcsin/arccos 函数原理
 * 
 * ## 数学基础
 * 
 * ### 泰勒级数展开
 * 
 * 对于小值输入（|x| < 2^-3），代码使用泰勒级数展开：
 * 
 * \f[
 * \arcsin(x) = x + \frac{1}{6}x^3 + \frac{3}{40}x^5 + \frac{5}{112}x^7 + \frac{35}{1152}x^9 + \cdots
 * \f]
 * 
 * 通项公式为：
 * 
 * \f[
 * \arcsin(x) = \sum_{n=0}^{\infty} \frac{(2n)!}{4^n (n!)^2 (2n+1)} x^{2n+1}
 * \f]
 * 
 * 代码中预计算了前几项的系数：
 * ```c
 * f1 = 1.66666666666666666666666666666667E-01,  // 1/6
 * f2 = 7.50000000000000000000000000000000E-02,  // 3/40
 * f3 = 4.46428571428571428571428571428571E-02,  // 5/112
 * f4 = 3.03819444444444444444444444444444E-02,  // 35/1152
 * f5 = 2.23721590909090909090909090909091E-02,  // 63/2816
 * f6 = 1.73527644230769230769230769230769E-02   // 231/13312
 * ```
 * 
 * ### 查表与多项式逼近
 * 
 * 对于中等大小的输入（0.125 ≤ |x| < 1），代码使用预计算的查找表（`__IBM_asncs_table`）结合多项式逼近的方法。表中存储了：
 * 
 * 1. 参考点 \f$ a_i \f$
 * 2. 多项式系数 \f$ c_{i,j} \f$
 * 3. 参考点处的函数值 \f$ \arcsin(a_i) \f$
 * 
 * 计算方式为：
 * 
 * \f[
 * \arcsin(x) \approx \arcsin(a_i) + c_{i,1}(x-a_i) + c_{i,2}(x-a_i)^2 + \cdots + c_{i,n}(x-a_i)^n
 * \f]
 * 
 * ### 特殊处理接近1的值
 * 
 * 对于接近1的值（0.96875 ≤ |x| < 1），使用变换：
 * 
 * \f[
 * \arcsin(x) = \frac{\pi}{2} - 2\arcsin\left(\sqrt{\frac{1-x}{2}}\right)
 * \f]
 * 
 * 这利用了恒等式：
 * 
 * \f[
 * \arcsin(x) = \frac{\pi}{2} - \arccos(x) = \frac{\pi}{2} - 2\arcsin\left(\sqrt{\frac{1-x}{2}}\right)
 * \f]
 * 
 * ## 实现细节
 * 
 * ### 输入范围划分
 * 
 * 代码根据输入x的大小分为多个区间，每个区间采用不同的计算方法：
 * 
 * 1. **极小值** （|x| < 2^-26）：直接返回x
 * 2. **小值** （2^-26 ≤ |x| < 2^-3）：泰勒级数展开
 * 3. **中等值** （0.125 ≤ |x| < 0.96875）：查表+多项式逼近
 * 4. **接近1的值** （0.96875 ≤ |x| < 1）：特殊变换处理
 * 5. **边界值** （|x| = 1）：直接返回π/2或-π/2
 * 6. **非法值** （|x| > 1）：返回NaN
 * 
 * ### 查表结构
 * 
 * `__IBM_asncs_table`是一个大型查找表，存储了不同区间内的多项式和参考值。表的结构根据输入范围不同而变化：
 * 
 * - 对于0.125 ≤ |x| < 0.5：11个条目一组
 * - 对于0.5 ≤ |x| < 0.75：3个条目一组
 * - 对于0.75 ≤ |x| < 0.921875：13个条目一组
 * - 对于0.921875 ≤ |x| < 0.953125：14个条目一组
 * - 对于0.953125 ≤ |x| < 0.96875：15个条目一组
 * 
 * ### 高精度计算
 * 
 * 在接近1的区域，代码使用了高精度计算技术：
 * 
 * 1. 计算平方根使用近似方法：
 * ```c
 * t = __IBM_inroot_table[...] * powtwo[...];  // 初始近似
 * r = 1.0 - t * t * z;
 * t = t * (rt0 + r * (rt1 + r * (rt2 + r * rt3)));  // 多项式修正
 * ```
 * 
 * 2. 使用双精度补偿技术减少舍入误差：
 * ```c
 * y = (c + t24) - t24;  // 高精度部分
 * cc = (z - y * y) / (t + y);  // 补偿部分
 * ```
 * 
 * ### 常数定义
 * 
 * 代码中定义了几个重要常数：
 * ```c
 * hp0 = 1.5707963267948966;    // π/2
 * hp1 = 6.123233995736766e-17; // π/2的高精度补偿部分
 * ```
 * 
 * ## 反余弦实现
 * 
 * 反余弦函数`__IBM_ACOS64F`实际上是基于反正弦函数实现的，利用了数学关系：
 * 
 * \f[
 * \arccos(x) = \frac{\pi}{2} - \arcsin(x)
 * \f]
 * 
 * 对于不同区间，代码进行了相应的优化和调整，以保持高精度。
 * 
 * ## 误差控制
 * 
 * 代码注释表明，该实现的最大ULP（Unit in the Last Place）误差约为0.516，这意味着结果与精确值的差异不超过0.516个最小精度单位，属于高精度实现。
 * 
 * 这种实现结合了多种数值计算方法，针对不同输入范围采用最优策略，在保证性能的同时实现了高精度。
 *
 * @{
 */

/**
 * @brief 角度单位转换
 */
struct Angle
{
    constexpr static const float64 Turns    = 360.l;    ///< 圆周角（360度）
    constexpr static const float64 Degrees  = 1.l;      ///< 角度（基本单位）
    constexpr static const float64 Radians  = 57.295779513082320876798154814105l; ///< 弧度
    constexpr static const float64 Gradians = 0.9l;     ///< 百分度

    float64 Data; ///< 角度存储值

    Angle() : Data(_NoDataDbl) {}
    Angle(float64 _Data) : Data(_Data) {}

    /// 从圆周角构造
    static Angle FromTurns(float64 _Turns) {return Angle{_Turns * Turns};}
    /// 从角度构造
    static Angle FromDegrees(float64 _Degs) {return Angle{_Degs * Degrees};}
    /// 从弧度构造
    static Angle FromRadians(float64 _Rads) {return Angle{_Rads * Radians};}
    /// 从百分度构造
    static Angle FromGradians(float64 _Gons) {return Angle{_Gons * Gradians};}

    /// 转换为圆周角
    constexpr float64 ToTurns()const {return Data / Turns;}
    /// 转换为角度
    constexpr float64 ToDegrees()const {return Data / Degrees;}
    /// 转换为弧度
    constexpr float64 ToRadians()const {return Data / Radians;}
    /// 转换为百分度
    constexpr float64 ToGradians()const {return Data / Gradians;}

    operator float64() {return Data;} ///< 隐式转换为浮点数
};

/**
 * @brief 计算正弦函数
 * @param _X 角度值（实函数使用度，复函数使用弧度）
 * @return 正弦值
 */
float64 __cdecl sin(Angle _X);
/// @see sin
complex64 __cdecl sinc(complex64 _X);

/**
 * @brief 计算向量各元素的正弦值
 * @tparam N 向量维度
 * @param _X 角度向量
 * @return 各元素正弦值向量
 */
template<std::size_t N>
fvec<N> __cdecl sin(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE sin(_X[i]))
}

/**
 * @brief 计算余弦函数
 * @param _X 角度值（实函数使用度，复函数使用弧度）
 * @return 余弦值
 */
float64 __cdecl cos(Angle _X);
/// @see cos
complex64 __cdecl cosc(complex64 _X);

/**
 * @brief 计算向量各元素的余弦值
 * @tparam N 向量维度
 * @param _X 角度向量
 * @return 各元素余弦值向量
 */
template<std::size_t N>
fvec<N> __cdecl cos(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE cos(_X[i]))
}

/**
 * @brief 计算正切函数
 * @param _X 角度值（实函数使用度，复函数使用弧度）
 * @return 正切值
 */
float64 __cdecl tan(Angle _X);
/// @see tan
complex64 __cdecl tanc(complex64 _X);

/**
 * @brief 计算向量各元素的正切值
 * @tparam N 向量维度
 * @param _X 角度向量
 * @return 各元素正切值向量
 */
template<std::size_t N>
fvec<N> __cdecl tan(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE tan(_X[i]))
}

/**
 * @brief 计算余切函数
 * @param _X 角度值（实函数使用度，复函数使用弧度）
 * @return 余切值
 */
float64 __cdecl ctg(Angle _X);
/// @see ctg
complex64 __cdecl ctgc(complex64 _X);

/**
 * @brief 计算向量各元素的余切值
 * @tparam N 向量维度
 * @param _X 角度向量
 * @return 各元素余切值向量
 */
template<std::size_t N>
fvec<N> __cdecl ctg(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE ctg(_X[i]))
}

/**
 * @brief 计算正割函数
 * @param _X 角度值（实函数使用度，复函数使用弧度）
 * @return 正割值
 */
float64 __cdecl sec(Angle _X);
/// @see sec
complex64 __cdecl secc(complex64 _X);

/**
 * @brief 计算向量各元素的正割值
 * @tparam N 向量维度
 * @param _X 角度向量
 * @return 各元素正割值向量
 */
template<std::size_t N>
fvec<N> __cdecl sec(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE sec(_X[i]))
}

/**
 * @brief 计算余割函数
 * @param _X 角度值（实函数使用度，复函数使用弧度）
 * @return 余割值
 */
float64 __cdecl csc(Angle _X);
/// @see csc
complex64 __cdecl cscc(complex64 _X);

/**
 * @brief 计算向量各元素的余割值
 * @tparam N 向量维度
 * @param _X 角度向量
 * @return 各元素余割值向量
 */
template<std::size_t N>
fvec<N> __cdecl csc(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE csc(_X[i]))
}

/**
 * @brief 确定角度所在象限
 * @param _X 角度值
 * @return 象限编号：
 *  0 -> x轴正方向,
 *  1 -> 第一象限,
 *  2 -> y轴正方向,
 *  3 -> 第二象限,
 *  4 -> x轴负方向,
 *  5 -> 第三象限,
 *  6 -> y轴负方向,
 *  7 -> 第四象限
 */
int64 __cdecl Quadrant(Angle _X);

/**
 * @brief 计算坐标点的反正切值
 * @param _Y y坐标值
 * @param _X x坐标值
 * @return 点(_X,_Y)的角度（范围：-180°到180°）
 */
Angle __cdecl Arctan2(float64 _Y, float64 _X);

/**
 * @brief 计算反正弦函数
 * @param _X 正弦值（范围：[-1,1]）
 * @return 角度值（范围：[-90°,90°]）
 */
Angle __cdecl arcsin(float64 _X);

/**
 * @brief 计算复数的反正弦
 * @param _X 复数值
 * @param _N 平方根分支索引（默认为0）
 * @param _K 分支选择偏移量（默认为0）
 * @return 复数的反正弦值
 */
complex64 __cdecl arcsinc(complex64 _X, int _N = 0, int64 _K = 0);

/**
 * @brief 计算向量各元素的反正弦
 * @tparam N 向量维度
 * @param _X 输入向量
 * @return 各元素反正弦结果向量
 */
template<std::size_t N>
fvec<N> __cdecl arcsin(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE arcsin(_X[i]))
}

/**
 * @brief 计算反余弦函数
 * @param _X 余弦值（范围：[-1,1]）
 * @return 角度值（范围：[0°,180°]）
 */
Angle __cdecl arccos(float64 _X);

/**
 * @brief 计算复数的反余弦
 * @param _X 复数值
 * @param _N 平方根分支索引（默认为0）
 * @param _K 分支选择偏移量（默认为0）
 * @return 复数的反余弦值
 */
complex64 __cdecl arccosc(complex64 _X, int _N = 0, int64 _K = 0);

/**
 * @brief 计算向量各元素的反余弦
 * @tparam N 向量维度
 * @param _X 输入向量
 * @return 各元素反余弦结果向量
 */
template<std::size_t N>
fvec<N> __cdecl arccos(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE arccos(_X[i]))
}

/**
 * @brief 计算反正切函数
 * @param _X 正切值
 * @return 角度值（范围：[-90°,90°]）
 */
Angle __cdecl arctan(float64 _X);

/**
 * @brief 计算复数的反正切
 * @param _X 复数值
 * @param _K 分支选择偏移量（默认为0）
 * @return 复数的反正切值
 */
complex64 __cdecl arctanc(complex64 _X, int64 _K = 0);

/**
 * @brief 计算向量各元素的反正切
 * @tparam N 向量维度
 * @param _X 输入向量
 * @return 各元素反正切结果向量
 */
template<std::size_t N>
fvec<N> __cdecl arctan(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE arctan(_X[i]))
}

/**
 * @brief 计算反余切函数
 * @param _X 余切值
 * @return 角度值（范围：[0°,180°]）
 */
Angle __cdecl arcctg(float64 _X);
/// @see arcctg
complex64 __cdecl arcctgc(complex64 _X, int64 _K = 0);
/// @see arcctg
template<std::size_t N>
fvec<N> __cdecl arcctg(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE arcctg(_X[i]))
}

/**
 * @brief 计算反正割函数
 * @param _X 正割值
 * @return 角度值（范围：[0°,180°]）
 */
Angle __cdecl arcsec(float64 _X);
/// @see arcsec
complex64 __cdecl arcsecc(complex64 _X, int _N = 0, int64 _K = 0);
/// @see arcsec
template<std::size_t N>
fvec<N> __cdecl arcsec(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE arcsec(_X[i]))
}

/**
 * @brief 计算反正割函数
 * @param _X 正割值
 * @return 角度值（范围：[-90°,90°]）
 */
Angle __cdecl arccsc(float64 _X);
/// @see arccsc
complex64 __cdecl arccscc(complex64 _X, int _N = 0, int64 _K = 0);
/// @see arccsc
template<std::size_t N>
fvec<N> __cdecl arccsc(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE arccsc(_X[i]))
}

/**@}*/

/**
 * @defgroup MathFuncsGamma Gamma函数
 * @ingroup MathFuncs
 * @todo 此函数目前尚未实现，如有需要请使用::tgamma(x)
 * @{
 */

/**
 * @brief 计算Gamma函数
 * @param _X 输入值
 * @return Gamma函数值
 */
float64 gamma(float64 _X);

/**
 * @brief 计算向量各元素的Gamma函数
 * @tparam N 向量维度
 * @param _X 输入向量
 * @return 各元素Gamma结果向量
 */
template<std::size_t N>
fvec<N> __cdecl gamma(fvec<N> _X);

/**@}*/

/////////////////////////////////////////////////////////////////
/// @defgroup MathSolvePoly 解方程
/// @ingroup MathFuncs
/// @brief 实现从一次到高次多项式方程的求解算法
/// @details
/// 求解由以下形式定义的多项式方程的解(降幂排序)：
///      \f[\sum^{n}_{i=0} (a_i \cdot x^{n - i - 1}) = 0\f]
///                                                             
/// 其中：                                                      
///   - 一次和二次方程使用求根公式                              
///   - 三次方程使用范盛金算法                                  
///   - 四次方程使用沈天珩算法                                  
///   - 五次及以上使用杜兰德-肯纳迭代                           
/// @{
/////////////////////////////////////////////////////////////////

using InputArray  = const std::vector<float64>&;  ///< 输入系数数组类型
using OutputArray = std::vector<complex64>&;      ///< 输出解数组类型

/**
 * @brief 验证输入输出参数的有效性
 * @param Coeffs 多项式系数数组(降幂排序)
 * @param Roots  解输出数组
 * @param Power  多项式阶数
 * @exception 当输入无效时抛出异常
 */
void __Verify_Input_Output(InputArray Coeffs, OutputArray Roots, uint64 Power) noexcept(0);

/**
 * @brief 求解一元一次方程 \f$ax + b = 0\f$
 * @param Coeffs 系数数组 [a, b]
 * @param Roots  输出解数组(1个元素)
 * @return 实际找到的解数量
 */
int SolveLinear(InputArray Coeffs, OutputArray Roots);

/**
 * @brief 求解一元二次方程 \f$ax^2 + bx + c = 0\f$
 * @param Coeffs 系数数组 [a, b, c]
 * @param Roots  输出解数组(2个元素)
 * @return 实际找到的解数量
 */
int SolveQuadratic(InputArray Coeffs, OutputArray Roots);

/**
 * @brief 求解一元三次方程 \f$ax^3 + bx^2 + cx + d = 0\f$ (范盛金算法)
 * @details
 * 求解三次方程有两种算法，分别为卡尔丹公式和盛金公式。但是后来有人发现这两种算法都有问题：
 *  1. 卡尔丹公式没有认真对待复数解，且算法中含有大量复数开方。
 *  2. 范盛金的算法仍有部分缺陷需要更正
 * 
 * 对此2019年田红亮基于范盛金的成果进行了修正，下文采用此方法。
 *
 * @note 本算法的原作者范盛金于2018年9月6日晚在东莞离世，享年63岁
 *
 * @par 参考文献
 *  [1] 范盛金.一元三次方程的新求根公式与新判别法[J].海南师范学院学报(自然科学版),1989,2(2):91-98.<br>
 *  [2] 田红亮.一元三次方程根的解法[J].湖北工程学院学报,2019,39(06):97-105.<br>
 *  [3] 雪鳕.卡丹公式欺骗了五百年所有数学家[EB/OL].(2013-11-08)[2019-08-18]. http://blog.sina.com.cn/s/blog_6aa947af0101dy4f.html<br>
 *  [4] 沈天珩.一元高次方程寻根记杂谈[EB/OL].(2020-02-03)[2025-05-09]. https://zhuanlan.zhihu.com/p/104832975<br>
 *  [5] 毕式明,潘俊宇.“盛金公式”发明者、著名数学家范盛金辞世[EB/OL].南方Plus,(2018-09-08)[2025-05-09] https://www.sohu.com/a/252667814_100116740<br>
 *
 * @param Coeffs    系数数组 [a, b, c, d]
 * @param Roots     输出解数组(3个元素)
 * @param Tolerence 误差容限(\f$10^{-Tolerence}\f$)，默认10
 * @return 实际找到的解数量
 */
int SolveCubic(InputArray Coeffs, OutputArray Roots, float64 Tolerence = 10);

/**
 * @brief 求解一元四次方程 \f$ax^4 + bx^3 + cx^2 + dx + e = 0\f$ (沈天珩算法)
 * @details
 * @subsubsection 一些碎碎念
 * <b>丹霞</b>：历史上最先提出一般形式的四次方程的解法的是卡尔丹的学生费拉里。但是其公式复杂度极高，对此，沈天珩对公式进行了简化。同样的，相比费拉里算法，沈天珩算法中不会出现复数开方。<br>
 * 沈天珩在知乎上另一篇四次方程求根公式证明文章下的评论说，他并没有对此公开发表过论文，仅仅只是在他本科辅修的毕设中提到过这个算法。对于计算机求解代码而言，不该出现根号时应尽量避免，万不得已出现根号最好避免复数开方，这些都是优势所在。毕竟像沈天珩算法这种除了求解以外还能判虚实判重根的算法太少了。
 * @param Coeffs    系数数组 [a, b, c, d, e]
 * @param Roots     输出解数组(4个元素)
 * @param Tolerence 误差容限(\f$10^{-Tolerence}\f$)，默认10
 * @return 实际找到的解数量
 */
int SolveQuartic(InputArray Coeffs, OutputArray Roots, float64 Tolerence = 10);

/**
 * @page SolvePolyHistory 一些碎碎念
 * @details
 * <b>丹霞</b>：寻找五次或以上方程的解法是一个困扰了数学家们300多年的问题。历史上无数的数学家试图推导这些多项式方程求解算法，但无一例外都失败了。1824年，阿贝尔等人证明了五次及更高次的多项式方程没有一般的代数解法，即这样的方程不能由方程的系数经有限次四则运算和开方运算求解。<br>
 * 1926年，苏家驹发表了一篇文章，声称自己发现了一个五次方程的求解算法。随后华罗庚在看了苏家驹的文章以后尝试继续推导六次方程的求解算法，这意味着他要尝试否定先前阿贝尔等人已经证实的“五次以上方程的解无法用系数表示”这一理论，但是当他看了阿贝尔的论文以后，认为其“条理精严，无懈可击”。随后他就转而开始尝试在苏家驹的论文中寻找漏洞，最终发表了他的成名之作《苏家驹之代数的五次方程式解法不能成立之理由》。<br>
 * 阿贝尔定律已经证实五次以上方程的解无法使用系数组成的根式表示。也就是这些方程没有通用的求解算法。但是，这并不意味着它们完全没办法求解，此处使用杜兰德-肯纳算法求解这些方程。<br>
 * 
 * @par 参考文献：
 *   [1] 苏家驹. 代数的五次方程式之解法[J]. 学艺, 1926.<br>
 *   [2] 华罗庚. 苏家驹之代数的五次方程式解法不能成立之理由[J]. 科学, 1930.<br>
 *   [3] Durand-Kerner method. Wikipedia. https://en.wikipedia.org/wiki/Durand-Kerner_method
 *
 * @note
 * 事实上，五次方程的解无法用根式表示的根本原因，是五次方程经过计算、消元后会变成特殊的六次方程，而那个特殊六次方程是否存在有理数根也是判断方程的解能否用根式表示的依据。<br>
 * 而且六次方程用相同方法经过多次变换及消元后最终会得到10次或15次方程<br>
 * <i>「所以别去对这些>4次的一般形式硬消了，消不掉的。」</i>
 */

/**
 * @interface SolvePolyRoutine
 * @brief 多项式求解算法接口
 */
struct SolvePolyRoutine
{
public:
    /**
     * @brief 执行多项式求解
     * @param Coeffs 多项式系数数组
     * @param Roots  解输出数组
     * @return 迭代次数或状态码
     */
    virtual int Run(InputArray Coeffs, OutputArray Roots) const = 0;
};

/**
 * @class DurandKernerSolvePoly
 * @brief 杜兰德-肯纳多项式求解器实现
 */
class DurandKernerSolvePoly : public SolvePolyRoutine
{
public:
    std::vector<complex64> InitValue;          ///< 初始迭代值数组
    float64 AbsoluteTolerence = 14;            ///< 绝对容差(10^(-14))
    float64 RelativeTolerence = 14;            ///< 相对容差(10^(-14))
    float64 MaxIter = 3;                       ///< 最大迭代次数(指数级, 实际为10^MaxIter)

    /**
     * @brief 生成幂次初始值
     * @param Power  多项式阶数
     * @param IValue 初始复数值(默认0.4+0.9i)
     * @return 初始值数组
     */
    static std::vector<complex64> GetExponentialInitValue(float64 Power, complex64 IValue = 0.4 + 0.9i);

    /**
     * @brief 生成圆形分布初始值
     * @param Coeffs 多项式系数
     * @return 初始值数组
     */
    static std::vector<complex64> GetCircularInitValue(InputArray Coeffs);

    /**
     * @brief 生成同伦连续法初始值
     * @param Coeffs 多项式系数
     * @param a      同伦参数(默认0.5)
     * @return 初始值数组
     * @see 算法来源：江源.应用同伦方法加速DURAND-KERNER算法[D].中国科学技术大学,2011.
     */
    static std::vector<complex64> GetHomotopicInitValue(InputArray Coeffs, float64 a = 0.5);

    /**
     * @brief 执行杜兰德-肯纳迭代
     * @param Coeffs 多项式系数数组
     * @param Roots  解输出数组
     * @return 实际迭代次数
     */
    int Run(InputArray Coeffs, OutputArray Roots) const override;
};

/**
 * @brief 通用多项式求解函数
 * @param Coeffs  多项式系数数组(降幂排序)
 * @param Roots   解输出数组
 * @param Routine 求解算法实例(默认DurandKerner)
 * @return 迭代次数
 * @see https://en.wikipedia.org/wiki/Durand-Kerner_method
 */
uint64 SolvePoly(
    InputArray Coeffs, 
    OutputArray Roots, 
    const SolvePolyRoutine& Routine = DurandKernerSolvePoly()
);

/// @}

/**
 * @defgroup MathFuncsHyp 双曲函数
 * @ingroup MathFuncs
 * @brief 双曲函数包含sinh, cosh, tanh, coth, sech和csch这六个函数和它们的反函数
 * @details 这些函数的实现均采用Sun Microsystems的方案。
 * @{
 */

/**
 * @brief 计算双曲正弦函数
 * @param _X 输入值(双精度浮点)
 * @return 双曲正弦值
 */
float64 __cdecl sinh(float64 _X);

/**
 * @brief 计算复数双曲正弦函数
 * @param _X 输入值(复数)
 * @return 复数双曲正弦值
 * @todo 待实现
 */
complex64 __cdecl sinhc(complex64 _X); // TODO

/**
 * @brief 计算向量双曲正弦函数
 * @tparam N 向量长度
 * @param _X 输入向量
 * @return 双曲正弦向量
 */
template<std::size_t N>
fvec<N> __cdecl sinh(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE sinh(_X[i]))
}

/**
 * @brief 计算双曲余弦函数
 * @param _X 输入值(双精度浮点)
 * @return 双曲余弦值
 */
float64 __cdecl cosh(float64 _X);

/**
 * @brief 计算复数双曲余弦函数
 * @param _X 输入值(复数)
 * @return 复数双曲余弦值
 * @todo 待实现
 */
complex64 __cdecl coshc(complex64 _X); // TODO

/**
 * @brief 计算向量双曲余弦函数
 * @tparam N 向量长度
 * @param _X 输入向量
 * @return 双曲余弦向量
 */
template<std::size_t N>
fvec<N> __cdecl cosh(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE cosh(_X[i]))
}

/**
 * @brief 计算双曲正切函数
 * @param _X 输入值(双精度浮点)
 * @return 双曲正切值
 */
float64 __cdecl tanh(float64 _X);

/**
 * @brief 计算复数双曲正切函数
 * @param _X 输入值(复数)
 * @return 复数双曲正切值
 * @todo 待实现
 */
complex64 __cdecl tanhc(complex64 _X); // TODO

/**
 * @brief 计算向量双曲正切函数
 * @tparam N 向量长度
 * @param _X 输入向量
 * @return 双曲正切向量
 */
template<std::size_t N>
fvec<N> __cdecl tanh(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE tanh(_X[i]))
}

/**
 * @brief 计算双曲余切函数
 * @param _X 输入值(双精度浮点)
 * @return 双曲余切值
 */
float64 __cdecl coth(float64 _X);

/**
 * @brief 计算复数双曲余切函数
 * @param _X 输入值(复数)
 * @return 复数双曲余切值
 * @todo 待实现
 */
complex64 __cdecl coth(complex64 _X); // TODO

/**
 * @brief 计算向量双曲余切函数
 * @tparam N 向量长度
 * @param _X 输入向量
 * @return 双曲余切向量
 */
template<std::size_t N>
fvec<N> __cdecl coth(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE coth(_X[i]))
}

/**
 * @brief 计算双曲正割函数
 * @param _X 输入值(双精度浮点)
 * @return 双曲正割值
 */
float64 __cdecl sech(float64 _X);

/**
 * @brief 计算复数双曲正割函数
 * @param _X 输入值(复数)
 * @return 复数双曲正割值
 * @todo 待实现
 */
complex64 __cdecl sechc(complex64 _X); // TODO

/**
 * @brief 计算向量双曲正割函数
 * @tparam N 向量长度
 * @param _X 输入向量
 * @return 双曲正割向量
 */
template<std::size_t N>
fvec<N> __cdecl sech(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE sech(_X[i]))
}

/**
 * @brief 计算双曲余割函数
 * @param _X 输入值(双精度浮点)
 * @return 双曲余割值
 */
float64 __cdecl csch(float64 _X);

/**
 * @brief 计算复数双曲余割函数
 * @param _X 输入值(复数)
 * @return 复数双曲余割值
 * @todo 待实现
 */
complex64 __cdecl cschc(complex64 _X); // TODO

/**
 * @brief 计算向量双曲余割函数
 * @tparam N 向量长度
 * @param _X 输入向量
 * @return 双曲余割向量
 */
template<std::size_t N>
fvec<N> __cdecl csch(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE csch(_X[i]))
}

/**
 * @brief 计算反双曲正弦函数
 * @param _X 输入值(双精度浮点)
 * @return 反双曲正弦值
 */
float64 __cdecl arsinh(float64 _X);

/**
 * @brief 计算复数反双曲正弦函数
 * @param _X 输入值(复数)
 * @param _N 分支选择参数(默认为0)
 * @param _K 分支选择参数(默认为0)
 * @return 复数反双曲正弦值
 * @todo 待实现
 */
complex64 __cdecl arsinhc(complex64 _X, int _N = 0, int64 _K = 0); // TODO

/**
 * @brief 计算向量反双曲正弦函数
 * @tparam N 向量长度
 * @param _X 输入向量
 * @return 反双曲正弦向量
 */
template<std::size_t N>
fvec<N> __cdecl arsinh(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE arsinh(_X[i]))
}

/**
 * @brief 计算反双曲余弦函数
 * @param _X 输入值(双精度浮点)
 * @return 反双曲余弦值
 */
float64 __cdecl arcosh(float64 _X);

/**
 * @brief 计算复数反双曲余弦函数
 * @param _X 输入值(复数)
 * @param _N 分支选择参数(默认为0)
 * @param _K 分支选择参数(默认为0)
 * @return 复数反双曲余弦值
 * @todo 待实现
 */
complex64 __cdecl arcoshc(complex64 _X, int _N = 0, int64 _K = 0); // TODO

/**
 * @brief 计算向量反双曲余弦函数
 * @tparam N 向量长度
 * @param _X 输入向量
 * @return 反双曲余弦向量
 */
template<std::size_t N>
fvec<N> __cdecl arcosh(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE arcosh(_X[i]))
}

/**
 * @brief 计算反双曲正切函数
 * @param _X 输入值(双精度浮点)
 * @return 反双曲正切值
 */
float64 __cdecl artanh(float64 _X);

/**
 * @brief 计算复数反双曲正切函数
 * @param _X 输入值(复数)
 * @param _K 分支选择参数(默认为0)
 * @return 复数反双曲正切值
 * @todo 待实现
 */
complex64 __cdecl artanhc(complex64 _X, int64 _K = 0); // TODO

/**
 * @brief 计算向量反双曲正切函数
 * @tparam N 向量长度
 * @param _X 输入向量
 * @return 反双曲正切向量
 */
template<std::size_t N>
fvec<N> __cdecl artanh(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE artanh(_X[i]))
}

/**
 * @brief 计算反双曲余切函数
 * @param _X 输入值(双精度浮点)
 * @return 反双曲余切值
 */
float64 __cdecl arcoth(float64 _X);

/**
 * @brief 计算复数反双曲余切函数
 * @param _X 输入值(复数)
 * @param _K 分支选择参数(默认为0)
 * @return 复数反双曲余切值
 * @todo 待实现
 */
complex64 __cdecl arcothc(complex64 _X, int64 _K = 0); // TODO

/**
 * @brief 计算向量反双曲余切函数
 * @tparam N 向量长度
 * @param _X 输入向量
 * @return 反双曲余切向量
 */
template<std::size_t N>
fvec<N> __cdecl arcoth(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE arcoth(_X[i]))
}

/**
 * @brief 计算反双曲正割函数
 * @param _X 输入值(双精度浮点)
 * @return 反双曲正割值
 */
float64 __cdecl arsech(float64 _X);

/**
 * @brief 计算复数反双曲正割函数
 * @param _X 输入值(复数)
 * @param _N 分支选择参数(默认为0)
 * @param _K 分支选择参数(默认为0)
 * @return 复数反双曲正割值
 * @todo 待实现
 */
complex64 __cdecl arsechc(complex64 _X, int _N = 0, int64 _K = 0); // TODO

/**
 * @brief 计算向量反双曲正割函数
 * @tparam N 向量长度
 * @param _X 输入向量
 * @return 反双曲正割向量
 */
template<std::size_t N>
fvec<N> __cdecl arsech(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE arsech(_X[i]))
}

/**
 * @brief 计算反双曲余割函数
 * @param _X 输入值(双精度浮点)
 * @return 反双曲余割值
 */
float64 __cdecl arcsch(float64 _X);

/**
 * @brief 计算复数反双曲余割函数
 * @param _X 输入值(复数)
 * @param _N 分支选择参数(默认为0)
 * @param _K 分支选择参数(默认为0)
 * @return 复数反双曲余割值
 * @todo 待实现
 */
complex64 __cdecl arcschc(complex64 _X, int _N = 0, int64 _K = 0); // TODO

/**
 * @brief 计算向量反双曲余割函数
 * @tparam N 向量长度
 * @param _X 输入向量
 * @return 反双曲余割向量
 */
template<std::size_t N>
fvec<N> __cdecl arcsch(fvec<N> _X)
{
    __stelcxx_array_math_function_body(f, i, _CSE arcsch(_X[i]))
}

/**@}*/