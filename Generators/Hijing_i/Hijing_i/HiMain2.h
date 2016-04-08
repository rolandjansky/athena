/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef HiMain2_h
#define HiMain2_h

extern "C" { void* himain2_address_(void); }
/**
@class HiMain2
@brief       Class definition for HiMain2, which is used
      to modify the Hijing HIMAIN2 common.
*/
class HiMain2 {
public:
    HiMain2();
    ~HiMain2();
    
    int&	katt	(int i, int j);
    float&	patt	(int i, int j);
    float&	vatt	(int i, int j);

    void	init	(void);

    // return common array lengths
    inline int	leniKatt() const {return s_leniKatt;}
    inline int	lenjKatt() const {return s_lenjKatt;}
    inline int	leniPatt() const {return s_leniPatt;}
    inline int	lenjPatt() const {return s_lenjPatt;}
    inline int	leniVatt() const {return s_leniVatt;}
    inline int	lenjVatt() const {return s_lenjVatt;}

private: 

    // Lengths of array in HiMain2 common
    static const int s_leniKatt	= 130000;
    static const int s_lenjKatt	= 4;
    static const int s_leniPatt	= 130000;
    static const int s_lenjPatt	= 4;
    static const int s_leniVatt	= 130000;
    static const int s_lenjVatt	= 4;

    struct HIMAIN2;
    friend struct HIMAIN2;

    struct HIMAIN2
    {
	int  	katt[s_lenjKatt][s_leniKatt];
	float  	patt[s_lenjPatt][s_leniPatt];
	float  	vatt[s_lenjVatt][s_leniVatt];
    };

    int  m_dummy;
    float  m_realdummy;
    
    static HIMAIN2* s_himain2;
};

// set pointer to zero at start
HiMain2::HIMAIN2* HiMain2::s_himain2 =0;

inline void
HiMain2::init(void)
{ if (!s_himain2) s_himain2 = static_cast<HIMAIN2*>(himain2_address_()); }

// Constructor
inline
HiMain2::HiMain2()
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

// Destructor
inline
HiMain2::~HiMain2()
{}

inline int&
HiMain2::katt	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > leniKatt() ||
	j < 1 || j > lenjKatt() ) return m_dummy;

    return s_himain2->katt[j-1][i-1];
}

inline float&
HiMain2::patt	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > leniPatt() ||
	j < 1 || j > lenjPatt() ) return m_realdummy;

    return s_himain2->patt[j-1][i-1];
}

inline float&
HiMain2::vatt	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > leniVatt() ||
	j < 1 || j > lenjVatt() ) return m_realdummy;

    return s_himain2->vatt[j-1][i-1];
}

#endif
