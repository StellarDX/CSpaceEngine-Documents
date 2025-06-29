/// @defgroup DynamicMatrix 变长矩阵
/// @brief 可变长度矩阵类型及其运算
/// @ingroup AdvMath
/// @{

/**
 * @class DynamicMatrix
 * @brief 可变大小的数学矩阵模板类
 * @tparam _Ty 矩阵元素类型
 * @ingroup DynamicMatrix
 * 
 * @par 功能特点:
 * - 支持动态行列调整
 * - 按列主序存储数据
 * - 提供完整矩阵运算接口
 * - 兼容STL容器和GLM矩阵
 * 
 * @par 存储结构:
 * 数据按列主序存储在一维数组中，存储顺序为：
 * \f[
 * \begin{bmatrix}
 * a_{00} & a_{01} \\
 * a_{10} & a_{11} 
 * \end{bmatrix}
 * \Rightarrow [a_{00}, a_{10}, a_{01}, a_{11}]
 * \f]
 */
template<typename _Ty>
class DynamicMatrix
{
public:
    typedef _Ty               value_type;      ///< 元素类型
    typedef value_type*       pointer;         ///< 元素指针类型
    typedef const value_type* const_pointer;   ///< 常量元素指针类型
    typedef value_type&       reference;       ///< 元素引用类型
    typedef const value_type& const_reference; ///< 常量元素引用类型
    typedef uvec2             size_type;       ///< 尺寸类型 (x:列数, y:行数)

    typedef std::vector<_Ty>  col_type;        ///< 列向量类型
    typedef std::vector<_Ty>  row_type;        ///< 行向量类型
    typedef DynamicMatrix     transpose_type;  ///< 转置矩阵类型

private:
    std::vector<_Ty> Data; ///< 矩阵数据存储(列主序: 列0,列1,...,列n)
    size_type Size = size_type(0, 0); ///< 矩阵尺寸(x:列数, y:行数)

public:
    /**
     * @brief 默认构造函数
     * @post 创建0x0空矩阵
     */
    DynamicMatrix() = default;

    /**
     * @brief 创建指定尺寸矩阵
     * @param[in] _Sz 矩阵尺寸 (x:列数, y:行数)
     * @post 元素默认初始化
     */
    explicit DynamicMatrix(size_type _Sz) : Size(_Sz)
    {
        Data.resize(_Sz.x * _Sz.y);
    }

    /**
     * @brief 从行主序数组构造矩阵
     * @param[in] _Ptr 源数据指针(行主序存储)
     * @param[in] _Sz  矩阵尺寸 (x:列数, y:行数)
     * 
     * @par 存储转换:
     * 将行主序数据转换为列主序存储:
     * \f[
     * \text{源索引} = col + row \times cols
     * \f]
     */
    explicit DynamicMatrix(const pointer _Ptr, size_type _Sz) : Size(_Sz)
    {
        Data.resize(_Sz.x * _Sz.y);
        for (std::size_t col = 0; col < _Sz.x; ++col)
        {
            for (std::size_t row = 0; row < _Sz.y; ++row)
            {
                at(col, row) = _Ptr[col + row * _Sz.x];
            }
        }
    }

    /// @brief 拷贝构造函数
    constexpr DynamicMatrix(DynamicMatrix const& m)
        : Data(m.Data), Size(m.Size) {};

    /**
     * @brief 创建标量对角矩阵
     * @param[in] scalar 对角元素值
     * @param[in] _Sz    矩阵尺寸
     * 
     * @par 示例:
     * @code
     * DynamicMatrix<float> m(1.0f, {3,3}); 
     * // 创建3x3单位矩阵
     * // [[1,0,0]
     * //  [0,1,0]
     * //  [0,0,1]]
     * @endcode
     */
    explicit constexpr DynamicMatrix(_Ty scalar, size_type _Sz) : Size(_Sz)
    {
        Data.resize(_Sz.x * _Sz.y);
        for (int i = 0; i < min(_Sz.x, _Sz.y); ++i)
        {
            at(i, i) = scalar;
        }
    }

