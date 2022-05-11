// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GoodRunsListsUser_GoodRunsListSelectorAlg_H
#define GoodRunsListsUser_GoodRunsListSelectorAlg_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include <vector>
#include <string>

/////////////////////////////////////////////////////////////////////////////

class IGoodRunsListSelectorTool;

class GoodRunsListSelectorAlg : public AthAlgorithm 
{
 public:
  GoodRunsListSelectorAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~GoodRunsListSelectorAlg(); 

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  ToolHandle< IGoodRunsListSelectorTool > m_GoodRunsListSelectorTool;

  unsigned int m_counter;
  unsigned int m_accept;
  unsigned int m_prescale;

  std::vector<std::string> m_grlnameVec;
  std::vector<std::string> m_brlnameVec;
  std::string m_myname;
};

#endif // GoodRunsListsUser_GoodRunsListSelectorAlg_H

