/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MYCPPFILTERTEST_H
#define MYCPPFILTERTEST_H

// *****************************************************************
//
// Algorithm:  myCppFilterTest
// Author: Joao Costa <joao.costa@cern.ch>
// Created: July 2010
//
// *********************************************************************

#include "AthenaBaseComps/AthFilterAlgorithm.h"
#include <string>
#include <stdint.h>
#include <fstream>
#include "AthenaKernel/ICutFlowSvc.h"
#include "GaudiKernel/ServiceHandle.h"

// Prevent checkreq warning.
#include "AthenaPoolKernel/IMetaDataTool.h"


//class StoreGateSvc;


class myCppFilterTest:public AthFilterAlgorithm {
public:
  myCppFilterTest(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();


protected:
  virtual ~myCppFilterTest();



private:
  CutIdentifier m_filterCutID;
  CutIdentifier m_cut1ID;
  CutIdentifier m_cut2ID;


  int m_counter;
  double m_cut1;
  double m_cut2;
  //double m_cycle;

};

#endif // end of MYCPPFILTERTEST_H
