/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArFecLvTempDcsTest.h
 *
 * @brief This file contains an algorithm for testing lar conditions
 * data access
 *
 * @author D O Damazio  <Denis.Oliveira.Damazio@cern.ch>
 *
 * $Id: LArFecLvTempDcsTest.h,v 1.4 2009-01-23 08:56:00 gunal Exp $ */
#ifndef LARIOV_LARFECLVTEMPDCSTEST_H
#define LARIOV_LARFECLVTEMPDCSTEST_H

/**
 * @class LArFecLvTempDcsTest
 *
 * @brief Athena algorithm used for testing LAr conditions data access
 *
**/

#include "AthenaBaseComps/AthAlgorithm.h" 
#include "GaudiKernel/IIncidentListener.h" 
#include "GaudiKernel/ToolHandle.h"
#include "LArElecCalib/ILArFecLvTempDcsTool.h"

class LArFecLvTempDcsTest : public AthAlgorithm 
{

public:


    LArFecLvTempDcsTest(const std::string& name, ISvcLocator* pSvcLocator);
    ~LArFecLvTempDcsTest();

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

private:
   ToolHandle<ILArFecLvTempDcsTool> m_tool;

};
#endif 

