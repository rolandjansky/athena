///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef THINNINGUTILS_THINIPARTICLESTOOL_H
#define THINNINGUTILS_THINIPARTICLESTOOL_H

/**
   @class ThinIParticles
   @brief Tool to thin (remove) individual xAOD::IParticles from a container.
          The IParticles to keep are selected by a given list of input
          containers that either have the interesting IParticles as
          constituents or directly point to them via an ElementLink.
          An additional option is to give a selection string,
          e.g., "Muons.pt > 10.0*GeV".

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

// EDM includes
#include "xAODBase/IParticleContainer.h"
#include "xAODParticleEvent/IParticleLink.h"
#include "xAODParticleEvent/IParticleLinkContainer.h"
#include "StoreGate/ThinningHandleKey.h"
#include "StoreGate/ReadHandleKeyArray.h"

#ifndef XAOD_ANALYSIS
#include "ExpressionEvaluation/ExpressionParserUserWithTrigSupport.h"
using ThinIParticlesToolBase = ExpressionParserUserWithTrigSupport<::AthAlgTool>;
#else
#include "ExpressionEvaluation/ExpressionParserUser.h"
using ThinIParticlesToolBase = ExpressionParserUser<::AthAlgTool>;
#endif



class ThinIParticlesTool
 :          public ThinIParticlesToolBase,
    virtual public ::DerivationFramework::IThinningTool
{
public:
  /// Standard constructor
  ThinIParticlesTool( const std::string& type,
                	    const std::string& name,
                      const IInterface* parent );

  /// Standard destructor
  virtual ~ThinIParticlesTool();

public:
  /// Gaudi Service Interface method implementations: initialize
  StatusCode initialize();

  /// Gaudi Service Interface method implementations: finalize
  StatusCode finalize();

  /// The main method from the inherited interface
  StatusCode doThinning() const;


private:
  /// Helper method to find all IParticles pointed to from the given partLink
  /// and keep those when thinning
  StatusCode selectFromIParticleLink( std::vector<bool>& mask,
                                      const xAOD::IParticleContainer* iParticleContainer,
                                      const xAOD::IParticleLink& partLink ) const;

  /// Select relevant IParticles based on the selection string
  StatusCode selectFromString( std::vector<bool>& mask,
                               const xAOD::IParticleContainer* iParticleContainer ) const;

  StringProperty m_streamName
  { this, "StreamName", "", "Name of the stream for which thinning is done" };

  /// The xAOD::IParticleContainer to thin
  SG::ThinningHandleKey<xAOD::IParticleContainer> m_ipartKey
  { this, "IParticlesToThin", "", "The xAOD::IParticleContainer to be thinned" };

  /// List of names of the object collections
  SG::ReadHandleKeyArray<xAOD::IParticleLinkContainer> m_inCollKeyList
  { this, "InputContainerList", {}, "Containers from which to extract the information which xAOD::IParticles should be kept" };

  /// The selection string that will select which xAOD::IParticles to keep from
  /// an xAOD::IParticleContainer
  StringProperty m_selection
  { this, "Selection", "", "The selection string that defines which xAOD::IParticles to select from the container" };

  // Declare some counters and initialize them to zero
  /// Event counter
  mutable std::atomic<unsigned long> m_nEventsProcessed;

};

#endif
