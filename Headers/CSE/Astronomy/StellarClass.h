/**
 * @file StellarClassification.h
 * @defgroup StellarClass 恒星光谱分析工具（近乎全覆盖）
 * @ingroup Astronomy
 * @brief 恒星光谱分类系统 - 基于工厂模式的光谱数据解析框架
 * @details
 * 丹霞：由于不同类型的恒星光谱字符串采用了不同的结构，同种恒星的光谱型也可能会出现不同的写法，所以就导致恒星光谱字符串的分析变得难上加难，甚至分析的逻辑为了最大兼容不得不肆无忌惮的“拧麻花”。为增加灵活性，此处使用一种基于工厂函数的方式。也就是先将恒星分类，每一类对应一种继承自同一个接口的数据结构，构造时就可以用工厂函数进行统一管理。分析字符串时先大致把字符串扫描一遍以确定它可能属于哪一类恒星，在调用对应数据结构的分析函数分析字符串，完成后的数据结构以接口指针的形式返回。分析出来的数据字段用一堆Getter访问，这样就可以无视数据结构的具体实现，而且历史兼容性和扩展性也强。
 */

namespace cse {

/**
 * @brief 恒星光谱数据接口类
 * @ingroup StellarClass
 * @details
 * 定义恒星光谱数据的基本操作方法，包括光谱类型、光度等级、特殊特征等信息的获取。
 * 所有具体的恒星光谱数据类都需要继承此接口以保证统一的访问方式。
 */
class StellarClassData 
{
public:
    /**
     * @brief 获取恒星光谱的描述信息
     * @return 恒星光谱描述字符串
     */
    virtual ustring Description() const = 0;

    /**
     * @brief 将恒星光谱数据转换为字符串表示
     * @return 恒星光谱字符串表示
     */
    virtual ustring ToString() const = 0;

    /**
     * @brief 获取光谱分类字符
     * @param Index 索引，用于处理多重光谱分类的情况，默认为0
     * @return 光谱分类字符（如O、B、A、F、G、K、M等）
     */
    virtual char SpectralClass(size_t Index = 0) const = 0;

    /**
     * @brief 获取光谱分类的不确定性值
     * @param Index 索引，用于处理多重光谱分类的情况，默认为0
     * @return 光谱分类不确定性值
     */
    virtual uint16_t SpectralClassUncertained(size_t Index = 0) const = 0;

    /**
     * @brief 获取专门化分类值
     * @param Index 索引，用于处理多重光谱分类的情况，默认为0
     * @return 专门化分类值（*100后）
     */
    virtual uint16_t SpecializedClass(size_t Index = 0) const = 0;

    /**
     * @brief 获取专门化分类的不确定性值
     * @param Index 索引，用于处理多重光谱分类的情况，默认为0
     * @return 专门化分类不确定性值
     */
    virtual uint16_t SpecializedClassUncertained(size_t Index = 0) const = 0;

    /**
     * @brief 获取光度分类字符串
     * @param Index 索引，用于处理多重光度分类的情况，默认为0
     * @return 光度分类字符串（如I、II、III、IV、V等）
     */
    virtual ustring LuminosityClass(size_t Index = 0) const = 0;

    /**
     * @brief 获取光度分类的不确定性值
     * @param Index 索引，用于处理多重光度分类的情况，默认为0
     * @return 光度分类不确定性值
     */
    virtual uint16_t LuminosityClassUncertained(size_t Index = 0) const = 0;

    /**
     * @brief 获取光谱特殊性集合
     * @return 包含所有光谱特殊性标识符的集合
     */
    virtual std::set<ustring> SpectralPeculiarities() const = 0;

    /**
     * @brief 获取元素符号集合
     * @return 包含所有元素符号的集合
     */
    virtual std::set<ustring> ElementSymbols() const = 0;

    /**
     * @brief 获取波段特殊性映射
     * @return 包含波段特殊性信息的映射表
     */
    virtual std::map<ustring, std::variant<int16_t, ustring>> BandPecularities() const = 0;

