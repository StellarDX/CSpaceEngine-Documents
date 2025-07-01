/*!
 * @file SCSBase.h
 * @brief SpaceEngine sc解析器基础数据结构
 * @copyright Copyright (c) StellarDX Astronomy
 * @details 
 * 本文件定义了SC解析器的核心数据结构，包括：
 *        - 词法分析Token类型
 *        - 多类型值存储结构
 *        - SC数据表结构
 *        - 表数据操作辅助函数
 */

namespace cse {
namespace scenario {

/// @defgroup SCSBase 核心数据结构（内部使用）
/// @brief SpaceEngine sc解析器基础数据结构
/// @ingroup Parser
/// @{

/*!
 * @class ParseException
 * @brief 解析异常类
 * @details 继承自std::runtime_error，用于输出解析过程中的错误信息
 */
class ParseException : public std::runtime_error
{
public:
    using _Mybase = std::runtime_error;
    ParseException(const std::string& Error) : _Mybase{ Error } {}
    ParseException(const std::string& err, size_t line_number) : _Mybase{ err + " at line " + std::to_string(line_number) } {}
};

/*!
 * @enum TerminalTypes
 * @brief 终端符号类型枚举
 * @details 定义词法分析中不同类型的Token
 */
enum TerminalTypes
{
    Identifier = 'i', ///< 标识符类型
    Variable   = 'v', ///< 变量类型
    Operator   = 'o', ///< 操作符类型
    Number     = 'n', ///< 数值类型
    String     = 's', ///< 字符串类型
    Boolean    = 'b', ///< 布尔类型
    Punctuator = 'p', ///< 标点符号类型
};

/// @brief Token数组类型定义
using TokenArrayType  = std::vector<TokenType>;
/// @brief 智能指针模板
template<typename _Ty>
using SharedPointer   = std::shared_ptr<_Ty>;

/*!
 * @struct TokenType
 * @brief Token数据结构
 * @details 存储词法分析单元的类型、值和位置信息
 */
struct TokenType
{
    TerminalTypes Type; ///< Token类型
    ustring       Value; ///< Token字符串值
    ivec2         Posiston; ///< Token在源文件中的位置(行,列)
    int           NumBase = 10; ///< 数字进制(默认为10进制)
};

/*!
 * @brief 字符串转布尔值
 * @param __str 输入字符串
 * @return 当字符串为"true"时返回true，否则返回false
 */
inline bool stob(const ustring& __str)
{
    return (__str == L"true") ? 1 : 0;
}

/*!
 * @struct ValueType
 * @brief 多类型值存储结构
 * @details 支持存储多种类型的值，包括数字、字符串、布尔值、数组和矩阵
 */
struct ValueType
{
    /// @brief 子矩阵类型定义
    using SubMatrixType = std::map<size_t, ValueType>;

    /*!
     * @enum TypeID
     * @brief 值类型标识枚举
     */
    enum TypeID
    {
        Number  = 0b0001, ///< 数值类型
        String  = 0b0010, ///< 字符串类型
        Boolean = 0b0011, ///< 布尔类型
        Array   = 0b0100, ///< 数组类型
        Matrix  = 0b1100, ///< 矩阵类型
        Others  = 0b0000, ///< 其他类型

        Mask    = 0b0011  ///< 类型掩码
    }Type;

    ustringlist Value; ///< 值存储列表
    SharedPointer<SubMatrixType> SubMatrices = nullptr; ///< 子矩阵数据
    int Base = 10; ///< 数值进制
    ivec2 Pos; ///< 值在源文件中的位置

    /*!
     * @brief 获取类型对应的TypeID
     * @tparam _Tp 类型参数
     * @return 对应的TypeID值
     */
    template<typename _Tp>
    static TypeID ToTypeID()
    {
        if (std::convertible_to<_Tp, float64>) {return Number;}
        else if (std::convertible_to<_Tp, ustring>) {return String;}
        else if (std::is_same_v<_Tp, bool>) {return Boolean;}
        else {return Others;}
    }

