/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Description:
//      Class definition for HyJets, which is used
//      to modify HYJETS common.

#ifndef HyJets_h
#define HyJets_h

extern "C" { void* hyjets_address_(void); }

class HyJets {
public:
    HyJets();
    ~HyJets();
    
    int&	nhj	(void);
    int&	nhp	(void);
    int&	khj	(int i, int j);
    double&	phj	(int i, int j);
    double&	vhj	(int i, int j);

    void	init	(void);

    // return common array lengths
    inline int	leniKhj() const {return _leniKhj;}
    inline int	lenjKhj() const {return _lenjKhj;}
    inline int	leniPhj() const {return _leniPhj;}
    inline int	lenjPhj() const {return _lenjPhj;}
    inline int	leniVhj() const {return _leniVhj;}
    inline int	lenjVhj() const {return _lenjVhj;}

private: 

    // Lengths of array in HyJets common
    static const int _leniKhj	= 150000;
    static const int _lenjKhj	= 5;
    static const int _leniPhj	= 150000;
    static const int _lenjPhj	= 5;
    static const int _leniVhj	= 150000;
    static const int _lenjVhj	= 5;

    struct HYJETS;
    friend struct HYJETS;

    struct HYJETS
    {
	int  	nhj;
	int  	nhp;
	int  	khj[_lenjKhj][_leniKhj];
	double  phj[_lenjPhj][_leniPhj];
	double  vhj[_lenjVhj][_leniVhj];
    };

    int  _dummy;
    double  _realdummy;
    
    static HYJETS* _hyjets;
};

// set pointer to zero at start
HyJets::HYJETS* HyJets::_hyjets =0;

inline void
HyJets::init(void)
{ if (!_hyjets) _hyjets = static_cast<HYJETS*>(hyjets_address_()); }

// Constructor
inline
HyJets::HyJets()
    : _dummy		(-999),
      _realdummy	(-999.)
{}

// Destructor
inline
HyJets::~HyJets()
{}

inline int&
HyJets::nhj (void)
{
  init();
  return _hyjets->nhj;
}

inline int&
HyJets::nhp (void)
{
  init();
  return _hyjets->nhp;
}

inline int&
HyJets::khj	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > leniKhj() ||
	j < 1 || j > lenjKhj() ) return _dummy;

    return _hyjets->khj[j-1][i-1];
}

inline double&
HyJets::phj	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > leniPhj() ||
	j < 1 || j > lenjPhj() ) return _realdummy;

    return _hyjets->phj[j-1][i-1];
}

inline double&
HyJets::vhj	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > leniVhj() ||
	j < 1 || j > lenjVhj() ) return _realdummy;

    return _hyjets->vhj[j-1][i-1];
}

#endif
