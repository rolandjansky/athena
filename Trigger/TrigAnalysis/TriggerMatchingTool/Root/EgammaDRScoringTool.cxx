/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

#include "TriggerMatchingTool/EgammaDRScoringTool.h"
#include "FourMomUtils/xAODP4Helpers.h"
#include "xAODBase/ObjectType.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEgamma/Egamma.h"
#include <sstream>

// Anonymous namespace for helper functions
namespace {
    float dr(const xAOD::CaloCluster &lhs, const xAOD::CaloCluster &rhs)
    {
        return xAOD::P4Helpers::deltaR(lhs.etaBE(2), lhs.phiBE(2), rhs.etaBE(2), rhs.phiBE(2));
    }
}

namespace Trig {
    EgammaDRScoringTool::EgammaDRScoringTool(const std::string &name) :
        asg::AsgTool(name)
    {}

    StatusCode EgammaDRScoringTool::initialize()
    {
        return StatusCode::SUCCESS;
    }

    float EgammaDRScoringTool::score(
            const xAOD::IParticle &online,
            const xAOD::IParticle &offline) const
    {
        switch(online.type())
        {
            case xAOD::Type::CaloCluster:
                return dr(
                        dynamic_cast<const xAOD::CaloCluster &>(online),
                        dynamic_cast<const xAOD::CaloCluster &>(offline));
            case xAOD::Type::Electron:
            case xAOD::Type::Photon:
                return dr(
                        *dynamic_cast<const xAOD::Egamma &>(online).caloCluster(),
                        *dynamic_cast<const xAOD::Egamma &>(offline).caloCluster());
            default:
            {
                std::ostringstream oss;
                oss << "Not an egamma type: " << online.type();
                throw std::runtime_error(oss.str());
            }
        }
        return -1;
    }
} //> end namespace Trig
