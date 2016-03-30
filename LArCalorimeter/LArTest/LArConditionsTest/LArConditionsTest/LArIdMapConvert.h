///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDITIONSTEST_LARIDMAPCONVERT_H
#define LARCONDITIONSTEST_LARIDMAPCONVERT_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArCabling/LArCablingService.h"

#include <iostream>

class LArIdMapConvert
  : public ::AthAlgorithm
{ 

 public: 

  /// Constructor with parameters: 
  LArIdMapConvert( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~LArIdMapConvert(); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

 private: 

  /// Default constructor: 
  LArIdMapConvert();
  void print (const HWIdentifier& hwid, std::ostream& out=std::cout); 

  const LArOnlineID* m_onlineID;
  const CaloCell_ID* m_caloCellID;

  ToolHandle<LArCablingService> m_cablingSvc;

}; 

#endif //> !LARCONDITIONSTEST_LARIDMAPCONVERT_H
