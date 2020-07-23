/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// HIJetClusterAssociationTool.h

#ifndef HIJETREC_HIJETCLUSTERINDEXASSOCIATIONTOOL_H
#define HIJETREC_HIJETCLUSTERINDEXASSOCIATIONTOOL_H


////////////////////////////////////////////////////////////////////////////////
///
/// \class HIJetClusterIndexAssociationTool
/// \author Aaron Angerami <angerami@cern.ch>
/// \date Jan, 2015
///
/// \brief JetModifier that associates HIClusters w/ a jet.
/// Necessary because you may want to know all clusters w/in DR < X rather than
/// just the clusters that are constituents of the jet.
/// Since HI-style clusters derive from towers they have a well-defined index
/// For each jet, this tool associates a set of clusters from a pre-determined
/// indexing.
///
/// Given a DR value and an eta/phi position a set of indices of towers
/// w/in DR of that position can be calculated, and can be done in advance
/// or at initialization time rather than in the event loop.
/// The set of such indices for each eta/phi position (defined by tower grid)
/// is supplied to the tool, which uses this to perform the association.
////////////////////////////////////////////////////////////////////////////////

#include <string>
#include "JetRec/JetModifierBase.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

class HIJetClusterIndexAssociationTool : public JetModifierBase
{

  ASG_TOOL_CLASS0(HIJetClusterIndexAssociationTool);

public:

  HIJetClusterIndexAssociationTool(const std::string& t);

  virtual StatusCode initialize() override;

  virtual StatusCode modify(xAOD::JetContainer& jets) const override;
  virtual int modifyJet(xAOD::Jet&) const override {return 1;};

private:

  /// \brief name of IParticleContainer w/ particles to associate
  SG::ReadHandleKey<xAOD::IParticleContainer> m_containerKey { this, "ContainerKey", "", "Name of IParticleContainer w/ particles to associat"};

  /// \brief Name of jet attribute providing link between jets and clusters
  Gaudi::Property< std::string > m_assocName { this, "AssociationName", "", "Name of jet attribute providing link between jets and clusters" };

  // %% NOT YET IMPLEMENTED
  // index passing actually needs to be implemented
  Gaudi::Property< float > m_DR { this, "DeltaR", 0.8, "Index passing actually needs to be implemented" };
  Gaudi::Property< bool > m_applyFilter { this, "ApplyFilter", false, "Apply filter boolean" };

  Gaudi::Property< float > m_Emin { this, "FilterMinE", -999., "Filter minimum Energy" };
  Gaudi::Property< float > m_pTmin { this, "FilterMinPt", 0., "Filter minimum Pt" };


};

#endif
