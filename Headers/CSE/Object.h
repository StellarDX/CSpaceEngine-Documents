/**
 * @file Object.h
 * @brief 天文对象定义文件（CSE）
 * @details 定义了一个庞大的天文对象结构体，包含天体物理、轨道、表面特征等多种属性
 * 
 * @copyright Copyright (c) 2025 StellarDX Astronomy
 * 
 * 本文件定义了天文对象的核心数据结构，包含天体物理属性、轨道参数、
 * 表面特征、大气层、海洋等全方位天文数据模型。
 */

namespace cse {

/**
 * @defgroup SE_Object 物体
 * @{
 */

/**
 * @brief 成分类型定义
 * @details 使用字符串作为键，双精度浮点数作为值的映射表，表示天体化学成分及其质量分数
 */
using CompositionType = std::map<ustring, float64>;

/**
 * @brief 天文对象基类
 * @details 定义了天文对象的各种属性，包括基本信息、物理特性、轨道参数等
 */
struct Object : public SEObject
{
    /*--------------------------------------------------------------*\
    |                           Basic Info                           |
    \*--------------------------------------------------------------*/

    ustring         Type                 = _NoDataStr;       ///< 对象类型
    ustringlist     Name                 = {};               ///< 对象名称列表
    _TIME CSEDate   DateUpdated          = _TIME CSEDate();  ///< 最后更新日期
    ustring         DiscMethod           = _NoDataStr;       ///< 发现方法
    _TIME CSEDate   DiscDate             = _TIME CSEDate();  ///< 发现日期
    ustring         ParentBody           = _NoDataStr;       ///< 母体
    ustring         Class                = _NoDataStr;       ///< 分类
    ustring         AsterType            = _NoDataStr;       ///< 仅用于小行星
    ustring         CometType            = _NoDataStr;       ///< 仅用于彗星
    ustring         SpecClass            = _NoDataStr;       ///< 仅用于恒星

    // ------------------------------------------------------------ //

    /*--------------------------------------------------------------*\
    |                       Physical properties                      |
    \*--------------------------------------------------------------*/

    float64         Mass                 = _NoDataDbl;       ///< 质量（千克）
    vec3            Dimensions           = vec3(_NoDataDbl); ///< 尺寸（米）
    float64         InertiaMoment        = _NoDataDbl;       ///< 惯性矩因子
    float64         AlbedoBond           = _NoDataDbl;       ///< 邦德反照率
    float64         AlbedoGeom           = _NoDataDbl;       ///< 几何反照率
    float64         EndogenousHeating    = _NoDataDbl;       ///< 内生加热（hack：使物体发光发热）
    float64         ThermalLuminosity    = _NoDataDbl;       ///< 热光度
    float64         ThermalLuminosityBol = _NoDataDbl;       ///< 热光度（总辐射）
    float64         Temperature          = _NoDataDbl;       ///< 有效温度（开氏度）
    float64         Luminosity           = _NoDataDbl;       ///< 视觉光度（瓦特）
    float64         LumBol               = _NoDataDbl;       ///< 总光度（瓦特）
    float64         FeH                  = _NoDataDbl;       ///< 金属丰度
    float64         CtoO                 = _NoDataDbl;       ///< 碳氧比
    float64         Age                  = _NoDataDbl;       ///< 年龄（年）
    float64         KerrSpin             = _NoDataDbl;       ///< 角动量（J）
    float64         KerrCharge           = _NoDataDbl;       ///< 电荷（Q）

    // ------------------------------------------------------------ //

    /*--------------------------------------------------------------*\
    |                       Optical properties                       |
    \*--------------------------------------------------------------*/

    vec3            Color                = vec3(_NoDataDbl); ///< 颜色
    float64         AbsMagn              = _NoDataDbl;       ///< 绝对星等
    float64         SlopeParam           = _NoDataDbl;       ///< 斜率参数
    float64         Brightness           = _NoDataDbl;       ///< 亮度
    float64         BrightnessReal       = _NoDataDbl;       ///< 实际亮度

    // ------------------------------------------------------------ //

    /*--------------------------------------------------------------*\
    |                            Rotation                            |
    \*--------------------------------------------------------------*/

    ustring         RotationModel        = _NoDataStr;       ///< 自转模型

    /**
     * @brief 简单旋转模型参数
     * @details 仅当RotationModel缺失时使用
     */
    struct SimpleRotationModel
    {
        float64     RotationEpoch     = _NoDataDbl; ///< 历元（J2000）
        float64     Obliquity         = _NoDataDbl; ///< 倾角（度）
        float64     EqAscendNode      = _NoDataDbl; ///< 升交点经度（度）
        float64     RotationOffset    = _NoDataDbl; ///< 旋转偏移（度）
        float64     RotationPeriod    = _NoDataDbl; ///< 旋转周期（秒）
        float64     Precession        = _NoDataDbl; ///< 进动（秒）
        bool        TidalLocked       = false;      ///< 是否潮汐锁定
    }Rotation;

    /**
     * @brief IAU旋转模型参数
     */
    struct IAURotationModel
    {
        ustring     ValidRange        = _NoDataStr; ///< 有效范围
        float64     Epoch             = _NoDataDbl; ///< 历元（J2000）
        float64     PoleRA            = _NoDataDbl; ///< 北极经度（度）
        float64     PoleRARate        = _NoDataDbl; ///< 北极经度变化率（度/世纪）
        float64     PoleDec           = _NoDataDbl; ///< 北极纬度（度）
        float64     PoleDecRate       = _NoDataDbl; ///< 北极纬度变化率（度/世纪）
        float64     PrimeMeridian     = _NoDataDbl; ///< 本初子午线（度）
        float64     RotationRate      = _NoDataDbl; ///< 旋转速率（度/天）
        float64     RotationAccel     = _NoDataDbl; ///< 旋转加速度（度/世纪²）
        bool        UsingSecular      = false;      ///< 在PeriodicTerms中使用世纪而非天作为时间单位

