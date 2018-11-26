/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef HiStrng_h
#define HiStrng_h

extern "C" { void* histrng_address_(void); }
/**
@class HiStrng
@brief       Class definition for HiStrng, which is used
      to modify the Hijing HISTRNG common.
*/
class HiStrng {
public:
    HiStrng();
    ~HiStrng();
    
    int&    	nfp	(int i, int j);
    float&  	pp	(int i, int j);
    int&    	nft	(int i, int j);
    float&  	pt	(int i, int j);
    
    void	init	(void);

    // return common array lengths
    inline int	lenI() const {return s_lenI;}
    inline int	lenJ() const {return s_lenJ;}

private: 

    // Lengths of array in HiMain2 common
    static const int s_lenI	= 300;
    static const int s_lenJ	= 15;

    struct HISTRNG;
    friend struct HISTRNG;

    struct HISTRNG
    {
	int	nfp	[s_lenJ][s_lenI];
	float  	pp	[s_lenJ][s_lenI];
	int	nft	[s_lenJ][s_lenI];
	float  	pt	[s_lenJ][s_lenI];
    };

    int  m_dummy;
    float  m_realdummy;

    static HISTRNG* s_histrng;
};

// set pointer to zero at start
HiStrng::HISTRNG* HiStrng::s_histrng =0;

inline void
HiStrng::init(void)
{ if (!s_histrng) s_histrng = static_cast<HISTRNG*>(histrng_address_()); }

// Constructor
inline
HiStrng::HiStrng()
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

// Destructor
inline
HiStrng::~HiStrng()
{}

inline int&
HiStrng::nfp	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > lenI() ||
	j < 1 || j > lenJ() ) return m_dummy;

    return s_histrng->nfp[j-1][i-1];
}

inline float&
HiStrng::pp	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > lenI() ||
	j < 1 || j > lenJ() ) return m_realdummy;

    return s_histrng->pp[j-1][i-1];
}

inline int&
HiStrng::nft	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > lenI() ||
	j < 1 || j > lenJ() ) return m_dummy;

    return s_histrng->nft[j-1][i-1];
}

inline float&
HiStrng::pt	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > lenI() ||
	j < 1 || j > lenJ() ) return m_realdummy;

    return s_histrng->pt[j-1][i-1];
}

#endif