    /**
     * @brief 从初始化列表构造(行主序)
     * @param[in] _Ilist 初始化列表
     * @param[in] _Sz    矩阵尺寸
     * 
     * @par 数据填充顺序:
     * 按行填充: (row0,col0), (row0,col1), ... , (row1,col0), ...
     */
    constexpr DynamicMatrix(std::initializer_list<_Ty> _Ilist, size_type _Sz) : Size(_Sz)
    {
        Data.resize(_Sz.x * _Sz.y);
        auto it = _Ilist.begin(), end = _Ilist.end();
        for (std::size_t row = 0; row < _Sz.y; ++row)
        {
            for (std::size_t col = 0; col < _Sz.x; ++col)
            {
                at(col, row) = *it;
                ++it;
                if (it == end) {break;}
            }
            if (it == end) {break;}
        }
    }

    /**
     * @brief 从列向量列表构造
     * @param[in] _Ilist 列向量初始化列表
     * 
     * @par 特性:
     * - 矩阵列数 = 初始化列表元素数
     * - 矩阵行数 = 最长列向量的长度
     * - 短列自动填充默认值
     */
    constexpr DynamicMatrix(std::initializer_list<col_type> _Ilist)
    {
        auto it = _Ilist.begin(), end = _Ilist.end();
        for (; it < end; ++it)
        {
            Size.y = it->size() > Size.y ? it->size() : Size.y;
        }

        it = _Ilist.begin();
        end = _Ilist.end();
        for (; it < end; ++it)
        {
            Data.insert(Data.end(), it->begin(), it->end());
            ++Size.x;
            Data.resize(Size.x * Size.y);
        }
    }

    /**
     * @brief 从向量构造列向量矩阵
     * @param[in] Right 输入向量
     * @post 创建Nx1矩阵(N=Right.size())
     */
    template<typename _Ty2> requires std::convertible_to<_Ty2, _Ty>
    constexpr DynamicMatrix(std::vector<_Ty2> Right) : Size(1)
    {
        Data.resize(this->col() * this->row());
        for (uint64 i = 0; i < Right.size(); ++i)
        {
            Data[i] = Right[i];
        }
    }

    /**
     * @brief 从GLM固定矩阵构造
     * @tparam _Ty2  元素类型
     * @tparam _Col  GLM矩阵列数
     * @tparam _Row  GLM矩阵行数
     * @param[in] Right GLM矩阵对象
     */
    template<typename _Ty2, std::size_t _Col, std::size_t _Row> requires std::convertible_to<_Ty2, _Ty>
    constexpr DynamicMatrix(__StelCXX_GLM_Generic_Matrix<_Ty2, _Col, _Row> Right) : Size(_Col, _Row)
    {
        Data.resize(this->col() * this->row());
        for (std::size_t col = 0; col < _Col; ++col)
        {
            for (std::size_t row = 0; row < _Row; ++row)
            {
                at(col, row) = Right[col][row];
            }
        }
    }

    /// @brief 获取矩阵尺寸
    _NODISCARD _CONSTEXPR20 size_type size() const noexcept {return Size;}

    /**
     * @brief 调整矩阵尺寸
     * @param[in] NewSize 新尺寸(x:列数, y:行数)
     * 
     * @par 调整规则:
     * - 列增加: 新增列填充默认值
     * - 列减少: 移除右侧列
     * - 行增加: 新增行填充默认值
     * - 行减少: 移除底部行
     * 
     * @throw std::logic_error 内部计算错误
     */
    _CONSTEXPR20 void resize(size_type NewSize)
    {
        if (NewSize.x < this->col())
        {
            Data.erase(Data.begin() + NewSize.x * this->row(), Data.end());
        }
        else if (NewSize.x > this->col())
        {
            for (int i = 0; i < (NewSize.x - this->col()) * this->row(); ++i)
            {
                Data.push_back(_Ty());
            }
        }
        Size.x = NewSize.x;

        if (NewSize.y < this->row())
        {
            std::size_t EraseSize = this->row() - NewSize.y;
            std::size_t OffsetSize = 0;
            for (std::size_t i = 0; i < this->col(); ++i)
            {
                Data.erase(Data.begin() + ((i + 1) * NewSize.y),
                           Data.begin() + ((i + 1) * this->row() - OffsetSize));
                OffsetSize += EraseSize;
            }
        }
        else if (NewSize.y > this->row())
        {
            std::size_t InsertSize = NewSize.y - this->row();
            std::size_t OffsetSize = 0;
            for (std::size_t i = 0; i < this->col(); ++i)
            {
                std::vector<_Ty> Insert(InsertSize);
                Data.insert(Data.begin() + (i + 1) * this->row() + OffsetSize,
                            Insert.begin(), Insert.end());
                OffsetSize += InsertSize;
            }
        }
        Size.y = NewSize.y;
    }