        /**
         * @brief 周期项表格格式
         * @details 计算最终值的公式：
         *     - FinalRA = PoleRA + PoleRARate * Time + SUM(PoleRAAmp[i] * sin(Phase[i] + Freq[i] * Time + FreqRate * pow(Time, 2)))
         *     - FinalDec = PoleDec + PoleDecRate * Time + SUM(PoleDecAmp[i] * cos(Phase[i] + Freq[i] * Time + FreqRate * pow(Time, 2)))
         *     - FinalRotation = PrimeMeridian + RotationRate * Time + RotationAccel * pow(Time, 2) + SUM(PrimeMerAmp[i] * sin(Phase[i] + Freq[i] * Time + FreqRate * pow(Time, 2)))
         * @see https://github.com/Bill-Gray/find_orb/blob/master/cospar.txt
         * @see https://github.com/cosmonium/cosmonium
         */
        std::vector<std::array<float64, 6>> PeriodicTerms;
    }RotationIAU;

    // ------------------------------------------------------------ //

    /*--------------------------------------------------------------*\
    |                              Orbit                             |
    \*--------------------------------------------------------------*/

    vec3            Position             = vec3(_NoDataDbl); ///< 位置

    /**
     * @brief 轨道参数
     */
    struct OrbitParams
    {
        bool        Binary            = false;      ///< 是否为双星系统
        float64     Separation        = _NoDataDbl; ///< 与母体的分离距离（米）
        float64     PositionAngle     = _NoDataDbl; ///< 位置角

        ustring     AnalyticModel     = _NoDataStr; ///< 分析模型
        ustring     RefPlane          = _NoDataStr; ///< 参考系
        float64     Epoch             = _NoDataDbl; ///< 历元（儒略日）
        float64     Period            = _NoDataDbl; ///< 周期（秒）
        float64     PericenterDist    = _NoDataDbl; ///< 近心点距离（米）
        float64     GravParam         = _NoDataDbl; ///< 重力参数
        float64     Eccentricity      = _NoDataDbl; ///< 离心率
        float64     Inclination       = _NoDataDbl; ///< 倾角（度）
        float64     AscendingNode     = _NoDataDbl; ///< 升交点经度（度）
        float64     AscNodePreces     = _NoDataDbl; ///< 升交点进动周期（秒）
        float64     ArgOfPericenter   = _NoDataDbl; ///< 近心点幅角（度）
        float64     ArgOfPeriPreces   = _NoDataDbl; ///< 近心点幅角进动周期（秒）
        float64     MeanAnomaly       = _NoDataDbl; ///< 平近点角
    }Orbit;

    // ------------------------------------------------------------ //

    /*--------------------------------------------------------------*\
    |                              Life                              |
    \*--------------------------------------------------------------*/

    uint64          LifeCount            = 0; ///< 生命参数数量

    /**
     * @brief 生命参数
     */
    struct LifeParams
    {
        ustring     Class             = _NoDataStr; ///< 生命分类
        ustring     Type              = _NoDataStr; ///< 生命类型
        ustringlist Biome             = {};         ///< 生物群落
        bool        Panspermia        = false;      ///< 是否泛种论
    }Life[2];

    // ------------------------------------------------------------ //

    /*--------------------------------------------------------------*\
    |                            Interior                            |
    \*--------------------------------------------------------------*/

    CompositionType Interior        = {}; ///< 内部成分（质量分数，百分比）

    // ------------------------------------------------------------ //

    /*--------------------------------------------------------------*\
    |                             Surface                            |
    \*--------------------------------------------------------------*/

    /**
     * @brief 表面纹理参数
     */
    struct SurfaceTextureParams
    {
        ustring     DiffMap        = _NoDataStr;       ///< 表面贴图路径
        ustring     DiffMapAlpha   = _NoDataStr;       ///< 表面贴图Alpha通道：<br> - "Water"（水）-- 水镜面反射（太阳镜面反射光斑在水面上），用作行星上的河流/湖泊/海洋遮罩<br> - "Ice"（冰）-- 冰镜面反射（太阳镜面反射光斑在主体的所有曲面上），用作寒冷星球上的冰遮罩<br> - "Transp"（透明度）-- 表面透明度，仅用于云层<br> - "None"（无）-- 忽略 alpha 通道。
        ustring     BumpMap        = _NoDataStr;       ///< 高度贴图路径
        float64     BumpHeight     = _NoDataDbl;       ///< 凹凸图的高度（米），即图层上最低点和最高点之间的高度差。
        float64     BumpOffset     = _NoDataDbl;       ///< 地形负偏移量，以米为单位。在SE中如果您想创建一个从 -8 km 到 +12 km 的景观，请输入 BumpHeight 20 和 BumpOffset 8。
        ustring     GlowMap        = _NoDataStr;       ///< 发光贴图路径
        ustring     GlowMode       = _NoDataStr;       ///< 发光模式：<br> - "Alpha"--模式由发光纹理的 alpha 通道设置（0.0-0.33 = 夜间，0.34-0.66 = 永久，0.67-1.0 = 热敏）<br> - "Night"--夜间城市灯光（白天不可见）<br> - "Permanent"--永久灯光（对外部灯光无反应）<br> - "Thermal"--热灯光，对当地温度有反应，格式复杂（纹理的 RGB 通道代表单一的 24 位温度值）。
        vec3        GlowColor      = vec3(_NoDataDbl); ///< 发光颜色（与原贴图乘算）
        float64     GlowBright     = _NoDataDbl;       ///< 发光亮度
        ustring     SpecMap        = _NoDataStr;       ///< 高光贴图路径
        bool        FlipMap        = false;            ///< 是否180°翻转贴图
    };

