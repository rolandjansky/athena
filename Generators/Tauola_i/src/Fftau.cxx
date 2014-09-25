#include "Tauola_i/Fftau.h"

// set pointer to zero at start
Fftau::FFTAU* Fftau::_fftau =0;

// Constructor
Fftau::Fftau() 
{
}

// Destructor
Fftau::~Fftau() 
{
}

int& Fftau::ifpol() {
  init(); // check COMMON is initialized
  
  return _fftau->ifpol;
}

int& Fftau::ifradcor() {
  init(); // check COMMON is initialized
  
  return _fftau->ifradcor;
}

int& Fftau::ifdmod() {
  init(); // check COMMON is initialized
  
  return _fftau->ifdmod;
}

int& Fftau::ifjak1() {
  init(); // check COMMON is initialized
  
  return _fftau->ifjak1;
}

int& Fftau::ifjak2() {
  init(); // check COMMON is initialized
  
  return _fftau->ifjak2;
}

float& Fftau::ffphx() {
  init(); // check COMMON is initialized
  
  return _fftau->ffphx;
}


