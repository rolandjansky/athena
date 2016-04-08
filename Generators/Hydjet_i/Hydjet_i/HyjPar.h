/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Description:
//      Class definition for HyjPar, which is used
//      to modify HYJPAR common.

#ifndef HyjPar_h
#define HyjPar_h

extern "C" { void* hyjpar_address_(void); }

class HyjPar {
public:
    HyjPar();
    ~HyjPar();
    
    double&    	ptmin	(void);
    double&    	sigin	(void);
    double&    	sigjet	(void);
    int&    	nhsel	(void);
    int&    	ishad	(void);
    int&    	njet	(void);

    void	init	(void);

private: 

    struct HYJPAR;
    friend struct HYJPAR;

    struct HYJPAR
    {
      double      ptmin;
      double      sigin;
      double      sigjet;
      int         nhsel;
      int         ishad;
      int         njet;
    };

    static HYJPAR* s_hyjpar;
};

// set pointer to zero at start
HyjPar::HYJPAR* HyjPar::s_hyjpar =0;

inline void
HyjPar::init(void)
{ if (!s_hyjpar) s_hyjpar = static_cast<HYJPAR*>(hyjpar_address_()); }

// Constructor
inline
HyjPar::HyjPar()
{}

// Destructor
inline
HyjPar::~HyjPar()
{}

inline double&
HyjPar::ptmin	(void)
{
    init();
    return s_hyjpar->ptmin;
}

inline double&
HyjPar::sigin	(void)
{
    init();
    return s_hyjpar->sigin;
}

inline double&
HyjPar::sigjet	(void)
{
    init();
    return s_hyjpar->sigjet;
}

inline int&
HyjPar::nhsel	(void)
{
    init();
    return s_hyjpar->nhsel;
}

inline int&
HyjPar::ishad	(void)
{
    init();
    return s_hyjpar->ishad;
}

inline int&
HyjPar::njet	(void)
{
    init();
    return s_hyjpar->njet;
}

#endif
