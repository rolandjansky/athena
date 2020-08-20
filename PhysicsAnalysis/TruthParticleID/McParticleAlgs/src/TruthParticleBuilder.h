///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TruthParticleBuilder.h 
// Header file for class TruthParticleBuilder
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEALGS_TRUTHPARTICLEBUILDER_H 
#define MCPARTICLEALGS_TRUTHPARTICLEBUILDER_H 

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Forward declaration
class ITruthParticleFilterTool;
class ITruthParticleCnvTool;

class TruthParticleBuilder : public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  TruthParticleBuilder( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~TruthParticleBuilder(); 

  // Assignment operator: 
  //TruthParticleBuilder &operator=(const TruthParticleBuilder &alg); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// Default constructor: 
  TruthParticleBuilder();

  /** Call-back method to configure the filter tool if needed (this is defined
   *  by the state of the "DoFiltering" property)
   */
  void setupFilterTool( Gaudi::Details::PropertyBase& doFiltering );

  /** Call-back method to configure the converter tool if needed (this is
   *  defined by the state of the "DoTruthParticles" property)
   */
  void setupCnvTool( Gaudi::Details::PropertyBase& doTruthParticles );

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  typedef ToolHandle<ITruthParticleFilterTool> FilterTool_t;
  /** Pointer to the filter algtool (which will create the filtered 
   *  McEventCollection)
   */
  FilterTool_t m_filterTool;

  typedef ToolHandle<ITruthParticleCnvTool> CnvTool_t;
  /** Pointer to the converter algtool which will create a
   *  TruthParticleContainer from the filtered McEventCollection
   */
  CnvTool_t m_cnvTool;

  /** Switch to build or not a filtered McEventCollection
   */
  BooleanProperty m_doFiltering;

  /** Switch to build or not the TruthParticleContainer from the 
   *  filtered GenEvent
   */
  BooleanProperty m_doTruthParticles;
}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> MCPARTICLEALGS_TRUTHPARTICLEBUILDER_H
