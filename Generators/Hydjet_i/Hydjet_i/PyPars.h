/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Description:
//      Class definition for PyPars which is used
//      to modify PYPARS common.

#ifndef PyPars_h
#define PyPars_h

extern "C" { void* pypars_address_(void); }

class PyPars {
public:
    PyPars();
    ~PyPars();
    
    int&	mstp	(int n);
    double& 	parp	(int n);
    int&	msti	(int n);
    double& 	pari	(int n);
    
    void 	init	(void);

    // return common array lengths
    inline int lenMstp() const {return s_lenMstp;}
    inline int lenParp() const {return s_lenParp;}
    inline int lenMsti() const {return s_lenMsti;}
    inline int lenPari() const {return s_lenPari;}

private: 

    // Lengths of the COMMONS
    static const int s_lenMstp = 200;
    static const int s_lenParp = 200;
    static const int s_lenMsti = 200;
    static const int s_lenPari = 200;

    struct PYPARS;
    friend struct PYPARS;

    struct PYPARS
    {
	int 	mstp[s_lenMstp];
	double	parp[s_lenParp];
	int 	msti[s_lenMsti];
	double	pari[s_lenPari];
    };

    int m_dummy;
    double m_realdummy;
    static PYPARS* s_pypars;
};

// set pointer to zero at start
PyPars::PYPARS* PyPars::s_pypars =0;

inline void
PyPars::init(void)
{ if (!s_pypars) s_pypars = static_cast<PYPARS*>(pypars_address_()); }

inline 
PyPars::PyPars() 
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

inline 
PyPars::~PyPars() 
{}

inline int&
PyPars::mstp	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenMstp()) return m_dummy;
    return s_pypars->mstp[n-1];
}

inline double&
PyPars::parp	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenParp()) return m_realdummy;
    return s_pypars->parp[n-1];
}

inline int&
PyPars::msti	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenMsti()) return m_dummy;
    return s_pypars->msti[n-1];
}

inline double&
PyPars::pari	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenPari()) return m_realdummy;
    return s_pypars->pari[n-1];
}

#endif
