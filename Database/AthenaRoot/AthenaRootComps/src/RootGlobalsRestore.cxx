///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RootGlobalsRestore.cxx 
// Implementation file for class RootGlobalsRestore
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// AthenaRootComps includes
#include "RootGlobalsRestore.h"

// STL includes

// ROOT includes
#include "TFile.h"
#include "TDirectory.h"
#include "TError.h"

namespace Athena {


RootGlobalsRestore::RootGlobalsRestore() 
{
  m_gd = gDirectory;
  m_gf = gFile;
  m_ge = gErrorIgnoreLevel;
}

RootGlobalsRestore::~RootGlobalsRestore() 
{
  gDirectory = m_gd;
  gFile = m_gf;
  gErrorIgnoreLevel = m_ge;
}

} //> end namespace Athena
