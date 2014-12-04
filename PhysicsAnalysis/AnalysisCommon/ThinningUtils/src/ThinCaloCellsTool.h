///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// AthAnalysisBase doesn't know about calo cells (geometry would be needed)
#ifndef XAOD_ANALYSIS

#ifndef THINNINGUTILS_THINCALOCELLSTOOL_H
#define THINNINGUTILS_THINCALOCELLSTOOL_H

/**
   @class ThinCaloCells
   @brief Tool to thin (remove) individual xAOD::CaloCells from a container.
          The CaloCells to keep are selected by a given list of input
          containers that either have the interesting CaloCells as
          constituents or directly point to them via an ElementLink.

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

// EDM includes
#include "AthLinks/ElementLink.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODBase/IParticle.h"
#include "xAODParticleEvent/IParticleLink.h"
#include "xAODEgamma/Egamma.h"
#include "xAODMuon/Muon.h"
#include "xAODTau/TauJet.h"
#include "xAODJet/Jet.h"
#include "xAODParticleEvent/CompositeParticle.h"


//forward declaration
class CaloCellContainer;




class ThinCaloCellsTool
  : virtual public ::DerivationFramework::IThinningTool,
            public ::AthAlgTool
{
public:
  /// Standard constructor
  ThinCaloCellsTool( const std::string& type,
                	   const std::string& name,
                     const IInterface* parent );

  /// Standard destructor
  virtual ~ThinCaloCellsTool();

public:
  /// Gaudi Service Interface method implementations: initialize
  StatusCode initialize();

  /// Gaudi Service Interface method implementations: finalize
  StatusCode finalize();

  /// The main method from the inherited interface
  StatusCode doThinning() const;


private:
  // /// Helper method to find all CaloCells pointed to from the given partLink
  // /// and keep those when thinning
  // StatusCode selectFromCaloCellLink( std::vector<bool>& mask,
  //                                    const CaloCellContainer* caloCellContainer,
  //                                    const ElementLink<CaloCellContainer>& cellLink ) const;

  /// Helper method to find all CaloCells pointed to from the given partLink
  /// and keep those when thinning
  StatusCode selectFromCaloCluster( std::vector<bool>& mask,
                                    const CaloCellContainer* caloCellContainer,
                                    const xAOD::CaloCluster* cluster ) const;

  /// Helper method to find all CaloCells pointed to from the given partLink
  /// and keep those when thinning
  StatusCode selectFromIParticleLink( std::vector<bool>& mask,
                                      const CaloCellContainer* caloCellContainer,
                                      const xAOD::IParticleLink& partLink ) const;

  /// Helper method to find all CaloCells pointed to from the given part
  /// and keep those when thinning
  StatusCode selectFromIParticle( std::vector<bool>& mask,
                                  const CaloCellContainer* caloCellContainer,
                                  const xAOD::IParticle* part ) const;

  /// Helper method to find all CaloCells pointed to from the given part
  /// and keep those when thinning
  StatusCode selectFromEgamma( std::vector<bool>& mask,
                               const CaloCellContainer* caloCellContainer,
                               const xAOD::Egamma* part ) const;

  /// Helper method to find all CaloCells pointed to from the given part
  /// and keep those when thinning
  StatusCode selectFromMuon( std::vector<bool>& mask,
                             const CaloCellContainer* caloCellContainer,
                             const xAOD::Muon* part ) const;

  /// Helper method to find all CaloCells pointed to from the given part
  /// and keep those when thinning
  StatusCode selectFromTauJet( std::vector<bool>& mask,
                               const CaloCellContainer* caloCellContainer,
                               const xAOD::TauJet* part ) const;

  /// Helper method to find all CaloCells pointed to from the given part
  /// and keep those when thinning
  StatusCode selectFromJet( std::vector<bool>& mask,
                            const CaloCellContainer* caloCellContainer,
                            const xAOD::Jet* part ) const;

  /// Helper method to find all CaloCells pointed to from the given part
  /// and keep those when thinning
  StatusCode selectFromCompositeParticle( std::vector<bool>& mask,
                                          const CaloCellContainer* caloCellContainer,
                                          const xAOD::CompositeParticle* part ) const;


  /// Pointer to IThinningSvc
  ServiceHandle<IThinningSvc> m_thinningSvc;


  /// Name of the CaloCellContainer to thin
  StringProperty m_caloCellKey;

  /// List of names of the object collections
  StringArrayProperty m_inCollKeyList;



  /// The number of given CaloCells in the current event
  mutable std::size_t m_nTotalCaloCells;

  // Declare some counters and initialize them to zero
  /// Event counter
  mutable unsigned long m_nEventsProcessed;

};

#endif

#endif