    /**
     * @brief Hapke参数
     */
    struct HapkeParams
    {
        float64     Gamma          = _NoDataDbl; ///< Gamma参数
        float64     Hapke          = _NoDataDbl; ///< 兰伯特光照模型与哈普克光照模型的比率。使用 Hapke 0.0 表示纯兰伯特模型（适用于气态巨行星），使用 Hapke 1.0 表示纯 Hapke 模型（适用于多尘天体，如不透气行星或沙漠行星--模拟对撞眩光效果）。混合光照模型使用中间值（线性插值）。
        float64     SpotBright     = _NoDataDbl; ///< 光斑亮度
        float64     SpotWidth      = _NoDataDbl; ///< 光斑宽度
        float64     SpotBrightCB   = _NoDataDbl; ///< 光斑亮度（CB）
        float64     SpotWidthCB    = _NoDataDbl; ///< 光斑宽度（CB）
        float64     TempMapMaxTemp = _NoDataDbl; ///< 温度贴图最高温度
        float64     DayAmbient     = _NoDataDbl; ///< 日间环境光，用于没有大气层的物体和云层。
        float64     RingsWinter    = _NoDataDbl; ///< 环冬季效应
        vec3        ModulateColor  = vec3(_NoDataDbl); ///< 将 Diff 纹理的 RGB 和 Alpha 颜色按此值缩放（相乘）。
        float64     ModulateBright = _NoDataDbl; ///< 调制亮度（Diff 纹理 Alpha 颜色）
    };

    bool            EnableSurface        = false; ///< 是否启用表面

    /**
     * @brief 地形参数
     */
    struct LandscapeParams : public SurfaceTextureParams, HapkeParams
    {
        ustring     Preset            = _NoDataStr; ///< 预设
        float64     SurfStyle         = _NoDataDbl; ///< 表面的样式（配色方案）。与调色板文件中的样式范围相对应。
        vec3        Randomize         = vec3(_NoDataDbl); ///< 随机化向量。如果你制作了两个完全相同的星球（所有参数都相同），可以输入不同的随机化值，使它们看起来不同。
        float64     colorDistMagn     = _NoDataDbl; ///< 颜色分布幅度（各种细节纹理“斑点”）
        float64     colorDistFreq     = _NoDataDbl; ///< 颜色分布频率
        float64     detailScale       = _NoDataDbl; ///< 细节纹理噪点的比例，靠近表面可见。
        float64     drivenDarkening   = _NoDataDbl; ///< 冰卫星的变暗程度。
        float64     seaLevel          = _NoDataDbl; ///< 海平面相对深度，已过时的参数（使用 Ocean 标签代替）。
        float64     snowLevel         = _NoDataDbl; ///< 雪线相对高度
        float64     tropicLatitude    = _NoDataDbl; ///< 热带边缘纬度的正弦值（0.0 - 赤道，1.0 - 极地）
        float64     icecapLatitude    = _NoDataDbl; ///< 冰盖边缘纬度的正弦值（0.0 - 赤道，1.0 - 极地）
        float64     icecapHeight      = _NoDataDbl; ///< 冰盖相对高度
        float64     climatePole       = _NoDataDbl; ///< 极地气候（它是彩色表格中的一个指数，0.0 - 沙漠，0.5 - 温带，1.0 - 雪）。
        float64     climateTropic     = _NoDataDbl; ///< 热带气候（它是彩色表格中的一个指数，0.0 - 沙漠，0.5 - 温带，1.0 - 雪）。
        float64     climateEquator    = _NoDataDbl; ///< 赤道气候（它是彩色表格中的一个指数，0.0 - 沙漠，0.5 - 温带，1.0 - 雪）。
        float64     climateSteppeMin  = _NoDataDbl; ///< 草原气候最小值
        float64     climateSteppeMax  = _NoDataDbl; ///< 草原气候最大值
        float64     climateForestMin  = _NoDataDbl; ///< 森林气候最小值
        float64     climateForestMax  = _NoDataDbl; ///< 森林气候最大值
        float64     climateGrassMin   = _NoDataDbl; ///< 草地气候最小值
        float64     climateGrassMax   = _NoDataDbl; ///< 草地气候最大值
        float64     humidity          = _NoDataDbl; ///< 湿度
        float64     heightTempGrad    = _NoDataDbl; ///< 高度温度梯度
        float64     beachWidth        = _NoDataDbl; ///< 海滩宽度
        float64     tropicWidth       = _NoDataDbl; ///< 热带宽度
        float64     mainFreq          = _NoDataDbl; ///< 主要（全球）噪音的频率，确定大陆/海洋的分布。（值越高越碎）
        float64     venusFreq         = _NoDataDbl; ///< 类金星地貌频率
        float64     venusMagn         = _NoDataDbl; ///< 类金星地貌规模
        float64     mareFreq          = _NoDataDbl; ///< 撞击盆地（月海）频率
        float64     mareDensity       = _NoDataDbl; ///< 撞击盆地（月海）密度（数量）
        float64     terraceProb       = _NoDataDbl; ///< 梯田山的概率（类似于龙脊梯田）
        float64     erosion           = _NoDataDbl; ///< 水对山脉的侵蚀程度（干燥星球为 0.0，潮湿星球为 1.0）
        float64     montesMagn        = _NoDataDbl; ///< 山脉规模
        float64     montesFreq        = _NoDataDbl; ///< 山脉频率
        float64     montesSpiky       = _NoDataDbl; ///< 山脉尖刺
        float64     montesFraction    = _NoDataDbl; ///< 山脉分数
        float64     dunesMagn         = _NoDataDbl; ///< 沙丘规模
        float64     dunesFreq         = _NoDataDbl; ///< 沙丘频率
        float64     dunesFraction     = _NoDataDbl; ///< 沙丘分数
        float64     hillsMagn         = _NoDataDbl; ///< 丘陵规模
        float64     hillsFreq         = _NoDataDbl; ///< 丘陵频率
        float64     hillsFraction     = _NoDataDbl; ///< 丘陵分数
        float64     hills2Fraction    = _NoDataDbl; ///< 次级丘陵分数
        float64     riversMagn        = _NoDataDbl; ///< 河流规模
        float64     riversFreq        = _NoDataDbl; ///< 河流频率
        float64     riversSin         = _NoDataDbl; ///< 河流曲率
        float64     riftsMagn         = _NoDataDbl; ///< 裂谷规模（类似木卫二上的缝）
        float64     riftsFreq         = _NoDataDbl; ///< 裂谷频率
        float64     riftsSin          = _NoDataDbl; ///< 裂谷曲率
        float64     eqridgeMagn       = _NoDataDbl; ///< 赤道脊规模
        float64     eqridgeWidth      = _NoDataDbl; ///< 赤道脊宽度
        float64     eqridgeModMagn    = _NoDataDbl; ///< 赤道脊调制规模
        float64     eqridgeModFreq    = _NoDataDbl; ///< 赤道脊调制频率
        float64     canyonsMagn       = _NoDataDbl; ///< 峡谷规模
        float64     canyonsFreq       = _NoDataDbl; ///< 峡谷频率
        float64     canyonsFraction   = _NoDataDbl; ///< 峡谷分数
        float64     cracksMagn        = _NoDataDbl; ///< 裂缝规模
        float64     cracksFreq        = _NoDataDbl; ///< 裂缝频率
        uint64      cracksOctaves     = _NoDataInt; ///< 裂缝倍频程数
        float64     craterMagn        = _NoDataDbl; ///< 陨石坑规模
        float64     craterFreq        = _NoDataDbl; ///< 陨石坑频率
        float64     craterDensity     = _NoDataDbl; ///< 陨石坑密度
        uint64      craterOctaves     = _NoDataInt; ///< 陨石坑倍频程数
        uint64      craterRayedFactor = _NoDataInt; ///< 辐射纹因子
        float64     volcanoMagn       = _NoDataDbl; ///< 火山规模
        float64     volcanoFreq       = _NoDataDbl; ///< 火山频率
        float64     volcanoDensity    = _NoDataDbl; ///< 火山密度
        uint64      volcanoOctaves    = _NoDataInt; ///< 火山倍频程数
        float64     volcanoActivity   = _NoDataDbl; ///< 活火山数量
        float64     volcanoFlows      = _NoDataDbl; ///< 火山熔岩流长度
        float64     volcanoRadius     = _NoDataDbl; ///< 火山半径
        float64     volcanoTemp       = _NoDataDbl; ///< 火山口和熔岩流中的熔岩温度（开氏度）
        float64     lavaCoverTidal    = _NoDataDbl; ///< 潮汐加热造成的熔岩覆盖率（仅适用于熔岩星）
        float64     lavaCoverSun      = _NoDataDbl; ///< 恒星加热造成的熔岩覆盖率（仅适用于熔岩星）
        float64     lavaCoverYoung    = _NoDataDbl; ///< 行星余温造成的熔岩覆盖率（仅适用于熔岩星）