    /*!
     * @brief 获取指定类型的值
     * @param Dst 目标存储地址
     * @param Idx 值在列表中的索引(默认为0)
     * @exception ParseException 当值无法转换为单值时抛出
     */
    void GetQualified(void* Dst, size_t Idx = 0)
    {
        switch (Type & Mask)
        {
        case Number:
            *((float64*)Dst) = std::stod(Value[Idx]);
            break;

        case String:
            *((ustring*)Dst) = Value[Idx];
            ((ustring*)Dst)->erase(((ustring*)Dst)->begin());
            ((ustring*)Dst)->pop_back();
            break;

        case Boolean:
            *((bool*)Dst) = stob(Value[Idx]);
            break;

        default:
            throw ParseException("Value is not convertable to single value.");
            break;
        }
    }

    /// @brief 获取基本类型ID
    /// @return 基本类型ID
    TypeID GetTypeID() {return TypeID(Type & Mask);}

    /*!
     * @brief 获取基于进制的整数值
     * @param Dst 目标存储地址
     * @param Idx 值在列表中的索引(默认为0)
     */
    void GetAsBasedInteger(void* Dst, size_t Idx = 0)
    {
        *((int64*)Dst) = std::stoll(Value.front(), nullptr, Base);
    }

    /*!
     * @brief 获取固定大小数组
     * @tparam _Tp 元素类型
     * @tparam _Nm 数组大小
     * @param Dst 目标数组
     * @exception ParseException 当类型不匹配时抛出
     */
    template<typename _Tp, std::size_t _Nm>
    void GetAsArray(gvec<_Tp, _Nm>* Dst)
    {
        if (ToTypeID<_Tp>() != GetTypeID())
        {
            throw ParseException("Type is not match.");
        }

        size_t Capacity = max(Value.size(), _Nm);
        for (int i = 0; i < Capacity; ++i)
        {
            GetQualified(&(*Dst)[i], i);
        }
    }

    /*!
     * @brief 获取动态数组
     * @tparam _Tp 元素类型
     * @param Dst 目标动态数组
     * @exception ParseException 当类型不匹配时抛出
     */
    template<typename _Tp>
    void GetAsDynamicArray(std::vector<_Tp>* Dst)
    {
        if (ToTypeID<_Tp>() != GetTypeID())
        {
            throw ParseException("Type is not match.");
        }

        if (Dst->size() < Value.size()) {Dst->resize(Value.size());}
        for (int i = 0; i < Value.size(); ++i)
        {
            GetQualified(&(*Dst)[i], i);
        }
    }

    /*!
     * @brief 获取日期时间值
     * @param Dst 目标日期时间对象
     * @exception ParseException 当值无效时抛出
     */
    void GetAsDateTime(CSEDateTime* Dst)noexcept(false)
    {
        if (GetTypeID() != String) {throw ParseException("Invalid value");}
        ustring Str;
        GetQualified(&Str);
        _REGEX_NS smatch Toks;
        std::string Data = Str.ToStdString();
        if (_REGEX_NS regex_match(Data, Toks, _TIME SEDateTimeStringRegex))
        {
            *Dst = CSEDateTime
            (
                {std::stoi(Toks[1]), std::stoi(Toks[2]), std::stoi(Toks[3])},
                {std::stoi(Toks[4]), std::stoi(Toks[5]), std::stod(Toks[6])}
            );
        }
        else if (_REGEX_NS regex_match(Data, Toks, _TIME SEDateStringRegex))
        {
            *Dst = CSEDateTime({std::stoi(Toks[1]), std::stoi(Toks[2]), std::stoi(Toks[3])});
        }
        else
        {
            *Dst = CSEDateTime({std::stoi(Str), 1, 1});
        }
    }