    /**
     * @brief 获取剩余未解析的字符串
     * @return 未被解析的剩余字符串
     */
    virtual ustring UnanalyzedString() const = 0;
};

/**
 * @brief 恒星光谱分类工厂类
 * @ingroup StellarClass
 * @details 用于创建和管理不同类型的恒星光谱数据对象。通过工厂模式，可以根据输入的光谱字符串自动检测类型并创建相应的数据对象。
 */
typedef class StellarClassificationFactoryClass
{
public:
    using ValueType    = StellarClassificationFactoryClass;
    using Pointer      = std::shared_ptr<StellarClassData>;
    using DetectorType = uint8_t(ustring);
    using ParserType   = Pointer(ustring);

    /**
     * @brief 恒星类型
     */
    enum StarTypes
    {
        Auto,   ///< 自动检测
        Normal  ///< 普通类型
    };

    /**
     * @brief 元数据结构体
     * 存储探测器和解析器函数指针
     */
    struct MetaDataType
    {
        DetectorType* DFunc; ///< 检测器函数指针
        ParserType*   PFunc; ///< 解析器函数指针
    };

private:
    /**
     * @brief 元数据映射表
     * 存储不同类型对应的探测器和解析器
     */
    static std::map<StarTypes, MetaDataType> MetaData;
    
    /**
     * @brief 自动检测函数
     * 根据输入字符串自动判断恒星类型
     * @param Class 输入的光谱分类字符串
     * @return 检测到的恒星类型
     */
    static ValueType AutoDetect(const ustring& Class);

    /**
     * @brief 数据指针
     * 存储实际的恒星光谱数据对象
     */
    Pointer Data;

public:
    /**
     * @brief 注册新的恒星类型处理器
     * @param Type 恒星类型
     * @param Ctor 解析器构造函数指针
     * @param Detector 探测器函数指针
     */
    static void Register(StarTypes Type, ParserType* Ctor, DetectorType* Detector);

    /**
     * @brief 从字符串创建恒星光谱对象
     * @param Class 光谱分类字符串
     * @param Type 恒星类型，默认为Auto（自动检测）
     * @return 恒星光谱工厂对象
     */
    static ValueType CreateFromString(const ustring& Class, StarTypes Type = Auto);

    /**
     * @brief 将恒星光谱对象导出为字符串
     * @param Class 恒星光谱工厂对象
     * @return 字符串表示
     */
    static ustring ExportToString(const ValueType& Class);

    /**
     * @brief 获取内部数据指针
     * @return 指向StellarClassData的智能指针
     */
    Pointer Get() const;

    /**
     * @brief 构造函数
     * @param SpType 光谱类型字符串
     */
    StellarClassificationFactoryClass(const ustring& SpType);

    /**
     * @brief 转换为字符串的运算符重载
     */
    operator ustring() const;
}StellarClassification;

namespace StelCls {

/**
 * @brief 最常见的光谱型恒星解析类
 * @ingroup StellarClass
 *
 * @details 此类恒星包含主序星、亚巨星、巨星、超巨星、特超巨星，以及带有特殊谱线的恒星、
 *          化学特殊星和CN恒星。由于是最常见的光谱型，也因此产生了最五花八门的写法。
 *
 * @note 为了最大限度的提高容错率和解决各种历史遗留问题，搞得整个分析的程序如同水多加面面多加水一般。
 * 当然这也意味着后期维护的难度会被提高到丧心病狂的程度。
 *
 * 举个例子：
 * |恒星|光谱|释义|
 * |---|---|---|
 * |太阳|G2V|G型主序星|
 * |大角|K1.5IIIFe-0.5|铁含量偏低的K型巨星|
 * |天津四|A2Ia|A型超巨星|
 * |参旗一|M3SIII|AGB上的一个S型恒星|
 * |王良二|BC0.7Ia|B型碳星，缺氮|
 * |HD 194525|G2Ib-II|光度级无法准确归类的G型恒星|
 * |HD 88205|F2/3IV/V|子类型和光度级无法准确归类的F型恒星|
 * |HD 46711|B8Ia/ab|光度级无法准确归类的B型超巨星|
 * |OH 231.8+04.2 A(QX Pup A)|M10III|温度超低（<2500K）的红巨星，当前正在转化为行星状星云，伴星是一个A0型主序星|
 * |HD 46711|B8Ia/ab|光度级无法准确归类的B型超巨星|
 * |HD 10607|G0Vm-2.25|金属线偏弱的G型主序星|
 * |HD 271182|F8_0|F型特超巨星|
 * |HD 214714|G3Ib-II:CN-1CH2Fe-1|拥有十分独特化学组分，且光度级无法准确归类的的G型恒星|
 * |常陈一|A0VpSiEu|A型化学特殊星，带有硅和铕的谱线且磁场比普通A型恒星强|
 * |壁宿二|B8IV-VHgMn|B型汞锰星|
 * |十二国·燕|G4Ib:Ba2|钡星，伴星为DA2.2型白矮星|
 * |房宿增三|K3-IIICN2|CN星|
 * |辇道四|B8pSi(FeII)|硅星，且铁多氦少|
 * |HD 210030|G8/K0II/III(pBa)|化学特殊星|
 * |HD 224906|B9IIIp?(HgMn?)|不确定是否是汞锰星的B型巨星|
 * |HD 54786|B1/2I(b)|B型超巨星，可能有白矮星伴星|
 * |HD 12834|G(6)IIICNIV/Vp|CN星|
 * |HD 126399|K(3/5)III|无法确定子类型的K型巨星|
 * |HD 144695|O8/9.5(Ib/II)|子类型和光度级无法准确归类的O型恒星|
 * |HD 146850|K3IIICNVp|含有锂光谱的CN星|
 *
 * @see https://en.wikipedia.org/wiki/Stellar_classification
 * @see https://en.wikipedia.org/wiki/Chemically_peculiar_star
 * @see https://en.wikipedia.org/wiki/CN_star
 */
class NormalStar : public StellarClassData
{
public:
    /**
     * @brief 解析器状态
     * @details 定义了解析过程中不同阶段的状态，包括光谱类型、次型、光度级等各个解析阶段
     */
    enum ParserStateType
    {
        PStart = 0b00000, ///< 开始状态
        PSpec = 0b00001,  ///< 光谱类型解析状态
        PSub = 0b00010,   ///< 次型解析状态
        PLum = 0b00011,   ///< 光度级解析状态
        PSLum = 0b00100,  ///< 子光度级解析状态
        PPec = 0b00101,   ///< 特殊谱线解析状态
        PChem = 0b00110,  ///< 化学元素解析状态
        PBar = 0b00111,   ///< 波段特征解析状态
        POpMask = 0b11000,///< 操作掩码
        PRange = 0b01000, ///< 范围解析状态
        PBracket = 0b10000///< 括号解析状态
    };

    /// 事件队列类型定义
    using EventQueueType = std::vector<std::function<void(NormalStar*, ustring, ustring*, ParserStateType*)>>;

    /**
     * @brief 不确定度符号处理类
     * @details 处理光谱型中的不确定标记，如 :、?、+、-、/ 以及括号分组
     */
    struct UncertaintyType
    {
        /// 不确定性模式正则表达式
        static const wregex UncertaintyPattern;

        /**
         * @brief 不确定性符号
         * @details 定义了光谱类型中可能出现的各种不确定性符号及其位掩码
         * @note 每个符号都有对应的二进制掩码，便于按位操作
         */
        enum UncertaintySymbols : uint16_t 
        {
            None        = 0b000000000,  ///< 无不确定标记
            Uncertained = 0b000000011,  ///< ":" — 不确定
            More        = 0b000000010,  ///< "+" — 更高/更亮
            Less        = 0b000000001,  ///< "-" — 更低/更暗
            Bracket     = 0b000011100,  ///< "()" — 括号相关掩码
            BrakStart   = 0b000001100,  ///< "(" — 左括号开始
            BrakIn      = 0b000000100,  ///< 括号内部标记
            BrakEnd     = 0b000010100,  ///< ")" — 右括号结束

            RangeMask   = 0b011100000,  ///< 范围标记掩码
            Ranged      = 0b000100000,  ///< 范围标记（如 G2-3）
            Slash       = 0b001000000,  ///< "/" — 斜杠分隔
            Simplified  = 0b010000000,  ///< 简化表示（如 M1-2, Ia-ab）

            Ques        = 0b100000011   ///< "?" — 疑问标记 (不确定符号的另一写法，如HD 224906的光谱为B9IIIp?(HgMn?))
        };

        /**
         * @brief 处理左括号
         * @param[in] Source 输入字符串
         * @param[out] Remain 剩余字符串
         * @param[in,out] Data 不确定性符号数据
         * @param[in,out] State 解析状态
         */
        static void BracketStart(ustring Source, ustring* Remain, UncertaintySymbols* Data, ParserStateType* State);

