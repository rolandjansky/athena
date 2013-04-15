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
#include "EventBookkeeperMetaData/EventBookkeeperCollection.h"
#include "EventBookkeeperMetaData/EventBookkeeper.h"
#include "AthenaKernel/ICutFlowSvc.h"
#include "GaudiKernel/ServiceHandle.h"


//class StoreGateSvc;


class myCppFilterTest:public AthFilterAlgorithm {
public:
  myCppFilterTest(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();


protected:
  virtual ~myCppFilterTest();


public:



protected:

  ServiceHandle<ICutFlowSvc> m_cutflowSvc; // for CutFlowSvc 
  //  StoreGateSvc* m_storeGate;    //cache the StoreGateSvc ptr for efficiency
  //  StoreGateSvc* m_metaStore;    //cache the StoreGateSvc ptr for efficiency
  StringProperty m_EvtBookCollName;
  StringProperty m_InputStreamName;  // input File name


private:
  CutIdentifier filterCutID;
  CutIdentifier cut1ID;
  CutIdentifier cut2ID;


  int m_counter;
  double m_cut1;
  double m_cut2;
  double m_cycle;

};

#endif // end of MYCPPFILTERTEST_H
