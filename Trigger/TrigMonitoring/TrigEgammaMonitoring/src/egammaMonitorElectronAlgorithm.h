/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef egammaMonitorElectronAlgorithm_H
#define egammaMonitorElectronAlgorithm_H


#include "egammaMonitorElectronAlgorithm.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/DataHandle.h"
#include "AthenaMonitoringKernel/Monitored.h"


class egammaMonitorElectronAlgorithm: public AthAlgorithm
{

  public:

    egammaMonitorElectronAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
    StatusCode initialize();
    virtual StatusCode execute() override final {

        return execute_r(Algorithm::getContext());
    }

    StatusCode execute_r(const EventContext& ctx) const;

  protected:

    void filltopoElectronTrackCaloMatch(const EventContext& ctx) const;
    void filltopoElectronShowerShapes(const EventContext& ctx) const;
    void filltopoElectronIsolation(const EventContext& ctx) const;
    
  
  private:
    SG::ReadHandleKey<xAOD::ElectronContainer> m_electronsKey{ this, "ElectronKey", "Electrons", ""};
    ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};
};
 


#endif
