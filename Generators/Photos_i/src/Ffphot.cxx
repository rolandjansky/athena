#include "Photos_i/Ffphot.h"

// set pointer to zero at start
Ffphot::FFPHOT* Ffphot::s_ffphot =0;

// Constructor
Ffphot::Ffphot() 
{
}

// Destructor
Ffphot::~Ffphot() 
{
}

int& Ffphot::ifpmode() {
  init(); // check COMMON is initialized
  
  return s_ffphot->ifpmode;
}

float& Ffphot::ffphcut() {
  init(); // check COMMON is initialized
  
  return s_ffphot->ffphcut;
}

float& Ffphot::ffalpha() {
  init(); // check COMMON is initialized
  
  return s_ffphot->ffalpha;
}

int& Ffphot::ifinter() {
  init(); // check COMMON is initialized
  
  return s_ffphot->ifinter;
}

int& Ffphot::ifsec() {
  init(); // check COMMON is initialized
  
  return s_ffphot->ifsec;
}

int& Ffphot::ifitre() {
  init(); // check COMMON is initialized
  
  return s_ffphot->ifitre;
}

int& Ffphot::ifiexp() {
  init(); // check COMMON is initialized
  
  return s_ffphot->ifiexp;
}

int& Ffphot::iftops() {
  init(); // check COMMON is initialized
  
  return s_ffphot->iftops;
}


