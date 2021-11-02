/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*    @Afile TrigMuonTruthMonMT.h
 *   
 *    author: Harriet Watson (hawatson@cern.ch)
 *       date: 14.04.2021
 * 
 * 
 *     This is a class for monitoring TruthMuonMT.
 *
*/

#ifndef TRIGMUONMONITORINGMT_TRIGMUONTRUTHMONMT_H
#define TRIGMUONMONITORINGMT_TRIGMUONTRUTHMONMT_H

#include "TrigMuonMonitorAlgorithm.h"
#include "xAODTruth/TruthParticleContainer.h"


class TrigMuonTruthMonMT : public TrigMuonMonitorAlgorithm{

    public:
        TrigMuonTruthMonMT(const std::string& name, ISvcLocator* pSvcLocator); 

        virtual StatusCode initialize() override;

    protected:
        virtual StatusCode fillVariablesPerChain(const EventContext &ctx, const std::string &chain) const override;

    private:
        SG::ReadHandleKey<xAOD::TruthParticleContainer> m_muonTruthParticlesKey{this, "MuonTruthParticlesKey", "MuonTruthParticles"};

};



#endif //TRIGMUONMONITORINGMT_TRIGMUONTRUTHMONMT_H