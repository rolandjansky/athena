// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  bool m_includeNu; //!< Include neutrinos in particles
  bool m_includeMu; //!< Include muons in particles
  bool m_includePromptLeptons; //!< Include particles from prompt decays, i.e. not from hadrons
  //  bool m_includeTau; //!< Include particles from tau decays
  // -- added for dark jet clustering -- //
  bool m_includeSM; //!< Include SM particles
  bool m_includeDark; //!< Include dark hadrons

  bool isPrompt( const xAOD::TruthParticle* tp,
		 std::map<const xAOD::TruthParticle*,MCTruthPartClassifier::ParticleOrigin>& originMap ) const;
  // bool fromTau( const xAOD::TruthParticle* tp,
  // 		std::map<const xAOD::TruthParticle*,MCTruthPartClassifier::ParticleOrigin>& originMap ) const;


  MCTruthPartClassifier::ParticleOrigin getPartOrigin(const xAOD::TruthParticle* tp,
						      std::map<const xAOD::TruthParticle*,MCTruthPartClassifier::ParticleOrigin>& originMap) const;

  float m_maxAbsEta;

  // this is set to mutable so that it changes if the metadata information is available
  //http://stackoverflow.com/questions/12247970/error-in-assignment-of-member-in-read-only-object
  mutable int m_barcodeOffset;

  /// Determine how the barcode offset is set from metadata
  ///  0 -> no metdata access, use BarCodeOffset property
  ///  >0 from metadata. If it is not found it will keep the BarCodeOffset property
  int m_barcodeFromMetadata;

  float m_photonCone;

  ToolHandle<IMCTruthClassifier> m_classif;
};


#endif
