/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_TestCablingAlg_h
#define SCT_TestCablingAlg_h
/**
 * @file SCT_TestCablingAlg.h
 * Header file for Algorithm to test cabling
 * @author Shaun Roe
 * @date 20 October, 2008
 **/

#include "AthenaBaseComps/AthAlgorithm.h"

//package includes
#include "SCT_Cabling/ISCT_CablingTool.h"

//Gaudi
#include "GaudiKernel/ToolHandle.h"

//STL
#include <string>

class ISvcLocator;
class StatusCode;
class SCT_ID;

/**
 * SCT_TestCablingAlg exercises the routines of the SCT cabling service
 **/
class SCT_TestCablingAlg:public AthAlgorithm {
  public:
    SCT_TestCablingAlg(const std::string& name, ISvcLocator* pSvcLocator);
    ~SCT_TestCablingAlg();
    // Standard Gaudi functions
    StatusCode initialize(); //!< Gaudi initialiser
    StatusCode execute();    //!< Gaudi executer
    StatusCode finalize();   //!< Gaudi finaliser

  private:
    ToolHandle<ISCT_CablingTool> m_cablingTool{this, "SCT_CablingTool", "SCT_CablingTool", "Tool to retrieve SCT Cabling"};
    const SCT_ID* m_idHelper; //!< helper for offlineId/hash conversions
    std::string coordString(const Identifier& offlineId);

};
#endif
