// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COPYTRUTHJETPARTICLES_H
#define COPYTRUTHJETPARTICLES_H

#include "ParticleJetTools/CopyTruthParticles.h"
#include "AsgTools/ToolHandle.h"
#include "xAODTruth/TruthParticle.h"
#include "MCTruthClassifier/MCTruthClassifierDefs.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"
#include <vector>
#include <map>

#include <mutex>          // std::call_once, std::once_flag

std::once_flag metaDataFlag;


class CopyTruthJetParticles : public CopyTruthParticles {
ASG_TOOL_CLASS2(CopyTruthJetParticles, CopyTruthParticles, IJetExecuteTool)
public:

  /// Constructor
  CopyTruthJetParticles(const std::string& name);

  /// Function initialising the tool
  virtual StatusCode initialize();


  /// redefine execute so we can call our own classify() with the barcode offset for the current event.
  virtual int execute() const;

  /// Redefine our own Classifier function(s)
  bool classifyJetInput(const xAOD::TruthParticle* tp, int barcodeOffset,
                        std::vector<const xAOD::TruthParticle*>& promptLeptons,
			std::map<const xAOD::TruthParticle*,MCTruthPartClassifier::ParticleOrigin>& originMap) const;

  // metadata check
  int setBarCodeFromMetaDataCheck() const ;

  /// The base classify() is not used 
  bool classify(const xAOD::TruthParticle* ) const {return false;}
  
private:
  // Options for storate
  bool m_includeBSMNonInt; //!< Include non-interacting BSM particles in particles
  bool m_includeNu; //!< Include neutrinos in particles
  bool m_includeMu; //!< Include muons in particles
  bool m_includePromptLeptons; //!< Include particles from prompt decays, i.e. not from hadrons
  bool m_includePromptPhotons; //!< Include photons from prompt decays, e.g. Higgs decays
  bool m_chargedOnly; //!< Include only charged particles
  // -- added for dark jet clustering -- //
  bool m_includeSM; //!< Include SM particles
  bool m_includeDark; //!< Include dark hadrons

  bool isPrompt( const xAOD::TruthParticle* tp,
		 std::map<const xAOD::TruthParticle*,MCTruthPartClassifier::ParticleOrigin>& originMap ) const;
  // bool fromTau( const xAOD::TruthParticle* tp,
  // 		std::map<const xAOD::TruthParticle*,MCTruthPartClassifier::ParticleOrigin>& originMap ) const;


  MCTruthPartClassifier::ParticleOrigin getPartOrigin(const xAOD::TruthParticle* tp,
						      std::map<const xAOD::TruthParticle*,MCTruthPartClassifier::ParticleOrigin>& originMap) const;

  /// Maximum allowed eta for particles in jets
  float m_maxAbsEta;

  /// Offset for Geant4 particle barcodes
  // this is set to mutable so that it changes if the metadata information is available
  //http://stackoverflow.com/questions/12247970/error-in-assignment-of-member-in-read-only-object
  mutable int m_barcodeOffset;

  /// Determine how the barcode offset is set from metadata
  ///  0 -> no metdata access, use BarCodeOffset property
  ///  1 -> from metadata. Fails if not found
  ///  2 -> from metadata, use BarCodeOffset property if not found (default)
  int m_barcodeFromMetadata;

  /// Cone to be used for removing FSR photons from around prompt leptons
  float m_photonCone;

  std::vector<int> m_vetoPDG_IDs; //! List of PDG IDs that should be ignored (and whose children should be ignored)
  bool comesFrom( const xAOD::TruthParticle* tp, const int pdgID, std::vector<int>& used_vertices ) const;

  /// Name of the decoration to be used for identifying FSR (dressing) photons
  std::string m_dressingName;

  /// Handle on MCTruthClassifier for finding prompt leptons
  ToolHandle<IMCTruthClassifier> m_classif;
};


#endif
