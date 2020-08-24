///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// BphysTrigDecoratorAlg.h 
// Header file for class BphysTrigDecoratorAlg
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef TRIGBPHYSMONITORING_BPHYSTRIGDECORATORALG_H
#define TRIGBPHYSMONITORING_BPHYSTRIGDECORATORALG_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODTracking/VertexContainer.h"

#include "TrigBphysMonitoring/IBphysTrigDiMuDecoratorTool.h"

class BphysTrigDecoratorAlg
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  BphysTrigDecoratorAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~BphysTrigDecoratorAlg(); 

  // Assignment operator: 
  //BphysTrigDecoratorAlg &operator=(const BphysTrigDecoratorAlg &alg); 

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
  BphysTrigDecoratorAlg();

  ToolHandleArray<IBphysTrigDiMuDecoratorTool> m_decorators{this, "Decorators", {}};
    
  /// Containers
  SG::ReadHandleKey<xAOD::VertexContainer> m_JpsiCandidatesKey{this, "JpsiContainerName", "JpsiCandidates"};


}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !TRIGBPHYSMONITORING_BPHYSTRIGDECORATORALG_H
