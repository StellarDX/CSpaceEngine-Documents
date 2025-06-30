/**
 * @file MathFuncs.h
 * @defgroup MathFuncs 数学函数
 * @ingroup Base
 * @brief CSpaceEngine数学函数库
 * @details 包含常用数学函数的声明，包括指数、对数、幂、三角函数等
 * 
 * @copyright 本文件及相关源文件中的数学函数取自多个开源数学库（如glibc的数学函数，LGPL-2.1许可；IBM精确数学库，GPL-2.0/LGPL-2.1?许可等）。无论何种情况，其版权均归原作者所有。您可自由使用此处及相关源文件中的内容，但不提供任何支持或担保。
 *
 * @note <i>「宇宙之大，粒子之微。火箭之速，化工之巧。星球之变，生命之谜。日用其繁，无不数学。」</i>
 */

/**
 * @defgroup MathFuncsExp 指数函数
 * @ingroup MathFuncs
 * @brief 指数函数，包含exp
 * @details
 * **以下内容为AI智能解析，仅供参考**
 * 
 * `exp(x)` 实现了双精度浮点数（`float64`）的指数函数，即计算 \f$ e^x \f$。最初记载于 GLibC 中，采用范围缩减（range reduction）和多项式逼近（polynomial approximation）技术，结合预计算表（lookup table）确保高效率和精度。以下是核心原理的解析：
 * 
 * ### **1. 特殊情况处理**
 * - **极小值处理** ：当 \f$ x \f$ 极小（接近 0）时，直接返回 \f$ 1 + x \f$（避免下溢）。
 * - **无穷大处理** ：
 * - \f$ x = +\infty \f$：返回 \f$ +\infty \f$。
 * - \f$ x = -\infty \f$：返回 \f$ 0 \f$。
 * - **NaN 处理** ：若 \f$ x \f$ 是 NaN，返回 \f$ x \f$（遵循 IEEE 754）。
 * 
 * ### **2. 范围缩减（Range Reduction）**
 * 核心思想是将 \f$ e^x \f$ 分解为：
 * \f[
 * e^x = 2^k \cdot e^r
 * \f]
 * 其中 \f$ k \f$ 是整数，\f$ r \f$ 是余数（满足 \f$ |r| \leq \frac{\ln 2}{2N} \f$，\f$ N = 128 \f$）。
 * 
 * #### **步骤：**
 * 1. **计算k** ：
 *  \f[
 *  k = \text{round}\left( x \cdot \frac{N}{\ln 2} \right)
 *  \f]
 *  使用浮点技巧 `z = x * InvLn2N + Shift`（`Shift = 0x1.8p52`）后舍入，再减去 `Shift` 得到整数 \f$ k \f$（`kd`）和索引 `ki`。
 * 
 * 2. **计算余数r** ：
 *  \f[
 *  r = x - k \cdot \left( \frac{\ln 2}{N} \right)
 *  \f]
 *  为减少误差，使用高低双精度乘法：
 *  ```cpp
 *  r = x + kd * NegLn2hiN + kd * NegLn2loN; // 高精度计算
 *  ```
 * 
 * ### **3. 查表获取 2^(k/N)**
 * - **查表索引** ：`idx = 2 * (ki % N)`（`N = 128`）。
 * - **表结构** ：`__Exp64f_table` 存储预计算的 \f$ 2^{j/N} \f$（\f$ j = 0 \dots 127 \f$)，每项含两个 64 位整数：
 * - `table[idx]` ：尾数部分（`tail`）。
 * - `table[idx + 1]` ：指数基值。
 * - **合成 `scale`** ：
 * ```cpp
 * sbits = table[idx + 1] + (ki << (52 - 7)); // 合并指数
 * scale = __Float64::FromBytes(sbits);       // 转为浮点数
 * ```
 * 此时 `scale` 近似 \f$ 2^{k/N} \f$。
 * 
 * ### **4. 计算 e^r 的多项式逼近**
 * 因 \f$ r \f$ 很小，用泰勒级数近似 \f$ e^r \f$：
 * \f[
 * e^r \approx 1 + r + \frac{r^2}{2!} + \frac{r^3}{3!} + \frac{r^4}{4!} + \frac{r^5}{5!}
 * \f]
 * 代码使用秦九韶算法（Horner 法）高效计算：
 * ```cpp
 * tmp = tail + r * (C1 + r * (C2 + r * (C3 + r * (C4 + r * C5))));
 * ```
 * 其中 `tail` 是查表值（补偿精度），系数 `C1`~`C5` 为 \f$ \frac{1}{n!} \f$。
 * 
 * ### **5. 合并结果**
 * 最终结果为：
 * \f[
 * e^x = \text{scale} \times (1 + \text{tmp})
 * \f]
 * 实际计算避免额外舍入：
 * ```cpp
 * result = scale + scale * tmp;
 * ```
 * 
 * ### **6. 边界处理（特殊路径）**
 * 当 |x| 极大时：
 * - **k > 0（可能上溢）** ：
 * ```cpp
 * sbits -= 1009ull << 52;      // 调整指数
 * result = 0x1p1009 * (scale + scale * tmp);
 * ```
 * - **k < 0（可能下溢）** ：
 * ```cpp
 * sbits += 1022ull << 52;      // 调整指数
 * y = scale + scale * tmp;
 * // 精细处理下溢舍入
 * result = 0x1p-1022 * y;
 * ```
 * 
 * ### **复数指数函数 `expc`**
 * 对复数 \f$ z = a + bi \f$，利用欧拉公式：
 * \f[
 * e^z = e^a \cdot (\cos b + i \sin b)
 * \f]
 * 代码实现：
 * ```cpp
 * complex64 expc(complex64 z)
 * {
 *     return exp(z.real()) * (cos(b) + 1i * sin(b)); // b = z.imag()
 * }
 * ```
 * 
 * ### **总结**
 * - **精度** ：通过范围缩减将问题限制在 \f$[- \frac{\ln 2}{256}, \frac{\ln 2}{256}]\f$，结合 5 阶多项式逼近，确保高精度。
 * - **性能** ：预计算表和 Horner 法减少计算量。
 * - **健壮性** ：特殊值处理完备，避免上溢/下溢。
 * 
 * 此实现是数值计算的经典实践，平衡了效率与精度。
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
 * **以下内容为AI智能解析，仅供参考**
 *
 * ### `ln` 函数实现原理分析
 * 
 * `ln(x)` 实现了双精度浮点数的自然对数函数 ，为 GLibC 中 Stephen L. Moshier 的算法（LGPL 许可）。以下是核心原理分析：
 * 
 * ### **1. 核心算法原理**
 * 算法基于以下数学分解：
 * \f[
 * x = 2^k \cdot z
 * \f]
 * 其中：
 * - \f$k\f$ 是指数（整数）
 * - \f$z\f$ 是尾数（归一化到区间 \f$[1.6 \cdot 2^{-1}, 1.6 \cdot 2^0] \approx [0.75, 1.5]\f$）
 * 
 * 对数的计算转化为：
 * \f[
 * \ln(x) = k \cdot \ln(2) + \ln(z)
 * \f]
 * 尾数 \f$z\f$ 进一步分解为：
 * \f[
 * \ln(z) = \ln(c) + \ln\left(\frac{z}{c}\right)
 * \f]
 * - \f$c\f$ 是预计算的子区间中心值
 * - \f$\ln(\frac{z}{c})\f$ 用多项式逼近（泰勒级数）
 * 
 * ### **2. 关键步骤**
 * #### **(1) 范围缩减**
 * - **特殊值处理** ：
 * - `x = 1`：直接返回 `0`
 * - `x < 0` 或 `NaN`：返回 `NaN`
 * - `x = +Inf`：返回 `+Inf`
 * - `x = 0`：返回 `-Inf`
 * - 次正规数：通过乘以 \f$2^52\f$ 正规化
 * 
 * - **接近 1 的输入（`|x-1| < 0.0625`）** ：
 * - 直接使用泰勒展开：
 *     \f[
 *     \ln(1+r) = r - \frac{r^2}{2} + \frac{r^3}{3} - \cdots \quad (r = x-1)
 *     \f]
 * - 分离高位 (`rhi`) 和低位 (`rlo`) 减少舍入误差
 * 
 * #### **(2) 查表与多项式逼近**
 * - **查表准备** ：
 * - 将 `z` 的区间分为 `128` 个子区间（查表索引 `i`）
 * - 预计算三项：
 *     - `invc = 1/c`（中心值的倒数）
 *     - `logc = ln(c)`（中心值的对数）
 *     - `chi` 和 `clo`（中心值 `c` 的高低双精度拆分）
 * 
 * - **计算残差 `r`** ：
 * ```c
 * r = (z - chi - clo) * invc;  // ≈ z/c - 1
 * ```
 * - 通过双精度减法保证高精度
 * 
 * - **组合结果** ：
 * \f[
 * \ln(x) = \underbrace{k \cdot \ln(2)_\text{hi} + \log c}_\text{主项} + \underbrace{r}_\text{残差} + \underbrace{\text{poly}(r)_\text{修正}}
 * \f]
 * - `ln(2)` 拆分为高位 (`ln2hi`) 和低位 (`ln2lo`) 减少舍入误差
 * - 多项式 `poly(r)` 拟合 \f$\ln(1+r) - r\f$：
 *     \f[
 *     \text{poly}(r) = r^2 \left( -\frac{1}{2} + r \left( \frac{1}{3} + r \left( -\frac{1}{4} + \cdots \right) \right) \right)
 *     \f]
 * 
 * ### **3. 误差控制**
 * **1. 表项优化** ：
 * - 通过筛选确保 ：
 *     - `k*ln2hi + logc` 无舍入误差
 *     - `z - chi - clo` 的误差 \f$< 2^{-66}\f$
 *     - `logc` 的误差最小化

 * **2. 多项式阶数** ：
 * - 主路径使用 5 阶多项式（相对误差 `≤ 0.52 ULP`）
 * - 接近 1 的路径使用 11 阶多项式（相对误差 \f$≤ 2^{-63}\f$）
 * 
 * ### **4. 性能优化**
 * - **避免分支** ：通过位操作快速提取指数/尾数
 * - **并行计算** ：多项式使用 Horner 格式减少依赖
 * - **精度补偿** ：通过 `hi`/`lo` 双精度存储中间结果
 * 
 * ### 总结
 * 该算法通过**范围缩减**将问题转化到小区间，结合 **预计算表** 提供高精度初始逼近，再用 **低阶多项式** 拟合残差。关键创新点在于：
 * 1. 精心设计的表项保证关键步骤零舍入误差
 * 2. 双精度拆分 (`chi`/`clo`, `hi`/`lo`) 抑制累积误差
 * 3. 对特殊区间（接近 1）单独优化
 * 
 * 最终在保证 `≤ 0.52 ULP` 精度的同时，实现了高效的数值计算。
 * 
 * ---
 * 
 * **以下内容为AI智能解析，仅供参考**
 *
 * ### `log` 函数实现原理分析
 * 
 * `log(x)` 实现了双精度浮点数的常用对数（以10为底）函数。以下是其核心原理的解析：
 *
 * ### 1. **特殊值处理**
 * - **零** ：返回负无穷（`NEG_INF_DOUBLE`）。
 * - **无穷大** ：返回正无穷（`POS_INF_DOUBLE`）。
 * - **负数** ：返回 NaN（`BIG_NAN_DOUBLE`）。
 * - **1.0** ：直接返回 0.0。
 *
 * ### 2. **浮点数分解**
 * - 使用 `frexp(x, &e)` 将输入 `x` 分解为尾数（`x ∈ [0.5, 1.0)`）和指数 `e`（满足 \f$ x = \text{尾数} \times 2^e \f$）。
 *
 * ### 3. **逼近策略选择**
 * 根据指数 `e` 的绝对值大小选择两种不同的逼近方法：
 * - **当 |e| > 2** ：使用  **变换法** （更精确）。
 * - **当 |e| <= 2** ：使用  **直接多项式逼近法** （高效）。
 *
 * ### **变换法（|e| > 2)**
 * #### **步骤：**
 * 1. **调整尾数区间** ：
 *  - 若 x < sqrt(0.5) ：
 *     - 指数减 1：`e -= 1`。
 *     - 计算变换：  
 *     \f$ z = x - 0.5 \f$,  
 *     \f$ y = 0.5z + 0.5 = \frac{x + 0.5}{2} \f$。
 *  - 否则：
 *     - 计算变换：  
 *     \f$ z = x - 1 \f$,  
 *     \f$ y = \frac{x + 1}{2} \f$。
 *  - 更新：\f$ x = \frac{z}{y} \f$（此时 \f$ x \f$ 在 \f$[-1, 1]\f$ 内）。
 *
 * 2. **有理分式逼近** ：
 *  - 计算 \f$ z = x^2 \f$。
 *  - 使用两个多项式计算比值：
 *     - 分子多项式 `R[6]`：`neval(z, R, 5)`。
 *     - 分母多项式 `S[6]`：`deval(z, S, 5)`。
 *  - 结果：\f$ y = x \cdot \left( z \cdot \frac{\text{分子}}{\text{分母}} \right) \f$.
 *
 * 3. **合并结果** ：
 *  - 返回：  
 *     \f[
 *     \log_{10}(x) = \underbrace{(y \cdot \text{L10EB} + x \cdot \text{L10EB} + y \cdot \text{L10EA} + x \cdot \text{L10EA})}_{\text{尾数部分}} + \underbrace{(e \cdot \text{L102B} + e \cdot \text{L102A})}_{\text{指数部分}}
 *     \f]
 *  - 其中：
 *     - \f$L10EA + L10EB ≈ \log_{10}(e)\f$（自然对数转常用对数因子）。
 *     - \f$L102A + L102B ≈ \log_{10}(2)\f$（指数部分的对数）。
 *
 * ### **直接多项式逼近法（|e| <= 2)**
 * #### **步骤：**
 * 1. **调整尾数区间** ：
 *  - 若 \f$ x < \sqrt{0.5} \f$：
 *     - 指数减 1：`e -= 1`。
 *     - 更新：\f$ x = 2x - 1 \f$（区间平移）。
 *  - 否则：\f$ x = x - 1 \f$。
 *
 * 2. **多项式逼近** ：
 *  - 计算 \f$ z = x^2 \f$。
 *  - 使用两个多项式计算比值：
 *     - 分子多项式 `P[13]`：`neval(x, P, 12)`。
 *     - 分母多项式 `Q[12]`：`deval(x, Q, 11)`。
 *  - 结果：\f$ y = x \cdot \left( z \cdot \frac{\text{分子}}{\text{分母}} \right) \f$.
 *
 * 3. **泰勒展开修正** ：
 *  - 增加泰勒展开的前两项：\f$ y = y - 0.5 \cdot z \f$.
 *
 * 4. **合并结果** ：
 *  - 同变换法公式，组合尾数和指数部分。
 *
 * ### **关键辅助函数**
 * - **`neval`** ：使用 Horner 方法计算多项式值（高效且数值稳定）。
 * - **`deval`** ：类似 `neval`，但额外加一次常数项（针对分母多项式优化）。
 *
 * ### **常数设计**
 * - **`L102A/L102B`** ：拆分 \f$\log_{10}(2)\f$ 以减少浮点舍入误差。
 * - **`L10EA/L10EB`** ：拆分 \f$\log_{10}(e)\f$（其中 \f$e\f$ 是自然常数）。
 *
 * ### **精度保证**
 * - 通过分段策略和有理分式逼近，实现在全定义域的高精度（相对误差约 \f$10^{-34}\f$ 级别）。
 * - 多项式系数通过数值优化确定，确保在每段区间内误差最小。
 *
 * ### **总结**
 * 该算法通过浮点数分解、分段逼近和常数拆分，高效且高精度地实现了 \f$\log_{10}(x)\f$。其核心思想是：
 * 1. 将问题拆解为尾数对数 + 指数对数。
 * 2. 对尾数部分，根据大小选择最优逼近方法。
 * 3. 使用有理分式减少高阶多项式计算的数值误差。
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
complex64 __cdecl lbc(complex64 _X, int64 K_OFFSET = 0);

/**
 * @brief 计算复数的自然对数（ln(z)）
 * @param _X 复数真数值
 * @param K_OFFSET 分支选择偏移量（默认为0）
 * @return 复数的自然对数
 */
