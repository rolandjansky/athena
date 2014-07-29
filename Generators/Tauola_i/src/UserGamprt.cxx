//--------------------------------------------------------------------------
// File and Version Information:
//      UserGamprt.cxx
//
// Description:
//      Needed for interface of Tauola
//
// Author List:
// Jan Kotanski, May 2011
//
// Copyright Information:
//
//------------------------------------------------------------------------
#include "Tauola_i/UserGamprt.h"


// set pointer to zero at start
UserGamprt::USERGAMPRT* UserGamprt::_usergamprt =0;

// Constructor
UserGamprt::UserGamprt() : _realdummy(-999.f) {
}

// Destructor
UserGamprt::~UserGamprt() {
}


int& UserGamprt::ugused() {
  init(); // check COMMON is initialized
  
  return _usergamprt->ugused;
}


float& UserGamprt::userGamprt(int nt) {
  init(); // check COMMON is initialized
  ugused()=1;

  if( nt < 1 || nt > nmax()) 
    {
      _realdummy = -999.f;
      return _realdummy;
    }

  return _usergamprt->usergamprt[nt-1];
}


