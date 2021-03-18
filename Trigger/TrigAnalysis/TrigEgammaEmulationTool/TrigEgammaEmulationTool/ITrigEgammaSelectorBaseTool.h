/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef ITrigEgammaSelectorBaseTool_H_
#define ITrigEgammaSelectorBaseTool_H_

#include "AsgTools/IAsgTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigNavStructure/TriggerElement.h"
#include "TrigEgammaEmulationTool/TrigEgammaInfo.h"

//xAOD include(s)
#include "xAODBase/IParticleContainer.h"
#include "xAODTrigger/EmTauRoI.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"


namespace Trig{
    class ITrigEgammaSelectorBaseTool : public virtual asg::IAsgTool {
        ASG_TOOL_INTERFACE( Trig::ITrigEgammaSelectorBaseTool )

        public:
            virtual StatusCode initialize()=0;
            virtual StatusCode finalize()=0;

            virtual bool emulation(const xAOD::EmTauRoI               * , bool&, const Trig::Info &)=0;
            virtual bool emulation(const xAOD::TrigEMCluster          * , bool&, const Trig::Info &)=0;
            virtual bool emulation(const xAOD::IParticle              * , bool&, const Trig::Info &)=0;
            virtual bool emulation(const xAOD::IParticleContainer     * , bool&, const Trig::Info &)=0;
           
            virtual void setTe(const HLT::TriggerElement *)=0; 
            virtual void setParents(ToolHandle<Trig::TrigDecisionTool> &, StoreGateSvc *)=0;
            virtual void setParents(Trig::TrigDecisionTool *, StoreGateSvc *)=0;

        protected:

        private:
    };
}
#endif