        /**
         * @brief 处理右括号
         * @param[in] Source 输入字符串
         * @param[out] Remain 剩余字符串
         * @param[in,out] Data 不确定性符号数据
         * @param[in,out] State 解析状态
         */
        static void BracketEnd(ustring Source, ustring* Remain, UncertaintySymbols* Data, ParserStateType* State);

        /**
         * @brief 检查括号是否结束
         * @param[in] Symbol 不确定性符号
         * @return 是否括号结束（已结束返回0）
         */
        static bool CheckBracketEnd(const UncertaintySymbols& Symbol);

        /**
         * @brief 添加不确定性标记
         * @param[in,out] In 字符串引用
         * @param[in] Symbol 不确定性符号
         */
        static void AddUncertainty(ustring* In, UncertaintyType::UncertaintySymbols Symbol);
    };

    /**
     * @brief 光谱型数值类
     * @details 存储光谱型的主要数据：光谱字母、数字次型、光度级及其子型
     */
    struct ValueType
    {
        /// 光谱类型模式正则表达式 (O|B|A|F|G|K|M)
        static const wregex SpecClassPattern;
        /// 数字模式正则表达式 (用于次型)
        static const wregex NumberPattern;
        /// 光度级模式正则表达式 (V|IV|III|II|I|_0)
        static const wregex LuminosityClassPattern;
        /// 光度级模式正则表达式2 (V|IV|III|II|I|0)
        static const wregex LuminosityClassPattern2;
        /// 子光度级模式正则表达式 (ab|a|b)
        static const wregex SubLumPattern;

        /// uint16最大值常量
        static const uint16_t U16Npos = ~(uint16_t(0));

        /**
         * @brief 详细数据结构
         * @details 包含光谱类型、次型、光度级等所有详细信息
         */
        struct Details
        {
            /**
             * @brief 光谱类型
             * @details 恒星的主要光谱分类，从O型到M型
             * @note 每个类型都有对应的二进制值
             */
            enum : uint8_t 
            {
                O = 0b001,  ///< O型恒星 (>= 33,000 K)
                B = 0b010,  ///< B型恒星 (10,000-33,000 K)
                A = 0b011,  ///< A型恒星 (7,300-10,000 K)
                F = 0b100,  ///< F型恒星 (6,000-7,300 K)
                G = 0b101,  ///< G型恒星 (5,300-6,000 K，太阳属于此类)
                K = 0b110,  ///< K型恒星 (3,900-5,300 K)
                M = 0b111   ///< M型恒星 (2,300-3,900 K，红矮星)
            } Spec = decltype(Spec)(0); ///< 主光谱类型
            
            /// 光谱类型的不确定性符号
            UncertaintyType::UncertaintySymbols SpecU = UncertaintyType::None;

            /**
             * @brief 数字次型（乘以100存储）
             * @details 例如：0 → 0, 2 → 200, 9.75 → 975
             * @note 先前见过一例O9.75的和380例M10甚至M10.5的，不过O9.75后续未能查到对应恒星，推测可能为上世纪的论文中出现过的但后来不再分这么细了。M10及以下的均为温度只有2000K出头的红超巨星，最低可低至1800K以下，如葫芦星云的中心恒星OH 231.8+04.2(QX Pup)主星的光谱型为M10III，伴星是一颗A型恒星。著名的V* V838 Mon的光谱也是M10（现修正为M5.5I）。
             */
            uint16_t Sub = U16Npos;

            /// 次型的不确定性符号
            UncertaintyType::UncertaintySymbols SubU = UncertaintyType::None;

            /**
             * @brief 光度级
             * @details 恒星的光度等级，从超巨星到主序星
             * @note 包含特超巨星、超巨星、亮巨星、巨星、亚巨星、主序星
             */
            enum : uint8_t 
            {
                hg = 0b111,    ///< 特超巨星 (Hypergiants)
                sg = 0b110,    ///< 超巨星 (Supergiants)
                bg = 0b101,    ///< 亮巨星 (Bright giants)
                g = 0b100,     ///< 巨星 (Giants)
                gmask = 0b100, ///< 巨星掩码
                subg = 0b011,  ///< 亚巨星 (Subgiants)
                ms = 0b001     ///< 主序星 (Main Sequence)
            } Lum = decltype(Lum)(0); ///< 光度级

            /// 光度级的不确定性符号
            UncertaintyType::UncertaintySymbols LumU = UncertaintyType::None;

            /**
             * @brief 子光度级
             * @details 在光度级基础上的进一步细分
             */
            enum : uint8_t
            {
                a  = 0b01,   ///< a型（亮端）
                b  = 0b10,   ///< b型（暗端）
                ab = 0b11    ///< ab型（中间型）
            } SLum = decltype(SLum)(0); ///< 子光度级

            /// 子光度级的不确定性符号
            UncertaintyType::UncertaintySymbols SLumU = UncertaintyType::None;
        }DetailedData;

        /**
         * @brief 加载光谱类型实现
         * @param[in] SpecString 光谱类型字符串
         * @param[out] Destination 目标对象
         */
        static void LoadSpecImpl(ustring SpecString, ValueType* Destination);

        /**
         * @brief 加载数字(次型)
         * @param[in] SpecString 次型字符串
         * @param[out] Destination 目标对象
         * @note 至于为什么要*100后储存，问就是计算机算不出0.1+0.2=0.3
         */
        static void LoadNumber(ustring SpecString, uint16_t* Destination);

        /**
         * @brief 加载光度级实现
         * @param[in] SpecString 光度级字符串
         * @param[out] Destination 目标对象
         */
        static void LoadLumImpl(ustring SpecString, ValueType* Destination);

        /**
         * @brief 加载子光度级实现
         * @param[in] SpecString 子光度级字符串
         * @param[out] Destination 目标对象
         */
        static void LoadSubLumImpl(ustring SpecString, ValueType* Destination);

        /**
         * @brief 特殊谱线括号开始预检查
         * @param[in] Destination 目标对象
         * @param[in] Depth 深度，默认4
         * @return 预检查结果
         */
        static bool PecularitiesBrakStartPrecheck(ValueType* Destination, int Depth = 4);

        /**
         * @brief 光谱类型转字符
         * @param[in] Data 光谱类型数据
         * @return 对应字符
         */
        static char SpecToChar(decltype(DetailedData.Spec) Data);

