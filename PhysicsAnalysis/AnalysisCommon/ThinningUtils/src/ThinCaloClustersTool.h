///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef THINNINGUTILS_THINCALOCLUSTERSTOOL_H
#define THINNINGUTILS_THINCALOCLUSTERSTOOL_H

/**
   @class ThinCaloClusters
   @brief Tool to thin (remove) individual xAOD::CaloClusters from a container.
          The CaloClusters to keep are selected by a given list of input
          containers that either have the interesting CaloClusters as
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
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODBase/IParticle.h"
#include "xAODParticleEvent/IParticleLink.h"
#include "xAODEgamma/Egamma.h"
#include "xAODMuon/Muon.h"
#include "xAODTau/TauJet.h"
#include "xAODJet/Jet.h"
#include "xAODParticleEvent/CompositeParticle.h"

// Forward declarations
namespace ExpressionParsing {
  class ExpressionParser;
}




class ThinCaloClustersTool
  : virtual public ::DerivationFramework::IThinningTool,
            public ::AthAlgTool
{
public:
  /// Standard constructor
  ThinCaloClustersTool( const std::string& type,
                	      const std::string& name,
                        const IInterface* parent );

  /// Standard destructor
  virtual ~ThinCaloClustersTool();

public:
  /// Gaudi Service Interface method implementations: initialize
  StatusCode initialize();

  /// Gaudi Service Interface method implementations: finalize
  StatusCode finalize();

  /// The main method from the inherited interface
  StatusCode doThinning() const;


private:
  /// Helper method to find all CaloClusters pointed to from the given partLink
  /// and keep those when thinning
  StatusCode selectFromIParticleLink( std::vector<bool>& mask,
                                      const xAOD::CaloClusterContainer* caloClusterContainer,
                                      const xAOD::IParticleLink& partLink ) const;

  /// Helper method to find all CaloClusters pointed to from the given part
  /// and keep those when thinning
  StatusCode selectFromIParticle( std::vector<bool>& mask,
                                  const xAOD::CaloClusterContainer* caloClusterContainer,
                                  const xAOD::IParticle* part ) const;

  /// Helper method to find all CaloClusters pointed to from the given part
  /// and keep those when thinning
  StatusCode selectFromEgamma( std::vector<bool>& mask,
                               const xAOD::CaloClusterContainer* caloClusterContainer,
                               const xAOD::Egamma* part ) const;

  /// Helper method to find all CaloClusters pointed to from the given part
  /// and keep those when thinning
  StatusCode selectFromMuon( std::vector<bool>& mask,
                             const xAOD::CaloClusterContainer* caloClusterContainer,
                             const xAOD::Muon* part ) const;

  /// Helper method to find all CaloClusters pointed to from the given part
  /// and keep those when thinning
  StatusCode selectFromTauJet( std::vector<bool>& mask,
                               const xAOD::CaloClusterContainer* caloClusterContainer,
                               const xAOD::TauJet* part ) const;

  /// Helper method to find all CaloClusters pointed to from the given part
  /// and keep those when thinning
  StatusCode selectFromJet( std::vector<bool>& mask,
                            const xAOD::CaloClusterContainer* caloClusterContainer,
                            const xAOD::Jet* part ) const;

  /// Helper method to find all CaloClusters pointed to from the given part
  /// and keep those when thinning
  StatusCode selectFromCompositeParticle( std::vector<bool>& mask,
                                          const xAOD::CaloClusterContainer* caloClusterContainer,
                                          const xAOD::CompositeParticle* part ) const;

  /// Select relevant IParticles based on the selection string
  StatusCode selectFromString( std::vector<bool>& mask,
                               const xAOD::CaloClusterContainer* caloClusterContainer ) const;


  /// The trigger decision tool
// AthAnalysisBase doesn't currently include the Trigger Service
#ifndef XAOD_ANALYSIS
  ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;
#endif

  /// The expression parser
  ExpressionParsing::ExpressionParser *m_parser;

  /// Pointer to IThinningSvc
  ServiceHandle<IThinningSvc> m_thinningSvc;


  /// Name of the CaloClusterContainer to thin
  StringProperty m_caloClusKey;

  /// List of names of the object collections
  StringArrayProperty m_inCollKeyList;


  /// The selection string that will select which xAOD::IParticles to keep from
  /// an xAOD::IParticleContainer
  StringProperty m_selection;


  /// The number of given CaloClusters in the current event
  mutable std::size_t m_nTotalCaloClusters;

  // Declare some counters and initialize them to zero
  /// Event counter
  mutable unsigned long m_nEventsProcessed;

};

#endif
