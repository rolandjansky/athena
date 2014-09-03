/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//  Sept 09 1997 P.Murat (CDF)
//  this routine is supposed to be called from FORTRAN, it returns an
//  address of its parameter
//  revision history:
//new version for complex -- Ian Hinchliffe
//  -----------------
//------------------------------------------------------------------------------
extern "C" void* adrescx_(void* var) {
  return var;
}