        /**
         * @brief 次型转字符串
         * @param[in] Data 次型数据
         * @return 对应字符串
         */
        static ustring SubToString(decltype(DetailedData.Sub) Data);

        /**
         * @brief 光度级转字符串
         * @param[in] Data 光度级数据
         * @param[in] __2 是否使用第二种格式，默认false
         * @return 对应字符串
         */
        static ustring LumToString(decltype(DetailedData.Lum) Data, bool __2 = 0);

        /**
         * @brief 子光度级转字符串
         * @param[in] Data 子光度级数据
         * @return 对应字符串
         */
        static ustring SLumToString(decltype(DetailedData.SLum) Data);
    };

    /**
     * @brief 碳星/S型星类型
     * @details 处理碳星（C、N）和S型星的特殊分类
     */
    struct CarbonType
    {
        /// 碳或S型正则表达式
        static const wregex CarbonOrSTypePattern;

        /**
         * @brief 碳型数据枚举
         * @details 包含C型(BC/OC)、N型(BN/ON)、S型星(M?S)
         */
        enum {C, N, S} Data;
    };

    /**
     * @brief 特殊谱线标识
     * @details 表示恒星光谱中的特殊特征，如发射线、吸收线等
     *
     * 常见特殊谱线标记(来源：<a href="https://en.wikipedia.org/wiki/Stellar_classification">Wikipedia</a>)
     * <table>
     * <tbody><tr>
     * <th>Code
     * </th>
     * <th>Spectral peculiarities for stars
     * </th></tr>
     * <tr>
     * <th>:
     * </th>
     * <td>uncertain spectral value
     * </td></tr>
     * <tr>
     * <th>...
     * </th>
     * <td>Undescribed spectral peculiarities exist
     * </td></tr>
     * <tr>
     * <th>!
     * </th>
     * <td>Special peculiarity
     * </td></tr>
     * <tr>
     * <th>comp
     * </th>
     * <td>Composite spectrum
     * </td></tr>
     * <tr>
     * <th>e
     * </th>
     * <td>Emission lines present
     * </td></tr>
     * <tr>
     * <th>[e]
     * </th>
     * <td><a href="https://en.wikipedia.org/wiki/Forbidden_mechanism" title="Forbidden mechanism">"Forbidden" emission lines</a> present
     * </td></tr>
     * <tr>
     * <th>er
     * </th>
     * <td>"Reversed" center of emission lines weaker than edges
     * </td></tr>
     * <tr>
     * <th>eq
     * </th>
     * <td>Emission lines with <a href="https://en.wikipedia.org/wiki/P_Cygni_profile" class="mw-redirect" title="P Cygni profile">P Cygni profile</a>
     * </td></tr>
     * <tr>
     * <th>f
     * </th>
     * <td>N III and He II emission
     * </td></tr>
     * <tr>
     * <th>f*
     * </th>
     * <td>N IV 4058Å is stronger than the N III 4634Å, 4640Å, &amp; 4642Å lines
     * </td></tr>
     * <tr>
     * <th>f+
     * </th>
     * <td>Si IV 4089Å &amp; 4116Å are emitted, in addition to the N III line
     * </td></tr>
     * <tr>
     * <th>f?
     * </th>
     * <td>C III 4647-4650-4652Å emission lines with comparable strength to the N III line
     * <tr>
     * <th>(f)
     * </th>
     * <td>N III emission, absence or weak absorption of He II
     * </td></tr>
     * <tr>
     * <th>(f+)
     * </th>
     * <td>See: <a href="https://iopscience.iop.org/article/10.1088/0004-6256/138/2/510/pdf">FarinA C , Bosch G L , Morrell N I ,et al.Spectroscopic study of the N159/N160 complex in the Large Magellanic Cloud[J].The Astronomical Journal, 2009, 138(2):510.DOI:10.1088/0004-6256/138/2/510.</a>
     * </td></tr>
     * <tr>
     * <th>((f))
     * </th>
     * <td>Displays strong He II absorption accompanied by weak N III emissions
     * </td></tr>
     * <tr>
     * <th>((f*))
     * </th>
     * <td>See: <a href="https://iopscience.iop.org/article/10.1088/0004-6256/138/2/510/pdf">FarinA C , Bosch G L , Morrell N I ,et al.Spectroscopic study of the N159/N160 complex in the Large Magellanic Cloud[J].The Astronomical Journal, 2009, 138(2):510.DOI:10.1088/0004-6256/138/2/510.</a>
     * </td></tr>
     * <tr>
     * <th>h
     * </th>
     * <td>WR stars with hydrogen emission lines.
     * </td></tr>
     * <tr>
     * <th>ha
     * </th>
     * <td>WR stars with hydrogen seen in both absorption and emission.
     * </td></tr>
     * <tr>
     * <th>He wk
     * </th>
     * <td>Weak Helium lines
     * </td></tr>
     * <tr>
     * <th>k
     * </th>
     * <td>Spectra with interstellar absorption features
     * </td></tr>
     * <tr>
     * <th>m
     * </th>
     * <td>Enhanced metal features
     * </td></tr>
     * <tr>
     * <th>n
     * </th>
     * <td>Broad ("nebulous") absorption due to spinning
     * </td></tr>
     * <tr>
     * <th>nn
     * </th>
     * <td>Very broad absorption features
     * </td></tr>
     * <tr>
     * <th>neb
     * </th>
     * <td>A nebula's spectrum mixed in
     * </td></tr>
     * <tr>
     * <th>p
     * </th>
     * <td>Unspecified peculiarity, <a href="https://en.wikipedia.org/wiki/Peculiar_star" class="mw-redirect" title="Peculiar star">peculiar star</a>.
     * </td></tr>
     * <tr>
     * <th>pq
     * </th>
     * <td>Peculiar spectrum, similar to the spectra of novae
     * </td></tr>
     * <tr>
     * <th>q
     * </th>
     * <td><a href="https://en.wikipedia.org/wiki/P_Cygni_profile" class="mw-redirect" title="P Cygni profile">P Cygni profiles</a>
     * </td></tr>
     * <tr>
     * <th>s
     * </th>
     * <td>Narrow ("sharp") absorption lines
     * </td></tr>
     * <tr>
     * <th>ss
     * </th>
     * <td>Very narrow lines
     * </td></tr>
     * <tr>
     * <th>sh
     * </th>
     * <td><a href="https://en.wikipedia.org/wiki/Shell_star" title="Shell star">Shell star</a> features
     * </td></tr>
     * <tr>
     * <th>var
     * </th>
     * <td>Variable spectral feature
     * </td></tr>
     * <tr>
     * <th>wl
     * </th>
     * <td>Weak lines (also "w" &amp; "wk")
     * </td></tr>
     * <tr>
     * <th>z
     * </th>
     * <td>indicating an abnormally strong ionised helium line at <span class="nowrap"><span data-sort-value="6993468600000000000♠"></span>468.6 nm</span>
     */
    struct PeculiarityType
    {
        /// 光谱特殊谱线表
        static ustringlist SpectralPecularitiesTable;
        /// 特殊谱线正则表达式
        static wregex Pattern;

