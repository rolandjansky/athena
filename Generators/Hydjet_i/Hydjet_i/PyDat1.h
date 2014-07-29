/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Description:
//      Class definition for PyDat1 which is used
//      to modify PYDAT1 common.

#ifndef PyDat1_h
#define PyDat1_h

extern "C" { void* pydat1_address_(void); }

class PyDat1 {
public:
    PyDat1();
    ~PyDat1();
    
    int&	mstu	(int n);
    double& 	paru	(int n);
    int&	mstj	(int n);
    double& 	parj	(int n);
    
    void 	init	(void);

    // return common array lengths
    inline int lenMstu() const {return _lenMstu;}
    inline int lenParu() const {return _lenParu;}
    inline int lenMstj() const {return _lenMstj;}
    inline int lenParj() const {return _lenParj;}

private: 

    // Lengths of the COMMONS
    static const int _lenMstu = 200;
    static const int _lenParu = 200;
    static const int _lenMstj = 200;
    static const int _lenParj = 200;

    struct PYDAT1;
    friend struct PYDAT1;

    struct PYDAT1
    {
	int 	mstu[_lenMstu];
	double	paru[_lenParu];
	int 	mstj[_lenMstj];
	double	parj[_lenParj];
    };

    int _dummy;
    double _realdummy;
    static PYDAT1* _pydat1;
};

// set pointer to zero at start
PyDat1::PYDAT1* PyDat1::_pydat1 =0;

inline void
PyDat1::init(void)
{ if (!_pydat1) _pydat1 = static_cast<PYDAT1*>(pydat1_address_()); }

inline 
PyDat1::PyDat1() 
    : _dummy		(-999),
      _realdummy	(-999.)
{}

inline 
PyDat1::~PyDat1() 
{}

inline int&
PyDat1::mstu	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenMstu()) return _dummy;
    return _pydat1->mstu[n-1];
}

inline double&
PyDat1::paru	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenParu()) return _realdummy;
    return _pydat1->paru[n-1];
}

inline int&
PyDat1::mstj	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenMstj()) return _dummy;
    return _pydat1->mstj[n-1];
}

inline double&
PyDat1::parj	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenParj()) return _realdummy;
    return _pydat1->parj[n-1];
}

#endif
