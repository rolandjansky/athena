/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// access Lhef_i common block LHEFINFO
#include "Herwig_i/Lhefinfo.h"

// set pointer to zero at start
Lhefinfo::LHEFINFO* Lhefinfo::_lhefinfo = 0;

// Constructor
Lhefinfo::Lhefinfo() 
{}

// Destructor
Lhefinfo::~Lhefinfo() 
{}

int& Lhefinfo::id1()
{
  init();
  return _lhefinfo->id1;
}

int& Lhefinfo::id2()
{
  init();
  return _lhefinfo->id2;
}

double& Lhefinfo::x1()
{
  init();
  return _lhefinfo->x1;
}

double& Lhefinfo::x2()
{
  init();
  return _lhefinfo->x2;
}

double& Lhefinfo::scalePdf()
{
  init();
  return _lhefinfo->scalePdf;
}

double& Lhefinfo::xPdf1()
{
  init();
  return _lhefinfo->xPdf1;
}

double& Lhefinfo::xPdf2()
{
  init();
  return _lhefinfo->xPdf2;
}

