///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ParticleSelectionTool.h
// Header file for class ParticleSelectionTool
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef EVENTUTILS_PARTICLESELECTIONTOOL_H
#define EVENTUTILS_PARTICLESELECTIONTOOL_H 1

// STL includes
#include <vector>
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

// EDM includes
#include "AthContainers/OwnershipPolicy.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODParticleEvent/IParticleLinkContainer.h"


// Forward declarations
namespace ExpressionParsing {
  class ExpressionParser;
}



class ParticleSelectionTool
  : virtual public ::DerivationFramework::IAugmentationTool,
            public ::AthAlgTool
{

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Copy constructor:

  /// Constructor with parameters:
  ParticleSelectionTool( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

  /// Destructor:
  virtual ~ParticleSelectionTool();

  /// Athena algtool's initialize
  virtual StatusCode  initialize() override;

  /// Athena algtool's finalize
  virtual StatusCode  finalize() override;


  /// Implement the method from the ISkimmingTool interface
  virtual StatusCode addBranches() const final override;



// Private methods
private:

  /// A helper method to get the input containers is and
  /// create and record the output link container
  StatusCode
  prepareContainers( const xAOD::IParticleLinkContainer*& inLinkContainer,
                     const xAOD::IParticleContainer*& inContainer,
                     xAOD::IParticleLinkContainer*& outLinkContainer ) const;


  /// Helper method to actually iterate over the input particles and selection
  /// results and fill the output container(s)
  StatusCode
  evaluate( const std::vector<int>& resultVec,
            const xAOD::IParticleLinkContainer* inLinkContainer,
            const xAOD::IParticleContainer* inContainer,
            xAOD::IParticleLinkContainer* outLinkContainer  ) const;


  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:
  /// The trigger decision tool
  ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;

  /// The expression parser
  ExpressionParsing::ExpressionParser *m_parser;


  /// Input container name
  StringProperty m_inCollKey;

  /// Output collection name (deep copies of the original ones)
  StringProperty m_outCollKey;

  /// The type of the output container, e.g., 'xAOD::JetContainer'
  StringProperty m_outCollType;

  /// Decide if we want to write a fully-split AuxContainer such that we can remove any variables
  BooleanProperty m_writeSplitAux;

  /// Defines the ownership policy of the output container
  /// (default: 'OWN_ELEMENTS'; also allowed: 'VIEW_ELEMENTS')"
  StringProperty m_outOwnPolicyName;

  /// The internally used translation for the ownership policy
  SG::OwnershipPolicy m_outOwnPolicy;

  /// Output link collection name (ElementLinks to selected IParticles)
  StringProperty m_outLinkCollKey;

  /// The cut string
  StringProperty m_selection;


  /// Internal container type identifier
  mutable unsigned int m_contID;


  /// Internal event counter
  mutable unsigned long m_nEventsProcessed;


};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////


#endif //> !EVENTUTILS_PARTICLESELECTIONTOOL_H
