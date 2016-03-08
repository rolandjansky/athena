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
UserGamprt::USERGAMPRT* UserGamprt::s_usergamprt =0;

// Constructor
UserGamprt::UserGamprt() : m_realdummy(-999.f) {
}

// Destructor
UserGamprt::~UserGamprt() {
}


int& UserGamprt::ugused() {
  init(); // check COMMON is initialized
  
  return s_usergamprt->ugused;
}


float& UserGamprt::userGamprt(int nt) {
  init(); // check COMMON is initialized
  ugused()=1;

  if( nt < 1 || nt > nmax()) 
    {
      m_realdummy = -999.f;
      return m_realdummy;
    }

  return s_usergamprt->usergamprt[nt-1];
}


