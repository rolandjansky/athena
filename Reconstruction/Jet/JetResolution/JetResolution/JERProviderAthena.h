/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// Dear emacs, this is -*-c++-*-
///***********************************************************************
///
/// JERProviderAthena
/// Authors: Gaston Romeo, T.J. Khoo
///
/// JERProvider-derived class, for use in ATHENA
///
///#### Usage:
///
///
///***********************************************************************

#ifndef _JERPROVIDERATHENA_
#define _JERPROVIDERATHENA_


#include "JetResolution/JERProvider.h"
#include "PathResolver/PathResolver.h"

#include "TFile.h"
#include "TH2D.h"
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
using std::cout;

namespace JetResolution {

  class JERProviderAthena : public JERProvider {

  public:
  
    // Constructors, destructor
    JERProviderAthena(std::string CollectionName="AntiKt6H1TopoJets",
		      std::string MethodName = "Truth",
		      std::string FileName="JERProviderPlots.root");
    virtual ~JERProviderAthena();

  private:  
      
  };

}

#endif
