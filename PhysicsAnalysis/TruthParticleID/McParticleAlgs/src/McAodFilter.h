///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// McAodFilter.h 
// Header file for class McAodFilter
// Author: S.Binet<binet@cern.ch>
// Purpose: Builds a filtered HepMC::GenEvent from decay vertices pattern
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEALGS_MCAODFILTER_H 
#define MCPARTICLEALGS_MCAODFILTER_H 

/** \class McAodFilter is an Algorithm which exercises the use of 
 *  McVtxFilterTool and TruthParticleCnvTool.
 *  It does more or less the same job of SpclMCAodBuilder but has a slightly 
 *  different philosophy : where SpclMCAodBuilder takes a new McEventCollection
 *  and removes the particles which are not "special", McAodFilter builds a 
 *  McEventCollection from scratch with the particles previously flagged 
 *  "special" by McVtxFilterTool (so it merely records particles connected to 
 *  "special" vertices).
 *  Eventually it calls (or not, depending on a property switch) the 
 *  TruthParticleCnvTool to create a TruthParticleContainer from this filtered 
 *  McEventCollection.
 */

// STL includes
#include <string>

// HepMC / CLHEP includes

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// McParticleUtils includes
#include "McParticleUtils/McVtxFilter.h"

// Forward declaration
class ITruthParticleCnvTool;
class IMcVtxFilterTool;

class McAodFilter : public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  // Constructor with parameters: 
  McAodFilter( const std::string& name, ISvcLocator* pSvcLocator );

  // Destructor: 
  virtual ~McAodFilter(); 

  // Assignment operator: 
  //McAodFilter &operator=(const McAodFilter &alg); 

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
  McAodFilter();

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** Switch to build or not the filtered GenEvent
   */
  BooleanProperty m_doGenAod;

  typedef ToolHandle<IMcVtxFilterTool> IMcVtxFilterTool_t;
  /** Tool to filter an McEventCollection according to some DecayPattern
   */
  IMcVtxFilterTool_t m_mcVtxFilterTool;

  /** Switch to build or not the TruthParticleContainer from the 
   *  filtered GenEvent
   */
  BooleanProperty m_doTruthParticles;

  typedef ToolHandle<ITruthParticleCnvTool> CnvTool_t;
  /** Tool to convert a GenEvent into a TruthParticleContainer
   */
  CnvTool_t m_truthParticleCnvTool;
}; 

#endif //> MCPARTICLEALGS_MCAODFILTER_H
