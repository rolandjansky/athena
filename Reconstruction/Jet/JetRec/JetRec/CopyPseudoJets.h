// This file is   -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CopyPseudoJets.h

#ifndef JETREC_COPYPSEUDOJETS_H
#define JETREC_COPYPSEUDOJETS_H
//////////////////////////////////////////////////////
/// \class CopyPseudoJets
/// \author Pierre-Antoine Delsart
/// \date August 2014
///
/// This JetModifier copy the PseudoJet contained in the jets it receives
/// to the jets in an other container.
/// This is to allow to re-set PJ to a container read from file : this
/// container can then be used as input to a grooming alg or sub-structure calculations.
///
/// The other container is given by a string as a tool property (TargetContainer)
///  !!! IMPORTANT !!! 
///    - the constness of the other container is casted away
///    - it is assumed the other container has been found by the exact same jet alg
///
/// Typical configuration when reading an xAOD would be like :
///     copyPJTool = CopyPseudoJets("CopyPJAntiKtLCTopo", TargetContainer="AntiKtLCTopoJets)
///     jtm += copyPJTool 
///     # create a temporary AntiKtLCTopo collection, & copy PJ to the existing AntiKtLCTopoJets
///      jtm.addJetFinder("tmp_AntiKt10LCTopoJets",  "AntiKt", 1.0,   "lctopo", [copyPJTool], ghostArea=0.0, ptmin= 2000) 
///
//////////////////////////////////////////////////////

#include <string>
#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetModifier.h"

class CopyPseudoJets
: virtual public asg::AsgTool,
  virtual public IJetModifier {

  ASG_TOOL_CLASS(CopyPseudoJets, IJetModifier)

public:
    
  CopyPseudoJets(const std::string& t);
  
  int modify(xAOD::JetContainer& jets) const;

protected:

  std::string m_targetContainerName;
};

#endif
