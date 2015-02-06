/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HIJetDRAssociationTool.h

#ifndef __HIJETREC_HIJETDRASSOCIATIONTOOL_H__
#define __HIJETREC_HIJETDRASSOCIATIONTOOL_H__
////////////////////////////////////////////////////////////////////////////////
///
/// \class HIJetDRAssociationTool
/// \author Aaron Angerami <angerami@cern.ch>
/// \date Jan, 2015
///
/// \brief JetModifier that stores links to IParticles w/in DR of a jet.
/// In HIJetRec this association is needed to determine which clusters t.o exclude
/// from the HIEventShape recalculation on the basis of some seed jets
/// May not want to only use constituents of seed jets, e.g. R=0.2 jets as seeds
/// but want to exclude DR=0.6 around jet position, track jets, etc.
///
/// Tool can also be used for generic DR associations as an alternative to
/// ghost associations.
///
////////////////////////////////////////////////////////////////////////////////

#include <string>
#include "JetRec/JetModifierBase.h"

class HIJetDRAssociationTool : public JetModifierBase
{

  ASG_TOOL_CLASS0(HIJetDRAssociationTool);

public:

  HIJetDRAssociationTool(const std::string& t);
 
  /// \brief Implementing abstract methods from base
  int modify(xAOD::JetContainer& jets) const;


  /// \brief Implementing abstract methods from base, not used
  int modifyJet(xAOD::Jet&) const {return 1;};

private:

  /// \brief name of IParticleContainer w/ particles to associate
  std::string m_container_key;

  /// \brief Name of jet attribute providing link between jets and IParticles
  std::string m_assoc_name;

  /// \brief DR cut defining association
  float m_DR;

  /// \brief Flag to enable kinematic requirements on associated IParticle
  bool m_apply_filter;

  /// \brief Minimum E requirement for associated IParticle, in addition to DR
  float m_E_min;

  /// \brief Minimum pT requirement for associated IParticle, in addition to DR
  float m_pT_min;
    
};

#endif
