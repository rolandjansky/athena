/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFPTOFALGORITHM_H
#define AFPTOFALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODForward/AFPToFHitContainer.h"
#include "xAODForward/AFPToFHit.h"

#include "TRandom3.h"

class AFPToFAlgorithm : public AthMonitorAlgorithm {
public:
    AFPToFAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~AFPToFAlgorithm();
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

private:
   SG::ReadHandleKey<xAOD::AFPToFHitContainer> m_afpToFHitContainerKey;

protected:
   std::vector<std::string> m_pixlayers = { "P0", "P1", "P2", "P3"};
   std::vector<std::string> m_stationnames = { "farAside", "nearAside" , "nearCside" , "farCside"};

};
#endif

