/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef ITrigEgammaEmulationTool_H_
#define ITrigEgammaEmulationTool_H_

#include "AsgTools/IAsgTool.h"
#include "xAODEgamma/Egamma.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "TrigNavStructure/TriggerElement.h"
#include "PATCore/AcceptInfo.h"
#include "PATCore/AcceptData.h"

#include <string>


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
            virtual bool emulationL2Calo(const xAOD::TrigEMCluster *,  std::string)=0;
        private:
    };
}
#endif