        ustring Element; ///< 特殊谱线元素

        /**
         * @brief 不确定性符号
         * @note 基本用不到，但也可能会出现，如HD 210030为G8/K0II/III(pBa)
         */
        UncertaintyType::UncertaintySymbols Uncertainty = UncertaintyType::None;
    };
    
    /**
     * @brief 波段特征
     * @details 表示恒星的分子带、谱带特征，如 CN、CH、C2 等
     */
    struct BandPecularitiesType
    {
        /// 波段特殊谱线正则表达式框架
        static const std::string PatternSkeleton;
        /// 吸收特殊谱线表
        static ustringlist AbsorptionPeculiarityTable;
        /// 波段特殊谱线正则表达式
        static wregex Pattern;

        ustring Key; ///< 波段特征键（如 CN、CH、Fe）
        std::variant<int16_t, ustring> Value; ///< 波段值(数字或字符串)，数字会乘以100存储，问就是发现了一例HD 10607的光谱为G0Vm-2.25
        UncertaintyType::UncertaintySymbols Uncertainty = UncertaintyType::None; ///< 不确定性符号

        /**
         * @brief 加载波段特殊谱线的实现
         * @param[in] Source 输入字符串
         * @return 波段特殊谱线对象
         */
        static BandPecularitiesType Load(ustring Source);
    };

    /**
     * @brief 化学元素特殊星
     * @details 表示具有异常化学丰度的恒星，如 HgMn 星、Si 星、Cr 星等
     * @note 包含周期表中的元素，但不包括放射性元素（不可能出现）
     */
    struct ChemicalPeculiaritySpec
    {
        /// 元素周期表
        static const ustringlist ChemicalElementsTable;
        /// 化学元素正则表达式
        static wregex Pattern;

        ustring Element; ///< 化学元素
        UncertaintyType::UncertaintySymbols Uncertainty = UncertaintyType::None; ///< 不确定性符号
    };

protected:
    ValueType Data; ///< 主要数据 Ex: G2V, K3III, A2Ia, ...（至少有一个SpecType）

    std::optional<CarbonType> CData; ///< 碳型数据 Ex: M3SIII, OC9.7Ib（与浮动数据可同时存在）
    std::optional<ValueType> FloatData; ///< 浮动数据 Ex: G2Ib-II, F2/3IV/V, B8Ia/ab

    /**
     * @brief 特殊谱线 Ex: (n)fp, e, ...
     * @note 原以为特殊谱线，元素符号和波段特征三种字段有互斥关系，但后来发现一例三种字段同时出现的：辇道四 B8pSi(FeII)
     */
    std::vector<PeculiarityType> Pecularities;
    std::vector<ChemicalPeculiaritySpec> ChemicalElems; ///< 化学元素 Ex: SiSrCr, ...
    std::vector<BandPecularitiesType> BandPecs; ///< 波段特征 Ex: Ba1，Fe-0.5, CNIV, ...
    bool PecularitiesBehindBandPecs = 0; ///< 改变输出顺序，问就是发现一例HD 146850的光谱为K3IIICNVp

    ustring RemainString; // 剩余无法解析的字符串信息

    /**
     * @brief ParseFunc中的事件队列
     */
    static const EventQueueType ParserEventQueue;
    
    /**
     * @brief 设置解析状态
     * @param[in,out] State 状态指针
     * @param[in] Value 新状态值
     */
    static void SetState(ParserStateType* State, ParserStateType Value);

    /**
     * @brief 跳过空格字符
     * @param[in] Source 源字符串
     * @param[out] Remain 剩余字符串
     */
    static void ConsumeSpace(ustring Source, ustring* Remain);

    /**
     * @brief 不确定性处理函数
     * @param[in,out] Output 输出对象
     * @param[in] Source 输入字符串
     * @param[out] Remain 剩余字符串
     * @param[in,out] State 解析状态
     */
    static void UncertaintyHandler(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State);

    /**
     * @brief 加载光谱类型
     * @param[in,out] Output 输出对象
     * @param[in] Source 输入字符串
     * @param[out] Remain 剩余字符串
     * @param[in,out] State 解析状态
     */
    static void LoadSpec(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State);

    /**
     * @brief 加载碳型
     * @param[in,out] Output 输出对象
     * @param[in] Source 输入字符串
     * @param[out] Remain 剩余字符串
     * @param[in,out] State 解析状态
     */
    static void LoadCarbonType(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State);

    /**
     * @brief 加载次型
     * @param[in,out] Output 输出对象
     * @param[in] Source 输入字符串
     * @param[out] Remain 剩余字符串
     * @param[in,out] State 解析状态
     */
    static void LoadSub(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State);

    /**
     * @brief 加载S型
     * @param[in,out] Output 输出对象
     * @param[in] Source 输入字符串
     * @param[out] Remain 剩余字符串
     * @param[in,out] State 解析状态
     */
    static void LoadSType(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State);

    /**
     * @brief 加载光度级
     * @param[in,out] Output 输出对象
     * @param[in] Source 输入字符串
     * @param[out] Remain 剩余字符串
     * @param[in,out] State 解析状态
     */
    static void LoadLum(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State);

    /**
     * @brief 加载子光度级
     * @param[in,out] Output 输出对象
     * @param[in] Source 输入字符串
     * @param[out] Remain 剩余字符串
     * @param[in,out] State 解析状态
     */
    static void LoadSubLum(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State);

