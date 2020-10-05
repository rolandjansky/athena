/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef EVENTUTILS_PARTICLECOMBINERALG_H
#define EVENTUTILS_PARTICLECOMBINERALG_H

//============================================================================
// Name: ParticleCombinerAlg.h
//
/**
   @class ParticleCombinerAlg

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date July 2014

   @brief Combine particles to composite particles

   Class to combine two or more particles from a given list of
   input containers. All combinations will be tried and only the
   ones passing the used selections will be written to StoreGate.
*/
//=============================================================================


// STL includes
#include <string>
#include <vector>

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// Athena includes
#include "AthenaBaseComps/AthAlgorithm.h"


// forward declarations
class IJobOptionsSvc;
namespace DerivationFramework {
  class IAugmentationTool;
}



class ParticleCombinerAlg : public AthAlgorithm
{
public:

  /** Default constructor */
  ParticleCombinerAlg(const std::string& name, ISvcLocator* pSvcLocator);

  /** Default destructor */
  virtual ~ParticleCombinerAlg() ;


public:
  /** Gaudi Service Interface method implementations - initialize */
  StatusCode initialize() ;

  /** Gaudi Service Interface method implementations - execute */
  StatusCode execute() ;

  /** Gaudi Service Interface method implementations - finalize */
  StatusCode finalize() ;


private:
  // The update handlers

  /// This internal method will realize if a user sets the 'InputContainerList' property
  void setupInputContainerList( Gaudi::Details::PropertyBase& /*prop*/ );

  /// This internal method will realize if a user sets the 'MissingETObject' property
  void setupMissingETObject( Gaudi::Details::PropertyBase& /*prop*/ );

  /// This internal method will realize if a user sets the 'OutputContainer' property
  void setupOutputContainer( Gaudi::Details::PropertyBase& /*prop*/ );

  /// This internal method will realize if a user sets the 'WriteSplitOutputContainer' property
  void setupWriteSplitOutputContainer( Gaudi::Details::PropertyBase& /*prop*/ );

  /// This internal method will realize if a user sets the 'SetPdgId' property
  void setupSetPdgId( Gaudi::Details::PropertyBase& /*prop*/ );

  /// This internal method will realize if a user sets the 'SortConstituents' property
  void setupSort( Gaudi::Details::PropertyBase& /*prop*/ );



private:
  /// The job options service (will be used to forward this algs properties to
  /// the private tool)
  ServiceHandle<IJobOptionsSvc> m_jos;

  /// The ToolHandle to the AugmentationTool
  ToolHandle<DerivationFramework::IAugmentationTool> m_tool;

  /// List of input collection names
  StringArrayProperty m_inCollKeyList;

  /// This boolean is true if the user sets the 'InputContainerList' property
  bool m_setInCollKeyList;


  /// The name of the missing ET object, e.g., "Final"
  StringProperty m_metName;

  /// This boolean is true if the user sets the 'MissingETObject' property
  bool m_setMetName;


  /// Output collection name (the composite particles)
  StringProperty m_outCollKey;

  /// This boolean is true if the user sets the 'OutputContainer' property
  bool m_setOutCollKey;


  /// Decide if we want to write a fully-split AuxContainer such that we can remove any variables
  BooleanProperty m_writeSplitAux;

  /// This boolean is true if the user sets the 'WriteSplitOutputContainer' property
  bool m_setWriteSplitAux;


  /// The PDG_ID of the CompositeParticle
  IntegerProperty m_pdgId;

  /// This boolean is true if the user sets the 'SetPdgId' property
  bool m_setPdgId;


  /// If true: sort the constituents in decending pt order
  BooleanProperty m_sortConstit;

  /// This boolean is true if the user sets the 'SortConstituents' property
  bool m_setSort;



  /// Internal event counter
  unsigned long m_nEventsProcessed;


}; // End class ParticleCombinerAlg


// Inline methods

/// This internal method will realize if a user sets the 'InputContainerList' property
inline void ParticleCombinerAlg::setupInputContainerList( Gaudi::Details::PropertyBase& /*prop*/ ) {
  m_setInCollKeyList = true;
  return;
}

/// This internal method will realize if a user sets the 'MissingETObject' property
inline void ParticleCombinerAlg::setupMissingETObject( Gaudi::Details::PropertyBase& /*prop*/ ) {
  m_setMetName = true;
  return;
}

/// This internal method will realize if a user sets the 'OutputContainer' property
inline void ParticleCombinerAlg::setupOutputContainer( Gaudi::Details::PropertyBase& /*prop*/ ) {
  m_setOutCollKey = true;
  return;
}

/// This internal method will realize if a user sets the 'WriteSplitOutputContainer' property
inline void ParticleCombinerAlg::setupWriteSplitOutputContainer( Gaudi::Details::PropertyBase& /*prop*/ ) {
  m_setWriteSplitAux = true;
  return;
}

/// This internal method will realize if a user sets the 'SetPdgId' property
inline void ParticleCombinerAlg::setupSetPdgId( Gaudi::Details::PropertyBase& /*prop*/ ) {
  m_setPdgId = true;
  return;
}

/// This internal method will realize if a user sets the 'SortConstituents' property
inline void ParticleCombinerAlg::setupSort( Gaudi::Details::PropertyBase& /*prop*/ ) {
  m_setSort = true;
  return;
}


#endif // EVENTUTILS_PARTICLECOMBINERALG_H
