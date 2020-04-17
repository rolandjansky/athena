/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITrigEgammaMatchingToolMT_H
#define ITrigEgammaMatchingToolMT_H

#include "AsgTools/IAsgTool.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigEgamma/TrigPhotonContainer.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"





namespace Trig{
    class ITrigEgammaMatchingToolMT : public virtual asg::IAsgTool {
        ASG_TOOL_INTERFACE( Trig::ITrigEgammaMatchingToolMT )

        public:
            virtual StatusCode initialize() = 0;
            
            virtual bool match(const xAOD::Egamma *,const std::string&) const=0;
            virtual bool match(const xAOD::Egamma *,const std::string&, const TrigCompositeUtils::Decision *&) const=0;
    

    };
}
#endif
