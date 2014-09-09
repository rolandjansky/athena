/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <math.h>
#include <float.h>
#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"

#include "fastjet/ClusterSequence.hh"

using namespace std;
using fastjet::PseudoJet;

JetSubStructureMomentToolsBase::JetSubStructureMomentToolsBase(std::string name) : 
  JetModifierBase(name)
{
}

