// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IJETTAGGER_H
#define IJETTAGGER_H

#include "PATInterfaces/CorrectionCode.h"
#include "JetAnalysisInterfaces/IJetSelectorTool.h"

#include "xAODJet/JetContainer.h"
#include "xAODTruth/TruthParticleContainer.h"

namespace CP {

  class IJetTagger : public virtual IJetSelectorTool {

    ASG_TOOL_INTERFACE( CP::IJetTagger )

    public:

    virtual ~IJetTagger() {}

    virtual Root::TAccept tag(const xAOD::Jet& jet) const = 0;

    virtual StatusCode decorateTruthLabel(const xAOD::Jet& jet,
				    const xAOD::TruthParticleContainer* truthPartsW_TRUTH3=nullptr,
				    const xAOD::TruthParticleContainer* truthPartsZ_TRUTH3=nullptr,
				    const xAOD::TruthParticleContainer* truthPartsTop_TRUTH3=nullptr,
				    const xAOD::JetContainer* truthJets=nullptr,
				    std::string decorName="WTopContainmentTruthLabel", 
				    double dR_truthJet=0.75, double dR_truthPart=0.75,
				    double mLowTop=140, double mHighTop=200, double mLowW=50, double mHighW=100, double mLowZ=60, double mHighZ=110) const = 0;

    virtual StatusCode decorateTruthLabel(const xAOD::Jet& jet,
				    const xAOD::TruthParticleContainer* truthParts=nullptr,
				    const xAOD::JetContainer* truthJets=nullptr,
				    std::string decorName="WTopContainmentTruthLabel", 
				    double dR_truthJet=0.75, double dR_truthPart=0.75,
				    double mLowTop=140, double mHighTop=200, double mLowW=50, double mHighW=100, double mLowZ=60, double mHighZ=110) const = 0;

  };
  
} // namespace CP

#endif
