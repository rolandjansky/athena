/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HIJetClusterAssociationTool.h

#ifndef __HIJETREC_HIJETCLUSTERINDEXASSOCIATIONTOOL_H__
#define __HIJETREC_HIJETCLUSTERINDEXASSOCIATIONTOOL_H__


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

class HIJetClusterIndexAssociationTool : public JetModifierBase
{

  ASG_TOOL_CLASS0(HIJetClusterIndexAssociationTool);

public:

  HIJetClusterIndexAssociationTool(const std::string& t);
 
  int modify(xAOD::JetContainer& jets) const;
  int modifyJet(xAOD::Jet&) const {return 1;};

private:

  /// \brief name of IParticleContainer w/ particles to associate
  std::string m_container_key;

  /// \brief Name of jet attribute providing link between jets and clusters
  std::string m_assoc_name;

  // %% NOT YET IMPLEMENTED
  // index passing actually needs to be implemented
  float m_DR;
  bool m_apply_filter;
  float m_E_min;
  float m_pT_min;
    
};

#endif
