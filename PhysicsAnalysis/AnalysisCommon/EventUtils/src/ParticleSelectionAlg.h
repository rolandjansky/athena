///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ParticleSelectionAlg.h
// Header file for class ParticleSelectionAlg
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef EVENTUTILS_PARTICLESELECTIONALG_H
#define EVENTUTILS_PARTICLESELECTIONALG_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"


// forward declarations
class IJobOptionsSvc;
namespace DerivationFramework {
  class IAugmentationTool;
}



class ParticleSelectionAlg
  : public ::AthAlgorithm
{

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
 public:

  // Copy constructor:

  /// Constructor with parameters:
  ParticleSelectionAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor:
  virtual ~ParticleSelectionAlg();


  /// Athena algorithm's initalize hook
  virtual StatusCode  initialize();

  /// Athena algorithm's execute hook
  virtual StatusCode  execute();

  /// Athena algorithm's finalize hook
  virtual StatusCode  finalize();


private:
  // The update handlers

  /// This internal method will realize if a user sets the 'InputContainer' property
  void setupInputContainer( Property& /*prop*/ );

  /// This internal method will realize if a user sets the 'OutputContainer' property
  void setupOutputContainer( Property& /*prop*/ );

  /// This internal method will realize if a user sets the 'OutputContainerType' property
  void setupOutputContainerType( Property& /*prop*/ );

  /// This internal method will realize if a user sets the 'WriteSplitOutputContainer' property
  void setupWriteSplitOutputContainer( Property& /*prop*/ );

  /// This internal method will realize if a user sets the 'OutputContainerOwnershipPolicy' property
  void setupOutputContainerOwnPolicy( Property& /*prop*/ );

  /// This internal method will realize if a user sets the 'OutputLinkContainer' property
  void setupOutputLinkContainer( Property& /*prop*/ );

  /// This internal method will realize if a user sets the 'Selection' property
  void setupSelection( Property& /*prop*/ );


  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
 private:
  /// The job options service (will be used to forward this algs properties to
  /// the private tool)
  ServiceHandle<IJobOptionsSvc> m_jos;

  /// The ToolHandle to the SkimmingTool
  ToolHandle<DerivationFramework::IAugmentationTool> m_tool;

  /// Input container name
  StringProperty m_inCollKey;

  /// This boolean is true if the user sets the 'InputContainer' property
  bool m_setInCollKey;


  /// Output collection name (deep copies of the original ones)
  StringProperty m_outCollKey;

  /// This boolean is true if the user sets the 'OutputContainer' property
  bool m_setOutCollKey;


  /// The type of the output container, e.g., 'xAOD::JetContainer'
  StringProperty m_outCollType;

  /// This boolean is true if the user sets the 'OutputContainerType' property
  bool m_setOutCollType;


  /// Decide if we want to write a fully-split AuxContainer such that we can remove any variables
  BooleanProperty m_writeSplitAux;

  /// This boolean is true if the user sets the 'WriteSplitOutputContainer' property
  bool m_setWriteSplitAux;


  /// Defines the ownership policy of the output container
  /// (default: 'OWN_ELEMENTS'; also allowed: 'VIEW_ELEMENTS')"
  StringProperty m_outOwnPolicyName;

  /// This boolean is true if the user sets the 'OutputContainerOwnershipPolicy' property
  bool m_setOwnPolicy;


  /// Output link collection name (ElementLinks to selected IParticles)
  StringProperty m_outLinkCollKey;

  /// This boolean is true if the user sets the 'OutputLinkContainer' property
  bool m_setOutLinkCollKey;


  /// The selection string that will select which xAOD::IParticles to keep from
  /// an xAOD::IParticleContainer
  StringProperty m_selection;

  /// This boolean is true if the user sets the 'Selection' property
  bool m_setSelection;



  /// Internal event counter
  unsigned long m_nEventsProcessed;


};

// I/O operators
//////////////////////

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

/// This internal method will realize if a user sets the 'InputContainer' property
inline void ParticleSelectionAlg::setupInputContainer( Property& /*prop*/ ) {
  m_setInCollKey = true;
  return;
}

/// This internal method will realize if a user sets the 'OutputContainer' property
inline void ParticleSelectionAlg::setupOutputContainer( Property& /*prop*/ ) {
  m_setOutCollKey = true;
  return;
}

/// This internal method will realize if a user sets the 'OutputContainerType' property
inline void ParticleSelectionAlg::setupOutputContainerType( Property& /*prop*/ ) {
  m_setOutCollType = true;
  return;
}

/// This internal method will realize if a user sets the 'WriteSplitOutputContainer' property
inline void ParticleSelectionAlg::setupWriteSplitOutputContainer( Property& /*prop*/ ) {
  m_setWriteSplitAux = true;
  return;
}

/// This internal method will realize if a user sets the 'OutputContainerOwnPolicy' property
inline void ParticleSelectionAlg::setupOutputContainerOwnPolicy( Property& /*prop*/ ) {
  m_setOwnPolicy = true;
  return;
}

/// This internal method will realize if a user sets the 'OutputLinkContainer' property
inline void ParticleSelectionAlg::setupOutputLinkContainer( Property& /*prop*/ ) {
  m_setOutLinkCollKey = true;
  return;
}

/// This internal method will realize if a user sets the 'Selection' property
inline void ParticleSelectionAlg::setupSelection( Property& /*prop*/ ) {
  m_setSelection = true;
  return;
}




#endif //> !EVENTUTILS_PARTICLESELECTIONALG_H
