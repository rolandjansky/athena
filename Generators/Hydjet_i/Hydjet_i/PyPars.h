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
    inline int lenMstp() const {return _lenMstp;}
    inline int lenParp() const {return _lenParp;}
    inline int lenMsti() const {return _lenMsti;}
    inline int lenPari() const {return _lenPari;}

private: 

    // Lengths of the COMMONS
    static const int _lenMstp = 200;
    static const int _lenParp = 200;
    static const int _lenMsti = 200;
    static const int _lenPari = 200;

    struct PYPARS;
    friend struct PYPARS;

    struct PYPARS
    {
	int 	mstp[_lenMstp];
	double	parp[_lenParp];
	int 	msti[_lenMsti];
	double	pari[_lenPari];
    };

    int _dummy;
    double _realdummy;
    static PYPARS* _pypars;
};

// set pointer to zero at start
PyPars::PYPARS* PyPars::_pypars =0;

inline void
PyPars::init(void)
{ if (!_pypars) _pypars = static_cast<PYPARS*>(pypars_address_()); }

inline 
PyPars::PyPars() 
    : _dummy		(-999),
      _realdummy	(-999.)
{}

inline 
PyPars::~PyPars() 
{}

inline int&
PyPars::mstp	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenMstp()) return _dummy;
    return _pypars->mstp[n-1];
}

inline double&
PyPars::parp	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenParp()) return _realdummy;
    return _pypars->parp[n-1];
}

inline int&
PyPars::msti	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenMsti()) return _dummy;
    return _pypars->msti[n-1];
}

inline double&
PyPars::pari	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenPari()) return _realdummy;
    return _pypars->pari[n-1];
}

#endif
