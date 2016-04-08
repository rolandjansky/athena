/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Description:
//      Class definition for PyqPar, which is used
//      to modify PYQPAR common.

#ifndef PyqPar_h
#define PyqPar_h

extern "C" { void* pyqpar_address_(void); }

class PyqPar {
public:
    PyqPar();
    ~PyqPar();
    
    double&    	t0	(void);
    double&    	tau0	(void);
    int&    	nf	(void);
    int&    	ienglu	(void);
    int&    	ianglu	(void);

    void	init	(void);

private: 

    struct PYQPAR;
    friend struct PYQPAR;

    struct PYQPAR
    {
      double     t0;
      double     tau0;
      int        nf;
      int        ienglu;
      int        ianglu;
    };

    static PYQPAR* s_pyqpar;
};

// set pointer to zero at start
PyqPar::PYQPAR* PyqPar::s_pyqpar =0;

inline void
PyqPar::init(void)
{ if (!s_pyqpar) s_pyqpar = static_cast<PYQPAR*>(pyqpar_address_()); }

// Constructor
inline
PyqPar::PyqPar()
{}

// Destructor
inline
PyqPar::~PyqPar()
{}

inline double&
PyqPar::t0	(void)
{
    init();
    return s_pyqpar->t0;
}

inline double&
PyqPar::tau0	(void)
{
    init();
    return s_pyqpar->tau0;
}

inline int&
PyqPar::nf	(void)
{
    init();
    return s_pyqpar->nf;
}

inline int&
PyqPar::ienglu	(void)
{
    init();
    return s_pyqpar->ienglu;
}

inline int&
PyqPar::ianglu	(void)
{
    init();
    return s_pyqpar->ianglu;
}

#endif
