/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef READEVENTBOOKKEEPERS_H
#define READEVENTBOOKKEEPERS_H

// *****************************************************************
//
// Algorithm:  ReadEventBookkeepers
// Author: David Cote <david.cote@cern.ch>
// Created: September 2008
//
// *********************************************************************

#include "GaudiKernel/Algorithm.h"
//#include <vector>
#include <string>
#include <stdint.h>
#include <fstream>
#include "EventBookkeeperMetaData/EventBookkeeperCollection.h"
#include "EventBookkeeperMetaData/EventBookkeeper.h"
#include "AthenaKernel/ICutFlowSvc.h"
#include "GaudiKernel/ServiceHandle.h"


class StoreGateSvc;
class IHistogram1D;
class TFile;
class TTree;
class TH1F;


class ReadEventBookkeepers:public Algorithm {
public:
  ReadEventBookkeepers(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();


protected:
  virtual ~ReadEventBookkeepers();


public:

void  SetCycle(){
  m_counter++;}

int  GetCycle(){
    return m_counter;}


protected:

  ServiceHandle<ICutFlowSvc> m_cutflowSvc; // for CutFlowSvc 
  StoreGateSvc* m_storeGate;    //cache the StoreGateSvc ptr for efficiency
  StoreGateSvc* m_metaStore;    //cache the StoreGateSvc ptr for efficiency
  StringProperty m_EvtBookCollName;
  StringProperty m_InputStreamName;  // input File name





 TFile* outputFile; // N-tuple output file
 TTree* auxTree; // Tree for auxilliary n-tuple
 TH1F* H1;
 int val;
 int m_counter;
 // std::vector<uint64_t> m_entries;
 // std::vector<std::string> m_name;
 char m_name[60];
 int m_entries;
 //   std::vector<EventBookkeeper*> m_vector;
};

#endif // end of READEVENTBOOKKEEPERS_H
