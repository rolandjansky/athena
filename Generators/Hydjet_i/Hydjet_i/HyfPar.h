/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Description:
//      Class definition for HyfPar, which is used
//      to modify HYFPAR common.

#ifndef HyfPar_h
#define HyfPar_h

extern "C" { void* hyfpar_address_(void); }

class HyfPar {
public:
    HyfPar();
    ~HyfPar();
    
    double&    	bgen	(void);
    double&    	nbcol	(void);
    double&    	npart	(void);
    int&    	npyt	(void);
    int&    	nhyd	(void);

    void	init	(void);

private: 

    struct HYFPAR;
    friend struct HYFPAR;

    struct HYFPAR
    {
      double  	bgen;
      double  	nbcol;
      double   	npart;
      int    	npyt;
      int    	nhyd;
    };

    static HYFPAR* s_hyfpar;
};

// set pointer to zero at start
HyfPar::HYFPAR* HyfPar::s_hyfpar =0;

inline void
HyfPar::init(void)
{ if (!s_hyfpar) s_hyfpar = static_cast<HYFPAR*>(hyfpar_address_()); }

// Constructor
inline
HyfPar::HyfPar()
{}

// Destructor
inline
HyfPar::~HyfPar()
{}

inline  double&
HyfPar::bgen	(void)
{
    init();
    return s_hyfpar->bgen;
}

inline double&
HyfPar::nbcol	(void)
{
    init();
    return s_hyfpar->nbcol;
}

inline double&
HyfPar::npart	(void)
{
    init();
    return s_hyfpar->npart;
}

inline int&
HyfPar::npyt	(void)
{
    init();
    return s_hyfpar->npyt;
}

inline int&
HyfPar::nhyd	(void)
{
    init();
    return s_hyfpar->nhyd;
}

#endif
