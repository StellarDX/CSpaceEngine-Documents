/**
 * @file    OSCStream.h
 * @brief   SC格式输出模块：将天体对象数据输出到.sc文件
 * @details 本模块提供SC格式文件的输出功能，用于生成SpaceEngine软件兼容的天体数据文件。
 *          实现基于C++标准库的流式输出接口，支持自定义格式控制和编码处理。
 * @author  StellarDX Astronomy
 * @copyright Copyright (C) StellarDX Astronomy.
 */

namespace cse {
namespace scenario {

/**
 * @defgroup SCOutput SC 文件输出模块
 * @ingroup Parser
 * @{
 * @brief   SC格式输出模块：将天体对象数据输出到.sc文件
 * @details 本模块提供SC格式文件的输出功能，用于生成SpaceEngine软件兼容的天体数据文件。
 *          实现基于C++标准库的流式输出接口，支持自定义格式控制和编码处理。
 */

/**
 * @class OutputException
 * @brief 输出操作异常类
 * @details 继承自std::runtime_error，用于处理输出过程中的错误情况
 */
class OutputException : public std::runtime_error
{
public:
    /// @brief 构造函数
    /// @param _Message 异常信息字符串
    explicit OutputException(const std::string& _Message) : runtime_error(_Message) {}
};

/**
 * @struct __OSC_BASE
 * @brief 输出系统基础结构
 * @details 定义输出格式标志、自定义矩阵输出类型等基础组件
 */
struct __OSC_BASE
{
    using OutputType          = SCSTable;                      ///< 输出数据类型
    using CustomMatOutputType = std::map<ustring, int64>;      ///< 自定义矩阵输出类型
    enum _Fmtflags
    {
        _Fmtmask   = 0b11111111111111111111111111111111, ///< 格式标志掩码
        _Fmtzero   = 0b00000000000000000000000000000000, ///< 空格式标志
        _WaterMark = 0b10000000000000000000000000000000  ///< 水印标志位
    };

    CustomMatOutputType CustomMatOutputList; ///< 自定义矩阵输出选项列表
    virtual int _BaseInit() = 0;             ///< 基础初始化虚函数
};

/**
 * @class __SC_Smart_Output_Base
 * @brief 智能输出系统基类
 * @details 实现格式控制、编码处理、缓冲区管理等核心输出功能
 */
class __SC_Smart_Output_Base : public __OSC_BASE
{
public:
    using _TyBase  = __OSC_BASE;             ///< 基础类型
    using fmtflags = int;                     ///< 格式标志类型
    using outstate = int;                     ///< 输出状态类型
    using encoder  = __StelCXX_UString_Codec&;///< 编码器类型

    __SC_Smart_Output_Base(const __SC_Smart_Output_Base&) = delete;
    __SC_Smart_Output_Base& operator=(const __SC_Smart_Output_Base&) = delete;

private:
    __StelCXX_UString_Codec_65001 _DefEncod = __StelCXX_UString_Codec_65001(); ///< 默认编码器(UTF-8)

    fmtflags        _Fmtfl;     ///< 当前格式标志
    encoder         _Encod;     ///< 当前文件编码器
    std::streamsize _Prec;      ///< 字段精度（0=自动）
    std::streamsize _KeyWide;   ///< 键宽度（0=动态调整）

protected:
    /// @brief 基础初始化实现
    /// @return 初始化状态码
    int _BaseInit()override {return 0;}
    
    /// @brief 构造函数
    __SC_Smart_Output_Base(){_Init();}
    
    /// @brief 初始化内部状态
    void _Init();

public:
    /// @brief 生成输出文件水印
    #define OUTPUT_WATER_MARK(version, date, fmtfl, encod)\
    ustring(format\
        ("// File created by CSpaceEngine\n"\
         "// A Open-sourced C++ static library by StellarDX\n"\
         "// Compiler version : {}\n"\
         "// Date :             {}\n"\
         "// Format Flags :     0x{:X}\n"\
         "// Encoding :         {}\n",\
        version, date, fmtfl, encod))

    SCSTable _Buf; ///< 输出缓冲区

    /// @brief 获取当前格式标志
    /// @return 当前格式标志值
    _NODISCARD fmtflags flags() const;