    /// @brief 获取矩阵列数
    _NODISCARD _CONSTEXPR20 std::size_t col()const noexcept {return Size.x;}
    /// @brief 获取矩阵行数
    _NODISCARD _CONSTEXPR20 std::size_t row()const noexcept {return Size.y;}

    /// @brief 检查是否为空矩阵
    _NODISCARD _CONSTEXPR20 bool empty() const noexcept {return Data.empty();}

    /**
     * @brief 列访问运算符
     * @param[in] row 行索引
     * @return col_type 列向量副本
     * @note 等效于GetColumn()
     */
    _NODISCARD _CONSTEXPR20 col_type operator[](size_t row)const noexcept
    {
        return GetColumn(row);
    }

protected:
    /// @brief 抛出范围异常(内部使用)
    void _M_throw_out_of_range()const {throw std::logic_error("Dynamic Matrix index out of range.");}

    /**
     * @brief 索引范围检查(内部使用)
     * @param[in] __n 索引位置(x:列索引, y:行索引)
     * @throw std::logic_error 索引越界
     */
    _CONSTEXPR20 void _M_range_check(size_type __n)const
    {
        if (__n.x >= this->col() || __n.y >= this->row())
        {
            _M_throw_out_of_range();
        }
    }

public:
    /**
     * @brief 元素访问(at风格)
     * @param[in] pos 元素位置(x:列索引, y:行索引)
     * @return reference 元素引用
     * @throw std::logic_error 索引越界
     */
    _CONSTEXPR20 reference at(size_type pos)
    {
        return at(pos.x, pos.y);
    }

    /**
     * @brief 元素访问(at风格)
     * @param[in] col 列索引
     * @param[in] row 行索引
     * @return reference 元素引用
     * @throw std::logic_error 索引越界
     */
    _CONSTEXPR20 reference at(size_t col, size_t row)
    {
        _M_range_check({col, row});
        return Data[col * this->row() + row];
    }

    /// @see at(size_type)
    _CONSTEXPR20 const_reference at(size_type pos)const
    {
        return at(pos.x, pos.y);
    }

    /// @see at(size_t,size_t)
    _CONSTEXPR20 const_reference at(size_t col, size_t row)const
    {
        _M_range_check({col, row});
        return Data[col * this->row() + row];
    }

    /**
     * @brief 填充矩阵元素
     * @param[in] value 填充值
     * @post 所有元素设置为value
     */
    void fill(_Ty value)
    {
        std::fill(Data.begin(), Data.end(), value);
    }

    // ------------------------------ 增删改查接口 ------------------------------ //

    /**
     * @brief 插入列
     * @param[in] pos 插入位置(0-based索引)
     * @param[in] col 列向量数据
     * @throw std::logic_error 位置越界
     * 
     * @par 处理规则:
     * - 长列截断
     * - 短列补默认值
     */
    _CONSTEXPR20 void AddColumn(size_t pos, const col_type& col)noexcept
    {
        if (pos > this->col()) {_M_throw_out_of_range();}
        if (col.size() >= this->row())
        {
            Data.insert(Data.begin() + (pos * this->row()), col.begin(), col.begin() + this->row());
        }
        else
        {
            col_type NewCol(col);
            NewCol.resize(this->row());
            Data.insert(Data.begin() + (pos * this->row()), NewCol.begin(), NewCol.end());
        }
        ++Size.x;
    }

