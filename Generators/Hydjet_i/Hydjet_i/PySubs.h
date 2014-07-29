/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Description:
//      Class definition for PySubs, which is used
//      to modify PYSUBS common.

#ifndef PySubs_h
#define PySubs_h

extern "C" { void* pysubs_address_(void); }

class PySubs {
public:
    PySubs();
    ~PySubs();
    
    int&	msel	(void);
    int&	mselpd	(void);
    int&	msub	(int n);
    int&	kfin	(int i, int j);
    double& 	ckin	(int n);
    
    void 	init	(void);

    // return common array lengths
    inline int lenMsub() const {return _lenMsub;}
    inline int leniKfin() const {return _leniKfin;}
    inline int lenjKfin() const {return _lenjKfin;}
    inline int lenCkin() const {return _lenCkin;}

private: 

    // Lengths of the COMMONS
    static const int _lenMsub = 500;
    static const int _leniKfin = 2;
    static const int _lenjKfin = 81;
    static const int _lenCkin = 200;

    struct PYSUBS;
    friend struct PYSUBS;

    struct PYSUBS
    {
	int 	msel;
	int 	mselpd;
	int 	msub[_lenMsub];
        int 	kfin[_lenjKfin][_leniKfin];
	double	ckin[_lenCkin];
    };

    int _dummy;
    double _realdummy;
    static PYSUBS* _pysubs;
};

// set pointer to zero at start
PySubs::PYSUBS* PySubs::_pysubs =0;

inline void
PySubs::init(void)
{ if (!_pysubs) _pysubs = static_cast<PYSUBS*>(pysubs_address_()); }

inline 
PySubs::PySubs() 
    : _dummy		(-999),
      _realdummy	(-999.)
{}

inline 
PySubs::~PySubs() 
{}

inline int&
PySubs::msel   (void)
{
  init();
  return _pysubs->msel;
}

inline int&
PySubs::mselpd   (void)
{
  init();
  return _pysubs->mselpd;
}

inline int&
PySubs::msub	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenMsub()) return _dummy;
    return _pysubs->msub[n-1];
}

inline int&
PySubs::kfin   (int i, int j)
{
  init(); // check COMMON is initialized
  if( i < 1 || i > leniKfin() ||
      j < 1 || j > lenjKfin() ) return _dummy;

  return _pysubs->kfin[j-1][i-1];
}

inline double&
PySubs::ckin	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenCkin()) return _realdummy;
    return _pysubs->ckin[n-1];
}


#endif
