/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "JetRec/JetModifierBase.h"

#include "xAODCaloEvent/CaloCluster.h"

#include <string>


class JetClusterMomentsTool : public JetModifierBase {
    ASG_TOOL_CLASS(JetClusterMomentsTool,IJetModifier)
    public:
        // Constructor from tool name
        JetClusterMomentsTool(const std::string& name);

        // Inherited methods to modify a jet
        virtual int modifyJet(xAOD::Jet& jet) const;

    private:
        // properties
        bool m_doClsPt           ;
        bool m_doClsSecondLambda ;
        bool m_doClsCenterLambda ;
        bool m_doClsSecondR      ;

        // Find cluster with highest energy, or return NULL if unsuccessful 
        const xAOD::CaloCluster * findLeadingCluster(xAOD::Jet& jet) const;
        // Retrieve a moment from cluster
        float getMoment(const xAOD::CaloCluster* cluster, const xAOD::CaloCluster::MomentType& momentType) const;
};

#endif

