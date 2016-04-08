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
    inline int	leniKhj() const {return s_leniKhj;}
    inline int	lenjKhj() const {return s_lenjKhj;}
    inline int	leniPhj() const {return s_leniPhj;}
    inline int	lenjPhj() const {return s_lenjPhj;}
    inline int	leniVhj() const {return s_leniVhj;}
    inline int	lenjVhj() const {return s_lenjVhj;}

private: 

    // Lengths of array in HyJets common
    static const int s_leniKhj	= 150000;
    static const int s_lenjKhj	= 5;
    static const int s_leniPhj	= 150000;
    static const int s_lenjPhj	= 5;
    static const int s_leniVhj	= 150000;
    static const int s_lenjVhj	= 5;

    struct HYJETS;
    friend struct HYJETS;

    struct HYJETS
    {
	int  	nhj;
	int  	nhp;
	int  	khj[s_lenjKhj][s_leniKhj];
	double  phj[s_lenjPhj][s_leniPhj];
	double  vhj[s_lenjVhj][s_leniVhj];
    };

    int  m_dummy;
    double  m_realdummy;
    
    static HYJETS* s_hyjets;
};

// set pointer to zero at start
HyJets::HYJETS* HyJets::s_hyjets =0;

inline void
HyJets::init(void)
{ if (!s_hyjets) s_hyjets = static_cast<HYJETS*>(hyjets_address_()); }

// Constructor
inline
HyJets::HyJets()
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

// Destructor
inline
HyJets::~HyJets()
{}

inline int&
HyJets::nhj (void)
{
  init();
  return s_hyjets->nhj;
}

inline int&
HyJets::nhp (void)
{
  init();
  return s_hyjets->nhp;
}

inline int&
HyJets::khj	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > leniKhj() ||
	j < 1 || j > lenjKhj() ) return m_dummy;

    return s_hyjets->khj[j-1][i-1];
}

inline double&
HyJets::phj	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > leniPhj() ||
	j < 1 || j > lenjPhj() ) return m_realdummy;

    return s_hyjets->phj[j-1][i-1];
}

inline double&
HyJets::vhj	(int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > leniVhj() ||
	j < 1 || j > lenjVhj() ) return m_realdummy;

    return s_hyjets->vhj[j-1][i-1];
}

#endif
