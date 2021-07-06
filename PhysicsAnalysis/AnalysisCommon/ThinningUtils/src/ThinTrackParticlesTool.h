///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef THINNINGUTILS_THINTRACKPARTICLESTOOL_H
#define THINNINGUTILS_THINTRACKPARTICLESTOOL_H

/**
   @class ThinTrackParticles
   @brief Tool to thin (remove) individual xAOD::TrackParticles from a container.
          The TrackParticles to keep are selected by a given list of input
          containers that either have the interesting TrackParticles as
          constituents or directly point to them via an ElementLink.

   @author Karsten Koeneke
*/


// STL includes
#include <string>
#include <vector>
#include <atomic>

// Framework includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
// AthAnalysisBase doesn't currently include the Trigger Service
#ifndef XAOD_ANALYSIS
#include "TrigDecisionTool/TrigDecisionTool.h"
#endif
#include "StoreGate/ThinningHandleKey.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "AthContainers/DataVector.h"

// EDM includes
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODBase/IParticle.h"
#include "xAODParticleEvent/IParticleLink.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODMuon/Muon.h"
#include "xAODTau/TauJet.h"
#include "xAODJet/Jet.h"
#include "xAODParticleEvent/CompositeParticle.h"

#ifndef XAOD_ANALYSIS
#include "ExpressionEvaluation/ExpressionParserUserWithTrigSupport.h"
using ThinTrackParticlesToolBase = ExpressionParserUserWithTrigSupport<::AthAlgTool>;
#else
#include "ExpressionEvaluation/ExpressionParserUser.h"
using ThinTrackParticlesToolBase = ExpressionParserUser<::AthAlgTool>;
#endif


class ThinTrackParticlesTool
  : public ThinTrackParticlesToolBase,
    virtual public ::DerivationFramework::IThinningTool
{
public:
  /// Standard constructor
  ThinTrackParticlesTool( const std::string& type,
                          const std::string& name,
                          const IInterface* parent );

  /// Standard destructor
  virtual ~ThinTrackParticlesTool();

public:
  /// Gaudi Service Interface method implementations: initialize
  StatusCode initialize();

  /// Gaudi Service Interface method implementations: finalize
  StatusCode finalize();

  /// The main method from the inherited interface
  StatusCode doThinning() const;


private:
  /// Helper method to find all TrackParticles pointed to from the given partLink
  /// and keep those when thinning
  StatusCode selectFromIParticleLink( std::vector<bool>& mask,
                                      const xAOD::TrackParticleContainer* trackParticleContainer,
                                      const xAOD::IParticleLink& partLink ) const;

  /// Helper method to find all TrackParticles pointed to from the given part
  /// and keep those when thinning
  StatusCode selectFromIParticle( std::vector<bool>& mask,
                                  const xAOD::TrackParticleContainer* trackParticleContainer,
                                  const xAOD::IParticle* part ) const;

  /// Helper method to find all TrackParticles pointed to from the given part
  /// and keep those when thinning
  StatusCode selectFromElectron( std::vector<bool>& mask,
                                 const xAOD::TrackParticleContainer* trackParticleContainer,
                                 const xAOD::Electron* part ) const;

  /// Helper method to find all TrackParticles pointed to from the given part
  /// and keep those when thinning
  StatusCode selectFromPhoton( std::vector<bool>& mask,
                               const xAOD::TrackParticleContainer* trackParticleContainer,
                               const xAOD::Photon* part ) const;

  /// Helper method to find all TrackParticles pointed to from the given part
  /// and keep those when thinning
  StatusCode selectFromMuon( std::vector<bool>& mask,
                             const xAOD::TrackParticleContainer* trackParticleContainer,
                             const xAOD::Muon* part ) const;

  /// Helper method to find all TrackParticles pointed to from the given part
  /// and keep those when thinning
  StatusCode selectFromTauJet( std::vector<bool>& mask,
                               const xAOD::TrackParticleContainer* trackParticleContainer,
                               const xAOD::TauJet* part ) const;

  /// Helper method to find all TrackParticles pointed to from the given part
  /// and keep those when thinning
  StatusCode selectFromJet( std::vector<bool>& mask,
                            const xAOD::TrackParticleContainer* trackParticleContainer,
                            const xAOD::Jet* part ) const;

  /// Helper method to find all TrackParticles pointed to from the given part
  /// and keep those when thinning
  StatusCode selectFromCompositeParticle( std::vector<bool>& mask,
                                          const xAOD::TrackParticleContainer* trackParticleContainer,
                                          const xAOD::CompositeParticle* part ) const;

  /// Helper method to find all TrackParticles pointed to from the given part
  /// and keep those when thinning
  StatusCode selectFromVertex( std::vector<bool>& mask,
                               const xAOD::TrackParticleContainer* trackParticleContainer,
                               const xAOD::Vertex* part ) const;

  /// Select relevant TrackParticles based on the selection string
  StatusCode selectFromString( std::vector<bool>& mask,
                               const xAOD::TrackParticleContainer* trackParticleContainer ) const;

  StringProperty m_streamName
  { this, "StreamName", "", "Name of the stream for which thinning is being done." };

  /// Name of the TrackParticleContainer to thin
  SG::ThinningHandleKey<xAOD::TrackParticleContainer> m_trackParticleKey
  { this, "TrackParticlesToThin", "InDetTrackParticles", "The xAOD::TrackParticleContainer to be thinned" };

  /// List of object collections
  SG::ReadHandleKeyArray<SG::AuxVectorBase> m_inCollKeyList
  { this, "InputContainerList", {}, "Containers from which to extract the information which CaloCells should be kept" };


  /// The selection string that will select which xAOD::IParticles to keep from
  /// an xAOD::IParticleContainer
  StringProperty m_selection
  { this, "Selection", "", "The selection string that defines which xAOD::IParticles to select from the container" };


  /// Flag to steer if one should also keep conversion track particles from taus
  BooleanProperty m_tauConversion
  { this, "KeepTauConversions", false, "Flag to steer if one should also keep conversion track particles from taus" };

  /// Flag to steer if one should also keep 'wide track particles' from taus
  BooleanProperty m_tauWide
  { this, "KeepTauWide", false, "Flag to steer if one should also keep 'wide track particles' from taus" };

  /// Flag to steer if one should also keep 'other' track particles from taus
  BooleanProperty m_tauOther
  { this, "KeepTauOther", false, "Flag to steer if one should also keep 'other' track particles from taus" };

  /// Set the maximum number of TrackParticles from each electron to keep (default: -1 means all are kept)
  IntegerProperty m_nElectronPTMax
  { this, "NMaxElectronTrackParticles", -1, "Set the maximum number of TrackParticles from each electron to keep (default: -1 means all are kept" };


  // Declare some counters and initialize them to zero
  /// Event counter
  mutable std::atomic<unsigned long> m_nEventsProcessed;

  /// The number of total TrackParticles kept
  mutable std::atomic<unsigned long> m_nTrackPartsKept;

  /// The number of total TrackParticles processed
  mutable std::atomic<unsigned long> m_nTrackPartsProcessed;


};

#endif
