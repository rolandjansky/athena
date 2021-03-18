///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"

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
#include "CaloEvent/CaloClusterContainer.h"
#include "AthContainers/DataVector.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "StoreGate/ThinningHandleKey.h"
#include <atomic>

#ifndef XAOD_ANALYSIS
#include "ExpressionEvaluation/ExpressionParserUserWithTrigSupport.h"
using ThinCaloClustersToolBase = ExpressionParserUserWithTrigSupport<::AthAlgTool>;
#else
#include "ExpressionEvaluation/ExpressionParserUser.h"
using ThinCaloClustersToolBase = ExpressionParserUser<::AthAlgTool>;
#endif

class ThinCaloClustersTool
   :         public ThinCaloClustersToolBase,
     virtual public ::DerivationFramework::IThinningTool
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

  /// Name of the CaloClusterContainer to thin
  SG::ThinningHandleKey<xAOD::CaloClusterContainer> m_caloClusKey
  {this, "CaloClustersToThin", "CaloCalTopoCluster", "The xAOD::CaloClusterContainer to be thinned" };

  StringProperty m_streamName
  { this, "StreamName", "", "Name of the stream being thinned" };

  /// List of names of the object collections
  /// This can be used to read any DataVector type.
  SG::ReadHandleKeyArray<SG::AuxVectorBase> m_inCollKeyList
  { this, "InputContainerList", {}, "Containers from which to extract the information which CaloCells should be kept" };


  /// The selection string that will select which xAOD::IParticles to keep from
  /// an xAOD::IParticleContainer
  StringProperty m_selection
  { this, "Selection", "", "The selection string that defines which xAOD::CaloClusters to select from the container" };


  // Declare some counters and initialize them to zero
  /// Event counter
  mutable std::atomic<unsigned long> m_nEventsProcessed;

};

#endif
