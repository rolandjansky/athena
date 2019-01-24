/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ITrigEgammaEmulationTool_H_
#define ITrigEgammaEmulationTool_H_

#include "AsgTools/IAsgTool.h"
#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaContainer.h"
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
#include "PATCore/AcceptInfo.h"
#include "PATCore/AcceptData.h"
#include <vector>
#include <map>


namespace Trig{
    class ITrigEgammaEmulationTool : public virtual asg::IAsgTool {
        ASG_TOOL_INTERFACE( Trig::ITrigEgammaEmulationTool )

        public:
            virtual StatusCode initialize(void)=0;
            virtual StatusCode execute()=0;
            virtual StatusCode finalize()=0;

            virtual bool EventWiseContainer()=0;

            virtual asg::AcceptData executeTool( const HLT::TriggerElement *, const std::string & )=0;
            virtual asg::AcceptData executeTool( const std::string & )=0;
            virtual bool isPassed(const std::string&)=0;
            virtual bool isPassed(const std::string&, const std::string&)=0;
            virtual const asg::AcceptInfo& getAccept() const =0;
            /* Experimental methods */
            virtual void ExperimentalAndExpertMethods()=0;
            virtual void match( const xAOD::Egamma *, const HLT::TriggerElement *&)=0;
            virtual const HLT::TriggerElement* getTEMatched()=0;
        private:
    };
}
#endif
