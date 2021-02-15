/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 set of rpc modules 
 -----------------------------------------
 ***************************************************************************/

#include "MuonReadoutGeometry/RpcReadoutSet.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonIdHelpers/RpcIdHelper.h"

namespace MuonGM {

RpcReadoutSet::RpcReadoutSet(const MuonDetectorManager* mgr, Identifier id) 
{
  m_mgr = mgr;
  m_helper = m_mgr->rpcIdHelper();
  Identifier idp = m_helper->parentID(id);
  if (!m_helper->is_rpc(idp)) m_id = id;
  else {
    m_id = idp;
  }
}

void RpcReadoutSet::setMgr(const MuonDetectorManager* mgr)
{
  m_mgr = mgr;
  m_helper = m_mgr->rpcIdHelper();
}

void RpcReadoutSet::setId(Identifier id)
{
  Identifier idp = m_helper->parentID(id);
  if (!m_helper->is_rpc(idp)) m_id = id;
  else m_id = idp;
}

unsigned int RpcReadoutSet::NreadoutElements() const
{
  unsigned int nreadoutelemets = 0;
  for (int dbz = 1; dbz<=m_helper->doubletZMax(m_id); ++dbz) {
    const RpcReadoutElement* rpcold = nullptr;
    for (int dbp = 1; dbp<=m_helper->doubletPhiMax(m_id) ; ++dbp) {
      Identifier chid =  m_helper->channelID(m_id, dbz, dbp, 1, 0, 1);
      const RpcReadoutElement* rpc = m_mgr->getRpcReadoutElement(chid);
      if (rpc != nullptr) {
        if (dbp == 2 && rpc != rpcold ) {
          ++nreadoutelemets;
        } else if (dbp == 1) {
          ++nreadoutelemets;
          rpcold = rpc;
        }
      }
    }
  }
  return nreadoutelemets;
}

unsigned int RpcReadoutSet::NdoubletZ() const
{
  unsigned int ndoubletz = 0;
  for (int dbz = 1; dbz<=m_helper->doubletZMax(m_id); ++dbz)
  {
      int dbp = 1;
      Identifier chid =  m_helper->channelID(m_id, dbz, dbp, 1, 0, 1);
      const RpcReadoutElement* rpc = m_mgr->getRpcReadoutElement(chid);
      if (rpc != nullptr) ++ndoubletz;
  }
  return ndoubletz;
}

unsigned int RpcReadoutSet::NsegmentedDoubletZ() const
{
  unsigned int nsegmenteddoubletz = 0;
  for (int dbz = 1; dbz<=m_helper->doubletZMax(m_id); ++dbz)
  {
      int nre = 0;
      const RpcReadoutElement* rpcold = nullptr;
      for (int dbp = 1; dbp<=m_helper->doubletPhiMax(m_id); ++dbp)
      {
          Identifier chid =  m_helper->channelID(m_id, dbz, dbp, 1, 0, 1);
          const RpcReadoutElement* rpc = m_mgr->getRpcReadoutElement(chid);
          if (rpc != nullptr) {
              if (dbp == 2 && rpc != rpcold ) ++nre;
              else if (dbp == 1) {
                  ++nre;
                  rpcold = rpc;
              }
          }
      }
      if (nre > 1) nsegmenteddoubletz++;
  }
  return nsegmenteddoubletz;
}

unsigned int RpcReadoutSet::NPhimodules(int dbz) const
{
    int nre = 0;
    const RpcReadoutElement* rpcold = nullptr;
    for (int dbp = 1; dbp<=m_helper->doubletPhiMax(m_id); ++dbp)
    {
        Identifier chid =  m_helper->channelID(m_id, dbz, dbp, 1, 0, 1);
        const RpcReadoutElement* rpc = m_mgr->getRpcReadoutElement(chid);
        if (rpc != nullptr)
        {
            if (dbp == 2 && rpc != rpcold ) ++nre;
            else if (dbp == 1) {
                ++nre;
                rpcold = rpc;
            }
        }
        
    }
    return nre;
}

const RpcReadoutElement* RpcReadoutSet::readoutElement(Identifier id) const
{
  return m_mgr->getRpcReadoutElement(id);
}

const RpcReadoutElement* RpcReadoutSet::readoutElement(int dbz, int dbp) const
{
  Identifier chid =  m_helper->channelID(m_id, dbz, dbp, 1, 0, 1);
  return readoutElement(chid);
}

} // namespace MuonGM
