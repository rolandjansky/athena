/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// JetClusterMomentsTool.h

#ifndef JETMOMENTTOOLS_JETCLUSTERMOMENTSTOOL_H
#define JETMOMENTTOOLS_JETCLUSTERMOMENTSTOOL_H

/// Jakub Cuth \n
/// February 2014
///
/// Tool to calculate general cluster-based jet moments
///
/// Moments to add:
///     LeadingClusterPt            xAOD::CaloCluster::pt()
///     LeadingClusterSecondLambda  xAOD::CaloCluster::SECOND_LAMBDA
///     LeadingClusterCenterLambda  xAOD::CaloCluster::CENTER_LAMBDA
///     LeadingClusterSecondR       xAOD::CaloCluster::SECOND_R
///
///  

#include "AsgTools/AsgTool.h"
#include "AsgTools/PropertyWrapper.h"
#include "JetInterface/IJetDecorator.h"
#include "AsgDataHandles/WriteDecorHandleKey.h"

#include "xAODCaloEvent/CaloCluster.h"

#include <string>


class JetClusterMomentsTool : public asg::AsgTool,
                              virtual public IJetDecorator{
    ASG_TOOL_CLASS(JetClusterMomentsTool,IJetDecorator)
    public:
        // Constructor from tool name
        JetClusterMomentsTool(const std::string& name);

        virtual StatusCode decorate(const xAOD::JetContainer& jets) const override;
        virtual StatusCode initialize() override;

    private:
        Gaudi::Property<std::string> m_jetContainerName{this, "JetContainer", "", "SG key for the input jet container"};

        // SG keys for the moment decorations.
        // If configured to be an empty string, that decoration will not be created.
        SG::WriteDecorHandleKey<xAOD::JetContainer> m_clsPtKey{this, "PtKey", "LeadingClusterPt", "SG key for pt decoration"};
        SG::WriteDecorHandleKey<xAOD::JetContainer> m_clsSecondLambdaKey{this, "SecondLambdaKey", "LeadingClusterSecondLambda", "SG key for second lambda decoration"};
        SG::WriteDecorHandleKey<xAOD::JetContainer> m_clsCenterLambdaKey{this, "CenterLambdaKey", "LeadingClusterCenterLambda", "SG key for center lambda decoration"};
        SG::WriteDecorHandleKey<xAOD::JetContainer> m_clsSecondRKey{this, "SecondRKey", "LeadingClusterSecondR", "SG key for second R decoration"};

        // Find cluster with highest energy, or return NULL if unsuccessful
        const xAOD::CaloCluster* findLeadingCluster(const xAOD::Jet& jet) const;
        // Retrieve a moment from cluster
        float getMoment(const xAOD::CaloCluster* cluster, const xAOD::CaloCluster::MomentType& momentType) const;
};

#endif

