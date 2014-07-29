#include "Photos_i/Ffphot.h"

// set pointer to zero at start
Ffphot::FFPHOT* Ffphot::_ffphot =0;

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
  
  return _ffphot->ifpmode;
}

float& Ffphot::ffphcut() {
  init(); // check COMMON is initialized
  
  return _ffphot->ffphcut;
}

float& Ffphot::ffalpha() {
  init(); // check COMMON is initialized
  
  return _ffphot->ffalpha;
}

int& Ffphot::ifinter() {
  init(); // check COMMON is initialized
  
  return _ffphot->ifinter;
}

int& Ffphot::ifsec() {
  init(); // check COMMON is initialized
  
  return _ffphot->ifsec;
}

int& Ffphot::ifitre() {
  init(); // check COMMON is initialized
  
  return _ffphot->ifitre;
}

int& Ffphot::ifiexp() {
  init(); // check COMMON is initialized
  
  return _ffphot->ifiexp;
}

int& Ffphot::iftops() {
  init(); // check COMMON is initialized
  
  return _ffphot->iftops;
}