        float64     SpecBrightWater   = _NoDataDbl; ///< 水面高光亮度
        float64     SpecBrightIce     = _NoDataDbl; ///< 冰面高光亮度
        float64     RoughnessWater    = _NoDataDbl; ///< 水面粗糙度
        float64     RoughnessIce      = _NoDataDbl; ///< 冰面粗糙度
        float64     SpecularScale     = _NoDataDbl; ///< 高光比例
        float64     RoughnessBias     = _NoDataDbl; ///< 粗糙度偏差
        float64     RoughnessScale    = _NoDataDbl; ///< 粗糙度比例
    }Surface;

    // ------------------------------------------------------------ //

    /*--------------------------------------------------------------*\
    |                              Ocean                             |
    \*--------------------------------------------------------------*/

    bool            NoOcean              = true; ///< 是否无海洋

    /**
     * @brief 海洋参数
     */
    struct OceanParams : public HapkeParams
    {
        float64     Height            = _NoDataDbl; ///< 水深（米）
        CompositionType Composition;               ///< 成分（百分比）
    }Ocean;

    // ------------------------------------------------------------ //

    /*--------------------------------------------------------------*\
    |                             Clouds                             |
    \*--------------------------------------------------------------*/

    bool            NoClouds             = true; ///< 是否无云层

    /**
     * @brief 云层参数
     */
    struct CloudParams
    {
        bool        TidalLocked       = false; ///< 如果指定，云层将在行星的一侧形成一个巨大的气旋。用于与恒星潮汐锁定的行星。