complex64 __cdecl lnc(complex64 _X, int64 K_OFFSET = 0);

/**
 * @brief 计算复数的常用对数（log10(z)）
 * @param _X 复数真数值
 * @param K_OFFSET 分支选择偏移量（默认为0）
 * @return 复数的常用对数
 */
complex64 __cdecl logc(complex64 _X, int64 K_OFFSET = 0);

/**
 * @brief 计算复数的任意底数对数
 * @param _X 复数真数值
 * @param _Base 复数底数值
 * @param K_OFFSET 分支选择偏移量（默认为0）
 * @return 复数的任意底数对数
 */
complex64 __cdecl logc(complex64 _X, complex64 _Base, int64 K_OFFSET = 0);

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
 * **以下内容为AI智能解析，仅供参考**
 * 
 * ### `pow` 函数原理解析
 * 
 * `pow(x, y)` 函数计算 `x` 的 `y` 次幂（\f$x^y\f$）。其核心原理是将幂运算转化为指数和对数的组合：
 * 
 * \f[
 * x^y = e^{y \cdot \ln(x)}
 * \f]
 * 
 * #### 关键步骤：
 * 1. **特殊输入处理** ：
 *  - 检查 `x` 和 `y` 是否为 `0`、`Inf` 或 `NaN`，按 IEEE 754 规则返回结果。
 *  - 处理负数底数 `x < 0`：
 *     - 若 `y` 不是整数，返回 `NaN`（如 \f$(-2)^{0.5}\f$）。
 *     - 若 `y` 是整数，根据奇偶性确定符号（奇数为负，偶数为正）。
 * 
 * 2. **计算自然对数** ：
 *  - **分解 `x`** ：将 `x` 表示为 \f$x = 2^k \cdot z\f$，其中 \f$z \in [0.8, 1.6)\f$。
 *  - **查表近似** ：使用预计算表 `__Pow64f_ln_table` 获取 `ln(z)` 的高精度值：
 *     - `invc`：中心点倒数 \f$1/c\f$
 *     - `lnc`：\f$\ln(c)\f$ 的主部
 *     - `lnctail`：\f$\ln(c)\f$ 的尾部（补偿精度）
 *  - **泰勒展开** ：计算 \f$\ln(z/c) = \text{poly}(z/c - 1)\f$，多项式系数优化了精度。
 * 
 * 3. **计算 y * ln(x)** ：
 *  - 将 `y` 和 \f$\ln(x)\f$ 拆分为高位和低位部分，减少浮点误差：
 *     \f[
 *     y \cdot \ln(x) = (y_{\text{hi}} + y_{\text{lo}}) \cdot (\ln_{\text{hi}} + \ln_{\text{lo}})
 *     \f]
 *  - 分别计算乘积并累加，保留高精度。
 * 
 * 4. **计算指数e^sum** ：
 *  - **分解指数** ：将输入 \f$s = y \cdot \ln(x)\f$ 表示为：
 *     \f[
 *     s = \frac{k \cdot \ln 2}{N} + r, \quad N = 128
 *     \f]
 *  - **查表** ：使用 `__Exp64f_table` 获取 \f$2^{k/N}\f$ 的近似值 `scale` 和 `tail`。
 *  - **泰勒展开** ：计算 \f$e^r \approx 1 + r + \frac{r^2}{2!} + \cdots\f$。
 *  - **组合结果** ：
 *     \f[
 *     e^s \approx \text{scale} \cdot (1 + (\text{tail} + (e^r - 1)))
 *     \f]
 * 
 * 5. **处理边界条件** ：
 *  - 检测中间结果是否可能溢出/下溢，调整缩放因子。
 *  - 次正规数（Subnormal）结果特殊处理，避免双重舍入误差。
 * 
 * #### 精度优化技术 ：
 * - **高位低位拆分** ：关键计算步骤将浮点数拆分为高位和低位，减少舍入误差。
 * - **补偿精度** ：对数和指数表存储主部和尾部，补偿浮点精度损失。
 * - **定制多项式** ：泰勒级数系数针对浮点运算优化，平衡精度和性能。
 * 
 * #### 特殊值处理示例：
 * | `x`       | `y`       | 结果                 |
 * |-----------|-----------|----------------------|
 * | `NaN`     | 任意      | `NaN`                |
 * | `1`       | `NaN`     | `NaN`                |
 * | `负数`    | 非整数    | `NaN`                |
 * | `0`       | 负数      | `Inf`                |
 * | `+Inf`    | `>0`      | `+Inf`               |
 * | `+Inf`    | `<0`      | `0`                  |
 * 
 * ### 总结
 * 该 `pow` 实现通过转化为对数和指数运算，结合查表法与多项式近似，在保证 IEEE 754 精度的同时高效计算幂函数。特殊输入处理严谨，并采用多种技术减少浮点误差，适用于高性能数学库。
 *
 * @subsubsection 一些碎碎念
 * 丹霞：早期版本的glibc中数学函数使用的是IBM设计的函数，其中一些函数在输入特定值的时候会产生高到离谱的延迟，一个典型的例子就是这个pow函数。根据网络上的一些反馈，此函数在输入(x -> 1, 1.5)的时候就会出现一定的延迟，且当x越接近1延迟越高，而输入x = 1.000000000000002时，延迟高达~777毫秒，这显然是无法接受的。另外据说IBM设计的exp函数在输入[-0.00000000033301145180213807, -0.00000000033301145180369239]以内的值时也会产生比正常情况高上百倍的延迟。所以目前版本的glibc除平方根和弧度三角函数外已经换用了自行设计的数学函数。<br>
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
 * ### `sqrt` 函数原理分析（IBM Ultimate 算法）
 * 
 * `sqrt(x)` 函数实现了高精度双精度浮点数平方根计算，以下是其核心原理和步骤：
 * 
 * #### **1. 预处理输入值**
 * - **提取指数和尾数**  
 * 通过 `a.parts.msw` 获取浮点数的高32位（包含指数和部分尾数）。
 * - **规格化到 [0.5, 1) 区间**  
 * 修改指数位为 `0x3fe00000`，使输入值 \f$a\f$ 落在 \f$[0.5, 1)\f$ 范围内：
 * ```c
 * a.parts.msw = (k & 0x001fffff) | 0x3fe00000;  // k 是原始高32位
 * ```
 * 
 * #### **2. 初始近似值查找**
 * - **查表获取 1/sqrt(a)**  
 * 使用尾数的高7位作为索引，从预计算表 `__IBM_inroot_table` 中获取初始近似值 \f$t\f$：
 * ```c
 * t = __IBM_inroot_table[(k & 0x001fffff) >> 14];  // 128项查找表
 * ```
 * 
 * #### **3. 泰勒级数迭代优化**
 * - **计算残差 y**  
 * \f$y = 1 - t^2 \cdot a\f$，衡量当前近似值的误差。
 * - **多项式修正 t**  
 * 使用泰勒级数前4项优化 \f$t\f$（系数为 `rt0`-`rt3`）：
 * ```c
 * t = t * (rt0 + y * (rt1 + y * (rt2 + y * rt3));
 * ```
 * 
 * #### **4. 结果重构与修正**
 * - **指数补偿 c**  
 * 恢复原始输入的指数缩放：
 * ```c
 * c.parts.msw = 0x20000000 + ((k & 0x7fe00000) >> 1);  // 指数减半
 * ```
 * - **计算初步平方根 y = t * a**  
 * - **高位提取与误差修正**  
 * 用 `big = 134217728.0` 分离高低位，计算修正量 `del`：
 * ```c
 * hy = (y + big) - big;  // 高位部分
 * del = 0.5 * t * ((a - hy*hy) - (y - hy)*(y + hy)); // 误差修正
 * res = y + del;  // 修正后的结果
 * ```
 * 
 * #### **5. 精确舍入处理**
 * - **二次修正（必要时）**  
 * 若一次修正未收敛，通过精确乘法 `EMULV` 比较候选值：
 * ```c
 * EMULV(res, res1, z, zz);  // 计算 z + zz = res * res1
 * res = ((z + zz - a) < 0) ? max(res, res1) : min(res, res1);
 * ```
 * - **舍入模式调整**  
 * 检查 \f$x / \text{res} == \text{res}\f$ 判断是否精确，否则微调结果：
 * ```c
 * if (x / res != res) 
 *     res = (res - 0x1p-1022) * c;  // 处理非精确舍入
 * ```
 * 
 * #### **6. 特殊值处理**
 * - **负数** ：返回 `NaN`（`S_NAN_DOUBLE`）。
 * - **零** ：返回原值（保持符号）。
 * - **无穷/NaN** ：返回 \f$x \cdot x + x\f$（保留 NaN 或返回 +∞）。
 * - **次正规数** ：缩放至正规范围后递归计算：
 * ```c
 * return 0x1p-256 * __IBM_SQRT64F(x * 0x1p512);  // 缩放因子 2^512 / 2^256
 * ```
 * 
 * ### **关键优化技术**
 * 1. **查找表加速**  
 *  128项预计算表提供高精度初始值。
 * 2. **多项式逼近**  
 *  泰勒级数快速收敛到高精度解。
 * 3. **高低位分离法**  
 *  用 `big` 常量分离浮点数高低位，减少舍入误差。
 * 4. **精确乘积累加**  
 *  `EMULV` 函数通过 Dekker 算法实现双精度乘积的精确计算。
 * 
 * ### **复杂度**
 * - **时间复杂度** ：常数时间（无循环，仅固定步骤）。
 * - **空间复杂度** ：128项查找表（约 1KB）。
 * 
 * 此算法通过结合查表、多项式逼近和误差补偿，实现了 IEEE 754 双精度要求的正确舍入（round-to-nearest）。
 *
 * ---
 * 
 * **以下内容为AI智能解析，仅供参考**
 *
 * ### `cbrt` 函数原理分析
 *
 * #### 1. **核心算法：浮点数立方根计算**
 * 双精度浮点数（`float64`）的立方根计算步骤如下：
 *
 * 1. **规范化输入** ：
 *  - 使用 `frexp(fabs(x), &xe)` 将输入 `x` 分解为尾数 `xm`（范围 `[0.5, 1)`）和指数 `xe`：
 *     \f[
 *     x = \text{sign}(x) \times xm \times 2^{xe}
 *     \f]
 * 
 * 2. **特殊值处理** ：
 *  - 若 `x` 为 `0`、`Inf` 或 `NaN`，直接返回原值。
 *
 * 3. **多项式近似初始值** ：
 *  - 在区间 `[0.5, 1)` 上，使用 **7次多项式** 计算初始近似值 `u`：
 *     \f[
 *     u = \sum_{i=0}^{6} a_i \cdot xm^i
 *     \f]
 *     系数为：
 *     ```
 *     a0 = 0.354895765043919860
 *     a1 = 1.50819193781584896
 *     a2 = -2.11499494167371287
 *     a3 = 2.44693122563534430
 *     a4 = -1.83469277483613086
 *     a5 = 0.784932344976639262
 *     a6 = -0.145263899385486377
 *     ```
 *
 * 4. **哈雷迭代优化** ：
 *  - 通过 **一次哈雷迭代** 提升精度：
 *     \f[
 *     \begin{align*}
 *     t2 &= u^3 \\
 *     ym &= u \times \frac{t2 + 2 \times xm}{2 \times t2 + xm}
 *     \end{align*}
 *     \f]
 *     哈雷迭代针对方程 \f$ f(u) = u^3 - xm = 0 \f$ 设计，收敛速度快。
 * 
 * 5. **指数修正** ：
 *  - 分解指数：\f$ xe = 3q + r \f$（\f$ r \in \{0, 1, 2\} \f$)
 *  - 查表 `factor[5]` 补偿余数部分 \f$ 2^{r/3} \f$：
 *     ```cpp
 *     static const __Float64 factor[5] = {
 *         0.62996052494743658, // 2^{-2/3}
 *         0.79370052598409974, // 2^{-1/3}
 *         1.0,                 // 2^{0/3}
 *         1.25992104989487316, // 2^{1/3}
 *         1.58740105196819947  // 2^{2/3}
 *     };
 *     ```
 *  - 最终结果：
 *     \f[
 *     \text{cbrt}(x) = \text{sign}(x) \times ym \times \text{factor}[2 + r] \times 2^{q}
 *     \f]
 *     其中 \f$ q = \text{floor}(xe / 3) \f$，通过 `ldexp` 实现 \f$ 2^q \f$ 的缩放。
 * 
 * #### 2. **复数立方根计算**
 * `cbrtc` 函数计算复数立方根（3个解）：
 * 1. 极坐标转换：
 * \f[
 * \begin{align*}
 * r &= |x| \\
 * \theta &= \text{atan2}(\Im(x), \Re(x))
 * \end{align*}
 * \f]
 * 2. 计算模的立方根：\f$ ri = \text{cbrt}(r) \f$
 * 3. 生成三个解：
 * \f[
 * \begin{align*}
 * z_k &= ri \times \exp\left(i \frac{\theta + 2k\pi}{3}\right), \quad k = 0, 1, 2
 * \end{align*}
 * \f]
 * 使用欧拉公式 \f$ \exp(i\phi) = \cos\phi + i\sin\phi \f$ 计算。
 * 
 * #### 3. **关键优化点**
 * - **尾数区间压缩** ：`xm ∈ [0.5, 1)` 保证多项式近似高效。
 * - **快速迭代** ：一次哈雷迭代平衡精度与效率。
 * - **指数分解** ：查表法避免实时计算 \f$ 2^{r/3} \f$。
 * - **符号处理** ：全程使用 `fabs(x)`，最后恢复符号。
 * 
 * #### 4. **潜在缺陷**
 * - **负指数处理** ：  
 * 当 \f$ xe < 0 \f$ 时，`xe % 3` 可能产生负余数（如 \f$ xe = -1 \rightarrow r = -1 \f$），导致访问 `factor[1] = 2^{-1/3}`（正确应为 \f$ 2^{2/3} \f$）。  
 * **修复建议** ：调整余数为非负：
 * ```cpp
 * int r = xe % 3;
 * if (r < 0) r += 3;  // 确保 r ∈ {0,1,2}
 * ```
 * 
 * #### 5. **数学依据**
 * - **哈雷迭代公式** ：  
 * 对 \f$ f(u) = u^3 - xm \f$，迭代式为：
 * \f[
 * u_{n+1} = u_n - \frac{2f(u_n)f'(u_n)}{2[f'(u_n)]^2 - f(u_n)f''(u_n)} = u_n \frac{u_n^3 + 2xm}{2u_n^3 + xm}
 * \f]
 * - **指数分解** ：  
 * \f[
 * x = \pm \ xm \times 2^{xe} \implies \sqrt[3]{x} = \pm \ \sqrt[3]{xm} \times 2^{xe/3}
 * \f]
 * 其中 \f$ xe/3 = q + r/3 \f$（\f$ q \in \mathbb{Z}, r \in \{0,1,2\} \f$）。
 * 
 * ### 总结
 * - **实立方根** ：结合多项式逼近与哈雷迭代，通过指数分解和查表优化性能。
 * - **复立方根** ：基于极坐标和欧拉公式生成三个解。
 * - **工业级实现** ：代码源自 GNU C 库，兼顾精度与速度，适用于科学计算场景。
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
 * @param K_OFFSET 分支选择偏移量（默认为0）
 * @return 复数的幂结果
 */
