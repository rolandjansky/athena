/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArRegionSelector_LArRegionSelectorCheckOL_h
#define LArRegionSelector_LArRegionSelectorCheckOL_h

#include "AthenaBaseComps/AthAlgorithm.h"

#include "Identifier/Identifier.h" 
#include "Identifier/HWIdentifier.h" 
#include "CaloIdentifier/LArID_Exception.h" 

class CaloDetDescrManager;
class CaloTTDescrManager;
class CaloLVL1_ID;
class CaloTriggerTowerService; 

/////////////////////////////////////////////////////////////////////////////


/** @brief This algorithm is to be run in standalone mode for debug/exploration purposes of 
    the "public" service LArRegionSelectorTable */
class LArRegionSelectorCheckOL : public AthAlgorithm {

public:

  LArRegionSelectorCheckOL (const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  

private:

  StatusCode  checkOverlaps(); 
  StatusCode  testMaps(); 

  // Algorithm properties
  bool m_testTable;
  bool m_checkOVL;

  const CaloDetDescrManager*     m_ddman; 
  const CaloTTDescrManager*      m_ttman; 
  const CaloLVL1_ID*             m_TT_ID; 
        IToolSvc*                m_toolSvc;
        CaloTriggerTowerService*  m_ttSvc; 

};

#endif // LArRegionSelector_LArRegionSelectorCheckOL_h
