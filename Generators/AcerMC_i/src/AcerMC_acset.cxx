/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//--------------------------------------------------------------------------
//     AcerMC acset common block access
//     Borut Paul Kersevan (February 2003)
//------------------------------------------------------------------------

#include "AcerMC_i/AcerMC_acset.h"
// set pointer to zero at start
AcerMC_acset::ACERMC_ACSET* AcerMC_acset::s_acermc_acset =0;
// Constructor
AcerMC_acset::AcerMC_acset() 
{
}
// Destructor
AcerMC_acset::~AcerMC_acset() 
{
}
// access acset12 in common
int& AcerMC_acset::acset12() {
  init(); 
  return s_acermc_acset->acset12;
}

