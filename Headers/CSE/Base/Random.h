namespace cse {
namespace Probability {

/**
 * @file Random.h
 * @defgroup Random 随机数
 * @ingroup Base
 * @brief CSpaceEngine 随机数生成器模块
 * @details 该文件定义了多种随机数分布和生成器，基于标准库的随机数功能进行扩展
 * @copyright Copyright (c) StellarDX Astronomy
 * @{
 */

/**
 * @brief 三角形分布
 * @tparam _Ty 数据类型，默认为float64
 */
template<class _Ty = float64>
class triangular_distribution : public std::piecewise_linear_distribution<_Ty>
{
public:
    using _Mybase     = std::piecewise_linear_distribution<_Ty>;
    using _Mypbase    = typename _Mybase::param_type;
    using result_type = _Ty;

    triangular_distribution() {}

    /**
     * @brief 构造函数
     * @param _Min0 最小值
     * @param _Max0 最大值
     * @param _Peak 峰值
     */
    triangular_distribution(_Ty _Min0, _Ty _Max0, _Ty _Peak)
    {
        std::array<_Ty, 3> _Values{ _Min0, _Peak, _Max0 };
        std::array<_Ty, 3> _Weights{ 0, 1, 0 };
        _Mybase::_Par = _Mybase::param_type(_Values.begin(), _Values.end(), _Weights.begin());
    }
};

/**
 * @brief Beta分布
 * @tparam _Ty 数据类型，默认为float64
 */
template<class _Ty = float64>
class beta_distribution
{
public:
    using result_type = _Ty;

    _Ty _Ax; ///< Alpha参数
    _Ty _Bx; ///< Beta参数

    /**
     * @brief 构造函数
     * @param _Ax0 Alpha参数，默认为1
     * @param _Bx0 Beta参数，默认为1
     */
    explicit beta_distribution(const _Ty& _Ax0 = _Ty{1}, const _Ty& _Bx0 = _Ty{1}) noexcept
    {
        _Init(_Ax0, _Bx0);
    }

    /**
     * @brief 初始化Beta分布参数
     * @param _Ax0 Alpha参数
     * @param _Bx0 Beta参数
     * @throws std::logic_error 如果参数无效
     */
    void _Init(_Ty _Ax0, _Ty _Bx0) noexcept
    {
        if(0.0 >= _Ax0){ throw std::logic_error("invalid a argument for _Beta_distribution");}
        if(0.0 >= _Bx0){ throw std::logic_error("invalid b argument for _Beta_distribution");}
        _Ax = _Ax0;
        _Bx = _Bx0;
    }

    /**
     * @brief 生成随机数
     * @tparam _Engine 随机数引擎类型
     * @param _Eng 随机数引擎引用
     * @return 生成的随机数
     */
    template <class _Engine>
    result_type operator()(_Engine& _Eng) const
    {
        if (_Ax < _Ty{1} && _Bx < _Ty{1})
        {
            _Ty _Wx;
            _Ty _Px1;
            _Ty _Px2;
            for (;;)
            {
                _Px1 = std::generate_canonical<_Ty, std::numeric_limits<_Ty>::digits>(_Eng);
                _Px2 = std::generate_canonical<_Ty, std::numeric_limits<_Ty>::digits>(_Eng);
                _Px1 = pow(_Px1, _Ty{1} / _Ax);
                _Px2 = pow(_Px2, _Ty{1} / _Bx);
                _Wx  = _Px1 + _Px2;
                if (_Wx <= _Ty{1} && _Wx != _Ty{0}){break;}
            }
            return _Px1 / _Wx;
        }
        else
        {
            _Ty _Px1;
            _Ty _Px2;
            _Ty _PSum;
            std::gamma_distribution<_Ty> _Dist1(_Ax, 1);
            std::gamma_distribution<_Ty> _Dist2(_Bx, 1);

            for (;;)
            {
                _Px1  = _Dist1(_Eng);
                _Px2  = _Dist2(_Eng);
                _PSum = _Px1 + _Px2;

                if (_PSum != _Ty{0}) {break;}
            }

            return _Px1 / _PSum;
        }
    }
};

/**
 * @brief 自定义分布
 * @tparam _Ty 数据类型，默认为float64
 */
template <class _Ty = float64>
class _Custom_Distribution
{
public:
    using result_type  = _Ty;
    using icdf_ptr     = std::shared_ptr<_SCICXX InverseFunction>;
    using default_invf = _SCICXX BrentInverseFunction;

    icdf_ptr ICDF; ///< 累积分布函数的反函数指针
    vec2 Doamin = _SCICXX __Whole_Line; ///< 定义域

    /**
     * @brief 构造函数
     * @param InvF 累积分布函数的反函数指针
     */
    _Custom_Distribution(icdf_ptr InvF) : ICDF(InvF) {}

    /**
     * @brief 构造函数
     * @param CDF 累积分布函数
     * @param Domain 定义域
     */
    _Custom_Distribution(_SCICXX Function1D CDF, vec2 Domain) : Doamin(Domain)
    {
        ICDF = std::make_shared<default_invf>(
            default_invf([Domain, &CDF](float64 x)
        {
            vec2 _Domain = Domain;
            if (_Domain[0] > _Domain[1]) {std::swap(_Domain[0], _Domain[1]);}
            if (x < _Domain[0]) {return 0.;}
            if (x > _Domain[1]) {return 1.;}
            float64 Min = CDF(_Domain[0]), Max = CDF(_Domain[1]);
            return (CDF(x) - Min) / (Max - Min);
        }, Domain));
    }

    /**
     * @brief 生成随机数
     * @tparam _Engine 随机数引擎类型
     * @param _Eng 随机数引擎引用
     * @return 生成的随机数
     */
    template <class _Engine>
    result_type operator()(_Engine& _Eng) const
    {
        return (*ICDF)(std::generate_canonical<_Ty, std::numeric_limits<_Ty>::digits>(_Eng));
    }
};

/**
 * @brief 随机数引擎
 * @tparam _Rand_Engine 随机数引擎类型
 */
template<typename _Rand_Engine>
class _CSE_Random_Engine
{
public:
    using Engine  = _Rand_Engine; ///< 随机数引擎类型
    using Seed    = uint64;       ///< 种子类型

private:
    _Rand_Engine _Rd; ///< 随机数引擎实例
    Seed _Seed;       ///< 当前种子

public:
    // ---------- 构造函数 ---------- //

    _CSE_Random_Engine() = delete;

    /**
     * @brief 构造函数
     * @param _Xx0 种子值，默认为0x1571U
     */
    _CSE_Random_Engine(Seed _Xx0 = 0x1571U) : _Seed(_Xx0)
    {
        _Rd = _Rand_Engine(_Xx0);
    };

    /**
     * @brief 重置随机数引擎
     */
    void reset(){ _Rd = _Rand_Engine(_Seed); }

    // ---------- 簿记函数 ---------- //

    /**
     * @brief 获取当前种子
     * @return 当前种子值
     */
    Seed seed()const{return _Seed;}

    /**
     * @brief 设置新种子
     * @param _New 新种子值
     * @return 旧种子值
     */
    Seed seed(Seed _New)
    {
        Seed _Old = _Seed;
        _Seed = _New;
        reset();
        return _Old;
    }

    // ---------- 整数相关函数 ---------- //

    /**
     * @brief 从range(start, stop, step)中随机选择一个元素
     * @param _Begin 起始值
     * @param _End 结束值
     * @param _Step 步长
     * @return 随机选择的元素
     */
    template<typename genType>
    genType randrange(genType _Begin, genType _End, genType _Step)
    {
        std::vector<genType> _Range;
        for (genType i = _Begin; i < _End; i += _Step)
        {
            _Range.push_back(i);
        }
        std::uniform_int_distribution<uint64> _Gen(0, _Range.size() - 1);
        return _Range[_Gen(_Rd)];
    }

    /**
     * @brief 返回[a, b]范围内的随机整数
     * @param _Min0 最小值
     * @param _Max0 最大值
     * @return 随机整数
     */
    int64 randint(int64 _Min0, int64 _Max0)
    {
        return std::uniform_int_distribution<int64>(_Min0, _Max0)(_Rd);
    }

    // ---------- 序列相关函数 ---------- //

    /**
     * @brief 从非空序列中随机选择一个元素
     * @param _Begin 起始迭代器
     * @param _End 结束迭代器
     * @return 随机选择的元素
     * @throws std::logic_error 如果序列为空
     */
    template<typename Iterator, typename genType>
    genType choice(Iterator _Begin, Iterator _End)
    {
        if(_Begin > _End){throw std::logic_error("Sequence is empty.");}
        std::vector<genType> _Range;
        for (; _Begin < _End; ++_Begin)
        {
            _Range.push_back(*_Begin);
        }
        std::uniform_int_distribution<uint64> _Gen(0, _Range.size() - 1);
        return _Range[_Gen(_Rd)];
    }

    /**
     * @brief 随机打乱序列
     * @param _Begin 起始迭代器
     * @param _End 结束迭代器
     * @throws std::logic_error 如果序列为空
     */
    template<typename Iterator>
    void shuffle(Iterator _Begin, Iterator _End)
    {
        if(_Begin > _End){throw std::logic_error("Sequence is empty.");}
        if (_Begin == _End) {return;}

        using _Diff = ptrdiff_t;
        auto _UTarget = _Begin;
        _Diff _Target_index = 1;
        for ( ; ++_UTarget != _End; ++_Target_index)
        {
            _Diff _Off = randint(0, static_cast<_Diff>(_Target_index));
            if(0 > _Off && _Off > _Target_index){throw std::logic_error("Random value out of range");}
            if (_Off != _Target_index)
            {
                std::iter_swap(_UTarget, _Begin + _Off);
            }
        }
    }

    // ---------- 实数分布函数 ---------- //

    /**
     * @brief 返回[0.0, 1.0]范围内的随机浮点数
     * @return 随机浮点数
     */
    float64 random()
    {
        return std::uniform_real_distribution<float64>(0., 1.)(_Rd);
    }

    /**
     * @brief 返回[a, b]范围内的随机浮点数
     * @param _Min0 最小值
     * @param _Max0 最大值
     * @return 随机浮点数
     */
    float64 uniform(float64 _Min0, float64 _Max0)
    {
        return std::uniform_real_distribution<float64>(_Min0, _Max0)(_Rd);
    }

    /**
     * @brief 返回三角形分布的随机浮点数
     * @param _Min0 最小值，默认为0
     * @param _Max0 最大值，默认为1
     * @param _Peak 峰值，默认为中值
     * @return 随机浮点数
     */
    float64 triangular(float64 _Min0, float64 _Max0, float64 _Peak)
    {
        return triangular_distribution<float64>(_Min0, _Max0, _Peak)(_Rd);
    }

    /**
     * @brief 返回[0.0, 1.0]范围内，峰值为0.5的三角形分布随机数
     * @return 随机浮点数
     */
    float64 triangular()
    {
        return triangular(0., 1., 0.5);
    }

    /**
     * @brief 返回指定范围内的三角形分布随机数，峰值为中值
     * @param _Min0 最小值
     * @param _Max0 最大值
     * @return 随机浮点数
     */
    float64 triangular(float64 _Min0, float64 _Max0)
    {
        return triangular(_Min0, _Max0, (_Min0 + _Max0) / 2.);
    }

    /**
     * @brief 返回Beta分布随机数
     * @param _Alf Alpha参数，默认为1
     * @param _Bet Beta参数，默认为1
     * @return 范围在0到1之间的随机数
     */
    float64 betavariate(float64 _Alf = 1., float64 _Bet = 1.)
    {
        return beta_distribution<float64>(_Alf, _Bet)(_Rd);
    }

    /**
     * @brief 返回指数分布随机数
     * @param _Lam Lambda参数，默认为1
     * @return 随机数
     */
    float64 expovariate(float64 _Lam = 1.)
    {
        return std::exponential_distribution<float64>(_Lam)(_Rd);
    }

    /**
     * @brief 返回Gamma分布随机数
     * @param _Alf 形状参数，默认为1
     * @param _Bet 尺度参数，默认为1
     * @return 随机数
     */
    float64 gammavariate(float64 _Alf = 1., float64 _Bet = 1.)
    {
        return std::gamma_distribution<float64>(_Alf, _Bet)(_Rd);
    }

    /**
     * @brief 返回Gamma分布随机数（另一种参数化方式）
     * @param _K 形状参数
     * @param _Tet 尺度参数的倒数
     * @return 随机数
     */
    float64 kgamma(float64 _K = 1., float64 _Tet = 1.)
    {
        return gammavariate(_K, 1. / _Tet);
    }

    /**
     * @brief 返回正态（高斯）分布随机数
     * @param _Mean 均值，默认为0
     * @param _StDev 标准差，默认为1
     * @return 随机数
     */
    float64 normalvariate(float64 _Mean = 0., float64 _StDev = 1.)
    {
        return std::normal_distribution<float64>(_Mean, _StDev)(_Rd);
    }

    /**
     * @brief 返回对数正态分布随机数
     * @param _Mean 均值，默认为0
     * @param _StDev 标准差，默认为1
     * @return 随机数
     */
    float64 lognormvariate(float64 _Mean = 0., float64 _StDev = 1.)
    {
        return std::lognormal_distribution<float64>(_Mean, _StDev)(_Rd);
    }

    /**
     * @brief 返回Weibull分布随机数
     * @param _Lam 尺度参数
     * @param _k 形状参数
     * @return 随机数
     */
    float64 weibullvariate(float64 _Lam = 0., float64 _k = 1.)
    {
        return std::weibull_distribution<float64>(_Lam, _k)(_Rd);
    }

    /**
     * @brief 根据概率列表生成随机数
     * @param _Ilist 概率列表
     * @return 生成的随机索引
     */
    uint64 probability(std::initializer_list<float64> _Ilist)
    {
        return std::discrete_distribution<uint64>(_Ilist)(_Rd);
    }
};

///@}

} // End Probability

/**
 * @brief 全局随机数引擎实例
 * @ingroup Random
 * @details 使用Mersenne Twister算法实现
 */
extern Probability::_CSE_Random_Engine<std::mersenne_twister_engine
    <unsigned long long, 64, 312, 156, 31, 0xB5026F5AA96619E9ULL, 29,
    0x5555555555555555ULL, 17, 0x71D67FFFEDA60000ULL, 37, 0xFFF7EEE000000000ULL, 43, 0x5851F42D4C957F2DULL>>
    random;

}