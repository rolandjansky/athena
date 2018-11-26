/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef Atlas_HEPEVT_h
#define Atlas_HEPEVT_h

#include "CLIDSvc/CLASS_DEF.h"

extern "C" {
  void* atlas_hepevt_address_(void);
}
/*
          // HEPEVT standard event common
          PARAMETER (NMXHEP=10000)
          COMMON/HEPEVT/NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
        & JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)
*/

class Atlas_HEPEVT {
public:
  Atlas_HEPEVT();
  ~Atlas_HEPEVT();

  void fill();
  void spill();
  int& nevhep();
  int& nhep();
  int& isthep(int nh);
  int& idhep(int nh);
  int& jmohep(int i, int nh);
  int& jdahep(int i, int nh);
  double& phep(int j, int nh);
  double& vhep(int k, int nh);

  inline void init(); // inlined for speed of access (small function)

  // return common array lengths
  int depthRel() const {return s_depthRel;}
  int depthPhep() const {return s_depthPhep;}
  int depthVhep() const {return s_depthVhep;}
  int lenNmxhep() const {return s_lenNmxhep;}

private: 
  // Lengths of array in HEPEVT common
  static const int s_depthRel = 2 ; // Relatives -Mother & Daughter
  static const int s_depthPhep = 5 ;
  static const int s_depthVhep = 4 ;
  static const int s_lenNmxhep = 10000 ;

  struct HEPEVT;
  friend struct HEPEVT;

  struct HEPEVT {
    int nevhep;
    int nhep;
    int isthep[s_lenNmxhep];
    int idhep[s_lenNmxhep];
    int jmohep[s_lenNmxhep][s_depthRel];
    int jdahep[s_lenNmxhep][s_depthRel];
    double phep[s_lenNmxhep][s_depthPhep];
    double vhep[s_lenNmxhep][s_depthVhep];
  };

  int m_dummy;
  double m_realdummy;

  static HEPEVT* s_atlas_HEPEVT;

  HEPEVT m_atlas_HEPEVT;

};

// set pointer to zero at start
Atlas_HEPEVT::HEPEVT* Atlas_HEPEVT::s_atlas_HEPEVT =0;

inline void
Atlas_HEPEVT::init(void)
{ if (!s_atlas_HEPEVT) s_atlas_HEPEVT = static_cast<HEPEVT*>(atlas_hepevt_address_()); }

// Constructor
Atlas_HEPEVT::Atlas_HEPEVT() 
{
  m_atlas_HEPEVT.nevhep = 0;
  m_atlas_HEPEVT.nhep = 0;

  for(int i=0 ; i< s_lenNmxhep; i++ ){
    m_atlas_HEPEVT.idhep[i] = 0;
    m_atlas_HEPEVT.isthep[i] = 0;
  }

  m_dummy = 0;
  m_realdummy = 0.;
}

// Destructor
Atlas_HEPEVT::~Atlas_HEPEVT() { }

inline void Atlas_HEPEVT::fill() 
{ 
  init(); // check COMMON is initialized
  m_atlas_HEPEVT= *(s_atlas_HEPEVT);
  return;
}

void Atlas_HEPEVT::spill() 
{ 
  s_atlas_HEPEVT =0; //re-init the pointer
  init(); // check COMMON is initialized
  *(s_atlas_HEPEVT)=m_atlas_HEPEVT;
  return;
}

inline int&
Atlas_HEPEVT::nevhep     (void)
{
    init(); // check COMMON is initialized
    return s_atlas_HEPEVT->nevhep;
}

inline int&
Atlas_HEPEVT::nhep     (void)
{
    init(); // check COMMON is initialized
    return s_atlas_HEPEVT->nhep;
}

inline int&
Atlas_HEPEVT::isthep     (int i)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > lenNmxhep() ) return m_dummy;
    return s_atlas_HEPEVT->isthep[i-1];
}

inline int&
Atlas_HEPEVT::idhep     (int i)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > lenNmxhep() ) return m_dummy;
    return s_atlas_HEPEVT->idhep[i-1];
}


inline int& 
Atlas_HEPEVT::jmohep(int i, int nh) {
  init(); // check COMMON is initialized
  
  if( nh < 1 || nh > lenNmxhep() ||
      i  < 1 || i  > depthRel())
    {
      m_dummy = -999;
      return m_dummy;
    }

  return s_atlas_HEPEVT->jmohep[nh-1][i-1];
}

inline int& 
Atlas_HEPEVT::jdahep(int i, int nh) {
  init(); // check COMMON is initialized
  
  if( nh < 1 || nh > lenNmxhep() ||
      i  < 1 || i  > depthRel())
    {
      m_dummy = -999;
      return m_dummy;
    }

  return s_atlas_HEPEVT->jdahep[nh-1][i-1];
}


inline double& 
Atlas_HEPEVT::phep(int j, int nh) {
  init(); // check COMMON is initialized
  
  if( nh < 1 || nh > lenNmxhep() ||
      j  < 1 || j  > depthPhep())
    {
      m_realdummy = -999.;
      return m_realdummy;
    }

  return s_atlas_HEPEVT->phep[nh-1][j-1];
}

inline double& 
Atlas_HEPEVT::vhep(int k, int nh) {
  init(); // check COMMON is initialized
  
  if( nh < 1 || nh > lenNmxhep() ||
      k  < 1 || k  > depthVhep())
    {
      m_realdummy = -999.;
      return m_realdummy;
    }

  return s_atlas_HEPEVT->vhep[nh-1][k-1];
}




CLASS_DEF( Atlas_HEPEVT, 6166, 1)

//#include "Tauola_i/Atlas_HEPEVT.icc"

#endif

