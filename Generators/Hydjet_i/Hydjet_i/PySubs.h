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
    inline int lenMsub() const {return s_lenMsub;}
    inline int leniKfin() const {return s_leniKfin;}
    inline int lenjKfin() const {return s_lenjKfin;}
    inline int lenCkin() const {return s_lenCkin;}

private: 

    // Lengths of the COMMONS
    static const int s_lenMsub = 500;
    static const int s_leniKfin = 2;
    static const int s_lenjKfin = 81;
    static const int s_lenCkin = 200;

    struct PYSUBS;
    friend struct PYSUBS;

    struct PYSUBS
    {
	int 	msel;
	int 	mselpd;
	int 	msub[s_lenMsub];
        int 	kfin[s_lenjKfin][s_leniKfin];
	double	ckin[s_lenCkin];
    };

    int m_dummy;
    double m_realdummy;
    static PYSUBS* s_pysubs;
};

// set pointer to zero at start
PySubs::PYSUBS* PySubs::s_pysubs =0;

inline void
PySubs::init(void)
{ if (!s_pysubs) s_pysubs = static_cast<PYSUBS*>(pysubs_address_()); }

inline 
PySubs::PySubs() 
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

inline 
PySubs::~PySubs() 
{}

inline int&
PySubs::msel   (void)
{
  init();
  return s_pysubs->msel;
}

inline int&
PySubs::mselpd   (void)
{
  init();
  return s_pysubs->mselpd;
}

inline int&
PySubs::msub	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenMsub()) return m_dummy;
    return s_pysubs->msub[n-1];
}

inline int&
PySubs::kfin   (int i, int j)
{
  init(); // check COMMON is initialized
  if( i < 1 || i > leniKfin() ||
      j < 1 || j > lenjKfin() ) return m_dummy;

  return s_pysubs->kfin[j-1][i-1];
}

inline double&
PySubs::ckin	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenCkin()) return m_realdummy;
    return s_pysubs->ckin[n-1];
}


#endif
