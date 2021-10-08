/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGL0GEPPERF_CONEJETMAKER_H
#define TRIGL0GEPPERF_CONEJETMAKER_H

#include <string>

#include "xAODTrigger/JetRoI.h"
#include "xAODTrigger/JetRoIAuxContainer.h"
#include "xAODTrigger/JetRoIContainer.h"

#include "TrigL0GepPerf/IJetMaker.h"
#include "TrigL0GepPerf/CustomJet.h"
#include "TrigL0GepPerf/CustomTopoCluster.h"



namespace Gep
{
  class ConeJetMaker : public Gep::IJetMaker
  {
  public:

  ConeJetMaker(float jetR, float seedEtThreshold = 5.e3 /*MeV*/, std::string recombScheme = "EScheme") : 
      m_jetR(jetR),
      m_seedEtThreshold(seedEtThreshold),
      m_recombScheme(recombScheme)
    {}

    std::string getName() const { return "ConeJet"; }
    std::vector<Gep::CustomJet> makeJet(const std::vector<Gep::CustomTopoCluster> &clusters);
    void setSeeds(const xAOD::JetRoIContainer *seeds) { m_seeds = seeds; }
    float getJetR() { return m_jetR; }
    float getSeedEtThreshold() { return m_seedEtThreshold; }
    std::string getRecombScheme() { return m_recombScheme; }
  

  private:
    float m_jetR;
    float m_seedEtThreshold;
    std::string m_recombScheme;
    const mutable xAOD::JetRoIContainer *m_seeds;

  };

}

#endif //TRIGL0GEPPERF_CONEJETMAKER_H
