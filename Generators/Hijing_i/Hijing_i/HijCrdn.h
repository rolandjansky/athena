/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef HijCrdn_h
#define HijCrdn_h

extern "C" { void* hijcrdn_address_(void); }
/**
@brief Class definition for HijCrdn, which is used
      to modify the Hijing HIJCRDN common.

*/
class HijCrdn {
public:
    HijCrdn();
    ~HijCrdn();
    
    float&  	yp	(int i, int j);
    float&  	yt	(int i, int j);
    
    void	init	(void);

    // return common array lengths
    inline int	lenI() const {return s_lenI;}
    inline int	lenJ() const {return s_lenJ;}

private: 

    // Lengths of array in HiMain2 common
    static const int s_lenI	= 3;
    static const int s_lenJ	= 300;

    struct HIJCRDN;
    friend struct HIJCRDN;

    struct HIJCRDN
    {
	float  	yp	[s_lenJ][s_lenI];
	float  	yt	[s_lenJ][s_lenI];
    };

    int  m_dummy;
    float  m_realdummy;

    static HIJCRDN* s_hijcrdn;
};

// set pointer to zero at start
HijCrdn::HIJCRDN* HijCrdn::s_hijcrdn =0;

inline void
HijCrdn::init(void)
{ if (!s_hijcrdn) s_hijcrdn = static_cast<HIJCRDN*>(hijcrdn_address_()); }

// Constructor
inline
HijCrdn::HijCrdn()
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

// Destructor
inline
HijCrdn::~HijCrdn()
{}

inline float&
HijCrdn::yp	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > lenI() ||
	j < 1 || j > lenJ() ) return m_realdummy;

    return s_hijcrdn->yp[j-1][i-1];
}

inline float&
HijCrdn::yt	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > lenI() ||
	j < 1 || j > lenJ() ) return m_realdummy;

    return s_hijcrdn->yt[j-1][i-1];
}

#endif
