/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Description:
//      Class definition for HyiPar which is used
//      to modify HYIPAR common.

#ifndef HyiPar_h
#define HyiPar_h

extern "C" { void* hyipar_address_(void); }

class HyiPar {
public:
    HyiPar();
    ~HyiPar();
    
    double&	bminh	(void);
    double& 	bmaxh	(void);
    double&	AW	(void);
    double& 	RA	(void);
    double& 	npar0	(void);
    double& 	nbco0	(void);
    double& 	Apb	(void);
    double& 	Rpb	(void);
    int& 	np	(void);
    int& 	init	(void);
    int& 	ipr	(void);

    void 	initf	(void);

private: 

    struct HYIPAR;
    friend struct HYIPAR;

    struct HYIPAR
    {
      double      bminh;
      double      bmaxh;
      double      AW;
      double      RA;
      double      npar0;
      double      nbco0;
      double      Apb;
      double      Rpb;
      int         np;
      int         init;
      int         ipr;
    };

    int m_dummy;
    float m_realdummy;
    static HYIPAR* s_hyipar;
};

// set pointer to zero at start
HyiPar::HYIPAR* HyiPar::s_hyipar =0;

inline void
HyiPar::initf(void)
{ if (!s_hyipar) s_hyipar = static_cast<HYIPAR*>(hyipar_address_()); }

inline 
HyiPar::HyiPar() 
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

inline 
HyiPar::~HyiPar() 
{}

inline double&
HyiPar::bminh   (void)
{
  initf();
  return s_hyipar->bminh;
}

inline double&
HyiPar::bmaxh   (void)
{
  initf();
  return s_hyipar->bmaxh;
}

inline double&
HyiPar::AW   (void)
{
  initf();
  return s_hyipar->AW;
}

inline double&
HyiPar::RA   (void)
{
  initf();
  return s_hyipar->RA;
}

inline double&
HyiPar::npar0   (void)
{
  initf();
  return s_hyipar->npar0;
}

inline double&
HyiPar::nbco0   (void)
{
  initf();
  return s_hyipar->nbco0;
}

inline double&
HyiPar::Apb   (void)
{
  initf();
  return s_hyipar->Apb;
}

inline double&
HyiPar::Rpb   (void)
{
  initf();
  return s_hyipar->Rpb;
}

inline int&
HyiPar::np   (void)
{
  initf();
  return s_hyipar->np;
}

inline int&
HyiPar::init   (void)
{
  initf();
  return s_hyipar->init;
}

inline int&
HyiPar::ipr   (void)
{
  initf();
  return s_hyipar->ipr;
}
#endif
