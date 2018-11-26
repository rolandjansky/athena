/*
 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef Ludat1_h
#define Ludat1_h

extern "C" { void* ludat1_address_(void); }

/**
 * @class Ludat1
 * @brief      Class definition for Ludat1, which is used
 *       to modify the LUDAT1 common.
 *       */
class Ludat1 {
public:
    Ludat1();
    ~Ludat1();
    
    int& 	mstu	(int n);
    float&	paru	(int n);
    int& 	mstj	(int n);
    float&	parj	(int n);
    
    void 	init	(void);

    // return common array lengths
    inline int lenMstu() const {return s_lenMstu;}
    inline int lenParu() const {return s_lenParu;}
    inline int lenMstj() const {return s_lenMstj;}
    inline int lenParj() const {return s_lenParj;}

private: 

    // Lengths of the COMMONS
    static const int s_lenMstu = 200;
    static const int s_lenParu = 200;
    static const int s_lenMstj = 200;
    static const int s_lenParj = 200;

    struct LUDAT1;
    friend struct LUDAT1;

    struct LUDAT1
    {
	int 	mstu[s_lenMstu];
	float	paru[s_lenParu];
	int 	mstj[s_lenMstj];
	float	parj[s_lenParj];
    };

    int m_dummy;
    float m_realdummy;
    static LUDAT1* s_ludat1;
};

// set pointer to zero at start
Ludat1::LUDAT1* Ludat1::s_ludat1 = 0;

inline void
Ludat1::init(void)
{ if (!s_ludat1) s_ludat1 = static_cast<LUDAT1*>(ludat1_address_()); }

inline 
Ludat1::Ludat1() 
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

inline 
Ludat1::~Ludat1() 
{}

inline int&
Ludat1::mstu	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenMstu()) return m_dummy;
    return s_ludat1->mstu[n-1];
}

inline float&
Ludat1::paru	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenParu()) return m_realdummy;
    return s_ludat1->paru[n-1];
}

inline int&
Ludat1::mstj	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenMstj()) return m_dummy;
    return s_ludat1->mstj[n-1];
}

inline float&
Ludat1::parj	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenParj()) return m_realdummy;
    return s_ludat1->parj[n-1];
}

#endif