        /**
         * @brief 云层参数
         */
        struct CloudLayerParam : public SurfaceTextureParams, HapkeParams
        {
            float64 Height            = _NoDataDbl; ///< 云层在星球表面最低点以上的高度，以米为单位（即星球标签中的半径减去表面标签中的 BumpOffset）。如果希望云层不与山脉重叠，则应使其高于最高山脉（大于 BumpHeight）。
            float64 Velocity          = _NoDataDbl; ///< 赤道上云层相对于地球表面的速度，单位为米/秒。
            float64 RotationOffset    = _NoDataDbl; ///< 旋转偏移
            float64 SubsurfScatBright = _NoDataDbl; ///< 次表面散射亮度
            float64 SubsurfScatPower  = _NoDataDbl; ///< 次表面散射功率
            float64 Opacity           = _NoDataDbl; ///< 不透明度
        };
        std::vector<CloudLayerParam> Layers; ///< 云层列表

        float64     Coverage          = _NoDataDbl; ///< 云覆盖率 (0...1)。请注意，这是非常近似的覆盖量。
        float64     mainFreq          = _NoDataDbl; ///< 主频率
        uint64      mainOctaves       = _NoDataInt; ///< 主倍频程数
        float64     stripeZones       = _NoDataDbl; ///< 条纹区域
        float64     stripeFluct       = _NoDataDbl; ///< 条纹波动
        float64     stripeTwist       = _NoDataDbl; ///< 条纹扭曲
        float64     cycloneMagn       = _NoDataDbl; ///< 气旋幅度
        float64     cycloneFreq       = _NoDataDbl; ///< 气旋频率
        float64     cycloneDensity    = _NoDataDbl; ///< 气旋密度
        uint64      cycloneOctaves    = _NoDataInt; ///< 气旋倍频程数
        float64     cycloneMagn2      = _NoDataDbl; ///< 次级气旋幅度
        float64     cycloneFreq2      = _NoDataDbl; ///< 次级气旋频率
        float64     cycloneDensity2   = _NoDataDbl; ///< 次级气旋密度
        uint64      cycloneOctaves2   = _NoDataInt; ///< 次级气旋倍频程数
    }Clouds;

    // ------------------------------------------------------------ //

    /*--------------------------------------------------------------*\
    |                           Atmosphere                           |
    \*--------------------------------------------------------------*/

    bool            NoAtmosphere         = true; ///< 是否无大气层

    /**
     * @brief 大气层参数
     */
    struct AtmoParams
    {
        ustring     Model             = _NoDataStr; ///< 模型
        float64     Height            = _NoDataDbl; ///< 大气层顶界高度（米）。SE中的陆地行星的典型值为 50-100 千米，气态巨行星为 500-2000 千米
        float64     Density           = _NoDataDbl; ///< 表面空气密度（千克/立方米）
        float64     Pressure          = _NoDataDbl; ///< 表面压强（帕斯卡）
        float64     Adiabat           = _NoDataDbl; ///< 绝热指数
        float64     Greenhouse        = _NoDataDbl; ///< 温室效应造成的温升（开尔文）
        float64     Bright            = _NoDataDbl; ///<  渲染亮度。SE中的默认值为 10。
        float64     Opacity           = _NoDataDbl; ///< 渲染不透明度。SE中为避免视觉伪影，使用默认值 1。
        float64     SkyLight          = _NoDataDbl; ///< 白天天空照射行星表面的亮度，SE中的默认值为3.3333。
        float64     Hue               = _NoDataDbl; ///< 色调
        float64     Saturation        = _NoDataDbl; ///< 饱和度
        CompositionType Composition;               ///< 成分（质量分数）
    }Atmosphere;

    // ------------------------------------------------------------ //

    /*--------------------------------------------------------------*\
    |                             Climate                            |
    \*--------------------------------------------------------------*/

    bool            EnableClimate        = false; ///< 是否启用气候

    /**
     * @brief 气候参数
     */
    struct ClimateParams
    {
        float64     GlobalWindSpeed   = _NoDataDbl; ///< 全球风速
        float64     MinSurfaceTemp    = _NoDataDbl; ///< 最低表面温度
        float64     MaxSurfaceTemp    = _NoDataDbl; ///< 最高表面温度
        ustring     AtmoProfile       = _NoDataStr; ///< 大气剖面
    }Climate;

    // ------------------------------------------------------------ //

    /*--------------------------------------------------------------*\
    |                             Aurora                             |
    \*--------------------------------------------------------------*/

    bool            NoAurora             = true; ///< 是否无极光

    /**
     * @brief 极光参数
     */
    struct AuroraParams
    {
        float64     Height            = _NoDataDbl; ///< 高度（米）
        float64     NorthLat          = _NoDataDbl; ///< 北极极光环中心的纬度（度）
        float64     NorthLon          = _NoDataDbl; ///< 北极极光环中心的经度（度）
        float64     NorthRadius       = _NoDataDbl; ///< 北极极光环半径（米）
        float64     NorthWidth        = _NoDataDbl; ///< 北极极光环宽度（米）
        uint64      NorthRings        = _NoDataInt; ///< 北极极光环数量
        float64     NorthBright       = _NoDataDbl; ///< 北极极光环亮度
        float64     NorthFlashFreq    = _NoDataDbl; ///< 北极极光环闪烁频率
        float64     NorthMoveSpeed    = _NoDataDbl; ///< 北极极光环移动速度
        uint64      NorthParticles    = _NoDataInt; ///< 北极极光环粒子数
        float64     SouthLat          = _NoDataDbl; ///< 南极极光环中心的纬度（度）
        float64     SouthLon          = _NoDataDbl; ///< 南极极光环中心的经度（度）
        float64     SouthRadius       = _NoDataDbl; ///< 南极极光环半径（米）
        float64     SouthWidth        = _NoDataDbl; ///< 南极极光环宽度（米）
        uint64      SouthRings        = _NoDataInt; ///< 南极极光环数量
        float64     SouthBright       = _NoDataDbl; ///< 南极极光环亮度
        float64     SouthFlashFreq    = _NoDataDbl; ///< 南极极光环闪烁频率
        float64     SouthMoveSpeed    = _NoDataDbl; ///< 南极极光环移动速度
        uint64      SouthParticles    = _NoDataInt; ///< 南极极光环粒子数
        vec3        TopColor          = vec3(_NoDataDbl); ///< 顶部颜色（乘数）
        vec3        BottomColor       = vec3(_NoDataDbl); ///< 底部颜色
    }Aurora;

