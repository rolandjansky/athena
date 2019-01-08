/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRIGEGAMMAMATCHINGTOOL_H_
#define ITRIGEGAMMAMATCHINGTOOL_H_

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
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

namespace Trig{
    class ITrigEgammaMatchingTool : public virtual asg::IAsgTool {
        ASG_TOOL_INTERFACE( Trig::ITrigEgammaMatchingTool )

        public:
            virtual StatusCode initialize(void) = 0;
            virtual StatusCode execute()=0;
            virtual StatusCode finalize()=0;
            virtual bool match(const xAOD::Egamma *,const std::string,const HLT::TriggerElement*&)=0;
            virtual bool match(const xAOD::Egamma *,const std::string)=0;
            virtual bool matchHLT(const xAOD::Egamma *,const std::string)=0;
            virtual const xAOD::Egamma* closestHLTObject(const xAOD::Egamma *,const std::string)=0;
        protected:

        private:
    };
}
#endif
