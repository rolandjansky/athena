//--------------------------------------------------------------------------
// File and Version Information:
//      Atlas_HEPEVT_modified.cxx
//
// Description:
//
//      Needed for interface of Superchic_i  to  Superchic3.03 generator
//      to store the umodified HEPEVT common.
//
// Copied directly from the Tauolo_i and modified a bit
// Author List:
// Borut Paul Kersevan (BPK), June 2003
//
// Copyright Information:
//
// HEPEVT standard event common
//      PARAMETER (NMXHEP=10000)
//      COMMON/HEPEVT/NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
//     & JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)
//------------------------------------------------------------------------
#include "Superchic_i/Atlas_HEPEVT_modified.h"

// set pointer to zero at start
Atlas_HEPEVT_modified::HEPEVT* Atlas_HEPEVT_modified::s_atlas_HEPEVT =0;

// Constructor
Atlas_HEPEVT_modified::Atlas_HEPEVT_modified()
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
Atlas_HEPEVT_modified::~Atlas_HEPEVT_modified()
{
}


void Atlas_HEPEVT_modified::fill()
{
  init(); // check COMMON is initialized
  m_atlas_HEPEVT= *(s_atlas_HEPEVT);
  return;
}
void Atlas_HEPEVT_modified::spill()
{
  s_atlas_HEPEVT =0; //re-init the pointer
  init(); // check COMMON is initialized
  *(s_atlas_HEPEVT)=m_atlas_HEPEVT;
  return;
}

int& Atlas_HEPEVT_modified::nevhep() {
  init(); // check COMMON is initialized

  return s_atlas_HEPEVT->nevhep;
}

int& Atlas_HEPEVT_modified::nhep() {
  init(); // check COMMON is initialized

  return s_atlas_HEPEVT->nhep;
}

int& Atlas_HEPEVT_modified::isthep(int nh) {
  init(); // check COMMON is initialized

  if( nh < 1 || nh > lenNmxhep())
  {
    m_dummy = -999;
    return m_dummy;
  }

  return s_atlas_HEPEVT->isthep[nh-1];
}

int& Atlas_HEPEVT_modified::idhep(int nh) {
  init(); // check COMMON is initialized

  if( nh < 1 || nh > lenNmxhep())
  {
    m_dummy = -999;
    return m_dummy;
  }

  return s_atlas_HEPEVT->idhep[nh-1];
}

int& Atlas_HEPEVT_modified::jmohep(int i, int nh) {
  init(); // check COMMON is initialized

  if( nh < 1 || nh > lenNmxhep() ||
  i  < 1 || i  > depthRel())
  {
    m_dummy = -999;
    return m_dummy;
  }

  return s_atlas_HEPEVT->jmohep[nh-1][i-1];
}

int& Atlas_HEPEVT_modified::jdahep(int i, int nh) {
  init(); // check COMMON is initialized

  if( nh < 1 || nh > lenNmxhep() ||
  i  < 1 || i  > depthRel())
  {
    m_dummy = -999;
    return m_dummy;
  }

  return s_atlas_HEPEVT->jdahep[nh-1][i-1];
}

double& Atlas_HEPEVT_modified::phep(int j, int nh) {
  init(); // check COMMON is initialized

  if( nh < 1 || nh > lenNmxhep() ||
  j  < 1 || j  > depthPhep())
  {
    m_realdummy = -999.;
    return m_realdummy;
  }

  return s_atlas_HEPEVT->phep[nh-1][j-1];
}

double& Atlas_HEPEVT_modified::vhep(int k, int nh) {
  init(); // check COMMON is initialized

  if( nh < 1 || nh > lenNmxhep() ||
  k  < 1 || k  > depthVhep())
  {
    m_realdummy = -999.;
    return m_realdummy;
  }

  return s_atlas_HEPEVT->vhep[nh-1][k-1];
}
