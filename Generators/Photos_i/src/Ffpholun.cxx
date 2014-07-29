#include "Photos_i/Ffpholun.h"

// set pointer to zero at start
Ffpholun::PHOLUN* Ffpholun::_ffpholun =0;

// Constructor
Ffpholun::Ffpholun() 
{
}

// Destructor
Ffpholun::~Ffpholun() 
{
}

int& Ffpholun::phlun() {
  init(); // check COMMON is initialized
  
  return _ffpholun->phlun;
}


