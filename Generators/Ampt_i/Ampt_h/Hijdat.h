/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef Hijdat_h
#define Hijdat_h

extern "C" { void* hijdat_address_(void); }
/**
@brief Class definition for Hijdat, which is used
      to modify the HIJDAT common.

*/
class Hijdat {
public:
    Hijdat();
    ~Hijdat();
    
    float&  	hidat0	(int i, int j);
    float&  	hidat	(int i);
    
    void	init	(void);

    // return common array lengths
    inline int	lenI() const {return s_lenI;}
    inline int	lenJ() const {return s_lenJ;}

private: 

    // Lengths of array in Hijdat common
    static const int s_lenI	= 10;
    static const int s_lenJ	= 10;

    struct HIJDAT;
    friend struct HIJDAT;

    struct HIJDAT
    {
	float  	hidat0	[s_lenJ][s_lenI];
	float  	hidat	[s_lenI];
    };

    int  m_dummy;
    float  m_realdummy;

    static HIJDAT* s_hijdat;
};

// set pointer to zero at start
Hijdat::HIJDAT* Hijdat::s_hijdat =0;

inline void
Hijdat::init(void)
{ if (!s_hijdat) s_hijdat = static_cast<HIJDAT*>(hijdat_address_()); }

// Constructor
inline
Hijdat::Hijdat()
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

// Destructor
inline
Hijdat::~Hijdat()
{}

inline float&
Hijdat::hidat0	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > lenI() ||
	j < 1 || j > lenJ() ) return m_realdummy;

    return s_hijdat->hidat0[j-1][i-1];
}

inline float&
Hijdat::hidat	(int i)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > lenI() ) return m_realdummy;

    return s_hijdat->hidat[i-1];
}

#endif