complex64 __cdecl powc(complex64 _X, complex64 _Power, int64 K_OFFSET = 0);

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
 * @param K_OFFSET 分支选择偏移量（默认为0）
 * @return 复数根的结果向量
 */
std::vector<complex64> __cdecl yrootc(complex64 _X, complex64 _Expo, int64 K_OFFSET = 0);

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
 * **以下内容为AI智能解析，仅供参考**
 * 
 * 角度三角函数来自OpenCV，核心原理是 **查表法结合泰勒级数展开** ，下面逐层解析其实现机制：
 *
 * ### 1. **基础数学原理**
 * - **角度归一化** ：输入角度先被归一化到`[0°, 360°)`区间：
 * ```cpp
 * if (_X > 360)
 * {
 *     float64 fDec = _X - (uint64)_X; // 分离整数和小数部分
 *     _X = (uint64)_X % 360 + fDec;   // 取模后保留小数
 * }
 * ```
 * - **符号处理** ：正弦函数处理负角度时记录符号，余弦函数直接取绝对值（因余弦为偶函数）。
 *
 * ### 2. **核心数据结构：预计算正弦表**
 * ```cpp
 * const float64 __CV_SinCos_Tab[64] = { ... };
 * ```
 * - 存储了**64个等分点**的正弦值，覆盖`[0°, 360°)`区间（步长5.625°）。
 * - 索引`n`对应角度：`θ = (360/64) * n`。
 * - 余弦通过索引变换获取：`cos(θ) = sin(90° - θ)` → `IndexC = (16 - IndexS) & 63`。
 *
 * ### 3. **插值计算（泰勒级数展开）**
 * #### 关键变量：
 * - `k1 = 64 / 360.0`：角度→表索引的转换因子。
 * - `k2 = 2π / 64`：表步长对应的弧度值（因三角函数需弧度计算）。
 * - **分离整数与小数** ：
 * ```cpp
 * float64 t = _X * k1;     // 转换为表位置
 * int64 it = std::llround(t); // 四舍五入取整
 * t -= it;                  // 小数部分 t ∈ [-0.5, 0.5]
 * ```
 *
 * #### 泰勒级数系数：
 * - **正弦增量** `sinb` 的系数（基于 `sin(x) ≈ x - x³/3! + x⁵/5! - ...`）：
 * ```cpp
 * As0 = +2.755e-6 * k2^9, As1 = -1.984e-4 * k2^7, 
 * As2 = +0.00833 * k2^5,   As3 = -0.1667 * k2^3, 
 * As4 = +1.0 * k2
 * ```
 * - **余弦增量** `cosb` 的系数（基于 `cos(x) ≈ 1 - x²/2! + x⁴/4! - ...`）：
 * ```cpp
 * Ac0 = +2.480e-5 * k2^8, Ac1 = -0.001388 * k2^6, 
 * Ac2 = +0.04167 * k2^4,   Ac3 = -0.5 * k2^2, 
 * Ac4 = +1.0
 * ```
 *
 * #### 增量计算：
 * ```cpp
 * // 正弦增量：sinb = ((((As0*t² + As1)*t² + As2)*t² + As3)*t² + As4)*t
 * // 余弦增量：cosb = (((Ac0*t² + Ac1)*t² + Ac2)*t² + Ac3)*t² + Ac4
 * ```
 *
 * ### 4. **和角公式合并结果**
 * - **正弦** ：`sin(a + b) = sin(a)cos(b) + cos(a)sin(b)`  
 * ```cpp
 * result = sina * cosb + cosa * sinb;
 * ```
 * - **余弦** ：`cos(a + b) = cos(a)cos(b) - sin(a)sin(b)`  
 * ```cpp
 * result = cosa * cosb - sina * sinb;
 * ```
 * - **正切** ：直接复用 `tan(θ) = sin(θ)/cos(θ)`。
 *
 * ### 5. **设计优势与考量**
 * 1. **精度与速度平衡** ：
 *  - 查表法快速获取主体值（`a`部分）。
 *  - 泰勒展开精细计算偏移量（`b`部分），5阶展开满足`double`精度需求。
 * 2. **周期性优化** ：
 *  - 利用`64=2^6`特性，用位运算`& 63`替代取模提升效率。
 * 3. **内存效率** ：
 *  - 仅存储正弦表，余弦通过索引变换复用。
 * 4. **范围处理** ：
 *  - 大角度取模、负角度符号处理，覆盖全输入范围。
 *
 * ### 6. **潜在改进点**
 * - **正切函数的稳健性** ：未处理`cos(θ)=0`的边界情况（如θ=90°）。
 * - **常量复用** ：`k2`及相关系数在`sin/cos`中重复定义，可全局共享。
 * - **SIMD优化** ：适合并行计算场景（如同时计算多个角度）。
 *
 * ### 总结
 * 该实现通过 **预计算正弦表+泰勒级数插值** ，高效融合了查表的快速性与级数的精确性，是经典数值计算在工程实践中的典型应用。
 *
 * ---
 *
 * **以下内容为AI智能解析，仅供参考**
 *
 * 弧度正弦（`sin`）和余弦（`cos`）函数采用IBM的方案。以下是其核心原理的解析：
 *
 * ### 1. **范围缩小（Range Reduction）**
 * - **目标** ：将任意大的输入角度缩小到 \f$[- \pi/4, \pi/4]\f$ 区间，便于多项式近似。
 * - **方法** ：
 *     - **中等范围** （|x| < 105,414,350）：  
 *     使用公式 \f$ x = n \cdot \pi/2 + r \f$，其中 \f$ n \f$ 是整数，\f$ r \in [-\pi/4, \pi/4] \f$。  
 *     通过预计算的 \f$\pi/2\f$ 高精度近似值（`mp1`, `mp2`）实现快速缩减。
 *     - **大范围** （|x| ≥ 105,414,350）：  
 *     使用扩展精度算法和 \f$2/\pi\f$ 的查找表（`toverp`）计算 \f$ n \f$ 和余数 \f$ r \f$，确保高精度。
 *
 * ### 2. **核心计算（Kernel Functions）**
 * - **小角度处理** （|x| < 0.126）：  
 *     直接使用泰勒级数近似：
 *     \f[
 *     \sin(x) \approx x + x^3 \cdot s_1 + x^5 \cdot s_2 + \cdots
 *     \f]
 * - **一般角度处理** ：
 *     - **查表结合多项式** ：  
 *     通过查找表（`__IBM_sincostab`）获取基础点的 \f$\sin\f$ 和 \f$\cos\f$ 值及其高阶修正项（`sn`, `ssn`, `cs`, `ccs`）。
 *     - **泰勒修正** ：  
 *     对偏移量 \f$ a \f$ 进行低阶泰勒展开（如 \f$\sin(a) \approx a + a^3 \cdot \text{sn3} + \cdots\f$）。
 *     - **合并结果** ：  
 *     使用三角恒等式组合查表值和多式结果，例如：
 *     \f[
 *     \sin(x + a) = \sin(x)\cos(a) + \cos(x)\sin(a)
 *     \f]
 *     通过查表值和多项式计算 \f$\sin(a)\f$、\f$\cos(a)\f$，再合并。
 * 
 * ### 3. **查表优化（Table Lookup）**
 * - **表结构** ：`__IBM_sincostab` 包含 440 个条目，每个条目存储：
 *     - \f$\sin\f$ 值（`sn`）
 *     - \f$\sin\f$ 的高阶修正（`ssn`）
 *     - \f$\cos\f$ 值（`cs`）
 *     - \f$\cos\f$ 的高阶修正（`ccs`）
 * - **索引计算** ：通过输入值的高位直接计算索引，快速定位表项。
 * 
 * ### 4. **特殊处理**
 * - **极小输入** （|x| < 2^(-27)）：  
 *     \f$\sin(x) \approx x\f$，\f$\cos(x) \approx 1\f$。
 * - **大输入** （|x| > 2^1024)：  
 *     返回 `NaN`。
 * - **象限处理** ：  
 *     根据范围缩小后的 \f$n \mod 4\f$ 决定结果符号和使用 \f$\sin\f$ 或 \f$\cos\f$。
 * 
 * ### 5. **精度保障**
 * - **双精度扩展** ：计算中使用双精度数对（如 \f$a + aa\f$）表示中间值，减少舍入误差。
 * - **修正项** ：所有多项式展开和查表结果均包含高阶修正项（如 `ssn`, `ccs`），确保 ULP（Unit in Last Place）误差 < 0.55。
 * - **融合运算** ：关键步骤（如 \f$x - (u - \text{big})\f$）使用融合乘加（FMA）风格计算，避免中间舍入。
 * 
 * ### 关键函数
 * - `__IBM_branred()`：大范围缩小。
 * - `__IBM_reduce_sincos()`：中范围缩小。
 * - `__IBM_Kernal_sin()`/`__IBM_Kernal_cos()`：核心计算。
 * - `__IBM_SIN64F()`/`__IBM_COS64F()`：外部接口，处理所有情况。
 * 
 * ### 总结
 * 此实现通过 **范围缩小** 、 **查表结合多项式近似** 和 **扩展精度计算** ，在保证高效率的同时实现了高精度（< 0.55 ULP）。其设计充分利用了双精度浮点特性，尤其适合科学计算等对精度要求严格的场景。
 * 
 * ---
 *
 * **以下内容为AI智能解析，仅供参考**
 * 
 * 弧度正切（`tan`）函数采用IBM的方案。其核心原理基于 **分段近似** 和 **范围缩减** 技术，结合多项式展开和查表法来平衡精度与性能。以下是其工作原理的详细解析：
 * 
 * ### **1. 输入处理与特殊值**
 * - **特殊值处理** ：
 * - 若输入为 `±INF` 或 `NaN`，返回 `x - x`（得到 `NaN`）。
 * - 使用 `w = fabs(x)` 获取输入的绝对值。
 * 
 * ### **2. 分段处理策略**
 * 根据 `w` 的大小分为多个区间，采用不同的计算方法：
 * 
 * #### **(I) 极小区间：`w ≤ 1.259e-8`**
 * - **原理** ：当 `x` 极小时，`tan(x) ≈ x`（泰勒展开一阶近似）。
 * - **实现** ：直接返回 `x`。
 * 
 * #### **(II) 小区间：`1.259e-8 < w ≤ 0.0608`**
 * - **原理** ：使用 **5 次多项式** 逼近 `tan(x)`。
 * - **多项式形式** ：
 * ```c
 * t2 = x^3 * (d3 + x^2 * (d5 + x^2 * (d7 + x^2 * (d9 + x^2 * d11))))
 * result = x + t2
 * ```
 * - **系数** ：`d3`, `d5`, `d7`, `d9`, `d11` 是预先计算的泰勒级数系数。
 * 
 * #### **(III) 中等区间：`0.0608 < w ≤ 0.787`**
 * - **原理** ： **查表法 + 多项式修正** 。
 * - **步骤** ：
 * 1. **查表** ：根据 `w` 计算索引 `i`，从表 `__IBM_TANF64_XFGTBL` 中获取：
 *     - `xi`：基准点
 *     - `fi`：`tan(xi)` 的近似值
 *     - `gi`：辅助值（通常为 `1/fi`）
 * 2. **计算偏移** ：`z = w - xi`。
 * 3. **多项式修正** ：`pz = z + z^3 * (e0 + e1 * z^2)`（修正 `tan(z)`）。
 * 4. **组合结果** ：
 *     ```c
 *     t2 = pz * (fi + gi) / (gi - pz);
 *     result = fi + t2;  // 符号与输入一致
 *     ```
 * 
 * #### **(IV) 大区间：`0.787 < w ≤ 25`**
 * - **核心** ： **范围缩减到 `[-π/4, π/4]`** 。
 * - **步骤** ：
 * 1. **缩减** ：计算 `t = x * (2/π) + toint`，提取整数部分 `xn = t - toint`。
 * 2. **剩余部分** ：
 *     ```c
 *     t1 = x - xn * mp1;  // mp1 = π/2 的高位
 *     da = xn * mp2;      // mp2 = π/2 的中位
 *     a = t1 - da;
 *     da = (t1 - a) - da; // 误差补偿
 *     ```
 * 3. **奇偶判断** ：根据 `xn` 的奇偶性 (`n = xn & 1`) 决定输出 `tan` 或 `-cot`。
 * 4. **子区间处理** ：
 *     - 若 `|a| ≤ 0.0608`：使用 **(II)** 的多项式。
 *     - 否则：使用 **(III)** 的查表法。
 * 
 * #### **(V) 更大区间：`25 < w ≤ 1e8`**
 * - 类似 **(IV)** ，但使用更高精度的 π 值（`mp1`, `mp2`, `pp3`, `pp4`）。
 * - 同样进行范围缩减和子区间处理。
 * 
 * #### **(VI) 极大区间：`1e8 < w < 2^1024`**
 * - **范围缩减** ：调用 `__IBM_branred(x, &a, &da)` 将 `x` 缩减到 `[-π/4, π/4]`，并返回奇偶标志 `n`。
 * - **子区间处理** ：同 **(IV)** 。
 * 
 * ### **3. 关键技术细节**
 * #### **范围缩减 (Range Reduction)**
 * - **目的** ：将大输入缩减到小范围 `[-π/4, π/4]`，利用正切函数的周期性（周期 π）。
 * - **方法** ：
 * ```c
 * t = x * (2/π) + toint;  // toint = 2^52 + 2^51
 * xn = (int)(t - toint);   // 整数倍部分
 * remainder = x - xn * (π/2); // 剩余部分
 * ```
 * - **高精度 π** ：使用多个常量（`mp1`, `mp2`, `mp3`, `pp3`, `pp4`）表示 π/2 的不同部分，避免精度损失。
 * 
 * #### **误差补偿**
 * - **双精度算法** ：使用 `EADD`, `MUL12`, `DIV2` 等函数进行高精度加减乘除，保留误差项补偿舍入误差。
 * - **示例** （`EADD`）：
 * ```c
 * void EADD(double a, double b, double *c, double *cc) {
 *     *c = a + b;
 *     *cc = (|a| > |b|) ? ((a - *c) + b) : ((b - *c) + a);
 * }
 * ```
 * 
 * #### **符号处理**
 * - 始终记录输入符号 `sy = (x < 0) ? -1 : 1`，并在结果中恢复。
 * 
 * ### **4. 精度保证**
 * - **最大误差** ：注释表明各区间最大 ULP (Unit in Last Place) 误差在 `0.506~0.62` 之间，符合高精度要求。
 * - **表驱动优化** ：预计算的 `__IBM_TANF64_XFGTBL` 表包含 186 项，覆盖关键区间，减少实时计算量。
 * 
 * ### **总结**
 * IBM 的 `tan` 实现通过分层策略结合：
 * 1. **泰勒展开** ：极小区间的高效近似。
 * 2. **查表法 + 多项式修正** ：中等区间的精度平衡。
 * 3. **高精度范围缩减** ：大输入的关键处理。
 * 4. **误差补偿算法** ：保证计算稳定性。
 * 
 * 这种设计在广泛的输入范围内实现了高精度（< 1 ULP）和高效率，是数学库函数的经典实现范例。
 * 
 * ---
 *
 * 角度反三角函数仅实现反正切函数，反正弦和反余弦使用三角恒等式转化。
 *
 * **以下内容为AI智能解析，仅供参考**
 * 
 * #### 1. **`arctan` 函数（`__IEEE754_ATAN128F_C64F`）**
 * - **原理** ：
 * - 基于 **查表法** 和 **有理分式逼近** ，结合反正切的数学性质。
 * - 核心公式 ：  
 *     \f$\arctan(x) = \arctan(u) + \arctan\left(\frac{x - u}{1 + xu}\right)\f$，其中 \f$u\f$ 是预计算的值。
 * - 通过查表（`__Arctan128F_table_rad` 或 `__Arctan128F_table_deg`）获取 \f$\arctan(u)\f$，剩余部分用有理分式计算。
 * 
 * - **步骤** ：
 * 1. **处理特殊值** ：
 *     - 若 \f$x\f$ 是 `NaN`，返回 `NaN`。
 *     - 若 \f$x\f$ 是 \f$\pm\infty\f$，返回 \f$\pm \pi/2\f$（或 \f$\pm 90^\circ\f$）。
 *     - 若 \f$|x| \leq 2^{-55}\f$，直接返回 \f$x\f$（避免浮点下溢）。
 * 2. **符号处理** ：  
 *     若 \f$x < 0\f$，先计算 \f$|x|\f$，最后结果取负。
 * 3. **范围缩减** ：
 *     - 若 \f$|x| \geq 10.25\f$：  
 *     用 \f$\arctan(x) = \pi/2 - \arctan(1/x)\f$ 转换，查表得 \f$\pi/2\f$。
 *     - 否则 ：  
 *     计算索引 \f$k = \lfloor 8x + 0.25 \rfloor\f$，查表得 \f$\arctan(u)\f$（\f$u = k/8\f$）。  
 *     计算 \f$t = (x - u)/(1 + xu)\f$（小量）。
 * 4. **有理分式逼近** ：  
 *     对 \f$t\f$ 计算：  
 *     \f[
 *     \arctan(t) \approx t \cdot \frac{p_0 + p_1 t^2 + p_2 t^4 + p_3 t^6 + p_4 t^8}{q_0 + q_1 t^2 + q_2 t^4 + q_3 t^6 + q_4 t^8}
 *     \f]
 * 5. **合并结果** ：  
 *     \f$\arctan(x) = \arctan(u) + \arctan(t)\f$，调整符号。
 * 
 * - **精度** ：  
 * 使用 84 项的查找表和高阶有理分式，确保 IEEE 双精度下的高精度（相对误差 \f$\sim 10^{-34}\f$)。
 * 
 * #### 2. **`arcsin` 函数**
 * - **原理** ：  
 * 利用三角恒等式 \f$\arcsin(x) = \arctan\left(\frac{x}{\sqrt{1 - x^2}}\right)\f$。
 * - **实现** （非弧度模式）：
 * ```cpp
 * Angle arccos(float64 _X)
 * { 
 *     return arctan(_X / sqrt(1. - _X * _X)); 
 * }
 * ```
 * - **步骤** ：
 * 1. 计算 \f$\sqrt{1 - x^2}\f$。
 * 2. 计算比值 \f$x / \sqrt{1 - x^2}\f$。
 * 3. 调用 `arctan` 求结果。
 * - **边界处理** ：
 * - 若 \f$x = \pm 1\f$，分母为 0，返回 \f$\pm \pi/2\f$（通过 `arctan(±∞)` 处理）。
 * - 若 \f$|x| > 1\f$，返回 `NaN`（隐含在 `sqrt` 中）。
 * 
 * #### 3. **`arccos` 函数**
 * - **原理** ：  
 * 利用恒等式 \f$\arccos(x) = \arctan\left(\frac{\sqrt{1 - x^2}}{x}\right) + [x < 0 \ ? \ 180^\circ : 0]\f$。
 * - **实现** （非弧度模式）：
 * ```cpp
 * Angle arccos(float64 _X)
 * { 
 *     return arctan(sqrt(1. - _X * _X) / _X) + (_X < 0 ? 180 : 0); 
 * }
 * ```
 * - **步骤** ：
 * 1. 计算 \f$\sqrt{1 - x^2}\f$。
 * 2. 计算比值 \f$\sqrt{1 - x^2} / x\f$。
 * 3. 调用 `arctan` 并修正象限（\f$x < 0\f$ 时加 \f$180^\circ\f$）。
 * - **边界处理** ：
 * - 若 \f$x = 0\f$，返回 \f$90^\circ\f$（通过 `arctan(∞)`）。
 * - 若 \f$x = 1\f$ 或 \f$x = -1\f$，返回 \f$0^\circ\f$ 或 \f$180^\circ\f$。
 * 
 * ### 关键优化与设计
 * 1. **查表法加速** ：  
 *  预计算 84 个关键点（\f$0\f$ 到 \f$10.25\f$ 步长 \f$0.125\f$) 的 \f$\arctan\f$ 值，避免重复计算。
 * 2. **范围缩减** ：  
 *  将大输入转换为小输入（\f$x \to 1/x\f$），保证 \f$t\f$ 是小量，提高有理分式精度。
 * 3. **多项式逼近** ：  
 *  对 \f$t\f$ 使用 8 次有理分式（分子 4 次、分母 4 次），在 \f$|t| \ll 1\f$ 时误差极小。
 * 4. **分支处理** ：  
 *  特殊值（如 \f$\infty\f$、`NaN`）和符号单独处理，保证鲁棒性。
 * 
 * ### 复杂度与精度
 * - **时间复杂度** ：\f$O(1)\f$（查表+固定次数的多项式计算）。
 * - **精度** ：  
 * - `arctan` 在 \f$[-19, 19]\f$ 范围内相对误差 \f$\sim 10^{-34}\f$（IEEE 双精度）。
 * - `arcsin`/`arccos` 依赖 `sqrt` 和 `arctan`，精度与之相当。
 * 
 * ### 总结
 * - `arctan`：查表+有理分式逼近，高效高精度。
 * - `arcsin`/`arccos`：通过三角恒等式转化为 `arctan`，结合符号处理和边界条件。
 * - 设计兼顾数学严谨性（恒等式）、数值稳定性（范围缩减）和性能（查表+逼近）。
 *
 * ---
 *
 * 弧度反三角函数实现反正弦和反余弦，反正切与角度共用一个实现（表不一样）。
 *
 * **以下内容为AI智能解析，仅供参考**
 *
 * 弧度反正弦（arcsin）和反余弦（arccos）函数采用IBM的数学库实现。以下是其核心原理分析：
 * 
 * ### 1. 核心数据结构
 * - **预计算表（__IBM_asncs_table）** ：
 * - 包含2568个双精度浮点数（5136个32位整数）
 * - 存储不同区间上的：
 *     - 参考点坐标
 *     - 多项式展开系数
 *     - 偏移修正值
 * - 按不同精度需求分段存储（11-15个系数/段）
 * 
 * - **辅助常数** ：
 * - `hp0 = π/2` (高位部分)
 * - `hp1 = π/2` (低位部分，补偿浮点精度)
 * - 泰勒级数系数 `f1-f6`
 * - 平方根倒数近似系数 `rt0-rt3`
 * - 2的幂次表 `powtwo[27]`
 * 
 * ### 2. 分段处理策略
 * 函数根据输入值的大小分7个区间处理：
 * 
 * #### (1) 极小值（|x| < 2^-26）
 * ```c
 * if (k < 0x3e500000)
 *     return x;  // sin(x)≈x
 * ```
 * 
 * #### (2) 小值（2^-26 ≤ |x| < 0.125）
 * ```c
 * x2 = x*x;
 * t = ((((f6*x2 + f5)*x2 + ... )*x2*x;
 * return x + t;  // 泰勒展开：x + x³/6 + 3x⁵/40 + ...
 * ```
 * 
 * #### (3) 中值（0.125 ≤ |x| < 0.96875）
 * 使用5个细分区间，通过查表+多项式修正：
 * ```c
 * n = calculate_index(k);  // 计算表索引
 * xx = x - table[n];      // 相对参考点偏移
 * res = table[n+8] + table[n+1]*xx + xx²*(table[n+2] + xx*(table[n+3] + ...));
 * ```
 * 
 * #### (4) 大值（0.96875 ≤ |x| < 1）
 * 特殊处理避免精度损失：
 * ```c
 * z = (1 - |x|)/2;               // 变换输入
 * t = inroot_table[..]*powtwo[..]; // 平方根倒数近似
 * r = 1 - t*t*z;                 // 牛顿迭代
 * t *= rt0 + r*(rt1 + ...);      // 4次多项式优化
 * y = (t24*c + c) - t24*c;       // 精度截断
 * cc = (z - y²)/(t + y);         // 修正项
 * p = (f6*z + f5)*z + ...;       // 泰勒展开
 * res = π/2 - 2*(y + cc + y*p);  // 最终结果
 * ```
 * 
 * ### 3. 特殊值处理
 * ```c
 * if (|x| == 1) 
 *     return sign(x)*π/2;  // 边界值
 * 
 * if (|x| > 1) 
 *     return NaN;          // 非法输入
 * ```
 * 
 * ### 4. 精度保障技术
 * - **双常数补偿** ：  
 * `hp0 + hp1 ≈ π/2`（分离高低位）
 * - **精度截断** ：  
 * `y = (t24*c + c) - t24*c` 消除低位误差
 * - **牛顿迭代** ：  
 * 平方根倒数优化（1次迭代+4阶多项式）
 * - **ULP控制** ：  
 * 各分段最大ULP误差0.513-0.524
 * 
 * ### 5. 反余弦实现
 * 基于反正弦优化：
 * ```c
 * arccos(x) =
 * {
 *     π/2 - arcsin(x)                 // 主路径
 *     π/2 + arcsin(x)  (x < 0)       // 负值处理
 *     2*arcsin(sqrt((1-x)/2)) (x≈1)  // 高精度路径
 * }
 * ```
 * 
 * ### 性能优化
 * 1. **查表加速** ：快速定位近似区间
 * 2. **位操作** ：`k = msw & 0x7FFFFFFF` 高效取绝对值
 * 3. **多项式展开** ：减少复杂运算
 * 4. **分支预测** ：按数量级分段处理
 * 
 * 该实现通过精细的分段策略和误差补偿技术，在保证0.52ULP精度的同时，实现了高效的超越函数计算。特别在接近±1的临界区域，采用变量变换和混合算法，有效解决了浮点精度衰减问题。
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
 * @param _SqrtIdx 平方根分支索引（默认为0）
 * @param K_OFFSET 分支选择偏移量（默认为0）
 * @return 复数的反正弦值
 */
