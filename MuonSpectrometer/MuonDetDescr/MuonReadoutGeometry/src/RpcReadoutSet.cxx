/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 set of rpc modules 
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: RpcReadoutSet.cxx,v 1.3 2009-03-03 00:27:38 dwright Exp $
//<version>	$Name: not supported by cvs2svn $

#include "MuonReadoutGeometry/RpcReadoutSet.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

namespace MuonGM {

RpcReadoutSet::RpcReadoutSet(const MuonDetectorManager* mgr, Identifier id)
{
  m_mgr = mgr;
  m_helper = m_mgr->rpcIdHelper();
  Identifier idp = m_helper->parentID(id);
  // std::cerr << "RpcReadoutSet::RpcReadoutSet constructed from id " 
  //           <<m_helper->show_to_string(id) << std::endl;
  // std::cerr << "RpcReadoutSet::RpcReadoutSet parent id " 
  //           << m_helper->show_to_string(idp) << std::endl;
  if (!m_helper->is_rpc(idp)) m_id = id;
  else {
    m_id = idp;
    //std::cerr<<"RpcReadoutSet::RpcReadoutSet *** id redefined to be a module id "
    //         <<m_helper->show_to_string(m_id)<<std::endl;
  }
}

void RpcReadoutSet::setMgr(const MuonDetectorManager* mgr)
{
  //std::cerr<<"RpcReadoutSet::setMgr anything wrong here ??? "<<std::endl;
  m_mgr = mgr;
  //std::cerr<<"RpcReadoutSet::setMgr done"<<std::endl;
  m_helper = m_mgr->rpcIdHelper();
  //std::cerr<<"RpcReadoutSet::setMgr done"<<std::endl;
}

void RpcReadoutSet::setId(Identifier id)
{
  //std::cerr<<"RpcReadoutSet::setId anything wrong here ??? "<<std::endl;
  Identifier idp = m_helper->parentID(id);
  if (!m_helper->is_rpc(idp)) m_id = id;
  else {
      m_id = idp;
      //std::cout<<"Redefined the id to be a module id "<<m_helper->show_to_string(m_id);
  }
  //std::cerr<<"RpcReadoutSet::setId done"<<std::endl;
}

unsigned int RpcReadoutSet::NreadoutElements() const
{
  int nre = 0;
  //    for (int dbz = 1; dbz< 4; ++dbz)
  //     std::cerr<<" dbzmax, dbphimax = "<<m_helper->doubletZMax(m_id)<<" "
  //              <<m_helper->doubletPhiMax(m_id)<<std::endl;
  for (int dbz = 1; dbz<=m_helper->doubletZMax(m_id); ++dbz) {
    const RpcReadoutElement* rpcold = NULL;
    for (int dbp = 1; dbp<=m_helper->doubletPhiMax(m_id) ; ++dbp) {
      Identifier chid =  m_helper->channelID(m_id, dbz, dbp, 1, 0, 1);
      const RpcReadoutElement* rpc = m_mgr->getRpcReadoutElement(chid);
      //std::cerr<<" iz, ip "<<dbz<<" "<<dbp<<"  pointer "<<rpc<<std::endl;
      if (rpc != NULL) {
        if (dbp == 2 && rpc != rpcold ) {                    
          //std::cerr<<" componenti "<<rpc->getStationName()
          //         <<"/"<<rpc->getTechnologyName()<<std::endl;
          ++nre;
        } else if (dbp == 1) {
          //std::cerr<<" componenti "<<rpc->getStationName()
          //         <<"/"<<rpc->getTechnologyName()<<std::endl;
          ++nre;
          rpcold = rpc;
        }
      }
    }
  }
  return nre;
}

unsigned int RpcReadoutSet::NdoubletZ() const
{
    int ndbz = 0;
    for (int dbz = 1; dbz<=m_helper->doubletZMax(m_id); ++dbz) 
    {
        int dbp = 1;
        Identifier chid =  m_helper->channelID(m_id, dbz, dbp, 1, 0, 1);
        const RpcReadoutElement* rpc = m_mgr->getRpcReadoutElement(chid);
        if (rpc != NULL) ++ndbz;
    }
    return ndbz;
}

unsigned int RpcReadoutSet::NsegmentedDoubletZ() const
{
    int nc = 0;
    for (int dbz = 1; dbz<=m_helper->doubletZMax(m_id); ++dbz) 
    {
        int nre = 0;
        const RpcReadoutElement* rpcold = NULL;
        for (int dbp = 1; dbp<=m_helper->doubletPhiMax(m_id); ++dbp)
        {
            Identifier chid =  m_helper->channelID(m_id, dbz, dbp, 1, 0, 1);
            const RpcReadoutElement* rpc = m_mgr->getRpcReadoutElement(chid);
            if (rpc != NULL) {
                if (dbp == 2 && rpc != rpcold )
                {                    
                    std::cout<<" componenti "<<rpc->getStationName()
                             <<"/"<<rpc->getTechnologyName()<<std::endl;
                    ++nre;
                }
                else if (dbp == 1) {
                    std::cout<<" componenti "<<rpc->getStationName()
                             <<"/"<<rpc->getTechnologyName()<<std::endl;
                    ++nre;
                    rpcold = rpc;
                }
            }
        }
        if (nre > 1) nc++;
    }
    return nc;
}

unsigned int RpcReadoutSet::NPhimodules(int dbz) const
{
    int nre = 0;
    const RpcReadoutElement* rpcold = NULL;
    for (int dbp = 1; dbp<=m_helper->doubletPhiMax(m_id); ++dbp)
    {
        Identifier chid =  m_helper->channelID(m_id, dbz, dbp, 1, 0, 1);
        const RpcReadoutElement* rpc = m_mgr->getRpcReadoutElement(chid);
        if (rpc != NULL) 
        {
            if (dbp == 2 && rpc != rpcold )
            {                    
                //                std::cout<<" componenti "<<rpc->getStationName()
                //       <<"/"<<rpc->getTechnologyName()<<std::endl;
                ++nre;
            }
            else if (dbp == 1) {
                //std::cout<<" componenti "<<rpc->getStationName()
                //           <<"/"<<rpc->getTechnologyName()<<std::endl;
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
