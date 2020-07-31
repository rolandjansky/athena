/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// HIJetDRAssociationTool.h

#ifndef HIJETREC_HIJETDRASSOCIATIONTOOL_H
#define HIJETREC_HIJETDRASSOCIATIONTOOL_H
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

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

class HIJetDRAssociationTool : public JetModifierBase
{

  ASG_TOOL_CLASS0(HIJetDRAssociationTool);

public:

  HIJetDRAssociationTool(const std::string& t);

  virtual StatusCode initialize() override;

  /// \brief Implementing abstract methods from base
  virtual StatusCode modify(xAOD::JetContainer& jets) const override;

  /// \brief Implementing abstract methods from base, not used
  virtual int modifyJet(xAOD::Jet&) const override {return 1;};

private:

  /// \brief name of IParticleContainer w/ particles to associate
  SG::ReadHandleKey< xAOD::IParticleContainer > m_containerKey { this, "ContainerKey", "", "Name of IParticleContainer w/ particles to associate"};

  /// \brief Name of jet attribute providing link between jets and IParticles
  Gaudi::Property< std::string > m_assocName { this, "AssociationName", "" , "Name of jet attribute providing link between jets and IParticles" };

  /// \brief DR cut defining association
  Gaudi::Property< float > m_DR { this, "DeltaR", 0.8, "DR cut defining association" };
  /// \brief Flag to enable kinematic requirements on associated IParticle
  Gaudi::Property< bool > m_applyFilter { this, "ApplyFilter",false, "Flag to enable kinematic requirements on associated IParticle" };
  /// \brief Minimum E requirement for associated IParticle, in addition to DR
  Gaudi::Property< float > m_Emin { this, "FilterMinE", -999., "Minimum E requirement for associated IParticle, in addition to DR" };
  /// \brief Minimum pT requirement for associated IParticle, in addition to DR
  Gaudi::Property< float > m_pTmin { this, "FilterMinPt", 0., "Minimum pT requirement for associated IParticle, in addition to DR" };

};

#endif