    /*!
     * @brief 将值转换为字符串表示
     * @return 值的字符串表示
     */
    ustring ToString()const
    {
        if ((Type & (~Mask)) == Array)
        {
            ustring Str;
            Str.push_back(L'(');
            for (int i = 0; i < Value.size(); ++i)
            {
                Str += Value[i];
                if (i < Value.size() - 1) {Str += L", ";}
            }
            Str.push_back(L')');
            return Str;
        }
        else if (Type & 0b100000)
        {
            ustring Str;
            Str.push_back(L'{');
            for (int i = 0; i < Value.size(); ++i)
            {
                Str += Value[i];
                if (i < Value.size() - 1) {Str += L" ";}
            }
            Str.push_back(L'}');
            return Str;
        }
        else if ((Type & (~Mask)) == Matrix)
        {
            return __Matrix_To_String(*this);
        }
        else {return Value.front();}
    }
};

/*!
 * @brief 矩阵转字符串实现
 * @param Val 矩阵值对象
 * @return 矩阵的字符串表示
 */
inline ustring __Matrix_To_String(ValueType Val)
{
    ustring Str;
    Str += L"{ ";
    for (size_t i = 0; i <= Val.Value.size(); ++i)
    {
        if (Val.SubMatrices)
        {
            size_t j = i;
            while (Val.SubMatrices->contains(j))
            {
                Str += Val.SubMatrices->at(j).ToString();
                Str += L" ";
                ++j;
            }
        }
        if (i < Val.Value.size())
        {
            Str += Val.Value[i];
            Str += L" ";
        }
    }
    Str.push_back(L'}');
    return Str;
}

/// @brief 值列表类型定义
using ValueList = std::vector<ValueType>;

/*!
 * @struct SCSTable
 * @brief SC表结构
 * @details 存储键值对数据，支持嵌套子表
 */
struct SCSTable
{
    /// @brief 子表智能指针类型
    using SubTablePointer = SharedPointer<SCSTable>;

    /*!
     * @struct SCKeyValue
     * @brief 键值对结构
     */
    struct SCKeyValue
    {
        ustring            Key; ///< 键名
        ValueList          Value; ///< 值列表
        SubTablePointer    SubTable = nullptr; ///< 嵌套子表
    };

    using CatalogType     = std::vector<SCKeyValue>; ///< 目录类型
    using ValueType       = SCKeyValue; ///< 值类型
    using ReferType       = SCKeyValue&; ///< 引用类型
    using IterType        = CatalogType::iterator; ///< 迭代器类型
    using ConstIter       = CatalogType::const_iterator; ///< 常量迭代器类型

    CatalogType _M_Elems; ///< 元素存储容器

