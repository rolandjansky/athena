/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Description:
//      Class definition for LuJets, which is used
//      to modify LUJETS common.

#ifndef LuJets_h
#define LuJets_h

extern "C" { void* lujets_address_(void); }

class LuJets {
public:
    LuJets();
    ~LuJets();
    
    int&	n	(void);
    int&	k	(int i, int j);
    float&	p	(int i, int j);
    float&	v	(int i, int j);

    void	init	(void);

    // return common array lengths
    inline int	leniK() const {return s_leniK;}
    inline int	lenjK() const {return s_lenjK;}
    inline int	leniP() const {return s_leniP;}
    inline int	lenjP() const {return s_lenjP;}
    inline int	leniV() const {return s_leniV;}
    inline int	lenjV() const {return s_lenjV;}

private: 

    // Lengths of array in LuJets common
    static const int s_leniK	= 150000;
    static const int s_lenjK	= 5;
    static const int s_leniP	= 150000;
    static const int s_lenjP	= 5;
    static const int s_leniV	= 150000;
    static const int s_lenjV	= 5;

    struct LUJETS;
    friend struct LUJETS;

    struct LUJETS
    {
	int  	n;
	int  	k[s_lenjK][s_leniK];
	float  	p[s_lenjP][s_leniP];
	float  	v[s_lenjV][s_leniV];
    };

    int  m_dummy;
    float  m_realdummy;
    
    static LUJETS* s_lujets;
};

// set pointer to zero at start
LuJets::LUJETS* LuJets::s_lujets =0;

inline void
LuJets::init(void)
{ if (!s_lujets) s_lujets = static_cast<LUJETS*>(lujets_address_()); }

// Constructor
inline
LuJets::LuJets()
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

// Destructor
inline
LuJets::~LuJets()
{}

inline int&
LuJets::n (void)
{
  init();
  return s_lujets->n;
}

inline int&
LuJets::k	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > leniK() ||
	j < 1 || j > lenjK() ) return m_dummy;

    return s_lujets->k[j-1][i-1];
}

inline float&
LuJets::p	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > leniP() ||
	j < 1 || j > lenjP() ) return m_realdummy;

    return s_lujets->p[j-1][i-1];
}

inline float&
LuJets::v	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > leniV() ||
	j < 1 || j > lenjV() ) return m_realdummy;

    return s_lujets->v[j-1][i-1];
}

#endif
