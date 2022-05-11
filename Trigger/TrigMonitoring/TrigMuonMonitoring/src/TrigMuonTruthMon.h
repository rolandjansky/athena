/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*    @Afile TrigMuonTruthMon.h
 *   
 *    author: Harriet Watson (hawatson@cern.ch)
 *       date: 14.04.2021
 * 
 * 
 *     This is a class for monitoring TruthMuon.
 *
*/

#ifndef TRIGMUONMONITORING_TRIGMUONTRUTHMON_H
#define TRIGMUONMONITORING_TRIGMUONTRUTHMON_H

#include "TrigMuonMonitorAlgorithm.h"
#include "xAODTruth/TruthParticleContainer.h"


class TrigMuonTruthMon : public TrigMuonMonitorAlgorithm{

    public:
        TrigMuonTruthMon(const std::string& name, ISvcLocator* pSvcLocator); 

        virtual StatusCode initialize() override;

    protected:
        virtual StatusCode fillVariablesPerChain(const EventContext &ctx, const std::string &chain) const override;

    private:
        SG::ReadHandleKey<xAOD::TruthParticleContainer> m_muonTruthParticlesKey{this, "MuonTruthParticlesKey", "MuonTruthParticles"};

};


#endif //TRIGMUONMONITORING_TRIGMUONTRUTHMON_H
