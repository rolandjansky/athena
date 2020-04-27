///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// WriteThinnedData.h 
// Header file for class WriteThinnedData
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXTHINNING_WRITETHINNEDDATA_H 
#define ATHEXTHINNING_WRITETHINNEDDATA_H 

/** @class WriteThinnedData 
 *  A simple algorithm exercising the process of 
 *  thinning a container.
 *  This algorithm takes as input a @c AthExParticles container from 
 *  @c StoreGateSvc and thins part of the elements
 *  of this container with the aim (in real world algorithms) of reducing 
 *  dataset disk space.
 *  
 *  For the purpose of this exercise, the container is thinned on the base of a
 *  filter (a vector of booleans), keeping elements when the @c bool is @c true.
 *  This algorithm also exercises a two-steps thinning to make sure that
 *  multiple algorithms can cooperate to remove some elements in a multiple
 *  pass fashion (ie: alg1 removes particles with p_T < 10 GeV, then alg2
 *  removes particles with Chi2 < 0.2, etc...)
 */

// STL includes
#include <string>


// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "StoreGate/ThinningHandleKey.h"

// DataModel includes
#include "AthContainers/DataVector.h"

// AthExThinning includes
#include "AthExThinning/AthExIParticles.h"
#include "AthExThinning/AthExElephantino.h"
#include "AthExThinning/AthExDecay.h"

// Forward declaration
class AthExParticles;
//class AthExIParticles;
namespace SG {
  class ThinningDecisionBase;
}

namespace AthExThinning {

class WriteThinnedData : public ::AthAlgorithm
{ 
public: 
  /// Constructor with parameters: 
  WriteThinnedData( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~WriteThinnedData(); 

  // Assignment operator: 
  //WriteThinnedData &operator=(const WriteThinnedData &alg); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize() override;
  virtual StatusCode  execute() override;
  virtual StatusCode  finalize() override;


protected: 

  /// Default constructor: 
  WriteThinnedData();

  /// Exercise the following thinning tests:
  ///  [testName = "test1"]
  ///   retrieve a @c AthExParticles container
  ///   remove some of the @c AthExParticle using an 'AND' filter
  ///  [testName = "test2"]
  ///   retrieve a @c AthExParticles container
  ///   remove some of the @c AthExParticle using an 'OR' filter
  ///  [testName = "test3"]
  ///   retrieve a @c AthExIParticles container
  ///   remove some of the @c AthExIParticle using an 'AND' filter
  StatusCode test( const EventContext& ctx,
                   int testNum, const std::string& testName );

  /// Apply the real thinning
  StatusCode doThinningTest1( const EventContext& ctx,
                              const SG::ThinningHandleKey<AthExParticles>& particlesKey ) const;

  /// Apply the real thinning
  StatusCode doThinningTest2( const EventContext& ctx,
                              const SG::ThinningHandleKey<AthExParticles>& particlesKey ) const;

  /// Apply the real thinning
  StatusCode doThinningTest3( const EventContext& ctx,
                              const SG::ThinningHandleKey<AthExIParticles>& iparticlesKey ) const;


protected: 
  // Containers
  
  /// Particles input location
  StringProperty m_particlesName;

  SG::ThinningHandleKey<AthExParticles> m_particlesKey1
  { this, "ParticlesKey1", "", "" };
  SG::ThinningHandleKey<AthExParticles> m_particlesKey2
  { this, "ParticlesKey2", "", "" };

  SG::ReadHandleKeyArray<AthExIParticles> m_iparticlesKeys
  { this, "IParticlesKeys", {}, "" };

  SG::ThinningHandleKey<AthExIParticles> m_iparticlesKey3
  { this, "IParticlesKey3", "", "" };

  /// Decay input location
  StringProperty m_decayName
  { this, "Decay", "TwoBodyDecay", "Input location of Decay" };

  SG::ReadHandleKeyArray<AthExDecay> m_decayKeys
  { this, "DecayKeys", {}, "" };

  /// Elephantino input location
  StringProperty m_elephantinoName
  { this, "Elephantino", "PinkElephantino", "Input location of Elephantino" };

  SG::ReadHandleKeyArray<AthExElephantino> m_elephantinoKeys
  { this, "ElephantinoKeys", {}, "" };

  /// Filter to apply on the Particles. For each element of the filter being 
  /// @c true the corresponding particle will be kept.
  BooleanArrayProperty m_filter;

}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace AthExThinning

#endif //> ATHEXTHINNING_WRITETHINNEDDATA_H
