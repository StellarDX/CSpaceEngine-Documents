///////////////////////////////////////////////////////
/// @file       ISCStream.h
/// @brief      Space Engine SC 文件解析器
/// @details    提供对 Space Engine SC 文件的加载和解析功能
/// @author     StellarDX Astronomy
/// @copyright  Licensed under GNU General Public License v3.0
/// @note       支持 LALR1 和 LR1 解析算法
///////////////////////////////////////////////////////

namespace cse {
namespace scenario {

/**
 * @defgroup SCParser SC 文件解析模块
 * @ingroup Parser
 * @{
 * @brief Space Engine SC 文件解析器
 * @details
 * 本模块提供对 Space Engine SC 文件的加载和解析功能，支持多种解析算法。
 * 包含词法分析、语法分析功能。
 */

/// @name 解析算法类型定义
/// @{
#define LALR1 0  ///< LALR(1) 解析算法
#define LR1   1   ///< LR(1) 解析算法
#define IELR1 2   ///< IELR(1) 解析算法（已弃用）
/// @}

/// @name LR(1) 解析表参数（来自 YACC 解析器）
/// @{
#define SCFINAL    14  ///< 终止状态编号
#define SCLAST     481 ///< TABLE 最后索引（未使用）
#define SCNTOKENS  14  ///< 终结符数量
#define SCNNTS     9   ///< 非终结符数量
#define SCNRULES   24  ///< 规则数量
#define SCNSTATES  85  ///< 状态数量
/// @}

extern const __LR_Parser_Base<ucs2_t>::GrammaTableType __SE_Grammar_Production_Table; ///< 语法产生式表
extern const __LR_Parser_Base<ucs2_t>::StatesType __SE_State_Table;                  ///< 状态表

/// @name 预处理函数
/// @{

/**
 * @brief 跳过注释
 * @param Input 输入字符串
 */
void SkipComments(ustring& Input);

/// @}

/**
 * @brief SC文件词法分析器
 * @note 负责将输入文本转换为标记(token)序列
 */
class __SE_Lexicality
{
public:
    ustringlist VariableList; ///< 变量标识符列表

    __SE_Lexicality() {}

    /**
     * @brief 添加变量到标识符列表
     * @param Variable 变量列表
     */
    void AddVariable(ustringlist Variable);

    /// @name 字符分类函数
    /// @{

    /**
     * @brief 判断字符是否为标点符号
     * @param _C 待判断字符
     * @return 是标点符号返回1，否则返回0
     */
    static int IsPunctuator(ucs2_t _C);

    /**
     * @brief 判断字符是否为操作符
     * @param _C 待判断字符
     * @return 是操作符返回1，否则返回0
     */
    static int IsOperator(ucs2_t _C);

    /// @}

    /// @name 词素解析函数
    /// @{

    /**
     * @brief 跳过空白字符
     * @param it 字符串迭代器
     * @param end 字符串结束迭代器
     * @param line 行号指针
     * @param column 列号指针
     */
    static void ConsumeWhiteSpace(ustring::const_iterator& it, const ustring::const_iterator& end,
        uint64* line, uint64* column);

    /**
     * @brief 解析操作符
     * @param it 字符串迭代器
     * @param end 字符串结束迭代器
     * @param Output 输出字符串
     * @param line 行号指针
     * @param column 列号指针
     */
    static void ParseOperator(ustring::const_iterator& it, const ustring::const_iterator& end,
        ustring* Output, uint64* line, uint64* column);

    /**
     * @brief 解析标识符
     * @param it 字符串迭代器
     * @param end 字符串结束迭代器
     * @param Output 输出字符串
     * @param line 行号指针
     * @param column 列号指针
     */
    static void ParseIdentifier(ustring::const_iterator& it, const ustring::const_iterator& end,
        ustring* Output, uint64* line, uint64* column);

    /**
     * @brief 解析数字
     * @param it 字符串迭代器
     * @param end 字符串结束迭代器
     * @param Output 输出字符串
     * @param line 行号指针
     * @param column 列号指针
     * @param Base 数字基数指针
     */
    static void ParseNumber(ustring::const_iterator& it, const ustring::const_iterator& end,
        ustring* Output, uint64* line, uint64* column, int* Base);

