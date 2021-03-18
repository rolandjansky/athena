/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITrigEgammaMatchingTool_H
#define ITrigEgammaMatchingTool_H

#include "AsgTools/IAsgTool.h"
#include "xAODEgamma/Egamma.h"
#include "TrigNavStructure/TriggerElement.h"

#include <string>

namespace Trig{
    class ITrigEgammaMatchingTool : public virtual asg::IAsgTool {
        ASG_TOOL_INTERFACE( Trig::ITrigEgammaMatchingTool )

        public:
            virtual StatusCode initialize() = 0;
            virtual bool match(const xAOD::Egamma *,const std::string&,const HLT::TriggerElement*&)=0;
            virtual bool match(const xAOD::Egamma *,const std::string&)=0;
            virtual bool matchHLT(const xAOD::Egamma *,const std::string&)=0;
            virtual const xAOD::Egamma* closestHLTObject(const xAOD::Egamma *,const std::string&)=0;
        protected:

        private:
    };
}
#endif
