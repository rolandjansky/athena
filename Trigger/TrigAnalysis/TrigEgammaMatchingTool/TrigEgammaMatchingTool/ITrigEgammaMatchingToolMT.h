/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITrigEgammaMatchingToolMT_H
#define ITrigEgammaMatchingToolMT_H

#include "AsgTools/IAsgTool.h"
#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "xAODTrigEgamma/TrigElectronAuxContainer.h"
#include "xAODTrigEgamma/TrigPhotonContainer.h"
#include "xAODTrigEgamma/TrigPhotonAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "TrigDecisionTool/TrigDecisionTool.h"


namespace Trig{

    class ITrigEgammaMatchingToolMT : public virtual asg::IAsgTool {

        ASG_TOOL_INTERFACE( Trig::ITrigEgammaMatchingToolMT )

        public:
            virtual StatusCode initialize()=0;
    };
}
#endif