    /**
     * @brief 加载特殊谱线
     * @param[in,out] Output 输出对象
     * @param[in] Source 输入字符串
     * @param[out] Remain 剩余字符串
     * @param[in,out] State 解析状态
     * @param[in] __2 完成波段特殊谱线后的标记
     */
    static void LoadPecularities(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State, bool __2 = 0);

    /**
     * @brief 加载化学元素
     * @param[in,out] Output 输出对象
     * @param[in] Source 输入字符串
     * @param[out] Remain 剩余字符串
     * @param[in,out] State 解析状态
     */
    static void LoadChemElems(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State);

    /**
     * @brief 加载波段特殊谱线
     * @param[in,out] Output 输出对象
     * @param[in] Source 输入字符串
     * @param[out] Remain 剩余字符串
     * @param[in,out] State 解析状态
     */
    static void LoadBandPecs(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State);

    /**
     * @brief 加载特殊谱线阶段1
     * @param[in,out] Output 输出对象
     * @param[in] Source 输入字符串
     * @param[out] Remain 剩余字符串
     * @param[in,out] State 解析状态
     */
    static void LoadPecularitiesStage1(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State);

    /**
     * @brief 加载特殊谱线阶段2
     * @param[in,out] Output 输出对象
     * @param[in] Source 输入字符串
     * @param[out] Remain 剩余字符串
     * @param[in,out] State 解析状态
     */
    static void LoadPecularitiesStage2(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State);

    /**
     * @brief 加载特殊谱线阶段3
     * @param[in,out] Output 输出对象
     * @param[in] Source 输入字符串
     * @param[out] Remain 剩余字符串
     * @param[in,out] State 解析状态
     */
    static void LoadPecularitiesStage3(NormalStar* Output, ustring Source, ustring* Remain, ParserStateType* State);

    ustring ExportSpec() const;          ///< 导出光谱类型
    ustring ExportSub() const;           ///< 导出次型
    ustring ExportSpecSubRange() const;  ///< 导出光谱次型范围
    ustring ExportLum() const;           ///< 导出光度级
    ustring ExportLumRange() const;      ///< 导出光度级范围
    ustring ExportPec() const;           ///< 导出特殊谱线
    ustring ExportChem() const;          ///< 导出化学元素
    ustring ExportBand() const;          ///< 导出波段特征
    void ExportFullPecString(ustring* MainString) const; ///< 导出完整特殊谱线字符串

public:
    /**
     * @brief 获取描述
     * @return 描述字符串
     */
    ustring Description()const override;

    /**
     * @brief 解析函数
     * @param[in] StelClassString 恒星光谱类型字符串
     * @return 解析结果智能指针
     */
    static std::shared_ptr<StellarClassData> ParseFunc(ustring StelClassString);

    /**
     * @brief 转换为字符串
     * @return 字符串表示
     */
    ustring ToString()const override;

    /**
     * @brief 根据索引获取原始数据
     * @param[in] Index 索引
     * @return 原始数据引用
     */
    const ValueType& GetRawDataByIndex(size_t Index)const;

    /**
     * @brief 获取光谱类型
     * @param[in] Index 索引
     * @return 光谱类型字符
     */
    char SpectralClass(size_t Index)const override;

    /**
     * @brief 获取光谱类型不确定性
     * @param[in] Index 索引
     * @return 不确定性值
     */
    uint16_t SpectralClassUncertained(size_t Index)const override;

    /**
     * @brief 获取子类型
     * @param[in] Index 索引
     * @return 子类型值（*100后的值）
     */
    uint16_t SpecializedClass(size_t Index)const override;

    /**
     * @brief 获取子类型不确定性
     * @param[in] Index 索引
     * @return 不确定性值
     */
    uint16_t SpecializedClassUncertained(size_t Index)const override;

    /**
     * @brief 获取光度级
     * @param[in] Index 索引
     * @return 光度级字符串
     */
    ustring LuminosityClass(size_t Index)const override;

    /**
     * @brief 获取光度级不确定性
     * @param[in] Index 索引
     * @return 不确定性值
     */
    uint16_t LuminosityClassUncertained(size_t Index)const override;

    /**
     * @brief 获取光谱特殊谱线
     * @return 特殊谱线集合
     */
    std::set<ustring> SpectralPeculiarities()const override;

    /**
     * @brief 获取元素符号
     * @return 元素符号集合
     */
    std::set<ustring> ElementSymbols()const override;

    /**
     * @brief 获取波段特殊谱线
     * @return 波段特殊谱线映射
     */
    std::map<ustring, std::variant<int16_t, ustring>> BandPecularities()const override;

    /**
     * @brief 获取剩余未能分析成功的字符串，如果为空表示识别成功
     * @return 未分析的字符串
     */
    ustring UnanalyzedString()const override;
};

/**
 * @class AmStar
 * @ingroup StellarClass
 * @brief 表示Am型恒星（Metallic-line A-type star）的光谱分类数据
 *
 * @details
 * Am星是一类特殊的A型恒星，其核心特征为缺钙、钪。这种异常的相对丰度导致根据钙线
 * （即1价钙离子的393.366nm谱线）测定的光谱类型系统性地比根据其他金属线测定的光谱类型更早。
 * 通常情况下，仅根据氢谱线判断的光谱类型属于中间型。因此，此类恒星通常会给出两到三种光谱
 * 类型并写作 k...h...m... 的形式：
 * - **k**: 基于钙K线（Calcium K）的光谱型
 * - **h**: 基于氢Balmer线（Hydrogen）的光谱型
 * - **m**: 基于金属线（Metallic）的光谱型
 *
 * ### 典型命名案例
 * - **天狼星 (Sirius)**: 完整光谱型为 `kA0hA0VmA1`，意为钙线A0、氢线A0、光度级V、金属线A1。
 *   由于各谱线测得的光谱型差距不大，通常简写为 `A0mA1Va`。
 * - **瓠瓜三 (Delta Delphini)**: 属于无明确主光谱的复杂类型，格式如 `kA7hF1VmF1pSrEuCr:`。
 * - **弧矢增卅二 (Rho Puppis)**: 属于有明确主光谱的类型，格式如 `F5IIkF2IImF5II`，这类恒星通常
 *   演化更晚且更亮，位于主序带上方。
 * - **Kappa Octantis**: 金属线较强且可能带有X射线辐射，格式如 `A2mA5-A8`。
 *
 * @note
 * 1. Am恒星和天弁二变星（Delta Scuti variables）在赫罗图上大致位于相同位置，但属于不同类别。
 *    一个恒星同时是Am星又是天弁二变星的情况十分罕见（例如瓠瓜三和弧矢增卅二）。
 * 2. 部分Am星可能兼具玄戈变星（Lambda Boötis star）特征，除缺钙外还缺铁，位于零龄主序区附近。
 *
 * @see https://en.wikipedia.org/wiki/Am_star
 */
class AmStar : public StellarClassData
{
public:
    /// @brief 导入基础类型别名，用于复用NormalStar中的通用定义
    using ImportBase             = StelCls::NormalStar;
    /// @brief 特殊光谱类型
    using PeculiarityType         = ImportBase::PeculiarityType;
    /// @brief 化学特殊类型
    using ChemicalPeculiaritySpec = ImportBase::ChemicalPeculiaritySpec;