    // ------------------------------------------------------------ //

    /*--------------------------------------------------------------*\
    |                              Rings                             |
    \*--------------------------------------------------------------*/

    bool            NoRings              = true; ///< 是否无环

    /**
     * @brief 环参数
     */
    struct RingsParams : public HapkeParams
    {
        ustring     Texture           = _NoDataStr; ///< 纹理
        float64     InnerRadius       = _NoDataDbl; ///< 内半径（米）
        float64     OuterRadius       = _NoDataDbl; ///< 外半径（米）
        float64     EdgeRadius        = _NoDataDbl; ///< 密集部分外半径（米）
        float64     MeanRadius        = _NoDataDbl; ///< 最密集部分半径（米）
        float64     Thickness         = _NoDataDbl; ///< 厚度（米）
        float64     RocksMaxSize      = _NoDataDbl; ///< 岩石最大尺寸（米）
        float64     RocksSpacing      = _NoDataDbl; ///< 岩石间距
        float64     DustDrawDist      = _NoDataDbl; ///< 尘埃绘制距离
        float64     ChartRadius       = _NoDataDbl; ///< 图表模式外半径（米）
        float64     RotationPeriod    = _NoDataDbl; ///< 旋转周期
        float64     Brightness        = _NoDataDbl; ///< 亮度
        float64     FrontBright       = _NoDataDbl; ///< 正面亮度
        float64     BackBright        = _NoDataDbl; ///< 背面亮度
        float64     Density           = _NoDataDbl; ///< 密度
        float64     Opacity           = _NoDataDbl; ///< 不透明度
        float64     SelfShadow        = _NoDataDbl; ///< 自阴影
        float64     PlanetShadow      = _NoDataDbl; ///< 行星阴影
        float64     frequency         = _NoDataDbl; ///< 频率
        float64     densityScale      = _NoDataDbl; ///< 密度比例
        float64     densityOffset     = _NoDataDbl; ///< 密度偏移
        float64     densityPower      = _NoDataDbl; ///< 密度幂
        float64     colorContrast     = _NoDataDbl; ///< 颜色对比度
        vec3        FrontColor        = vec3(_NoDataDbl); ///< 正面颜色
        vec3        BackThickColor    = vec3(_NoDataDbl); ///< 背面厚颜色
        vec3        BackIceColor      = vec3(_NoDataDbl); ///< 背面冰颜色
        vec3        BackDustColor     = vec3(_NoDataDbl); ///< 背面尘埃颜色
    }Rings;

    /*--------------------------------------------------------------*\
    |                         Accretion Disk                         |
    \*--------------------------------------------------------------*/

    bool            NoAccretionDisk      = true; ///< 是否无吸积盘

    /**
     * @brief 吸积盘参数
     */
    struct DiskParams
    {
        float64     InnerRadius       = _NoDataDbl; ///< 内半径（米）
        float64     OuterRadius       = _NoDataDbl; ///< 外半径（米）
        float64     InnerThickness    = _NoDataDbl; ///< 内厚度（米）
        float64     OuterThickness    = _NoDataDbl; ///< 外厚度（米）
        float64     ThicknessPow      = _NoDataDbl; ///< 厚度幂
        float64     DetailScaleR      = _NoDataDbl; ///< 径向细节比例
        float64     DetailScaleV      = _NoDataDbl; ///< 垂直细节比例
        float64     OctaveDistortionX = _NoDataDbl; ///< X轴倍频程数
        float64     OctaveDistortionY = _NoDataDbl; ///< Y轴倍频程数
        float64     OctaveDistortionZ = _NoDataDbl; ///< Z轴倍频程数
        float64     OctaveScale       = _NoDataDbl; ///< 倍频程数比例
        float64     DiskNoiseContrast = _NoDataDbl; ///< 盘噪声对比度
        float64     DiskTempContrast  = _NoDataDbl; ///< 盘温度对比度
        float64     DiskOuterSpin     = _NoDataDbl; ///< 盘外旋转
        float64     TwistMagn         = _NoDataDbl; ///< 扭曲幅度
        float64     AccretionRate     = _NoDataDbl; ///< 吸积率
        float64     Temperature       = _NoDataDbl; ///< 温度
        float64     Density           = _NoDataDbl; ///< 密度
        float64     Luminosity        = _NoDataDbl; ///< 光度
        float64     LuminosityBol     = _NoDataDbl; ///< 总光度
        float64     Brightness        = _NoDataDbl; ///< 亮度

        float64     JetLength         = _NoDataDbl; ///< 喷流长度（米）
        float64     JetStartRadius    = _NoDataDbl; ///< 喷流起始半径（米）
        float64     JetEndRadius      = _NoDataDbl; ///< 喷流结束半径（米）
        float64     JetStartTemp      = _NoDataDbl; ///< 喷流起始温度
        float64     JetEndTemp        = _NoDataDbl; ///< 喷流结束温度
        float64     JetVelocity       = _NoDataDbl; ///< 喷流速度
        float64     JetDistortion     = _NoDataDbl; ///< 喷流扭曲
        float64     JetThickness      = _NoDataDbl; ///< 喷流厚度
        float64     JetDensity        = _NoDataDbl; ///< 喷流密度
        float64     JetBrightness     = _NoDataDbl; ///< 喷流亮度

