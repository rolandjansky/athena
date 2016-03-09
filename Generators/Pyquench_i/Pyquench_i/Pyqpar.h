/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PYQUENCH_I_PYQPAR_H
#define PYQUENCH_I_PYQPAR_H

/* Pyquen common block definition
 /pyqpar/ T0u,tau0u,nfu,ienglu,ianglu
*/

extern "C" { void* pyqpar_address_(void); }
/**
@class Pyqpar

@brief       Class definition for Pyqpar, which is used
      to modify the PYQUEN PYQPAR common.
*/

class Pyqpar {
public:
    Pyqpar();
    ~Pyqpar();

    double& T0u (void);
    double& tau0u (void);
    int& nfu (void);
    int& ienglu (void);
    int& ianglu (void);
    
private: 

    struct PYQPAR;
    friend struct PYQPAR;

    struct PYQPAR
    {
      double T0u;
      double tau0u;
      int nfu;
      int ienglu;
      int ianglu;

    };
    void init(void);
    static PYQPAR* s_pyqpar;
};

// set pointer to zero at start
Pyqpar::PYQPAR* Pyqpar::s_pyqpar =0;

inline void
Pyqpar::init(void)
{ if (!s_pyqpar) s_pyqpar = static_cast<PYQPAR*>(pyqpar_address_()); }

// Constructor
inline
Pyqpar::Pyqpar()
{}

// Destructor
inline
Pyqpar::~Pyqpar()
{}

inline double&
Pyqpar::T0u	(void)
{
    init();
    return s_pyqpar->T0u;
}

inline double&
Pyqpar::tau0u	(void)
{
    init();
    return s_pyqpar->tau0u;
}

inline int&
Pyqpar::nfu	(void)
{
    init();
    return s_pyqpar->nfu;
}

inline int&
Pyqpar::ienglu	(void)
{
    init();
    return s_pyqpar->ienglu;
}

inline int&
Pyqpar::ianglu	(void)
{
    init();
    return s_pyqpar->ianglu;
}




#endif
