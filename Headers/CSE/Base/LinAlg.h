/**
 * @file LinalgBasic.h
 * @brief 基础线性代数运算
 * 
 * 提供向量/矩阵基本运算：
 *   - 点积/内积/外积
 *   - 范数计算 (L1/L2/Lp/Uniform)
 *   - 距离度量 (Manhattan/Euclidean/Minkowski/Chebyshev)
 *   - 矩阵运算 (迹/行列式/逆矩阵/转置)
 * 
 * @copyright Copyright (c) StellarDX Astronomy.
 * @copyright Copyright (c) 2005-2023 NumPy Developers. License under BSD 3-Clause
 */

namespace cse {
namespace linalg{

/**
 * @defgroup LinAlg 线性代数
 * @ingroup Base
 * @brief 基础线性代数函数实现，部分函数译自NumPy
 * @copyright NumPy许可证声明：Copyright (c) 2005-2023, NumPy Developers. Licenced under BSD3
 * @{
 */

/**
 * @page LinAlgSection 一些碎碎念
 * @details
 * 丹灵：这个功能原本想着基于BLAS标准设计，但是......太难理解了。所以我们依然还是翻译NumPy中的同名函数组成一个线性代数库。<br>
 * 丹霞：看未来吧，我听说C++26标准要引入BLAS，到时如果出了就写个类封装那个功能就行。
 */

/**@}*/

#include "./LinAlg/LinalgBasic.inc"

}
}