        float64     LightingBright    = _NoDataDbl; ///< 光照亮度
        float64     ShadowContrast    = _NoDataDbl; ///< 阴影对比度
        float64     ShadowLength      = _NoDataDbl; ///< 阴影长度
    }AccretionDisk;

    /*--------------------------------------------------------------*\
    |                             Corona                             |
    \*--------------------------------------------------------------*/

    bool            NoCorona             = true; ///< 是否无日冕

    /**
     * @brief 日冕参数
     */
    struct CoronaParams
    {
        float64     Radius            = _NoDataDbl; ///< 半径
        float64     Period            = _NoDataDbl; ///< 周期
        float64     Brightness        = _NoDataDbl; ///< 亮度
        float64     RayDensity        = _NoDataDbl; ///< 射线密度
        float64     RayCurv           = _NoDataDbl; ///< 射线曲率
    }Corona;

    /*--------------------------------------------------------------*\
    |                           Comet Tail                           |
    \*--------------------------------------------------------------*/

    bool            NoCometTail         = true; ///< 是否无彗尾

    /**
     * @brief 彗尾参数
     */
    struct CometTailParams
    {
        float64     MaxLength         = _NoDataDbl; ///< 最大长度
        float64     GasToDust         = _NoDataDbl; ///< 气体与尘埃比
        uint64      Particles         = _NoDataInt; ///< 粒子数
        float64     GasBright         = _NoDataDbl; ///< 气体亮度
        float64     DustBright        = _NoDataDbl; ///< 尘埃亮度
        vec3        GasColor          = vec3(_NoDataDbl); ///< 气体颜色
        vec3        DustColor         = vec3(_NoDataDbl); ///< 尘埃颜色
    }CometTail;
};

typedef std::shared_ptr<Object> ObjectPtr_t; ///< 对象智能指针

/**
 * @brief 对象操作器
 * @details 定义了用于操作天文对象的标志位
 */
struct __Object_Manipulator
{
    static constexpr int32_t Physical   = static_cast<int32_t>(0b00000000000000000000000000000001); ///< 物理属性
    static constexpr int32_t Optical    = static_cast<int32_t>(0b00000000000000000000000000000010); ///< 光学属性
    static constexpr int32_t Rotation   = static_cast<int32_t>(0b00000000000000000000000000000100); ///< 旋转属性
    static constexpr int32_t Life       = static_cast<int32_t>(0b00000000000000000000000000001000); ///< 生命属性
    static constexpr int32_t Interior   = static_cast<int32_t>(0b00000000000000000000000000010000); ///< 内部属性
    static constexpr int32_t Surface    = static_cast<int32_t>(0b00000000000000000000000000100000); ///< 表面属性
    static constexpr int32_t Ocean      = static_cast<int32_t>(0b00000000000000000000000001000000); ///< 海洋属性
    static constexpr int32_t Clouds     = static_cast<int32_t>(0b00000000000000000000000010000000); ///< 云层属性
    static constexpr int32_t Atmosphere = static_cast<int32_t>(0b00000000000000000000000100000000); ///< 大气层属性
    static constexpr int32_t Aurora     = static_cast<int32_t>(0b00000000000000000000001000000000); ///< 极光属性
    static constexpr int32_t Rings      = static_cast<int32_t>(0b00000000000000000000010000000000); ///< 环属性
    static constexpr int32_t AccDisk    = static_cast<int32_t>(0b00000000000000000000100000000000); ///< 吸积盘属性
    static constexpr int32_t Corona     = static_cast<int32_t>(0b00000000000000000001000000000000); ///< 日冕属性
    static constexpr int32_t CometTail  = static_cast<int32_t>(0b00000000000000000010000000000000); ///< 彗尾属性
    static constexpr int32_t AutoOrbit  = static_cast<int32_t>(0b00000000000000000100000000000000); ///< 自动轨道
    static constexpr int32_t Climate    = static_cast<int32_t>(0b00000000000000001000000000000000); ///< 气候属性

    static constexpr int32_t Scientific = static_cast<int32_t>(0b01000000000000000000000000000000); ///< 科学输出
    static constexpr int32_t Booleans   = static_cast<int32_t>(0b00100000000000000000000000000000); ///< 布尔输出
    static constexpr int32_t FTidalLock = static_cast<int32_t>(0b00010000000000000000000000000000); ///< 强制潮汐锁定
    static constexpr int32_t AutoRadius = static_cast<int32_t>(0b00001000000000000000000000000000); ///< 自动半径输出
    static constexpr int32_t FlatObjDim = static_cast<int32_t>(0b00000100000000000000000000000000); ///< 异形物体改为输出尺寸

