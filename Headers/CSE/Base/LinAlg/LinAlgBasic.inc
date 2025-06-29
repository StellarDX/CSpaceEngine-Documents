/**
 * @defgroup LinAlgProduct 乘积
 * @ingroup LinAlg
 * @brief 向量和矩阵的各种乘积
 * @{
 */

/**
 * @brief 计算向量点积 (dot product)
 * @tparam N 向量维度
 * @param _Left 左操作向量
 * @param _Right 右操作向量
 * @return 点积计算结果 (标量)
 */
template<std::size_t N>
float64 dot(fvec<N> _Left, fvec<N> _Right);

/**
 * @brief 计算向量内积 (inner product)
 * @note 严格来说，内积是一个更广义的概念，在欧几里得空间中向量的内积即为点积。
 * @see dot
 */
template<std::size_t N>
float64 InnerProduct(fvec<N> _Left, fvec<N> _Right);

/**
 * @brief 计算矩阵内积 (Frobenius inner product)
 * @tparam N 矩阵列数
 * @tparam M 矩阵行数
 * @param _Left 左操作矩阵
 * @param _Right 右操作矩阵
 * @return 内积计算结果 (标量)
 */
template<std::size_t N, std::size_t M>
float64 InnerProduct(matrix<N, M> _Left, matrix<N, M> _Right);

/**
 * @brief 计算向量外积 (outer product)
 * @tparam N 结果矩阵列数
 * @tparam M 结果矩阵行数
 * @param u 输入向量 (决定行数)
 * @param v 输入向量 (决定列数)
 * @return 外积结果矩阵 (尺寸 M x N)
 */
template<std::size_t N, std::size_t M>
matrix<N, M> OuterProduct(fvec<M> u, fvec<N> v);

/**
 * @brief 计算矩阵外积 (Kronecker product)
 * @tparam N1 矩阵 A 列数
 * @tparam M1 矩阵 A 行数
 * @tparam N2 矩阵 B 列数
 * @tparam M2 矩阵 B 行数
 * @param A 输入矩阵 A
 * @param B 输入矩阵 B
 * @return Kronecker 积结果矩阵 (尺寸 M1*M2 x N1*N2)
 */
template<std::size_t N1, std::size_t M1, std::size_t N2, std::size_t M2>
matrix<N1 * N2, M1 * M2> OuterProduct(matrix<N1, M1> A, matrix<N2, M2> B);

/**
 * @brief 计算三维向量叉积 (cross product)
 * @param a 左操作向量
 * @param b 右操作向量
 * @return 叉积结果向量
 */
inline vec3 cross(vec3 a, vec3 b);

/**
 * @brief 计算四维向量叉积 (cross product)
 * @param u 超平面向量1
 * @param v 超平面向量2
 * @param t 超平面向量3
 * @return 叉积结果向量 (与u/v/t正交)
 */
inline vec4 cross(vec4 u, vec4 v, vec4 t);

/**@}*/

/**
 * @defgroup LinAlgNorm 范数
 * @ingroup LinAlg
 * @brief 向量和矩阵的各种范数
 * @{
 */

/**
 * @brief 计算矩阵/向量 L1 范数 (绝对值和)
 * @tparam N 矩阵列数
 * @tparam M 矩阵行数
 * @param a 输入矩阵
 * @return L1 范数值
 */
template<std::size_t N, std::size_t M>
float64 AbsoluteNorm(matrix<N, M> a);

/**
 * @brief 计算向量 L1 范数
 * @see AbsoluteNorm
 */
template<std::size_t N>
float64 AbsoluteNorm(fvec<N> a);

/**
 * @brief 计算矩阵/向量 L2 范数 (Euclidean 范数)
 * @tparam N 矩阵列数
 * @tparam M 矩阵行数
 * @param a 输入矩阵
 * @return L2 范数值
 */
template<std::size_t N, std::size_t M>
float64 EuclideanNorm(matrix<N, M> a);

/**
 * @brief 计算向量 L2 范数
 * @see EuclideanNorm
 */
template<std::size_t N>
float64 EuclideanNorm(fvec<N> a);

