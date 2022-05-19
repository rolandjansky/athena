/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VALALGVARIABLES_H
#define VALALGVARIABLES_H

#include "AthenaBaseComps/AthMessaging.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

class TTree;
class MuonIdHelper;

namespace MuonGM {
  class MuonDetectorManager;
}

class ValAlgVariables : public AthMessaging
{
 public:
  ValAlgVariables(StoreGateSvc* evtStore,
                 const MuonGM::MuonDetectorManager* detManager,
                 TTree* tree,
                 const std::string& containername,
                 MSG::Level msglvl) :
    AthMessaging(containername),
    m_evtStore(evtStore),
    m_detManager(detManager),
    m_tree(tree),
    m_ContainerName(containername)
  {
  	  setLevel(msglvl);
  }

  virtual ~ValAlgVariables() = default;

  virtual StatusCode initializeVariables() = 0;
  virtual StatusCode fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr) = 0;

 protected:

  virtual void setHelper(const MuonIdHelper* ) = 0;

  virtual void deleteVariables() = 0;
  virtual StatusCode clearVariables()  = 0;

  StoreGateSvc*                      m_evtStore{};
  const MuonGM::MuonDetectorManager* m_detManager{};
  TTree*                             m_tree{};
  std::string                        m_ContainerName;
};

#endif // VALALGVARIABLES_H