    /**
     * @brief 解析字符串
     * @param it 字符串迭代器
     * @param end 字符串结束迭代器
     * @param Output 输出字符串
     * @param line 行号指针
     * @param column 列号指针
     */
    static void ParseString(ustring::const_iterator& it, const ustring::const_iterator& end,
        ustring* Output, uint64* line, uint64* column);

    /// @}

    /**
     * @brief 执行词法分析
     * @param Input 输入字符串
     * @param Output 输出Token数组
     */
    void Run(const ustring& Input, TokenArrayType* Output);
};

/**
 * @brief SC文件通用解析器
 * @note 使用LR算法将词法标记转换为结构化数据
 */
class __SE_General_Parser : public __StelCXX_LR_Parser<ucs2_t>
{
public:
    using _Mybase = __StelCXX_LR_Parser<ucs2_t>;

    __SE_General_Parser() : _Mybase(__SE_Grammar_Production_Table, __SE_State_Table) {}

    /**
     * @brief 将Token转换为字符串
     * @param Tokens Token数组
     * @return 转换后的字符串
     */
    ustring TokenToString(TokenArrayType Tokens);

    /**
     * @brief 创建SCSTable
     * @param SubTableTempStack 子表临时栈
     * @return 共享指针指向创建的SCSTable
     */
    SharedPointer<SCSTable> MakeTable(std::stack<SCSTable::SCKeyValue>& SubTableTempStack);

    /// @name 子矩阵操作函数
    /// @{

    /**
     * @brief 创建子矩阵
     * @param ExpressionBuffer 表达式缓冲区
     * @param SubMatrix 子矩阵
     */
    void MakeSubMatrix(ValueType& ExpressionBuffer, ValueType SubMatrix);

    /**
     * @brief 移动子矩阵
     * @param ExpressionBuffer 表达式缓冲区
     */
    void MoveSubMatrix(ValueType& ExpressionBuffer);

    /// @}

    /**
     * @brief 抛出解析错误
     * @param CurrentState 当前状态
     * @param Pos 错误位置
     * @param Msg 错误信息
     */
    void ThrowError(size_t CurrentState, ivec2 Pos, std::string Msg);

    /**
     * @brief 执行解析
     * @param Tokens Token数组
     * @return 共享指针指向解析后的SCSTable
     * @exception 可能抛出解析异常
     */
    SharedPointer<SCSTable> Run(TokenArrayType Tokens) noexcept(0);
};

///@}

}

/**
 * @brief 基于流的SC文件解析器
 * @ingroup SCParser
 * @note 提供从输入流解析SC文件的接口
 */
class ISCStream
{
public:
    __StelCXX_UString_Codec_65001 _DefDecoder = __StelCXX_UString_Codec_65001(); ///< 默认解码器
    __StelCXX_UString_Codec& Decoder = _DefDecoder; ///< 当前使用的解码器
    ustringlist VariableList; ///< 变量标识符列表

    std::istream& input; ///< 输入流
    ISCStream(std::istream& is) : input(is){} ///< 构造函数
    ISCStream& operator=(const ISCStream& is) = delete; ///< 禁止赋值

    /**
     * @brief 设置解码器
     * @param NewDecoder 新解码器
     */
    void SetDecoder(__StelCXX_UString_Codec& NewDecoder) {Decoder = NewDecoder;}

    /**
     * @brief 解析输入流
     * @return 共享指针指向解析后的SCSTable
     */
    scenario::SharedTablePointer Parse()const;
};

/**
 * @brief 解析文件
 * @ingroup SCParser
 * @param FileName 文件路径
 * @return 共享指针指向解析后的SCSTable
 * @exception 可能抛出文件操作异常
 */
scenario::SharedTablePointer ParseFile(std::filesystem::path FileName)noexcept(false);

/**
 * @brief 解析脚本
 * @ingroup SCParser
 * @param FileName 文件路径
 * @param VariableList 变量列表
 * @return 共享指针指向解析后的SCSTable
 * @exception 可能抛出文件操作异常
 */
scenario::SharedTablePointer ParseScript(std::filesystem::path FileName, ustringlist VariableList)noexcept(false);

/**
 * @brief 从SCSTable中获取指定对象
 * @ingroup SCParser
 * @tparam _SEObject 对象类型，必须继承自SEObject
 * @param Table SCSTable指针
 * @param Name 对象名称
 * @return 请求的对象
 */
template<typename _SEObject> requires std::is_base_of_v<SEObject, _SEObject>
_SEObject GetObject(scenario::SharedTablePointer Table, ustring Name);

}