    /// @brief 获取目录引用
    /// @return 目录容器引用
    CatalogType& Get() {return _M_Elems;}
};

/// @brief 共享表指针类型定义
using SharedTablePointer = SharedPointer<SCSTable>;

/// @}

}

namespace __scstream_table_helpers {

/// @ingroup SCSBase
/// @{

/*!
 * @brief 检查字符串是否包含子串
 * @param Left 主字符串
 * @param Right 子字符串
 * @return 如果包含则返回true，否则返回false
 */
inline auto __Str_Contain(ustring Left, ustring Right)
{
    return Left.find(Right) != ustring::npos;
}

/*!
 * @brief 字符串分割函数
 * @param Str 输入字符串
 * @param Symb 分隔符(默认为'/')
 * @return 分割后的字符串列表
 */
inline ustringlist __Str_Split(ustring Str, ucs2_t Symb = L'/')
{
    ustringlist Toks;
    auto First = Str.find_first_not_of(Symb, 0);
    auto Current = Str.find_first_of(Symb, 0);
    for(; First != ustring::npos || Current != ustring::npos;)
    {
        Toks.emplace_back(Str.substr(First, Current - First));
        First = Str.find_first_not_of(Symb, Current);
        Current = Str.find_first_of(Symb, First);
    }
    return Toks;
}

/*!
 * @brief 在表中查找指定键
 * @param Src 源表指针
 * @param Key 查找键
 * @return 指向找到元素的迭代器，未找到则返回end()
 */
inline auto __Find_Table_From_List(const scenario::SharedTablePointer& Src, ustring Key)
{
    return find_if(Src->Get().begin(), Src->Get().end(), [Key](scenario::SCSTable::ValueType Tbl)
    {
        return Tbl.Key == Key;
    });
}

/*!
 * @brief 查找包含单位的键值
 * @param Src 源表指针
 * @param Key 查找键(前缀)
 * @return 指向找到元素的迭代器，未找到则返回end()
 */
inline auto __Find_Table_With_Unit(const scenario::SharedTablePointer& Src, ustring Key)
{
    return find_if(Src->Get().begin(), Src->Get().end(), [Key](scenario::SCSTable::ValueType Tbl)
    {
        return Tbl.Key.substr(0, Key.size()) == Key;
    });
}

/*!
 * @brief 查找表中所有匹配键的元素
 * @param Src 源表指针
 * @param Key 查找键
 * @return 包含所有匹配元素迭代器的向量
 */
inline auto __Find_Multi_Tables_From_List(const scenario::SharedTablePointer& Src, ustring Key)
{
    std::vector<decltype(Src->Get().begin())> Result;
    auto it = Src->Get().begin();
    for (; it != Src->Get().end();)
    {
        it = find_if(it, Src->Get().end(), [Key](scenario::SCSTable::ValueType Tbl)
        {
            return Tbl.Key == Key;
        });
        if (it != Src->Get().end())
        {
            Result.push_back(it);
            ++it;
        }
    }
    return Result;
}

/*!
 * @brief 查找包含关键字的键值
 * @param Src 源表指针
 * @param Key 关键字
 * @return 指向找到元素的迭代器，未找到则返回end()
 */
inline auto __Find_Table_With_KeyWord(const scenario::SharedTablePointer& Src, ustring Key)
{
    return find_if(Src->Get().begin(), Src->Get().end(), [Key](scenario::SCSTable::ValueType Tbl)
    {
        return Tbl.Key.find(Key) != ustring::npos;
    });
}

/*!
 * @brief 从表中获取指定键的值
 * @tparam VTy 值类型(仅限float64或ustring)
 * @param Dst 目标存储地址
 * @param Src 源表指针
 * @param Key 查找键
 * @param Alt 未找到时的默认值
 */
template<typename VTy> requires
(
    std::is_same_v<VTy, float64> ||
    std::is_same_v<VTy, ustring>
)
inline void __Get_Value_From_Table(VTy* Dst, const scenario::SharedTablePointer& Src, ustring Key, VTy Alt)
{
    auto it = __Find_Table_From_List(Src, Key);
    if (it != Src->Get().end()) {it->Value.front().GetQualified(Dst);}
    else {*Dst = Alt;}
}

/*!
 * @brief 从表中获取角度值
 * @param Dst 目标角度对象
 * @param Src 源表指针
 * @param Key 查找键
 * @param Alt 未找到时的默认值
 * @param Unit 单位标识(0:度,1:弧度,2:转,3:梯度)
 */
inline void __Get_Angle_From_Table(Angle* Dst, const scenario::SharedTablePointer& Src, ustring Key, Angle Alt, uint64 Unit = 0)
{
    auto it = __Find_Table_From_List(Src, Key);
    if (it != Src->Get().end())
    {
        float64 Value;
        it->Value.front().GetQualified(&Value);
        switch (Unit)
        {
        case 0:
        default:
            *Dst = Angle::FromDegrees(Value);
            return;
        case 1:
            *Dst = Angle::FromRadians(Value);
            return;
        case 2:
            *Dst = Angle::FromTurns(Value);
            return;
        case 3:
            *Dst = Angle::FromGradians(Value);
            return;
        }
    }
    else {*Dst = Alt;}
}

/*!
 * @brief 从表中获取布尔值
 * @tparam VTy 值类型(仅限bool)
 * @param Dst 目标存储地址
 * @param Src 源表指针
 * @param Key 查找键
 * @param Alt 未找到时的默认值
 */
template<typename VTy> requires std::is_same_v<VTy, bool>
inline void __Get_Value_From_Table(VTy* Dst, const scenario::SharedTablePointer& Src, ustring Key, VTy Alt)
{
    auto it = __Find_Table_From_List(Src, Key);
    if (it != Src->Get().end())
    {
        if (it->Value.front().Type == it->Value.front().Boolean)
        {
            it->Value.front().GetQualified(Dst);
        }
        else if (it->Value.front().Type == it->Value.front().Number)
        {
            int64 I;
            it->Value.front().GetAsBasedInteger(&I);
            *Dst = bool(I);
        }
        else {*Dst = Alt;}
    }
    else {*Dst = Alt;}
}

/*!
 * @brief 从表中获取整数值
 * @tparam VTy 值类型(仅限uint64或int64)
 * @param Dst 目标存储地址
 * @param Src 源表指针
 * @param Key 查找键
 * @param Alt 未找到时的默认值
 */
template<typename VTy> requires
(
    std::is_same_v<VTy, uint64> ||
    std::is_same_v<VTy, int64>
)
inline void __Get_Value_From_Table(VTy* Dst, const scenario::SharedTablePointer& Src, ustring Key, VTy Alt)
{
    auto it = __Find_Table_From_List(Src, Key);
    if (it != Src->Get().end()) {it->Value.front().GetAsBasedInteger(Dst);}
    else {*Dst = Alt;}
}

/*!
 * @brief 从表中获取固定大小数组
 * @tparam _Tp 元素类型
 * @tparam _Nm 数组大小
 * @param Dst 目标数组
 * @param Src 源表指针
 * @param Key 查找键
 * @param Alt 未找到时的默认值
 */
template<typename _Tp, size_t _Nm>
inline void __Get_Value_From_Table(gvec<_Tp, _Nm>* Dst, const scenario::SharedTablePointer& Src, ustring Key, gvec<_Tp, _Nm> Alt)
{
    auto it = __Find_Table_From_List(Src, Key);
    if (it != Src->Get().end()) {it->Value.front().GetAsArray(Dst);}
    else {*Dst = Alt;}
}

/*!
 * @brief 从表中获取日期值
 * @tparam VTy 值类型(仅限CSEDate)
 * @param Dst 目标日期对象
 * @param Src 源表指针
 * @param Key 查找键
 * @param Alt 未找到时的默认值
 */
template<typename VTy> requires std::is_same_v<VTy, _TIME CSEDate>
inline void __Get_Value_From_Table(VTy* Dst, const scenario::SharedTablePointer& Src, ustring Key, VTy Alt)
{
    auto it = __Find_Table_From_List(Src, Key);
    if (it != Src->Get().end())
    {
        CSEDateTime DateTime;
        it->Value.front().GetAsDateTime(&DateTime);
        *Dst = DateTime.date();
    }
    else {*Dst = Alt;}
}

/*!
 * @brief 从表中获取带单位的浮点值
 * @param Dst 目标存储地址
 * @param Src 源表指针
 * @param Key 查找键(前缀)
 * @param Alt 未找到时的默认值
 * @param DefMultiply 默认单位乘数
 * @param MultiplyOfUnits 单位乘数映射表
 */
inline void __Get_Value_With_Unit(float64* Dst, const scenario::SharedTablePointer& Src, ustring Key, float64 Alt, float64 DefMultiply, std::map<ustring, float64> MultiplyOfUnits)
{
    if (Src->Get().front().Value.front().GetTypeID() != scenario::ValueType::Number) {*Dst = Alt;}
    auto it = __Find_Table_With_Unit(Src, Key);
    if (it != Src->Get().end())
    {
        it->Value.front().GetQualified(Dst);
        if (it->Key != Key)
        {
            ustring Unit = ustring(it->Key.substr(Key.size(), it->Key.size() - Key.size()));
            if (MultiplyOfUnits.contains(Unit))
            {
                *Dst *= MultiplyOfUnits.at(Unit);
                return;
            }
        }
        *Dst *= DefMultiply;
    }
    else {*Dst = Alt;}
}

/*!
 * @brief 从表中获取带单位的浮点数组
 * @tparam _Nm 数组大小
 * @param Dst 目标数组
 * @param Src 源表指针
 * @param Key 查找键(前缀)
 * @param Alt 未找到时的默认值
 * @param DefMultiply 默认单位乘数
 * @param MultiplyOfUnits 单位乘数映射表
 */
template<size_t _Nm>
inline void __Get_Value_With_Unit(gvec<float64, _Nm>* Dst, const scenario::SharedTablePointer& Src, ustring Key, gvec<float64, _Nm> Alt, float64 DefMultiply, std::map<ustring, float64> MultiplyOfUnits)
{
    if (Src->Get().front().Value.front().GetTypeID() != scenario::ValueType::Number) {*Dst = Alt;}
    auto it = __Find_Table_With_Unit(Src, Key);
    if (it != Src->Get().end())
    {
        it->Value.front().GetAsArray(Dst);
        if (it->Key != Key)
        {
            ustring Unit = ustring(it->Key.substr(Key.size(), it->Key.size() - Key.size()));
            if (MultiplyOfUnits.contains(Unit))
            {
                *Dst = *Dst * MultiplyOfUnits.at(Unit);
                return;
            }
        }
        *Dst = *Dst * DefMultiply;
    }
    else {*Dst = Alt;}
}

// 转换辅助函数组

/*!
 * @brief 字符串列表连接为字符串
 * @param usl 字符串列表
 * @param pun 连接符(默认为'/')
 * @return 连接后的字符串
 */
inline ustring __Str_List_To_String(ustringlist usl, ucs2_t pun = L'/')
{
    ustring ustr;
    for (int i = 0; i < usl.size(); ++i)
    {
        ustr += usl[i];
        if (i < usl.size() - 1) {ustr += pun;}
    }
    return ustr;
}

/*! @{ */
/// @name 空数据检查函数
/// @{

/*!
 * @brief 检查浮点数是否为空数据
 * @param Val 待检查值
 * @return 为空数据返回true，否则false
 */
inline bool IsNoData(float64 Val)
{
    return IS_NO_DATA_DBL(Val);
}

/*!
 * @brief 检查整数是否为空数据
 * @param Val 待检查值
 * @return 为空数据返回true，否则false
 */
inline bool IsNoData(uint64 Val)
{
    return IS_NO_DATA_INT(Val);
}

/*!
 * @brief 检查字符串是否为空数据
 * @param Val 待检查值
 * @return 为空数据返回true，否则false
 */
inline bool IsNoData(ustring Val)
{
    return IS_NO_DATA_STR(Val);
}

/*!
 * @brief 检查向量是否包含空数据
 * @tparam Tp 元素类型
 * @tparam Nm 向量大小
 * @param Val 待检查向量
 * @return 包含空数据返回true，否则false
 */
template<typename Tp, size_t Nm>
inline bool IsNoData(gvec<Tp, Nm> Val)
{
    for (size_t i = 1; i < Val.size(); ++i)
    {
        if (IsNoData(Val[i])) {return 1;}
    }
    return 0;
}
/// @}

/*!
 * @brief 向表中添加空标签
 * @param Table 目标表
 */
inline void __Add_Empty_Tag(scenario::SCSTable* Table)
{
    Table->Get().push_back(scenario::SCSTable::SCKeyValue());
}

/*!
 * @brief 向表中添加键值对(通用类型)
 * @tparam genType 值类型(支持float64/uint64/int64/ustring)
 * @param Table 目标表
 * @param Key 键名
 * @param Value 键值
 * @param Fixed 是否固定浮点格式
 * @param Preci 浮点精度
 */
template<typename genType> requires
(
    std::is_same_v<genType, float64> ||
    std::is_same_v<genType, uint64> ||
    std::is_same_v<genType, int64> ||
    std::is_same_v<genType, ustring>
)
inline void __Add_Key_Value(scenario::SCSTable* Table, ustring Key, genType Value, bool Fixed, std::streamsize Preci)
{
    if (!IsNoData(Value))
    {
        scenario::SCSTable::SCKeyValue KeyValue;
        KeyValue.Key = Key;
        std::wostringstream ValueStr;
        if (Fixed) {ValueStr << std::fixed;}
        if (std::is_same_v<genType, float64>) {ValueStr.precision(Preci);}
        if (std::is_same_v<genType, ustring>) {ValueStr << L'\"' << Value << L'\"';}
        else {ValueStr << Value;}
        KeyValue.Value.push_back({.Type = scenario::ValueType::ToTypeID<decltype(Value)>(), .Value = {ustring(ValueStr.str())}});
        Table->Get().push_back(KeyValue);
    }
}

/*!
 * @brief 向表中添加布尔键值对
 * @tparam genType 值类型(仅限bool)
 * @param Table 目标表
 * @param Key 键名
 * @param Value 键值
 * @param Reverse 反转逻辑(为true时仅在Value为false时添加)
 * @param Preci 精度参数(未使用)
 */
template<typename genType> requires (std::is_same_v<genType, bool>)
inline void __Add_Key_Value(scenario::SCSTable* Table, ustring Key, genType Value, bool Reverse, std::streamsize Preci)
{
    if (Reverse ? !Value : Value)
    {
        scenario::SCSTable::SCKeyValue KeyValue;
        KeyValue.Key = Key;
        std::wostringstream ValueStr;
        ValueStr << std::boolalpha << Value;
        KeyValue.Value.push_back({.Type = scenario::ValueType::ToTypeID<decltype(Value)>(), .Value = {ustring(ValueStr.str())}});
        Table->Get().push_back(KeyValue);
    }
}

/*!
 * @brief 向表中添加向量键值对
 * @tparam Tp 元素类型
 * @tparam Nm 向量大小
 * @param Table 目标表
 * @param Key 键名
 * @param Value 键值向量
 * @param Fixed 是否固定浮点格式
 * @param Preci 浮点精度
 */
template<typename Tp, size_t Nm>
inline void __Add_Key_Value(scenario::SCSTable* Table, ustring Key, gvec<Tp, Nm> Value, bool Fixed, std::streamsize Preci)
{
    if (!IsNoData(Value))
    {
        scenario::SCSTable::SCKeyValue KeyValue;
        KeyValue.Key = Key;
        ustringlist VList;
        for (int i = 0; i < Value.size(); ++i)
        {
            std::wostringstream ValueStr;
            if (Fixed) {ValueStr << std::fixed;}
            ValueStr.precision(Preci);
            ValueStr << Value[i];
            VList.push_back(ustring(ValueStr.str()));
        }
        KeyValue.Value.push_back({.Type = scenario::ValueType::TypeID(scenario::ValueType::ToTypeID<decltype(Value[0])>() | scenario::ValueType::Array), .Value = VList});
        Table->Get().push_back(KeyValue);
    }
}

/*!
 * @brief 向表中添加日期键值对
 * @param Table 目标表
 * @param Key 键名
 * @param Value 日期值
 * @param Fixed 是否固定格式
 * @param Preci 精度参数(未使用)
 */
inline void __Add_Key_Value(scenario::SCSTable* Table, ustring Key, _TIME CSEDate Value, bool Fixed, std::streamsize Preci)
{
    if (Value.IsValid())
    {
        __Add_Key_Value(Table, Key,
        ustring(Value.ToString("{}-{:02}-{:02}")), Fixed, Preci);
    }
}

/// @}

}
}