/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef HijJet4_h
#define HijJet4_h

extern "C" { void* hijjet4_address_(void); }

/**
@class HijJet4
@brief  Class definition for HijJet4, which is used
     to modify the Hijing HIJJET4 common.
*/

class HijJet4 {
public:
    HijJet4();
    ~HijJet4();
    
    int&	ndr	(void);
    int&    	iadr	(int i, int j);
    int&    	kfdr	(int i);
    float&  	pdr	(int i, int k);
    float&  	vdr	(int i, int k);
    
    void	init	(void);

    // return common array lengths
    inline int	lenI() const {return s_lenI;}
    inline int	lenJ() const {return s_lenJ;}
    inline int	lenK() const {return s_lenK;}

private: 

    // Lengths of array in HiMain2 common
    static const int s_lenI	= 900;
    static const int s_lenJ	= 2;
    static const int s_lenK	= 5;

    struct HIJJET4;
    friend struct HIJJET4;

    struct HIJJET4
    {
	int    	ndr;
	int	iadr	[s_lenJ][s_lenI];
	int    	kfdr	[s_lenI];
	float  	pdr	[s_lenK][s_lenI];
	float  	vdr	[s_lenK][s_lenI];
    };

    int  m_dummy;
    float  m_realdummy;

    static HIJJET4* s_hijjet4;
};

// set pointer to zero at start
HijJet4::HIJJET4* HijJet4::s_hijjet4 =0;

inline void
HijJet4::init(void)
{ if (!s_hijjet4) s_hijjet4 = static_cast<HIJJET4*>(hijjet4_address_()); }

// Constructor
inline
HijJet4::HijJet4()
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

// Destructor
inline
HijJet4::~HijJet4()
{}

inline int&
HijJet4::ndr	(void)
{
    init(); // check COMMON is initialized
    return s_hijjet4->ndr;
}

inline int&
HijJet4::iadr	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > lenI() ||
	j < 1 || j > lenJ() ) return m_dummy;

    return s_hijjet4->iadr[j-1][i-1];
}

inline int&
HijJet4::kfdr    (int i)
{
    init(); // check COMMON is initialized
    if(i < 1 || i > lenI()) return m_dummy;
    return s_hijjet4->kfdr[i-1];
}

inline float&
HijJet4::pdr	(int i, int k)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > lenI() ||
	k < 1 || k > lenK() ) return m_realdummy;

    return s_hijjet4->pdr[k-1][i-1];
}


inline float&
HijJet4::vdr	(int i, int k)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > lenI() ||
	k < 1 || k > lenK() ) return m_realdummy;

    return s_hijjet4->vdr[k-1][i-1];
}

#endif