    /**
     * @enum ParserStateType
     * @brief 光谱字符串解析器的有限状态机状态定义
     *
     * @details
     * 由于Am星光谱表示法极其复杂，解析器采用位掩码组合状态：
     * - 低3位表示当前解析的内容类型（谱线、光谱型、子型、光度级等）
     * - 高2位表示操作模式（范围、括号嵌套等）
     * 
     * 这种设计是为了应对上下文有关的复杂度，例如波段标识与化学元素符号可能存在字符重叠，
     * 需要结合前后文（Lookahead）才能正确切分。
     */
    enum ParserStateType
    {
        PStart   = 0b00000, ///< 初始状态
        PSpec    = 0b00001, ///< 解析光谱型 (Spectral Type)
        PSub     = 0b00010, ///< 解析子型 (Subtype)
        PLum     = 0b00011, ///< 解析光度级 (Luminosity Class)
        PSLum    = 0b00100, ///< 解析特殊光度级或双光度级
        PPec     = 0b00101, ///< 解析特殊标记 (Peculiarities)
        PChem    = 0b00110, ///< 解析化学元素异常
        PLine    = 0b00111, ///< 解析谱线标识 (k/h/m)
        
        POpMask  = 0b11000, ///< 操作掩码
        PRange   = 0b01000, ///< 范围标记 (如 A5-A8)
        PBracket = 0b10000, ///< 括号嵌套标记
        
        PEnd     = -1       ///< 解析结束/错误状态
    };

    /// 解析事件队列类型：状态 -> 处理函数映射
    using EventQueueType = std::map<
        ParserStateType, 
        std::function<void(AmStar*, ustring, ustring*, ParserStateType*, ustring*)>
    >;

    /**
     * @struct KeyType
     * @brief 谱线段键值，用于唯一标识一个光谱特征
     *
     * @details
     * 每个谱线段由谱线名称（如"k"、"h"、"m"）、对应元素/化合物、波长及不确定度标志组成。
     * 例如：Key{"k", "Ca II", 393.366, None} 表示Ca II K线。
     * 排序依据为波长升序，确保输出顺序符合天文惯例。
     */
    struct KeyType
    {
        ustring Line;       ///< 谱线名称 (如 "k", "h", "m")
        ustring Element;    ///< 元素或化合物符号
        float64 WaveLength; ///< 波长（nm），用于排序

        ImportBase::UncertaintyType::UncertaintySymbols UncertaintyFlags = 
            ImportBase::UncertaintyType::None; ///< 不确定度标记

        /**
         * @brief 按波长升序比较两个KeyType
         * @param A 左侧键
         * @param B 右侧键
         * @return A.WaveLength < B.WaveLength
         */
        static bool Compare(KeyType A, KeyType B)
        {
            return A.WaveLength < B.WaveLength;
        }
    };

    /**
     * @struct ValueType
     * @brief 谱线段的值，包含主值和可选的浮动范围
     *
     * @details
     * 主值为整数光谱型+光度级组合（如 A7V）。
     * FloatValue用于表示浮动范围，例如柳宿增三的光谱 "kA7VmF0/2III/IVSr" 中，
     */
    struct ValueType
    {
        ImportBase::ValueType Value;                      ///< 主光谱值
        std::optional<ImportBase::ValueType> FloatValue;  ///< 浮动范围
    };

    /// @brief 谱线段映射表，按键（波长）排序
    using SegmentType = std::flat_map<KeyType, ValueType, bool(*)(KeyType, KeyType)>;

    /**
     * @enum SubFmts
     * @brief Am星光谱子格式
     */
    enum SubFmts
    {
        /**
         * @details 第一种表示方法类似瓠瓜三：kA7hF1VmF1pSrEuCr:，
         * 由多个段组成且没有明确的主光谱。
         */
        DeltaDelphini,

        /**
         * @details 第二种表示方法类似弧矢增卅二：F5IIkF2IImF5II，
         * 有明确的主光谱。这类Am恒星通常被认为演化的更晚且更亮，位
         * 于主序带上方。
         */
        RhoPuppis,

        /**
         * @details 还有第三种表示方法类似Kappa Octantis：A2mA5-A8，
         * 这类Am恒星金属线较强，部分带有X射线
         */
        KappaOctantis
    };

    // -------------------------------------------------------------------------
    // 各子格式对应的数据结构
    // -------------------------------------------------------------------------
    
    /// 瓠瓜三型数据：仅包含多段光谱
    struct DDelValueType
    {
        SegmentType Segments{KeyType::Compare}; ///< 所有谱线段集合
    };

    /// 弧矢增卅二型数据：包含主光谱段 + 精确段
    struct RPupValueType
    {
        ValueType MainSegment;                          ///< 主光谱段
        std::vector<PecularityType> MainSegPecs;        ///< 主段的peculiarity列表
        SegmentType Segments{KeyType::Compare};         ///< 精确谱线段集合
    };

    /// Kappa Octantis型数据：主型 + 金属线范围
    struct KOctValueType
    {
        ValueType MainType;                             ///< 主光谱型（如 A2）
        ImportBase::UncertaintyType::UncertaintySymbols LineUncertaintyFlags; ///< 金属线不确定度
        ValueType MetallicType1;                        ///< 金属线下限（如 A5）
        ValueType MetallicType2;                        ///< 金属线上限（如 A8）
    };

    // -------------------------------------------------------------------------
    // 静态成员
    // -------------------------------------------------------------------------
    /// @brief 全局谱线查找表（静态初始化）
    static std::flat_map<ustring, KeyType> LinesTable;
    /// @brief 用于快速匹配谱线标识的正则表达式
    static wregex LinesPattern;

protected:
    // -------------------------------------------------------------------------
    // 内部状态与数据
    // -------------------------------------------------------------------------
    SubFmts FmtFlag = DeltaDelphini; ///< 当前识别到的子格式
    
