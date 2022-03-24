// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GOODRUNSLISTSUSER_DUMMYDUMPERALG_H
#define GOODRUNSLISTSUSER_DUMMYDUMPERALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include <vector>
#include <string>
#include "TObjString.h"

/////////////////////////////////////////////////////////////////////////////

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

  std::vector<std::string> m_grlname;
  std::vector<TObjString> m_grlobj;

  TTree* m_tree;

  int m_eventCounter;
  int m_dummyInt;

  ITHistSvc* m_ths;
};

#endif