    /**
     * @brief 插入行
     * @param[in] pos 插入位置(0-based索引)
     * @param[in] row 行向量数据
     * @throw std::logic_error 位置越界
     * 
     * @par 处理规则:
     * - 长行截断
     * - 短行补默认值
     */
    _CONSTEXPR20 void AddRow(size_t pos, const row_type& row)noexcept
    {
        if (pos > this->row()) {_M_throw_out_of_range();}
        std::size_t OffsetSize = 0;
        for (std::size_t i = 0; i < this->col(); ++i)
        {
            Data.insert(Data.begin() + i * this->row() + pos + OffsetSize,
                        row.size() > i ? row[i] : _Ty());
            ++OffsetSize;
        }
        ++Size.y;
    }

    /**
     * @brief 设置列数据
     * @param[in] pos 列索引
     * @param[in] col 列向量数据
     * @throw std::logic_error 索引越界
     * @note 输入列长度必须等于当前行数
     */
    _CONSTEXPR20 void SetColumn(size_t pos, const col_type& col)noexcept
    {
        for (size_t i = 0; i < this->row(); ++i)
        {
            at(pos, i) = col[i];
        }
    }

    /**
     * @brief 设置行数据
     * @param[in] pos 行索引
     * @param[in] row 行向量数据
     * @throw std::logic_error 索引越界
     * @note 输入行长度必须等于当前列数
     */
    _CONSTEXPR20 void SetRow(size_t pos, const row_type& row)noexcept
    {
        for (size_t i = 0; i < this->col(); ++i)
        {
            at(i, pos) = row[i];
        }
    }

    /**
     * @brief 获取列数据
     * @param[in] pos 列索引
     * @return col_type 列向量副本
     * @throw std::logic_error 索引越界
     */
    _NODISCARD _CONSTEXPR20 col_type GetColumn(size_t pos)const noexcept
    {
        col_type col(this->row());
        for (size_t i = 0; i < this->row(); ++i)
        {
            col[i] = at(pos, i);
        }
        return col;
    }

    /**
     * @brief 获取行数据
     * @param[in] pos 行索引
     * @return row_type 行向量副本
     * @throw std::logic_error 索引越界
     */
    _NODISCARD _CONSTEXPR20 row_type GetRow(size_t pos)const noexcept
    {
        row_type row(this->col());
        for (size_t i = 0; i < this->col(); ++i)
        {
            row[i] = at(i, pos);
        }
        return row;
    }

    /**
     * @brief 删除列
     * @param[in] pos 列索引
     * @throw std::logic_error 索引越界
     */
    _CONSTEXPR20 void DeleteColumn(size_t pos)noexcept
    {
        if (pos > this->col()) {_M_throw_out_of_range();}
        Data.erase(Data.begin() + (pos * this->row()), Data.begin() + (pos * this->row() + 1));
        --Size.x;
    }

    /**
     * @brief 删除行
     * @param[in] pos 行索引
     * @throw std::logic_error 索引越界
     */
    _CONSTEXPR20 void DeleteRow(size_t pos)noexcept
    {
        if (pos > this->row()) {_M_throw_out_of_range();}
        std::size_t OffsetSize = 0;
        for (std::size_t i = 0; i < this->col(); ++i)
        {
            Data.erase(Data.begin() + (i * this->row() + pos - OffsetSize));
            ++OffsetSize;
        }
        --Size.y;
    }

    // ----------------------------- 运算符重载 ----------------------------- //

    /**
     * @brief 矩阵加法赋值
     * @param[in] other 加数矩阵
     * @return DynamicMatrix& 自引用
     * @throw std::logic_error 矩阵尺寸不匹配
     * 
     * @par 计算公式:
     * \f[
     * \mathbf{C} = \mathbf{A} + \mathbf{B} \quad \text{其中} \quad c_{ij} = a_{ij} + b_{ij}
     * \f]
     */
    DynamicMatrix& operator+=(const DynamicMatrix& other)
    {
        if (other.col() != col() || other.row() != row())
        {
            throw std::logic_error("Size of matrices is not equal.");
        }
        for (std::size_t col = 0; col < this->col(); ++col)
        {
            for (std::size_t row = 0; row < this->row(); ++row)
            {
                at(col, row) += other.at(col, row);
            }
        }
        return *this;
    }

