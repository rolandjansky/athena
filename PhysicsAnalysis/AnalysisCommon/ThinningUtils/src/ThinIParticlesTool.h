///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

// Framework includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IThinningSvc.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
// AthAnalysisBase doesn't currently include the Trigger Service
#ifndef XAOD_ANALYSIS
#include "TrigDecisionTool/TrigDecisionTool.h"
#endif

// EDM includes
#include "xAODBase/IParticleContainer.h"
#include "xAODParticleEvent/IParticleLink.h"

// Forward declarations
namespace ExpressionParsing {
  class ExpressionParser;
}



class ThinIParticlesTool
  : virtual public ::DerivationFramework::IThinningTool,
            public ::AthAlgTool
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


  /// The trigger decision tool
// AthAnalysisBase doesn't currently include the Trigger Service
#ifndef XAOD_ANALYSIS
  ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;
#endif

  /// The expression parser
  ExpressionParsing::ExpressionParser *m_parser;

  /// Pointer to IThinningSvc
  ServiceHandle<IThinningSvc> m_thinningSvc;


  /// Name of the xAOD::IParticleContainer to thin
  StringProperty m_ipartKey;

  /// List of names of the object collections
  StringArrayProperty m_inCollKeyList;

  /// The selection string that will select which xAOD::IParticles to keep from
  /// an xAOD::IParticleContainer
  StringProperty m_selection;

  /// Can take the mask from storegate, by specifying this key
  StringProperty m_maskStoregate;

  /// The number of given xAOD::IParticles in the current event
  mutable std::size_t m_nTotalIParticles;

  // Declare some counters and initialize them to zero
  /// Event counter
  mutable unsigned long m_nEventsProcessed;

};

#endif
