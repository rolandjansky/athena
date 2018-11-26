/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef Hbt_h
#define Hbt_h

extern "C" { void* hbt_address_(void); }
/**
@brief Class definition for Hbt, which is used
      to modify the HBT common.

*/
class Hbt {
public:
    Hbt();
    ~Hbt();
    
    int&  	lblast	(int j);
    float&  	xlast	(int i, int j);
    float&  	plast	(int i, int j);
    int&  	nlast	(void);
    
    void	init	(void);

    // return common array lengths
    inline int	lenI() const {return s_lenI;}
    inline int	lenJ() const {return s_lenJ;}

private: 

    // Lengths of array in Hijdat common
    static const int s_lenI	= 4;
    static const int s_lenJ	= 150001;

    struct HBT;
    friend struct HBT;

    struct HBT
    {
	int  	lblast	[s_lenJ];
	float  	xlast	[s_lenJ][s_lenI];
	float  	plast	[s_lenJ][s_lenI];
	int  	nlast;
    };

    int  m_dummy;
    float  m_realdummy;

    static HBT* s_hbt;
};

// set pointer to zero at start
Hbt::HBT* Hbt::s_hbt =0;

inline void
Hbt::init(void)
{ if (!s_hbt) s_hbt = static_cast<HBT*>(hbt_address_()); }

// Constructor
inline
Hbt::Hbt()
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

// Destructor
inline
Hbt::~Hbt()
{}

inline float&
Hbt::xlast	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > lenI() ||
	j < 1 || j > lenJ() ) return m_realdummy;

    return s_hbt->xlast[j-1][i-1];
}

inline float&
Hbt::plast      (int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > lenI() ||
        j < 1 || j > lenJ() ) return m_realdummy;

    return s_hbt->plast[j-1][i-1];
}

inline int&
Hbt::lblast	(int i)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > lenJ() ) return m_dummy;

    return s_hbt->lblast[i-1];
}

inline int&
Hbt::nlast     (void)
{
    init(); // check COMMON is initialized
    return s_hbt->nlast;
}

#endif

