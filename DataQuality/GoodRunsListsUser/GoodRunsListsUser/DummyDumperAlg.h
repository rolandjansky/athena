// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LumiBlockComps_DummyDumperAlg_H
#define LumiBlockComps_DummyDumperAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include <vector>
#include <string>
#include "TObjString.h"

/////////////////////////////////////////////////////////////////////////////

class ILumiBlockMetaDataTool;
class StoreGateSvc;
class TTree;
class ITHistSvc;

class DummyDumperAlg : public AthAlgorithm 
{
 public:
  DummyDumperAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~DummyDumperAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

  ToolHandle< ILumiBlockMetaDataTool > m_LumiBlockMetaDataTool;
  std::vector<std::string> m_grlname;
  std::vector<TObjString> m_grlobj;

  StoreGateSvc* m_storeGate;
  TTree* m_tree;

  int m_eventCounter;
  int m_dummyInt;

  ITHistSvc* m_ths;
};

#endif // LumiBlockComps_DummyDumperAlg_H

