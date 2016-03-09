/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// access Lhef_i common block LHEFINFO
#include "Pythia_i/Lhefinfo.h"

// set pointer to zero at start
Lhefinfo::LHEFINFO* Lhefinfo::s_lhefinfo = 0;

// Constructor
Lhefinfo::Lhefinfo() 
{}

// Destructor
Lhefinfo::~Lhefinfo() 
{}

int& Lhefinfo::id1()
{
  init();
  return s_lhefinfo->id1;
}

int& Lhefinfo::id2()
{
  init();
  return s_lhefinfo->id2;
}

double& Lhefinfo::x1()
{
  init();
  return s_lhefinfo->x1;
}

double& Lhefinfo::x2()
{
  init();
  return s_lhefinfo->x2;
}

double& Lhefinfo::scalePdf()
{
  init();
  return s_lhefinfo->scalePdf;
}

double& Lhefinfo::xPdf1()
{
  init();
  return s_lhefinfo->xPdf1;
}

double& Lhefinfo::xPdf2()
{
  init();
  return s_lhefinfo->xPdf2;
}