    /**
     * @brief 矩阵减法赋值
     * @param[in] other 减数矩阵
     * @return DynamicMatrix& 自引用
     * @throw std::logic_error 矩阵尺寸不匹配
     * 
     * @par 计算公式:
     * \f[
     * \mathbf{C} = \mathbf{A} - \mathbf{B} \quad \text{其中} \quad c_{ij} = a_{ij} - b_{ij}
     * \f]
     */
    DynamicMatrix& operator-=(const DynamicMatrix& other)
    {
        if (other.col() != col() || other.row() != row())
        {
            throw std::logic_error("Size of matrices is not equal.");
        }
        for (std::size_t col = 0; col < this->col(); ++col)
        {
            for (std::size_t row = 0; row < this->row(); ++row)
            {
                at(col, row) -= other.at(col, row);
            }
        }
        return *this;
    }

    /**
     * @brief 标量乘法赋值
     * @param[in] factor 标量乘数
     * @return DynamicMatrix& 自引用
     * 
     * @par 计算公式:
     * \f[
     * \mathbf{B} = k \times \mathbf{A} \quad \text{其中} \quad b_{ij} = k \times a_{ij}
     * \f]
     */
    DynamicMatrix& operator*=(_Ty factor)
    {
        for (std::size_t col = 0; col < this->col(); ++col)
        {
            for (std::size_t row = 0; row < this->row(); ++row)
            {
                at(col, row) *= factor;
            }
        }
        return *this;
    }

    /**
     * @brief 标量除法赋值
     * @param[in] divisor 标量除数
     * @return DynamicMatrix& 自引用
     * 
     * @par 计算公式:
     * \f[
     * \mathbf{B} = k / \mathbf{A} \quad \text{其中} \quad b_{ij} = k / a_{ij}
     * \f]
     */
    DynamicMatrix& operator/=(_Ty divisor)
    {
        for (std::size_t col = 0; col < this->col(); ++col)
        {
            for (std::size_t row = 0; row < this->row(); ++row)
            {
                at(col, row) /= divisor;
            }
        }
        return *this;
    }

    /**
     * @brief 矩阵相等比较
     * @param[in] other 比较矩阵
     * @return bool 相等结果
     * 
     * @par 比较规则:
     * - 尺寸不同: 返回false
     * - 逐元素比较: 所有元素相等返回true
     */
    bool operator==(const DynamicMatrix& other) const
    {
        if (other.col() != col() || other.row() != row())
        {
            return false;
        }
        for (std::size_t col = 0; col < this->col(); ++col)
        {
            for (std::size_t row = 0; row < this->row(); ++row)
            {
                if (at(col,row) != other.at(col,row))
                {
                    return false;
                }
            }
        }
        return true;
    }

    /// @see operator==
    bool operator!=(const DynamicMatrix& other) const
    {
        return !(*this == other);
    }
};

// ----------------------------- 全局向量运算符 ----------------------------- //

/**
 * @brief 向量一元正号
 * @return 向量副本
 */
template<typename _Tp>
constexpr std::vector<_Tp> operator+(std::vector<_Tp> const& v)
{
    return v;
}

/**
 * @brief 向量一元负号
 * @return 负向量
 */
template<typename _Tp>
constexpr std::vector<_Tp> operator-(std::vector<_Tp> const& v)
{
    std::vector<_Tp> Res;
    for (std::size_t i = 0; i < v.size(); ++i)
    {
        Res[i] = -v[i];
    }
    return Res;
}

/**
 * @brief 向量标量加法
 * @return 结果向量
 * @throw std::logic_error 向量尺寸不匹配
 */
template<typename _Tp>
constexpr std::vector<_Tp> operator+(std::vector<_Tp> const& v, _Tp scalar)
{
    std::vector<_Tp> Res;
    for (std::size_t i = 0; i < v.size(); ++i)
    {
        Res.push_back(v[i] + scalar);
    }
    return Res;
}

/// @see operator+(std::vector<_Tp> const&, _Tp)
template<typename _Tp>
constexpr std::vector<_Tp> operator+(_Tp scalar, std::vector<_Tp> const& v)
{
    std::vector<_Tp> Res;
    for (std::size_t i = 0; i < v.size(); ++i)
    {
        Res.push_back(scalar + v[i]);
    }
    return Res;
}

