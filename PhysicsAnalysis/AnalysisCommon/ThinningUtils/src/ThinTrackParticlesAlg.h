///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef THINNINGUTILS_THINTRACKPARTICLESALG_H
#define THINNINGUTILS_THINTRACKPARTICLESALG_H 1

/**
   @class ThinTrackParticlesAlg
   @brief Tool to thin (remove) individual xAOD::TrackParticles from a container.
          This algorithm is not doing anything itself. It is rather holding a
          private instance of the ThinTrackParticlesTool and it is forwarding
          its Properties to this tool and finally using this tool for every event.
          An additional option is to give a selection string,
          e.g., "Muons.pt > 10.0*GeV".

   @author Karsten Koeneke
*/


// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"


// forward declarations
class IJobOptionsSvc;
namespace DerivationFramework {
  class IThinningTool;
}



class ThinTrackParticlesAlg
  : public ::AthAlgorithm
{
 public:

  /// Constructor with parameters:
  ThinTrackParticlesAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor:
  virtual ~ThinTrackParticlesAlg();

  /// Athena algorithm's initalize hook
  virtual StatusCode  initialize();

  /// Athena algorithm's execute hook
  virtual StatusCode  execute();

  /// Athena algorithm's finalize hook
  virtual StatusCode  finalize();


private:
  // The update handlers

  /// This internal method will realize if a user sets the 'TrackParticlesToThin' property
  void setupTrackPartsToThin( Gaudi::Details::PropertyBase& /*prop*/ );

  /// This internal method will realize if a user sets the 'InputContainerList' property
  void setupInputContainerList( Gaudi::Details::PropertyBase& /*prop*/ );

  /// This internal method will realize if a user sets the 'Selection' property
  void setupSelection( Gaudi::Details::PropertyBase& /*prop*/ );

  /// This internal method will realize if a user sets the 'KeepTauConversions' property
  void setupTauConv( Gaudi::Details::PropertyBase& /*prop*/ );

  /// This internal method will realize if a user sets the 'KeepTauWide' property
  void setupTauWide( Gaudi::Details::PropertyBase& /*prop*/ );

  /// This internal method will realize if a user sets the 'KeepTauOther' property
  void setupTauOther( Gaudi::Details::PropertyBase& /*prop*/ );

  /// This internal method will realize if a user sets the 'NMaxElectronTrackParticles' property
  void setupNEleTPMax( Gaudi::Details::PropertyBase& /*prop*/ );


  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
 private:
  /// The job options service (will be used to forward this algs properties to
  /// the private tool)
  ServiceHandle<IJobOptionsSvc> m_jos;

  StringProperty m_streamName
  { this, "StreamName", "", "Name of the stream being thinned" };

  /// The ToolHandle to the ThinningTool
  ToolHandle<DerivationFramework::IThinningTool> m_thinTool;


  /// Name of the TrackParticleContainer to thin
  StringProperty m_trackParticleKey;

  /// This boolean is true if the user sets the 'TrackParticlesToThin' property
  bool m_setTrackPartKey;


  /// List of names of the object collections
  StringArrayProperty m_inCollKeyList;

  /// This boolean is true if the user sets the 'InputContainerList' property
  bool m_setInCollKey;


  /// The selection string that will select which xAOD::IParticles to keep from
  /// an xAOD::IParticleContainer
  StringProperty m_selection;

  /// This boolean is true if the user sets the 'Selection' property
  bool m_setSelection;


  /// Flag to steer if one should also keep conversion track particles from taus
  BooleanProperty m_tauConversion;

  /// This boolean is true if the user sets the 'KeepTauConversions' property
  bool m_setTauConv;


  /// Flag to steer if one should also keep 'wide track particles' from taus
  BooleanProperty m_tauWide;

  /// This boolean is true if the user sets the 'KeepTauWide' property
  bool m_setTauWide;


  /// Flag to steer if one should also keep 'other' track particles from taus
  BooleanProperty m_tauOther;

  /// This boolean is true if the user sets the 'KeepTauOther' property
  bool m_setTauOther;


  /// Set the maximum number of TrackParticles from each electron to keep (default: -1 means all are kept)
  IntegerProperty m_nElectronPTMax;

  /// This boolean is true if the user sets the 'NMaxElectronTrackParticles' property
  bool m_setNEleTPMax;


  /// Internal event counter
  unsigned long m_nEventsProcessed;

};


///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

/// This internal method will realize if a user sets the 'TrackParticlesToThin' property
inline void ThinTrackParticlesAlg::setupTrackPartsToThin( Gaudi::Details::PropertyBase& /*prop*/ ) {
  m_setTrackPartKey = true;
  return;
}

/// This internal method will realize if a user sets the 'InputContainerList' property
inline void ThinTrackParticlesAlg::setupInputContainerList( Gaudi::Details::PropertyBase& /*prop*/ ) {
  m_setInCollKey = true;
  return;
}

/// This internal method will realize if a user sets the 'Selection' property
inline void ThinTrackParticlesAlg::setupSelection( Gaudi::Details::PropertyBase& /*prop*/ ) {
  m_setSelection = true;
  return;
}

/// This internal method will realize if a user sets the 'KeepTauConversions' property
inline void ThinTrackParticlesAlg::setupTauConv( Gaudi::Details::PropertyBase& /*prop*/ ) {
  m_setTauConv = true;
  return;
}

/// This internal method will realize if a user sets the 'KeepTauWide' property
inline void ThinTrackParticlesAlg::setupTauWide( Gaudi::Details::PropertyBase& /*prop*/ ) {
  m_setTauWide = true;
  return;
}

/// This internal method will realize if a user sets the 'KeepTauOther' property
inline void ThinTrackParticlesAlg::setupTauOther( Gaudi::Details::PropertyBase& /*prop*/ ) {
  m_setTauOther = true;
  return;
}

/// This internal method will realize if a user sets the 'NMaxElectronTrackParticles' property
inline void ThinTrackParticlesAlg::setupNEleTPMax( Gaudi::Details::PropertyBase& /*prop*/ ){
  m_setNEleTPMax = true;
  return;
}


#endif //> !THINNINGUTILS_THINTRACKPARTICLESALG_H
