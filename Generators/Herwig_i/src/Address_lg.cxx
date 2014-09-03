/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//  this routine is supposed to be called from FORTRAN, it returns an
//  address of its parameter
//  revision history:
//  -----------------
//------------------------------------------------------------------------------
extern "C" void* adreslg_(void* var) {
  return var;
}