/**
 * @brief 向量加法
 * @return 结果向量
 * @throw std::logic_error 向量尺寸不匹配
 */
template<typename _Tp>
constexpr std::vector<_Tp> operator+(std::vector<_Tp> const& v1, std::vector<_Tp> const& v2)
{
    if (v1.size() != v2.size()) {throw std::logic_error("Size of vectors are not equal.");}
    std::vector<_Tp> Res;
    for (std::size_t i = 0; i < v1.size(); ++i)
    {
        Res.push_back(v1[i] + v2[i]);
    }
    return Res;
}

/**
 * @brief 向量标量减法
 * @return 结果向量
 * @throw std::logic_error 向量尺寸不匹配
 */
template<typename _Tp>
constexpr std::vector<_Tp> operator-(std::vector<_Tp> const& v, _Tp scalar)
{
    std::vector<_Tp> Res;
    for (std::size_t i = 0; i < v.size(); ++i)
    {
        Res.push_back(v[i] - scalar);
    }
    return Res;
}

/// @see operator-(std::vector<_Tp> const&, _Tp)
template<typename _Tp>
constexpr std::vector<_Tp> operator-(_Tp scalar, std::vector<_Tp> const& v)
{
    std::vector<_Tp> Res;
    for (std::size_t i = 0; i < v.size(); ++i)
    {
        Res.push_back(scalar - v[i]);
    }
    return Res;
}

/**
 * @brief 向量减法
 * @return 结果向量
 * @throw std::logic_error 向量尺寸不匹配
 */
template<typename _Tp>
constexpr std::vector<_Tp> operator-(std::vector<_Tp> const& v1, std::vector<_Tp> const& v2)
{
    if (v1.size() != v2.size()) {throw std::logic_error("Size of vectors are not equal.");}
    std::vector<_Tp> Res;
    for (std::size_t i = 0; i < v1.size(); ++i)
    {
        Res.push_back(v1[i] - v2[i]);
    }
    return Res;
}

/**
 * @brief 向量标量乘法
 * @return 结果向量
 * @throw std::logic_error 向量尺寸不匹配
 */
template<typename _Tp>
constexpr std::vector<_Tp> operator*(std::vector<_Tp> const& v, _Tp scalar)
{
    std::vector<_Tp> Res;
    for (std::size_t i = 0; i < v.size(); ++i)
    {
        Res.push_back(v[i] * scalar);
    }
    return Res;
}

/// @see operator*(std::vector<_Tp> const&, _Tp)
template<typename _Tp>
constexpr std::vector<_Tp> operator*(_Tp scalar, std::vector<_Tp> const& v)
{
    std::vector<_Tp> Res;
    for (std::size_t i = 0; i < v.size(); ++i)
    {
        Res.push_back(scalar * v[i]);
    }
    return Res;
}

/**
 * @brief 向量逐元素乘法
 * @return 结果向量
 * @throw std::logic_error 向量尺寸不匹配
 */
template<typename _Tp>
constexpr std::vector<_Tp> operator*(std::vector<_Tp> const& v1, std::vector<_Tp> const& v2)
{
    if (v1.size() != v2.size()) {throw std::logic_error("Size of vectors are not equal.");}
    std::vector<_Tp> Res;
    for (std::size_t i = 0; i < v1.size(); ++i)
    {
        Res.push_back(v1[i] * v2[i]);
    }
    return Res;
}

/**
 * @brief 向量标量除法
 * @return 结果向量
 * @throw std::logic_error 向量尺寸不匹配
 */
template<typename _Tp>
constexpr std::vector<_Tp> operator/(std::vector<_Tp> const& v, _Tp scalar)
{
    std::vector<_Tp> Res;
    for (std::size_t i = 0; i < v.size(); ++i)
    {
        Res.push_back(v[i] / scalar);
    }
    return Res;
}

/// @see operator/(std::vector<_Tp> const&, _Tp)
template<typename _Tp>
constexpr std::vector<_Tp> operator/(_Tp scalar, std::vector<_Tp> const& v)
{
    std::vector<_Tp> Res;
    for (std::size_t i = 0; i < v.size(); ++i)
    {
        Res.push_back(scalar / v[i]);
    }
    return Res;
}

