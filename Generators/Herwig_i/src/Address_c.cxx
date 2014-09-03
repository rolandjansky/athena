/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//  Sept 09 1997 P.Murat (CDF)
//  this routine is supposed to be called from FORTRAN, it returns an
//  address of its parameter
//  revision history:
//  -----------------
//------------------------------------------------------------------------------
extern "C" void* adressc_(void* var) {
  return var;
}

