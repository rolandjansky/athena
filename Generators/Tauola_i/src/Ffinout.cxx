#include "Tauola_i/Ffinout.h"

// set pointer to zero at start
Ffinout::INOUT* Ffinout::s_ffinout =0;

// Constructor
Ffinout::Ffinout() 
{
}

// Destructor
Ffinout::~Ffinout() 
{
}

int& Ffinout::inut() {
  init(); // check COMMON is initialized
  
  return s_ffinout->inut;
}
int& Ffinout::iout() {
  init(); // check COMMON is initialized
  
  return s_ffinout->iout;
}


