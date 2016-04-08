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
    inline int lenMstu() const {return s_lenMstu;}
    inline int lenParu() const {return s_lenParu;}
    inline int lenMstj() const {return s_lenMstj;}
    inline int lenParj() const {return s_lenParj;}

private: 

    // Lengths of the COMMONS
    static const int s_lenMstu = 200;
    static const int s_lenParu = 200;
    static const int s_lenMstj = 200;
    static const int s_lenParj = 200;

    struct PYDAT1;
    friend struct PYDAT1;

    struct PYDAT1
    {
	int 	mstu[s_lenMstu];
	double	paru[s_lenParu];
	int 	mstj[s_lenMstj];
	double	parj[s_lenParj];
    };

    int m_dummy;
    double m_realdummy;
    static PYDAT1* s_pydat1;
};

// set pointer to zero at start
PyDat1::PYDAT1* PyDat1::s_pydat1 =0;

inline void
PyDat1::init(void)
{ if (!s_pydat1) s_pydat1 = static_cast<PYDAT1*>(pydat1_address_()); }

inline 
PyDat1::PyDat1() 
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

inline 
PyDat1::~PyDat1() 
{}

inline int&
PyDat1::mstu	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenMstu()) return m_dummy;
    return s_pydat1->mstu[n-1];
}

inline double&
PyDat1::paru	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenParu()) return m_realdummy;
    return s_pydat1->paru[n-1];
}

inline int&
PyDat1::mstj	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenMstj()) return m_dummy;
    return s_pydat1->mstj[n-1];
}

inline double&
PyDat1::parj	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenParj()) return m_realdummy;
    return s_pydat1->parj[n-1];
}

#endif
