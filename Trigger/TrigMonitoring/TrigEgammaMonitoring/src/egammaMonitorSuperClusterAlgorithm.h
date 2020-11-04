/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef egammaMonitorSuperClusterAlgorithm_H
#define egammaMonitorSuperClusterAlgorithm_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "egammaMonitorSuperClusterAlgorithm.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "GaudiKernel/EventContext.h"
#include "egammaRecEvent/egammaRec.h"
#include "egammaRecEvent/egammaRecContainer.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/DataHandle.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

class egammaRec;

class egammaMonitorSuperClusterAlgorithm: public AthReentrantAlgorithm 
{

  public:

    egammaMonitorSuperClusterAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual StatusCode initialize() override;
    virtual StatusCode execute (const EventContext& ctx) const override;

  protected:

    
    void fillSuperClusterQuantities(const EventContext& ctx) const;
    
    
  
  private:
    SG::ReadHandleKey<EgammaRecContainer> m_inputEgammaRecContainerKey{
      this,
       "InputEgammaRecContainerName",
       "egammaRecCollection",
       "input egammaRec container"
    };

    ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};
};
 


#endif
