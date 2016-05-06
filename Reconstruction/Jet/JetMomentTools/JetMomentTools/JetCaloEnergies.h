// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMOMENTTOOLS_JETCALOENERGIES_H
#define JETMOMENTTOOLS_JETCALOENERGIES_H

#include "JetRec/JetModifierBase.h"
#include <vector>

class JetCaloEnergies : public JetModifierBase {
  ASG_TOOL_CLASS0(JetCaloEnergies)
public:
  
  JetCaloEnergies(const std::string & t);


  virtual int modifyJet(xAOD::Jet& ) const ;

protected:
  void fillEperSamplingCluster(xAOD::Jet &jet, std::vector<float> & ePerSampling ) const ;
  void fillEperSamplingPFO(xAOD::Jet &jet, std::vector<float> & ePerSampling ) const ;

      
};


#undef ASG_DERIVED_TOOL_CLASS
#endif

