//--------------------------------------------------------------------------
// File and Version Information:
//      Atlas_HEPEVT.cxx
//
// Description:
//      Needed fot interface of Tauola/Photos to Herwig 6.5 and Pythia 6.2+
//      to store the umodified HEPEVT common.
//
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
#include "Tauola_i/Atlas_HEPEVT.h"

// set pointer to zero at start
Atlas_HEPEVT::HEPEVT* Atlas_HEPEVT::_atlas_HEPEVT =0;

// Constructor
Atlas_HEPEVT::Atlas_HEPEVT() 
{
  s_atlas_HEPEVT.nevhep = 0;
  s_atlas_HEPEVT.nhep = 0;

  for(int i=0 ; i< _lenNmxhep; i++ ){
    s_atlas_HEPEVT.idhep[i] = 0;
    s_atlas_HEPEVT.isthep[i] = 0;
  }

  _dummy = 0;
  _realdummy = 0.;
}

// Destructor
Atlas_HEPEVT::~Atlas_HEPEVT() 
{
}


void Atlas_HEPEVT::fill() 
{ 
  init(); // check COMMON is initialized
  s_atlas_HEPEVT= *(_atlas_HEPEVT);
  return;
}
void Atlas_HEPEVT::spill() 
{ 
  _atlas_HEPEVT =0; //re-init the pointer
  init(); // check COMMON is initialized
  *(_atlas_HEPEVT)=s_atlas_HEPEVT;
  return;
}

int& Atlas_HEPEVT::nevhep() {
  init(); // check COMMON is initialized
  
  return _atlas_HEPEVT->nevhep;
}

int& Atlas_HEPEVT::nhep() {
  init(); // check COMMON is initialized
  
  return _atlas_HEPEVT->nhep;
}

int& Atlas_HEPEVT::isthep(int nh) {
  init(); // check COMMON is initialized
  
  if( nh < 1 || nh > lenNmxhep()) 
    {
      _dummy = -999;
      return _dummy;
    }

  return _atlas_HEPEVT->isthep[nh-1];
}

int& Atlas_HEPEVT::idhep(int nh) {
  init(); // check COMMON is initialized
  
  if( nh < 1 || nh > lenNmxhep()) 
    {
      _dummy = -999;
      return _dummy;
    }

  return _atlas_HEPEVT->idhep[nh-1];
}

int& Atlas_HEPEVT::jmohep(int i, int nh) {
  init(); // check COMMON is initialized
  
  if( nh < 1 || nh > lenNmxhep() ||
      i  < 1 || i  > depthRel())
    {
      _dummy = -999;
      return _dummy;
    }

  return _atlas_HEPEVT->jmohep[nh-1][i-1];
}

int& Atlas_HEPEVT::jdahep(int i, int nh) {
  init(); // check COMMON is initialized
  
  if( nh < 1 || nh > lenNmxhep() ||
      i  < 1 || i  > depthRel())
    {
      _dummy = -999;
      return _dummy;
    }

  return _atlas_HEPEVT->jdahep[nh-1][i-1];
}

double& Atlas_HEPEVT::phep(int j, int nh) {
  init(); // check COMMON is initialized
  
  if( nh < 1 || nh > lenNmxhep() ||
      j  < 1 || j  > depthPhep())
    {
      _realdummy = -999.;
      return _realdummy;
    }

  return _atlas_HEPEVT->phep[nh-1][j-1];
}

double& Atlas_HEPEVT::vhep(int k, int nh) {
  init(); // check COMMON is initialized
  
  if( nh < 1 || nh > lenNmxhep() ||
      k  < 1 || k  > depthVhep())
    {
      _realdummy = -999.;
      return _realdummy;
    }

  return _atlas_HEPEVT->vhep[nh-1][k-1];
}