complex64 __cdecl arcsinc(complex64 _X, int _SqrtIdx = 0, int64 K_OFFSET = 0);

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
 * @param _SqrtIdx 平方根分支索引（默认为0）
 * @param K_OFFSET 分支选择偏移量（默认为0）
 * @return 复数的反余弦值
 */
complex64 __cdecl arccosc(complex64 _X, int _SqrtIdx = 0, int64 K_OFFSET = 0);

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
 * @param K_OFFSET 分支选择偏移量（默认为0）
 * @return 复数的反正切值
 */
complex64 __cdecl arctanc(complex64 _X, int64 K_OFFSET = 0);

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
complex64 __cdecl arcctgc(complex64 _X, int64 K_OFFSET = 0);
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
complex64 __cdecl arcsecc(complex64 _X, int _SqrtIdx = 0, int64 K_OFFSET = 0);
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
complex64 __cdecl arccscc(complex64 _X, int _SqrtIdx = 0, int64 K_OFFSET = 0);
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
 * 丹霞：历史上最先提出一般形式的四次方程的解法的是卡尔丹的学生费拉里。但是其公式复杂度极高，对此，沈天珩对公式进行了简化。同样的，相比费拉里算法，沈天珩算法中不会出现复数开方。<br>
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
 * 丹霞：寻找五次或以上方程的解法是一个困扰了数学家们300多年的问题。历史上无数的数学家试图推导这些多项式方程求解算法，但无一例外都失败了。1824年，阿贝尔等人证明了五次及更高次的多项式方程没有一般的代数解法，即这样的方程不能由方程的系数经有限次四则运算和开方运算求解。<br>
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
    static std::vector<complex64> GetInitValuePower(float64 Power, complex64 IValue = 0.4 + 0.9i);

    /**
     * @brief 生成圆形分布初始值
     * @param Coeffs 多项式系数
     * @return 初始值数组
     */
    static std::vector<complex64> GetInitValueCircle(InputArray Coeffs);

    /**
     * @brief 生成同伦连续法初始值
     * @param Coeffs 多项式系数
     * @param a      同伦参数(默认0.5)
     * @return 初始值数组
     * @see 算法来源：江源.应用同伦方法加速DURAND-KERNER算法[D].中国科学技术大学,2011.
     */
    static std::vector<complex64> GetInitValueHomotopy(InputArray Coeffs, float64 a = 0.5);

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