/**
 * @brief 向量逐元素除法
 * @return 结果向量
 * @throw std::logic_error 向量尺寸不匹配
 */
template<typename _Tp>
constexpr std::vector<_Tp> operator/(std::vector<_Tp> const& v1, std::vector<_Tp> const& v2)
{
    if (v1.size() != v2.size()) {throw std::logic_error("Size of vectors are not equal.");}
    std::vector<_Tp> Res;
    for (std::size_t i = 0; i < v1.size(); ++i)
    {
        Res.push_back(v1[i] / v2[i]);
    }
    return Res;
}

// ----------------------------- 流输出运算符 ----------------------------- //

/**
 * @brief 向量输出运算符
 * @par 输出格式: (v0, v1, ..., vn)
 */
template<typename _Tp>
std::ostream& operator<<(std::ostream& os, std::vector<_Tp> const& vec)
{
    os << '(' << vec[0];
    for (int i = 1; i < vec.size(); ++i)
    {
        os << ", " << vec[i];
    }
    os << ')';
    return os;
}

/// @see operator<<(std::ostream&, std::vector<_Tp> const&)
template<typename _Tp>
std::wostream& operator<<(std::wostream& os, std::vector<_Tp> const& vec)
{
    os << '(' << vec[0];
    for (int i = 1; i < vec.size(); ++i)
    {
        os << L", " << vec[i];
    }
    os << ')';
    return os;
}

// ----------------------------- 全局矩阵运算符 ----------------------------- //

/**
 * @brief 矩阵一元正号
 * @return 矩阵副本
 */
template<typename _Tp>
inline DynamicMatrix<_Tp> operator+(const DynamicMatrix<_Tp>& matrix)
{
    return matrix;
}

/**
 * @brief 矩阵一元负号
 * @return 负矩阵
 */
template<typename _Tp>
inline DynamicMatrix<_Tp> operator-(const DynamicMatrix<_Tp>& matrix)
{
    DynamicMatrix<_Tp> result(matrix);
    for (std::size_t col = 0; col < result.col(); ++col)
    {
        for (std::size_t row = 0; row < result.row(); ++row)
        {
            result.at(col, row) = -result.at(col, row);
        }
    }
    return result;
}

/**
 * @brief 矩阵加法
 * @param[in] m1 矩阵A
 * @param[in] m2 矩阵B
 * @return 结果矩阵
 * @throw std::logic_error 矩阵尺寸不匹配
 */
template<typename _Tp>
inline DynamicMatrix<_Tp> operator+(const DynamicMatrix<_Tp>& m1, const DynamicMatrix<_Tp>& m2)
{
    if (m1.col() != m2.col() || m1.row() != m2.row())
    {
        throw std::logic_error("Size of matrices are not equal.");
    }
    DynamicMatrix<_Tp> result(m1);
    for (std::size_t col = 0; col < result.col(); ++col)
    {
        for (std::size_t row = 0; row < result.row(); ++row)
        {
            result.at(col, row) += m2.at(col, row);
        }
    }
    return result;
}

/**
 * @brief 矩阵减法
 * @param[in] m1 矩阵A
 * @param[in] m2 矩阵B
 * @return 结果矩阵
 * @throw std::logic_error 矩阵尺寸不匹配
 */
template<typename _Tp>
inline DynamicMatrix<_Tp> operator-(const DynamicMatrix<_Tp>& m1, const DynamicMatrix<_Tp>& m2)
{
    if (m1.col() != m2.col() || m1.row() != m2.row())
    {
        throw std::logic_error("Size of matrices are not equal.");
    }
    DynamicMatrix<_Tp> result(m1);
    for (std::size_t col = 0; col < result.col(); ++col)
    {
        for (std::size_t row = 0; row < result.row(); ++row)
        {
            result.at(col, row) -= m2.at(col, row);
        }
    }
    return result;
}

/**
 * @brief 矩阵标量乘法
 * @param[in] factor 标量
 * @param[in] matrix 输入矩阵
 * @return 结果矩阵
 */
template<typename _Tp>
inline DynamicMatrix<_Tp> operator*(_Tp factor, const DynamicMatrix<_Tp>& matrix)
{
    DynamicMatrix<_Tp> result(matrix);
    for (std::size_t col = 0; col < result.col(); ++col)
    {
        for (std::size_t row = 0; row < result.row(); ++row)
        {
            result.at(col, row) *= factor;
        }
    }
    return result;
}

