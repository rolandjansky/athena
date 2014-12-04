///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MenuAwareDQMonitoring.h 
// Header file for class MenuAwareDQMonitoring
// Author: Tatsiana Klimkovich <tklimk@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef TRIGHLTMONITORING_MENUAWAREDQMONITORING_H
#define TRIGHLTMONITORING_MENUAWAREDQMONITORING_H 1

#include <iostream>
#include <fstream>

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/DataHandle.h"

class StoreGateSvc;

class MenuAwareDQMonitoring
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  MenuAwareDQMonitoring( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~MenuAwareDQMonitoring(); 

  // Assignment operator: 
  //MenuAwareDQMonitoring &operator=(const MenuAwareDQMonitoring &alg); 

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

  StoreGateSvc*             m_sgSvc;
  StoreGateSvc*             m_detStore;
  StoreGateSvc*             m_metadataStore;
  std::string m_MCK;
  std::ofstream myfile;

  /// Default constructor: 
  MenuAwareDQMonitoring();

  /// Containers
  

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !TRIGHLTMONITORING_MENUAWAREDQMONITORING_H
