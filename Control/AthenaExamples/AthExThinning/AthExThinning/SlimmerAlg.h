///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SlimmerAlg.h 
// Header file for class SlimmerAlg
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXTHINNING_SLIMMERALG_H
#define ATHEXTHINNING_SLIMMERALG_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

// fwd declares
class IThinningSvc;

namespace AthExThinning {

class SlimmerAlg
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  SlimmerAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~SlimmerAlg(); 

  // Assignment operator: 
  //SlimmerAlg &operator=(const SlimmerAlg &alg); 

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
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  SlimmerAlg();

  typedef ServiceHandle<IThinningSvc> IThinningSvc_t;
  /// handle to the thinning service
  IThinningSvc_t m_thinSvc;

  // Containers
  
  /// storegate key of athex particles
  std::string m_particlesName;

  /// storegate key of where to put fat-objects
  std::string m_fatObjectName;

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace AthExThinning
#endif //> !ATHEXTHINNING_SLIMMERALG_H