    /// 存储解析结果，根据 FmtFlag 选择对应结构
    std::variant<DDelValueType, RPupValueType, KOctValueType> Data;
    
    bool AddSuffix = false; ///< 是否附加 "Am" 后缀（部分案例需要）
    
    /**
     * @brief 特殊标记列表
     * @note 部分恒星具有Am星与玄戈变星双重身份，除缺钙特征外还缺铁
     * @see https://en.wikipedia.org/wiki/Lambda_Bo%C3%B6tis_star
     */
    std::vector<PeculiarityType> Pecularities;
    
    std::vector<ChemicalPeculiaritySpec> ChemElems; ///< 化学元素异常规格
    ustring RemainString;                           ///< 未成功解析的剩余字符串

    static const EventQueueType ParserEventQueue;   ///< 解析器事件分发表

    // -------------------------------------------------------------------------
    // 解析器内部方法
    // -------------------------------------------------------------------------
    /**
     * @brief 安全设置解析器状态
     * @param State 指向当前状态的指针
     * @param Value 新状态值
     */
    static void SetState(ParserStateType* State, ParserStateType Value);

    /// 各状态对应的加载函数
    static void LoadLine(AmStar* output, ustring source, ustring* remain, 
                         ParserStateType* state, ustring* line);
    static void LoadSpec(AmStar* output, ustring source, ustring* remain, 
                         ParserStateType* state, ustring* line);
    static void LoadSub(AmStar* output, ustring source, ustring* remain, 
                        ParserStateType* state, ustring* line);
    static void LoadLum(AmStar* output, ustring source, ustring* remain, 
                        ParserStateType* state, ustring* line);
    static void LoadSLum(AmStar* output, ustring source, ustring* remain, 
                         ParserStateType* state, ustring* line);
    static void LoadPec(AmStar* output, ustring source, ustring* remain, 
                        ParserStateType* state, ustring* line);
    static void LoadChem(AmStar* output, ustring source, ustring* remain, 
                         ParserStateType* state, ustring* line);

    /**
     * @brief 下一段内容预检查（解决上下文歧义）
     *
     * @details
     * 波段标识、特殊标记和化学元素符号中存在重复字符，必须向后查看最多3个块才能确定类型。
     * “上下文有关语言”的复杂度在此体现的淋漓尽致，但目前仍没有足够证据证明光谱字符串属于1型或以下的语言。优先级规则：化学元素 < 特殊标识 < 波段
     *
     * @param src1 待检查的源字符串片段
     * @return true 如果确认为有效段
     */
    static bool __ChkSeg(ustring src1);

    /**
     * @brief 执行下一段的上下文检查并更新状态
     * @param Output 输出对象
     * @param Source 当前源字符串
     * @param State 状态指针（将被修改）
     */
    static void NextSegmentCheck(AmStar* output, ustring source, ParserStateType* state);

    /// 各子格式的专用解析例程
    static void DdelRoutine(AmStar* output, ustring source, ustring* remain, ParserStateType* state);
    static void RPupRoutine(AmStar* output, ustring source, ustring* remain, ParserStateType* state);
    static void TransferToKOct(AmStar* output, ParserStateType* state);

    // -------------------------------------------------------------------------
    // 导出/序列化辅助方法
    // -------------------------------------------------------------------------
    static ustring ExportSpec(const ValueType& value);
    static ustring ExportSub(const ValueType& value);
    static ustring ExportSpecSubRange(const ValueType& value);
    static ustring ExportLum(const ValueType& value);
    static ustring ExportLumRange(const ValueType& value);
    static ustring ExportSegmentString(const SegmentType::value_type& value);
    static ustring ExportPec(const std::vector<PeculiarityType>& table);
    static ustring ExportChem(const std::vector<ChemicalPeculiaritySpec>& table);

public:
    // -------------------------------------------------------------------------
    // 公共接口
    // -------------------------------------------------------------------------
    
    /**
     * @brief 获取人类可读的描述字符串
     * @return 包含所有光谱信息的描述文本
     */
    ustring Description() const override;

    /**
     * @brief 从光谱字符串解析创建AmStar对象
     * @param stelClassString 光谱分类字符串（如 "kA0hA0VmA1"）
     * @return 解析成功返回共享指针
     */
    static std::shared_ptr<StellarClassData> ParseFunc(ustring stelClassString);

    /**
     * @brief 将内部数据序列化为标准光谱字符串
     * @return 格式化后的光谱字符串
     */
    ustring ToString() const override;

    /**
     * @brief 获取谱线段数量
     * @return 有效谱线段总数
     */
    size_t Size() const;

    /**
     * @brief 按索引获取原始谱线值
     * @param Index 谱线段索引（按主值/波长排序）
     * @return 对应ValueType引用
     * @throws std::out_of_range 若索引越界
     */
    const ValueType& GetRawDataByIndex(size_t index) const;

    /**
     * @brief 按谱线名称获取数据
     * @param Line 谱线标识（如 "k", "h", "m"）
     * @return 对应ValueType，若不存在则返回默认构造值
     */
    ValueType GetDataByLine(ustring line);

    /**
     * @brief 按谱线名称设置数据
     * @param Line 谱线标识
     * @param NewValue 新值
     */
    void SetDataByLine(ustring line, ValueType newValue);

    // -------------------------------------------------------------------------
    // StellarClassData 接口实现
    // -------------------------------------------------------------------------
    char SpectralClass(size_t index) const override;
    uint16_t SpectralClassUncertained(size_t index) const override;
    uint16_t SpecializedClass(size_t index) const override;
    uint16_t SpecializedClassUncertained(size_t index) const override;
    ustring LuminosityClass(size_t index) const override;
    uint16_t LuminosityClassUncertained(size_t index) const override;

    std::set<ustring> SpectralPeculiarities() const override;
    std::set<ustring> ElementSymbols() const override;
    std::map<ustring, std::variant<int16_t, ustring>> BandPecularities() const override;

    /**
     * @brief 获取未成功解析的剩余字符串
     * @return 空字符串表示完全解析成功；非空则表示存在无法识别的部分
     */
    ustring UnanalyzedString() const override;
};

}

}