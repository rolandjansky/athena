///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// WriteThinnedData.h 
// Header file for class WriteThinnedData
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXTHINNING_WRITETHINNEDDATA_H 
#define ATHEXTHINNING_WRITETHINNEDDATA_H 

/** @class WriteThinnedData 
 *  A simple algorithm exercizing the process of 
 *  thinning a container.
 *  This algorithm takes as input a @c AthExParticles container from 
 *  @c StoreGateSvc and, using the @c IThinningSvc, removes part of the elements
 *  of this container with the aim (in real world algorithms) of reducing 
 *  dataset disk space.
 *  
 *  For the purpose of this exercize, the container is thinned on the base of a
 *  filter (a vector of booleans), keeping elements when the @c bool is @c true.
 *  This algorithm also exercizes a two-steps thinning to make sure that
 *  multiple algorithms can cooperate to remove some elements in a multiple
 *  pass fashion (ie: alg1 removes particles with p_T < 10 GeV, then alg2
 *  removes particles with Chi2 < 0.2, etc...)
 */

// STL includes
#include <string>

// HepMC / CLHEP includes

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

// DataModel includes
#include "AthContainers/DataVector.h"

// AthExThinning includes
#include "AthExThinning/AthExIParticles.h"

// Forward declaration
class IThinningSvc;
class AthExParticles;
//class AthExIParticles;

namespace AthExThinning {

class WriteThinnedData : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  WriteThinnedData( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~WriteThinnedData(); 

  // Assignment operator: 
  //WriteThinnedData &operator=(const WriteThinnedData &alg); 

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
  WriteThinnedData();

  /// Exercize the following thinning tests:
  ///  [testName = "test1"]
  ///   retrieve a @c AthExParticles container
  ///   remove some of the @c AthExParticle using an 'AND' filter
  ///  [testName = "test2"]
  ///   retrieve a @c AthExParticles container
  ///   remove some of the @c AthExParticle using an 'OR' filter
  ///  [testName = "test3"]
  ///   retrieve a @c AthExIParticles container
  ///   remove some of the @c AthExIParticle using an 'AND' filter
  StatusCode test( const std::string& testName );

  /// Apply the real thinning
  StatusCode doThinningTest1( const AthExParticles& particles );

  /// Apply the real thinning
  StatusCode doThinningTest2( const AthExParticles& particles );

  /// Apply the real thinning
  StatusCode doThinningTest3( const AthExIParticles& iparticles );

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// Pointer to IThinningSvc
  ServiceHandle<IThinningSvc> m_thinningSvc;

  // Containers
  
  /// Particles input location
  StringProperty m_particlesName;

  /// Decay input location
  StringProperty m_decayName;

  /// Elephantino input location
  StringProperty m_elephantinoName;

  /// Filter to apply on the Particles. For each element of the filter being 
  /// @c true the @c IThinningSvc will be used to keep the according 
  /// @c AthExParticle and remove all the other ones.
  BooleanArrayProperty m_filter;

}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace AthExThinning

#endif //> ATHEXTHINNING_WRITETHINNEDDATA_H
