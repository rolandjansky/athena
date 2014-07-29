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
    inline int	leniK() const {return _leniK;}
    inline int	lenjK() const {return _lenjK;}
    inline int	leniP() const {return _leniP;}
    inline int	lenjP() const {return _lenjP;}
    inline int	leniV() const {return _leniV;}
    inline int	lenjV() const {return _lenjV;}

private: 

    // Lengths of array in LuJets common
    static const int _leniK	= 150000;
    static const int _lenjK	= 5;
    static const int _leniP	= 150000;
    static const int _lenjP	= 5;
    static const int _leniV	= 150000;
    static const int _lenjV	= 5;

    struct LUJETS;
    friend struct LUJETS;

    struct LUJETS
    {
	int  	n;
	int  	k[_lenjK][_leniK];
	float  	p[_lenjP][_leniP];
	float  	v[_lenjV][_leniV];
    };

    int  _dummy;
    float  _realdummy;
    
    static LUJETS* _lujets;
};

// set pointer to zero at start
LuJets::LUJETS* LuJets::_lujets =0;

inline void
LuJets::init(void)
{ if (!_lujets) _lujets = static_cast<LUJETS*>(lujets_address_()); }

// Constructor
inline
LuJets::LuJets()
    : _dummy		(-999),
      _realdummy	(-999.)
{}

// Destructor
inline
LuJets::~LuJets()
{}

inline int&
LuJets::n (void)
{
  init();
  return _lujets->n;
}

inline int&
LuJets::k	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > leniK() ||
	j < 1 || j > lenjK() ) return _dummy;

    return _lujets->k[j-1][i-1];
}

inline float&
LuJets::p	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > leniP() ||
	j < 1 || j > lenjP() ) return _realdummy;

    return _lujets->p[j-1][i-1];
}

inline float&
LuJets::v	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > leniV() ||
	j < 1 || j > lenjV() ) return _realdummy;

    return _lujets->v[j-1][i-1];
}

#endif