    /// @brief 设置格式标志
    /// @param _Newfmtflags 新格式标志
    /// @return 更新后的格式标志
    fmtflags flags(fmtflags _Newfmtflags);

    /// @brief 设置指定格式标志位
    /// @param _Newfmtflags 要设置的标志位
    /// @param _Mask 标志位掩码
    /// @return 更新前的格式标志
    fmtflags setf(fmtflags _Newfmtflags, fmtflags _Mask = _TyBase::_Fmtmask);

    /// @brief 清除指定格式标志位
    /// @param _Mask 要清除的标志位掩码
    void unsetf(fmtflags _Mask);

private:
    /// @brief 计算动态键宽度
    /// @param Arg 键值参数
    /// @return 计算出的键宽度
    constexpr std::streamsize _Dynamic_Key_Wide(uint64 Arg)const;

public:
    /// @brief 获取当前键宽度设置
    /// @return 当前键宽度值
    _NODISCARD std::streamsize width() const;

    /// @brief 设置键宽度
    /// @param _Newwidth 新键宽度值
    /// @return 更新前的键宽度值
    std::streamsize width(std::streamsize _Newwidth);

    /// @brief 获取当前字段精度
    /// @return 当前精度值
    _NODISCARD std::streamsize precision() const;
    
    /// @brief 设置字段精度
    /// @param _Newprec 新精度值
    /// @return 更新前的精度值
    std::streamsize precision(std::streamsize _Newprec);

    /// @brief 获取当前编码器
    /// @return 当前编码器引用
    _NODISCARD encoder encod() const;
    
    /// @brief 设置编码器
    /// @param _NewEnc 新编码器
    void encod(encoder _NewEnc);

    /// @brief 将SCSTable写入字符串
    /// @param Table 输入数据表
    /// @param DisableWM 是否禁用水印
    /// @param Indents 缩进级别
    /// @return 格式化后的字符串
    ustring __Write(const SCSTable& Table, bool DisableWM = false, uint64 Indents = 0);
    
    /// @brief 纯虚函数：执行实际输出操作
    virtual void Write() = 0;
};

///@}

}

/**
 * @class OSCStream
 * @ingroup SCOutput
 * @brief SC格式输出流
 * @details 封装标准输出流，提供SC格式数据的序列化输出功能
 */
class OSCStream : public scenario::__SC_Smart_Output_Base
{
public:
    using _Mybase = scenario::__SC_Smart_Output_Base; ///< 基类类型别名

    std::ostream& output; ///< 底层输出流引用
    
    /// @brief 构造函数
    /// @param os 输出流对象
    OSCStream(std::ostream& os) : output(os) {_Init();}
    
    OSCStream& operator=(const OSCStream&) = delete;

    /// @brief 重写基础初始化
    /// @return 格式掩码值
    int _BaseInit()override {return _Fmtmask;}
    
    /// @brief 实现输出操作
    void Write()override;
};

/// @brief 创建SCSTable的模板函数
/// @ingroup SCOutput
/// @tparam _SEObject 天体对象类型（需继承自SEObject）
/// @param Object 天体对象实例
/// @param 格式标志
/// @param 精度值
/// @return 生成的SCSTable对象
template<typename _SEObject> requires std::is_base_of_v<SEObject, _SEObject>
scenario::SCSTable MakeTable(_SEObject Object, int, std::streamsize);

/// @brief SCSTable输出操作符重载
/// @ingroup SCOutput
/// @param os 输出流引用
/// @param table SCSTable对象
/// @return 输出流引用
scenario::__SC_Smart_Output_Base& operator<<(scenario::__SC_Smart_Output_Base& os, const scenario::SCSTable& table);

/// @brief 天体对象输出操作符重载
/// @ingroup SCOutput
/// @tparam _SEObject 天体对象类型（需继承自SEObject）
/// @param os 输出流引用
/// @param Object 天体对象实例
/// @return 输出流引用
template<typename _SEObject> requires std::is_base_of_v<SEObject, _SEObject>
scenario::__SC_Smart_Output_Base& operator<<(scenario::__SC_Smart_Output_Base& os, _SEObject Object)
{
    os << MakeTable(Object, os.flags(), os.precision());
    return os;
}

}