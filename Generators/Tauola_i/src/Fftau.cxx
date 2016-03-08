#include "Tauola_i/Fftau.h"

// set pointer to zero at start
Fftau::FFTAU* Fftau::s_fftau =0;

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
  
  return s_fftau->ifpol;
}

int& Fftau::ifradcor() {
  init(); // check COMMON is initialized
  
  return s_fftau->ifradcor;
}

int& Fftau::ifdmod() {
  init(); // check COMMON is initialized
  
  return s_fftau->ifdmod;
}

int& Fftau::ifjak1() {
  init(); // check COMMON is initialized
  
  return s_fftau->ifjak1;
}

int& Fftau::ifjak2() {
  init(); // check COMMON is initialized
  
  return s_fftau->ifjak2;
}

float& Fftau::ffphx() {
  init(); // check COMMON is initialized
  
  return s_fftau->ffphx;
}


