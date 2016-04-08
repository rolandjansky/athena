/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef HijJet1_h
#define HijJet1_h

extern "C" { void* hijjet1_address_(void); }
/**
@brief   Class definition for HijJet1, which is used
      to modify the Hijing HIJJET1 common.
*/
class HijJet1 {
public:
    HijJet1();
    ~HijJet1();
    
    int&    	npj	(int i);
    int&    	kfpj	(int i, int j);
    float&  	pjpx	(int i, int j);
    float&  	pjpy	(int i, int j);
    float&  	pjpz	(int i, int j);
    float&  	pjpe	(int i, int j);
    float&  	pjpm	(int i, int j);
    int&    	ntj	(int i);
    int&    	kftj	(int i, int j);
    float&  	pjtx	(int i, int j);
    float&  	pjty	(int i, int j);
    float&  	pjtz	(int i, int j);
    float&  	pjte	(int i, int j);
    float&  	pjtm	(int i, int j);
    
    void	init	(void);

    // return common array lengths
    inline int	lenI() const {return s_lenI;}
    inline int	lenJ() const {return s_lenJ;}

private: 

    // Lengths of array in HiMain2 common
    static const int s_lenI	= 300;
    static const int s_lenJ	= 500;

    struct HIJJET1;
    friend struct HIJJET1;

    struct HIJJET1
    {
	int    	npj	[s_lenI];
	int    	kfpj	[s_lenJ][s_lenI];
	float  	pjpx	[s_lenJ][s_lenI];
	float  	pjpy	[s_lenJ][s_lenI];
	float  	pjpz	[s_lenJ][s_lenI];
	float  	pjpe	[s_lenJ][s_lenI];
	float  	pjpm	[s_lenJ][s_lenI];
	int    	ntj	[s_lenI];
	int    	kftj	[s_lenJ][s_lenI];
	float  	pjtx	[s_lenJ][s_lenI];
	float  	pjty	[s_lenJ][s_lenI];
	float  	pjtz	[s_lenJ][s_lenI];
	float  	pjte	[s_lenJ][s_lenI];
	float  	pjtm	[s_lenJ][s_lenI];
    };

    int  m_dummy;
    float  m_realdummy;

    static HIJJET1* s_hijjet1;
};

// set pointer to zero at start
HijJet1::HIJJET1* HijJet1::s_hijjet1 =0;

inline void
HijJet1::init(void)
{ if (!s_hijjet1) s_hijjet1 = static_cast<HIJJET1*>(hijjet1_address_()); }

// Constructor
inline
HijJet1::HijJet1()
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

// Destructor
inline
HijJet1::~HijJet1()
{}

inline int&
HijJet1::npj	(int i)
{
    init(); // check COMMON is initialized
    if(i < 1 || i > lenI()) return m_dummy;
    return s_hijjet1->npj[i-1];
}

inline int&
HijJet1::kfpj	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > lenI() ||
	j < 1 || j > lenJ() ) return m_dummy;

    return s_hijjet1->kfpj[j-1][i-1];
}

inline float&
HijJet1::pjpx	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > lenI() ||
	j < 1 || j > lenJ() ) return m_realdummy;

    return s_hijjet1->pjpx[j-1][i-1];
}

inline float&
HijJet1::pjpy	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > lenI() ||
	j < 1 || j > lenJ() ) return m_realdummy;

    return s_hijjet1->pjpy[j-1][i-1];
}

inline float&
HijJet1::pjpz	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > lenI() ||
	j < 1 || j > lenJ() ) return m_realdummy;

    return s_hijjet1->pjpz[j-1][i-1];
}

inline float&
HijJet1::pjpe	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > lenI() ||
	j < 1 || j > lenJ() ) return m_realdummy;

    return s_hijjet1->pjpe[j-1][i-1];
}

inline float&
HijJet1::pjpm	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > lenI() ||
	j < 1 || j > lenJ() ) return m_realdummy;

    return s_hijjet1->pjpm[j-1][i-1];
}

inline int&
HijJet1::ntj	(int i)
{
    init(); // check COMMON is initialized
    if(i < 1 || i > lenI()) return m_dummy;
    return s_hijjet1->ntj[i-1];
}

inline int&
HijJet1::kftj	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > lenI() ||
	j < 1 || j > lenJ() ) return m_dummy;

    return s_hijjet1->kftj[j-1][i-1];
}

inline float&
HijJet1::pjtx	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > lenI() ||
	j < 1 || j > lenJ() ) return m_realdummy;

    return s_hijjet1->pjtx[j-1][i-1];
}

inline float&
HijJet1::pjty	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > lenI() ||
	j < 1 || j > lenJ() ) return m_realdummy;

    return s_hijjet1->pjty[j-1][i-1];
}

inline float&
HijJet1::pjtz	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > lenI() ||
	j < 1 || j > lenJ() ) return m_realdummy;

    return s_hijjet1->pjtz[j-1][i-1];
}

inline float&
HijJet1::pjte	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > lenI() ||
	j < 1 || j > lenJ() ) return m_realdummy;

    return s_hijjet1->pjte[j-1][i-1];
}

inline float&
HijJet1::pjtm	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > lenI() ||
	j < 1 || j > lenJ() ) return m_realdummy;

    return s_hijjet1->pjtm[j-1][i-1];
}

#endif
