/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VALALGVARIABLES_H
#define VALALGVARIABLES_H

#include "AthenaKernel/MsgStreamMember.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

class TTree;
class MuonIdHelper;

namespace MuonGM {
  class MuonDetectorManager;
}

class ValAlgVariables
{
 public:
  ValAlgVariables(StoreGateSvc* evtStore,
                 const MuonGM::MuonDetectorManager* detManager,
                 TTree* tree,
                 const std::string& containername,
                 MSG::Level msglvl) :
    m_evtStore(evtStore),
    m_detManager(detManager),
    m_tree(tree),
    m_ContainerName(containername)
  {
  	  m_msg.get().setLevel(msglvl);
  }

  virtual ~ValAlgVariables() = default;

  virtual StatusCode initializeVariables() = 0;
  virtual StatusCode fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr) = 0;

  MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
  bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }

 protected:

  virtual void setHelper(const MuonIdHelper* ) = 0;

  virtual void deleteVariables() = 0;
  virtual StatusCode clearVariables()  = 0;

  StoreGateSvc*                      m_evtStore{};
  const MuonGM::MuonDetectorManager* m_detManager{};
  TTree*                             m_tree{};
  std::string                        m_ContainerName;
  mutable Athena::MsgStreamMember    m_msg;


};

#endif // VALALGVARIABLES_H
