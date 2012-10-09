/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _BSFILTER_H 
#define _BSFILTER_H 

#include <string>
#include <vector>
#include <map>
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
class StoreGateSvc;

class BSFilter : public AthAlgorithm
{ 
 public: 

  BSFilter( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~BSFilter();
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

 protected: 
  int All, pass, EventCounter;
  int m_trigbit;
  std::string m_filterfile;
  class filterinfo{
  public:
    int trig, nvtx; double dt;
    int magic;
    filterinfo(){magic=99999;}
  };
  std::map< int, std::map < int, filterinfo > > filtermap;
  MsgStream mLog;
  StoreGateSvc* m_storeGate;

  FILE *efile; 
  std::string m_EventIdFile;
}; 
#endif 