/// @see operator*(_Tp, const DynamicMatrix<_Tp>&)
template<typename _Tp>
inline DynamicMatrix<_Tp> operator*(const DynamicMatrix<_Tp>& matrix, _Tp factor)
{
    DynamicMatrix<_Tp> result(matrix);
    for (std::size_t col = 0; col < result.col(); ++col)
    {
        for (std::size_t row = 0; row < result.row(); ++row)
        {
            result.at(col, row) *= factor;
        }
    }
    return result;
}

/**
 * @brief 矩阵标量除法
 * @param[in] matrix 被除矩阵
 * @param[in] divisor 标量除数
 * @return 结果矩阵
 */
template<typename _Tp>
inline DynamicMatrix<_Tp> operator/(const DynamicMatrix<_Tp>& matrix, _Tp divisor)
{
    DynamicMatrix<_Tp> result(matrix);
    for (std::size_t col = 0; col < result.col(); ++col)
    {
        for (std::size_t row = 0; row < result.row(); ++row)
        {
            result.at(col, row) /= divisor;
        }
    }
    return result;
}

/**
 * @brief 矩阵乘法
 * @param[in] m1 左矩阵(A)
 * @param[in] m2 右矩阵(B)
 * @return 结果矩阵(C)
 * @throw std::logic_error 矩阵尺寸不匹配
 * 
 * @par 计算公式:
 * \f[
 * \mathbf{C} = \mathbf{A} \times \mathbf{B} \quad \text{其中} \quad 
 * c_{ij} = \sum_{k=0}^{K} a_{ik} \times b_{kj}
 * \f]
 * @par 尺寸要求:
 * - A的列数 = B的行数
 * - 结果矩阵尺寸: (B.cols x A.rows)
 */
template<typename _Tp>
inline DynamicMatrix<_Tp> operator*(const DynamicMatrix<_Tp>& m1, const DynamicMatrix<_Tp>& m2)
{
    if (m1.col() != m2.row())
    {
        throw std::logic_error("Matrices can't multiply.");
    }
    DynamicMatrix<_Tp> result;
    result.resize({m2.col(), m1.row()});
    for (int row = 0; row < m1.row(); ++row)
    {
        for (int col = 0; col < m2.col(); ++col)
        {
            _Tp sum(0.0);
            for (int j = 0; j < m1.col(); ++j) {sum += m1.at(j, row) * m2.at(col, j);}
            result.at(col, row) = sum;
        }
    }
    return result;
}

/**
 * @brief 矩阵输出运算符
 * @par 输出格式:
 * [[m00, m01, ..., m0n]<br>
 *  [m10, m11, ..., m1n]<br>
 *  ...<br>
 *  [mm0, mm1, ..., mmn]]
 */
template<typename _Tp>
std::ostream& operator<<(std::ostream& dbg, const DynamicMatrix<_Tp> &m)
{
    dbg << '[';
    for (int row = 0; row < m.row(); ++row)
    {
        if (row != 0) {dbg << ' ';}
        for (int col = 0; col < m.col(); ++col)
        {
            dbg << m.at(col, row);
            if (row < m.row() - 1 || col < m.col() - 1) {dbg << ", ";}
        }
        if (row < m.row() - 1) {dbg << '\n';}
    }
    dbg << ']' << '\n';
    return dbg;
}

/// @see operator<<(std::ostream&, const DynamicMatrix<_Tp> &)
template<typename _Tp>
std::wostream& operator<<(std::wostream& dbg, const DynamicMatrix<_Tp> &m)
{
    dbg << '[';
    for (int row = 0; row < m.row(); ++row)
    {
        if (row != 0) {dbg << ' ';}
        for (int col = 0; col < m.col(); ++col)
        {
            dbg << m.at(col, row);
            if (row < m.row() - 1 || col < m.col() - 1) {dbg << L", ";}
        }
        if (row < m.row() - 1) {dbg << '\n';}
    }
    dbg << ']' << '\n';
    return dbg;
}

/// @}