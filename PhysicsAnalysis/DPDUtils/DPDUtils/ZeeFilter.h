/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _FAKEFILTER_H 
#define _FAKEFILTER_H 

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Forward declarations
class StoreGateSvc;
class IThinningSvc;
class ElectronContainer;

//Actual class 

class ZeeFilter : public AthAlgorithm
{ 
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  // Constructor with parameters: 
  ZeeFilter( const std::string& name, ISvcLocator* pSvcLocator );

  // Destructor: 
  virtual ~ZeeFilter(); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

  
  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 
  int m_All;
  int m_pass;
 
  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 
  int m_EventCounter;
  //Collection Names
  std::string m_ElectronCollectionName;
  //cuts
  double m_ElectronEtCut;
  double m_ElectronEtaCut;
  IThinningSvc* m_ThinningSvc;
}; 

#endif 