/**
 * @brief 计算矩阵/向量 Lp 范数
 * @tparam N 矩阵列数
 * @tparam M 矩阵行数
 * @param a 输入矩阵
 * @param p 范数阶数 (p >= 1)
 * @return Lp 范数值
 */
template<std::size_t N, std::size_t M>
float64 PNorm(matrix<N, M> a, uint64 p);

/**
 * @brief 计算向量 Lp 范数
 * @see PNorm
 */
template<std::size_t N>
float64 PNorm(fvec<N> a, uint64 p);

/**
 * @brief 计算矩阵/向量 Uniform 范数 (无穷范数)
 * @tparam N 矩阵列数
 * @tparam M 矩阵行数
 * @param a 输入矩阵
 * @return 最大绝对值元素
 */
template<std::size_t N, std::size_t M>
float64 UniformNorm(matrix<N, M> a);

/**
 * @brief 计算向量 Uniform 范数
 * @see UniformNorm
 */
template<std::size_t N>
float64 UniformNorm(fvec<N> a);

/**@}*/

/**
 * @defgroup LinAlgDist 距离
 * @ingroup LinAlg
 * @brief 各种距离
 * @{
 */

/**
 * @brief 计算曼哈顿距离 (L1 距离)
 * @tparam N 向量维度
 * @param _Left 左操作向量
 * @param _Right 右操作向量
 * @return 曼哈顿距离值
 */
template<std::size_t N>
float64 ManhattanDistance(fvec<N> _Left, fvec<N> _Right);

/**
 * @brief 计算欧几里得距离 (L2 距离)
 * @tparam N 向量维度
 * @param _Left 左操作向量
 * @param _Right 右操作向量
 * @return 欧几里得距离值
 */
template<std::size_t N>
float64 EuclideanDistance(fvec<N> _Left, fvec<N> _Right);

/**
 * @brief 计算闵可夫斯基距离 (Lp 距离)
 * @tparam N 向量维度
 * @param _Left 左操作向量
 * @param _Right 右操作向量
 * @param p 距离阶数 (p >= 1)
 * @return 闵可夫斯基距离值
 */
template<std::size_t N>
float64 MinkowskiDistance(fvec<N> _Left, fvec<N> _Right, uint64 p);

/**
 * @brief 计算切比雪夫距离 (无穷范数距离)
 * @tparam N 向量维度
 * @param _Left 左操作向量
 * @param _Right 右操作向量
 * @return 切比雪夫距离值
 */
template<std::size_t N>
float64 ChebyshevDistance(fvec<N> _Left, fvec<N> _Right);

/**
 * @brief 默认距离计算 (欧几里得距离)
 * @overload float64 EuclideanDistance(fvec<N>, fvec<N>)
 */
template<std::size_t N>
float64 distance(fvec<N> _Left, fvec<N> _Right);

/**@}*/

/**
 * @defgroup LinAlgMatOpt 矩阵操作
 * @ingroup LinAlg
 * @brief 各种矩阵操作
 * @{
 */

/**
 * @brief 计算矩阵迹 (trace)
 * @tparam N 矩阵维度
 * @param a 输入方阵
 * @return 主对角线元素之和
 */
template<std::size_t N>
float64 Trace(matrix<N, N> a);

/**
 * @brief 计算行列式 (determinant)
 * @tparam N 矩阵维度
 * @param a 输入方阵
 * @return 行列式值
 * @exception 可能因奇异矩阵抛出异常
 */
template<std::size_t N>
float64 Determinant(matrix<N, N> a);

/**
 * @brief 计算矩阵的逆 (inverse)
 * @tparam N 矩阵维度
 * @param a 输入方阵
 * @return 逆矩阵
 * @exception std::logic_error 输入矩阵不可逆时抛出
 */
template<std::size_t N>
matrix<N, N> Inverse(matrix<N, N> a);

/**
 * @brief 矩阵转置
 * @tparam N 矩阵列数
 * @tparam M 矩阵行数
 * @param a 输入矩阵
 * @return 转置矩阵 (尺寸 M x N)
 */
template<std::size_t N, std::size_t M>
auto Transpose(matrix<N, M> a);

/**
 * @brief 向量转置 (行向量转列向量)
 * @see Transpose
 */
template<std::size_t N>
auto Transpose(fvec<N> a);

/**@}*/