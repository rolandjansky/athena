//-*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARCALIBTOOLS_LARFILLDSPCONFIG_H
#define LARCALIBTOOLS_LARFILLDSPCONFIG_H

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArCabling/LArCablingService.h"
#include "GaudiKernel/ToolHandle.h"

class LArOnlineID;

class LArFillDSPConfig: public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 
  /// Constructor with parameters: 
  LArFillDSPConfig( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~LArFillDSPConfig(); 

  // Athena algorithm's Hooks
  StatusCode  initialize();
  StatusCode  execute() {return StatusCode::SUCCESS;}
  StatusCode  finalize() {return StatusCode::SUCCESS;}
  virtual StatusCode  stop();

 private: 
  /// Default constructor: 
  LArFillDSPConfig();
  const LArOnlineID* m_onlineID;

  ToolHandle<LArCablingService> m_cablingSvc;


  std::string m_folderName;
  bool m_dump;
  bool m_lowmu;
}; 

#endif 