    static constexpr int32_t Default    = static_cast<int32_t>(0b11111100000000001111111111111111); ///< 默认标志
};

/**
 * @brief 可操作OSC流
 * @details 继承自OSCStream，提供天文对象的流操作功能
 */
class ManipulatableOSCStream : public __Object_Manipulator, virtual public OSCStream
{
public:
    using _Mybase = OSCStream;
    int _BaseInit()override
    {
        CustomMatOutputList.insert({L"PeriodicTermsDiurnal", 6});
        CustomMatOutputList.insert({L"PeriodicTermsSecular", 6});
        return Default;
    }
    ManipulatableOSCStream(std::ostream& os) : _Mybase(os) {_Init();}
    ManipulatableOSCStream& operator=(const ManipulatableOSCStream&) = delete;
};

Object GetObjectFromKeyValue(_SC SCSTable::SCKeyValue KeyValue);
template<> Object GetObject(_SC SharedTablePointer Table, ustring Name);
template<> _SC SCSTable MakeTable(Object Obj, int Fl, std::streamsize Prec);

///@}

namespace ObjectLiterals {

/// @addtogroup SE_Object
/// @{

/**
 * @brief 返回远日点距离
 * @param Obj 天文对象
 * @return 远日点距离
 */
float64 Aphelion(Object Obj);

/**
 * @brief 返回近日点距离
 * @param Obj 天文对象
 * @return 近日点距离
 */
float64 Perihelion(Object Obj);

/**
 * @brief 返回半长轴
 * @param Obj 天文对象
 * @return 半长轴
 */
float64 SemiMajorAxis(Object Obj);

/**
 * @brief 返回平运动
 * @param Obj 天文对象
 * @return 平运动
 */
float64 MeanMotion(Object Obj);

/**
 * @brief 返回离心率
 * @param Obj 天文对象
 * @return 离心率
 */
float64 Eccentricity(Object Obj);

/**
 * @brief 返回恒星年
 * @param Obj 天文对象
 * @return 恒星年
 */
float64 SiderealOrbitalPeriod(Object Obj);

/**
 * @brief 返回平近点角
 * @param Obj 天文对象
 * @return 平近点角
 */
float64 MeanAnomaly(Object Obj);

/**
 * @brief 返回平黄经
 * @param Obj 天文对象
 * @return 平黄经
 */
float64 MeanLongitude(Object Obj);

/**
 * @brief 返回轨道倾角
 * @param Obj 天文对象
 * @return 轨道倾角
 */
float64 Inclination(Object Obj);

/**
 * @brief 返回升交点经度
 * @param Obj 天文对象
 * @return 升交点经度
 */
float64 LongitudeOfAscendingNode(Object Obj);

/**
 * @brief 返回近日点时间
 * @param Obj 天文对象
 * @return 近日点时间
 */
CSEDateTime TimeOfPerihelion(Object Obj);

/**
 * @brief 返回近日点幅角
 * @param Obj 天文对象
 * @return 近日点幅角
 */
float64 ArgumentOfPerihelion(Object Obj);

/**
 * @brief 返回近日点黄经
 * @param Obj 天文对象
 * @return 近日点黄经
 */
float64 LongitudeOfPerihelion(Object Obj);

/**
 * @brief 返回平均半径
 * @param Obj 天文对象
 * @return 平均半径
 */
float64 MeanRadius(Object Obj);

/**
 * @brief 返回赤道半径
 * @param Obj 天文对象
 * @return 赤道半径
 */
float64 EquatorialRadius(Object Obj);

/**
 * @brief 返回极半径
 * @param Obj 天文对象
 * @return 极半径
 */
float64 PolarRadius(Object Obj);

/**
 * @brief 返回扁率
 * @param Obj 天文对象
 * @return 扁率
 */
vec3 Flattening(Object Obj);

/**
 * @brief 返回赤道周长
 * @param Obj 天文对象
 * @return 赤道周长
 */
float64 EquatorialCircumference(Object Obj);

/**
 * @brief 返回子午线周长
 * @param Obj 天文对象
 * @return 子午线周长
 */
float64 MeridionalCircumference(Object Obj);

/**
 * @brief 返回表面积
 * @param Obj 天文对象
 * @return 表面积
 */
float64 SurfaceArea(Object Obj);

/**
 * @brief 返回体积
 * @param Obj 天文对象
 * @return 体积
 */
float64 Volume(Object Obj);

/**
 * @brief 返回质量
 * @param Obj 天文对象
 * @return 质量
 */
float64 Mass(Object Obj);

/**
 * @brief 返回平均密度
 * @param Obj 天文对象
 * @return 平均密度
 */
float64 MeanDensity(Object Obj);

/**
 * @brief 返回表面重力
 * @param Obj 天文对象
 * @return 表面重力
 */
float64 SurfaceGravity(Object Obj);

/**
 * @brief 返回逃逸速度
 * @param Obj 天文对象
 * @return 逃逸速度
 */
float64 EscapeVelocity(Object Obj);

/**
 * @brief 返回会合日
 * @param Obj 天文对象
 * @return 会合日
 */
float64 SynodicRotationPeriod(Object Obj);

/**
 * @brief 返回恒星日
 * @param Obj 天文对象
 * @return 恒星日
 */
float64 SiderealRotationPeriod(Object Obj);

/**
 * @brief 返回赤道自转速度
 * @param Obj 天文对象
 * @return 赤道自转速度
 */
float64 EquatorialRotationVelocity(Object Obj);

/**
 * @brief 返回轴向倾角
 * @param Obj 天文对象
 * @return 轴向倾角
 */
float64 AxialTilt(Object Obj);

/**
 * @brief 返回几何反照率
 * @param Obj 天文对象
 * @return 几何反照率
 */
float64 GeometricAlbedo(Object Obj);

/**
 * @brief 返回邦德反照率
 * @param Obj 天文对象
 * @return 邦德反照率
 */
float64 BondAlbedo(Object Obj);

/**
 * @brief 返回有效温度
 * @param Obj 天文对象
 * @return 有效温度
 */
float64 EffectiveTemperature(Object Obj);

/**
 * @brief 返回平衡温度
 * @param Parent 母体对象
 * @param Companion 伴星对象
 * @param Separation 分离距离
 * @return 平衡温度
 */
float64 EquilibriumTemperature(Object Parent, Object Companion, float64 Separation);

/**
 * @brief 返回绝对星等
 * @param Obj 天文对象
 * @return 绝对星等
 */
float64 AbsoluteMagnitude(Object Obj);

///@}

}

}