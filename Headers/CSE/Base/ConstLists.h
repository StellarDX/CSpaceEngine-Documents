/**
 * @file ConstLists.h
 * @brief 天文计算用常量
 * @copyright Copyright (c) StellarDX Astronomy
 */

namespace cse{
namespace consts{

/**
 * @defgroup ConstLists 常量列表
 * @ingroup Base
 * @details 本文件包含用于天文计算的各种常量定义，除非另有明确说明，所有值均以SI基本单位表示。
 * @par 参考
 * - NIST CODATA 2018值
 * - IAU定义
 * - 其他相关天文标准
 * @{
 */

/**
 * @page MathDefines 列表1：数学常数
 * @details
 * | 常量          | 值                                  | 描述                      |
 * | --------------|------------------------------------|---------------------------|
 * | CSE_E         | 2.7182818284590452353602874713527  | \f$e\f$                   |
 * | CSE_LBE       | 1.4426950408889634073599246810019  | \f$lb(e)\f$               |
 * | CSE_LOGE      | 0.43429448190325182765112891891661 | \f$log(e)\f$              |
 * | CSE_LN2       | 0.69314718055994530941723212145818 | \f$ln(2)\f$               |
 * | CSE_LN10      | 2.3025850929940456840179914546844  | \f$ln(10)\f$              |
 * | CSE_PI        | 3.1415926535897932384626433832795  | \f$\pi\f$                 |
 * | CSE_2PI       | 6.283185307179586476925286766559   | \f$\pi \cdot 2\f$         |
 * | CSE_PI_D2     | 1.5707963267948966192313216916398  | \f$\frac{\pi}{2}\f$       |
 * | CSE_PI_D4     | 0.78539816339744830961566084581988 | \f$\frac{\pi}{4}\f$       |
 * | CSE_1D_PI     | 0.31830988618379067153776752674503 | \f$\frac{1}{\pi}\f$        |
 * | CSE_2D_PI     | 0.63661977236758134307553505349006 | \f$\frac{2}{\pi}\f$        |
 * | CSE_2D_SQRTPI | 1.1283791670955125738961589031215  | \f$\frac{2}{\sqrt{\pi}}\f$ |
 * | CSE_SQRT2     | 1.4142135623730950488016887242097  | \f$\sqrt{2}\f$            |
 * | CSE_SQRT1_D2  | 0.70710678118654752440084436210485 | \f$\sqrt{\frac{1}{2}}\f$  |
 */

/**
 * @page CommonPhysicConstants 列表2：常用物理常数
 * @details
 * | 常量              | 描述                 | 符号                                                       | 值              | 绝对误差 | 相对误差 | 来源                                              |
 * |-----------------|-----------------------|----------------------------------------------------------|-------------------|---------|---------|---------------------------------------------------|
 * | GravConstant    | 引力常数              | \f$G\f$                                                  | 6.67430E-11       | 1.5E-15 | 2.2E-5  | 2022 CODATA (NIST)                                |
 * | SpeedOfLight    | 光速                  | \f$c\f$                                                  | 299792458         |         |         | 2022 CODATA (NIST)                                |
 * | PPM2Percent     | PPM转百分数           |                                                          | 0.0001            |         |         | 按定义，1ppm = 0.0001%                             |
 * | GasConstant     | 气体常数              | \f$R=N_{\text{A}}k_{\text{B}}\f$                         | 8.31446261815324  |         |         | 2022 CODATA (NIST)                                |
 * | AvogadroConst   | 阿伏伽德罗常数        | \f$N_{\text{A}}\f$                                       | 6.02214076E+23    |         |         | 2022 CODATA (NIST)                                |
 * | BoltzmannConst  | 玻尔兹曼常数          | \f$k_{\text{B}}\f$                                       | 1.380649E-23      |         |         | 2022 CODATA (NIST)                                |
 * | PlanckConst     | 普朗克常数            | \f$h\f$                                                  | 6.62607015E-34    |         |         | 2022 CODATA (NIST)                                |
 * | StBConstant     | 斯忒藩-玻尔兹曼常数    | \f$\sigma =\pi ^{2}k_{\text{B}}^{4}/60\hbar ^{3}c^{2}\f$ | 5.670374419E-8    |         |         | 按定义                                            |
 * | ElemCharge      | 基本电荷              | \f$e\f$                                                  | 1.602176634E-19   |         |         | 2022 CODATA (NIST)                                |
 * | VacPermeability | 真空磁导率            | \f$\mu _{0}=4\pi \alpha \hbar /e^{2}c\f$                 | 1.25663706127E-6  | 2E-16   | 1.6E-10 | 2022 CODATA (NIST)                                |
 * | VacPermittivity | 真空介电常量          | \f$\varepsilon _{0}=e^{2}/4\pi \alpha \hbar c\f$         | 8.8541878188E-12  | 1.4E-21 | 1.6E-10 | 2022 CODATA (NIST)                                |
 * | FineStructConst | 精细结构常数          | \f$\alpha =e^{2}/4\pi \varepsilon _{0}\hbar c\f$         | 0.0072973525643   | 1.1E-12 | 1.6E-10 | 2022 CODATA (NIST)                                |
 * | AtomMassConst   | 原子质量常数（道尔顿） | \f$m_{\text{u}}=m({}^{12}{\text{C}})/12\f$               | 1.66053906892E-27 | 5.2E-37 |         | 2022 CODATA (NIST)                                |
 * | StandardAtm     | 标准大气压            | \f$\text{atm}\f$                                         | 101325            |         |         | NOAA Pacific Marine Environmental Laboratory 2022 |
 * | StdSurfGravity  | 标准地球表面重力      | \f$g_0\f$                                                | 9.80665           |         |         | SI 2008                                           |
 * 
 * @par 参考文献
 * [1]	Mohr P J, Tiesinga E, Newell D B, 等. Codata Internationally Recommended 2022 Values of the Fundamental Physical Constants[A/OL]. Codata Internationally Recommended 2022 Values of the Fundamental Physical Constants, 2024. https://tsapps.nist.gov/publication/get_pdf.cfm?pub_id=958002.
 */

/**
 * @page MassUnits 列表3：质量单位
 * @details
 * | 常量               | 描述        | 值                | 绝对误差 | 相对误差 | 来源               |
 * |-------------------|-------------|-------------------|---------|---------|--------------------|
 * | ElectronMass      | 电子质量     | 9.1093837139E-31  | 2.8E-40 | 3.1E-10 | 2022 CODATA (NIST) |
 * | ProtonMass        | 质子质量     | 1.67262192595E-27 | 5.2E-37 | 3.1E-10 | 2022 CODATA (NIST) |
 * | NeutronMass       | 中子质量     | 1.67492750056E-27 | 8.5E-37 | 5.1E-10 | 2022 CODATA (NIST) |
 * | LunarMass         | 月球质量     | 7.34579E+22       |         |         | DE440              |
 * | EarthMass         | 地球质量     | 5.9721684E+24     | 2.8E+20 |         | DE440              |
 * | JupiterMass       | 木星质量     | 1.898125E+27      | 8.8E+22 |         | JUP310             |
 * | SolarMass         | 太阳质量     | 1.988475E+30      |         |         | IAU 2015           |
 * | NorminalSolarMass | 标准太阳质量 | 1.988416E+30      |         |         | 开普勒第三定律       |
 * @par 参考文献
 * [1]	Mohr P J, Tiesinga E, Newell D B, 等. Codata Internationally Recommended 2022 Values of the Fundamental Physical Constants[A/OL]. Codata Internationally Recommended 2022 Values of the Fundamental Physical Constants, 2024. https://tsapps.nist.gov/publication/get_pdf.cfm?pub_id=958002.<br>
 * [2]	Prsa A, Harmanec P, Torres G, 等. NOMINAL VALUES FOR SELECTED SOLAR AND PLANETARY QUANTITIES: IAU 2015 RESOLUTION B3*  †[J/OL]. The Astronomical Journal, 2016, 152(2): 41. https://dx.doi.org/10.3847/0004-6256/152/2/41. DOI:10.3847/0004-6256/152/2/41.
 */

/**
 * @page LengthUnits 列表4：长度单位
 * @details
 * | 常量                | 描述         | 值                               | 来源                  |
 * |---------------------|-------------|----------------------------------|----------------------|
 * | LunarRadius         | 月球半径     | 1738100                          | NASA 2006            |
 * | LunarPolarRadius    | 月球极半径   | 1736000                          | NASA 2006            |
 * | EarthRadius         | 地球半径     | 6378137                          | IUGG 1980(WGS 84)    |
 * | EarthPolarRadius    | 地球极半径   | 6356752.31414                    | GRS 80               |
 * | JupiterRadius       | 木星半径     | 71492000                         | IAU 2015             |
 * | JupiterPolarRadius  | 木星极半径   | 66854000                         | IAU 2015             |
 * | SolarRadius         | 太阳半径     | 695660000                        | 由2008年的日震影像得到 |
 * | NorminalSolarRadius | 标称太阳半径 | 695700000                        | IAU 2015             |
 * | AU                  | 天文单位    | 149597870700                      | IAU 2012             |
 * | LightYear           | 光年        | 9460730472580800                  | IAU 1976             |
 * | Parsec              | 秒差距      | 30856775814913672.789139379577965 | IAU 2015             |
 * @note 气态行星的表面定义为100Kpa等压面的位置，恒星的表面定义为光深2/3（光球层顶）的位置
 * @par 参考文献
 * [3]	Haberreiter M, Schmutz W, Kosovichev A G. Solving the Discrepancy between the Seismic and Photospheric Solar Radius[J/OL]. The Astrophysical Journal, 2008, 675(1): L53. https://dx.doi.org/10.1086/529492. DOI:10.1086/529492.
 */

/**
 * @page TimeUnits 列表5：时间单位
 * @details
 * | 常量          | 描述        | 值                | 来源                              |
 * |--------------|-------------|-------------------|-----------------------------------|
 * | SynodicDay   | 会合日      | 86400             | 一天的长度                         |
 * | SiderealDay  | 恒星日      | 86164.09053083288 | IERS 2018                         |
 * | StellarDay   | 地球自转周期 | 86164.098903691   | IERS 2018                         |
 * | JulianYear   | 儒略年      | 31557600          | 按定义                             |
 * | TropicalYear | 回归年      | 31556925.18747072 | 2000年1月1日的值                   |
 * | SiderealYear | 恒星年      | 31558149.7635456  | IERS 2014                         |
 * | J2000        |            | 2451545           | 2000年1月1日 12:00:00对应的儒略日数 |
 * @note 恒星日由地球自转周期与春分点位置定义，大约比地球真正的自转周期少8.4毫秒
 */

/**
 * @page LumUnits 列表6：光度单位
 * @details
 * | 常量         | 描述      | 值         | 来源      |
 * |-------------|-----------|------------|----------|
 * | SolarLum    | 太阳光度   | 3.828E+26  | IAU 2015 |
 * | SolarLumBol | 太阳热光度 | 3.0128E+28 | IAU 2015 |
 * @note 太阳热光度为绝对热星等计算的初始倍率
 */

/**@}*/

} // cse
} // consts