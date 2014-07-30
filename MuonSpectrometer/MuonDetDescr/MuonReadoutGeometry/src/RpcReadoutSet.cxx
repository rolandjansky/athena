/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 set of rpc modules 
 -----------------------------------------
 Copyright (C) 2004 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: RpcReadoutSet.cxx,v 1.3 2009-03-03 00:27:38 dwright Exp $
//<version>	$Name: not supported by cvs2svn $

#include "MuonReadoutGeometry/RpcReadoutSet.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

namespace MuonGM {

RpcReadoutSet::RpcReadoutSet(const MuonDetectorManager* mgr, Identifier id)
{
  _mgr = mgr;
  _helper = _mgr->rpcIdHelper();
  Identifier idp = _helper->parentID(id);
  // std::cerr << "RpcReadoutSet::RpcReadoutSet constructed from id " 
  //           <<_helper->show_to_string(id) << std::endl;
  // std::cerr << "RpcReadoutSet::RpcReadoutSet parent id " 
  //           << _helper->show_to_string(idp) << std::endl;
  if (!_helper->is_rpc(idp)) _id = id;
  else {
    _id = idp;
    //std::cerr<<"RpcReadoutSet::RpcReadoutSet *** id redefined to be a module id "
    //         <<_helper->show_to_string(_id)<<std::endl;
  }
}

void RpcReadoutSet::setMgr(const MuonDetectorManager* mgr)
{
  //std::cerr<<"RpcReadoutSet::setMgr anything wrong here ??? "<<std::endl;
  _mgr = mgr;
  //std::cerr<<"RpcReadoutSet::setMgr done"<<std::endl;
  _helper = _mgr->rpcIdHelper();
  //std::cerr<<"RpcReadoutSet::setMgr done"<<std::endl;
}

void RpcReadoutSet::setId(Identifier id)
{
  //std::cerr<<"RpcReadoutSet::setId anything wrong here ??? "<<std::endl;
  Identifier idp = _helper->parentID(id);
  if (!_helper->is_rpc(idp)) _id = id;
  else {
      _id = idp;
      //std::cout<<"Redefined the id to be a module id "<<_helper->show_to_string(_id);
  }
  //std::cerr<<"RpcReadoutSet::setId done"<<std::endl;
}

unsigned int RpcReadoutSet::NreadoutElements() const
{
  int nre = 0;
  //    for (int dbz = 1; dbz< 4; ++dbz)
  //     std::cerr<<" dbzmax, dbphimax = "<<_helper->doubletZMax(_id)<<" "
  //              <<_helper->doubletPhiMax(_id)<<std::endl;
  for (int dbz = 1; dbz<=_helper->doubletZMax(_id); ++dbz) {
    const RpcReadoutElement* rpcold = NULL;
    for (int dbp = 1; dbp<=_helper->doubletPhiMax(_id) ; ++dbp) {
      Identifier chid =  _helper->channelID(_id, dbz, dbp, 1, 0, 1);
      const RpcReadoutElement* rpc = _mgr->getRpcReadoutElement(chid);
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
    for (int dbz = 1; dbz<=_helper->doubletZMax(_id); ++dbz) 
    {
        int dbp = 1;
        Identifier chid =  _helper->channelID(_id, dbz, dbp, 1, 0, 1);
        const RpcReadoutElement* rpc = _mgr->getRpcReadoutElement(chid);
        if (rpc != NULL) ++ndbz;
    }
    return ndbz;
}

unsigned int RpcReadoutSet::NsegmentedDoubletZ() const
{
    int nc = 0;
    for (int dbz = 1; dbz<=_helper->doubletZMax(_id); ++dbz) 
    {
        int nre = 0;
        const RpcReadoutElement* rpcold = NULL;
        for (int dbp = 1; dbp<=_helper->doubletPhiMax(_id); ++dbp)
        {
            Identifier chid =  _helper->channelID(_id, dbz, dbp, 1, 0, 1);
            const RpcReadoutElement* rpc = _mgr->getRpcReadoutElement(chid);
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
    for (int dbp = 1; dbp<=_helper->doubletPhiMax(_id); ++dbp)
    {
        Identifier chid =  _helper->channelID(_id, dbz, dbp, 1, 0, 1);
        const RpcReadoutElement* rpc = _mgr->getRpcReadoutElement(chid);
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
  return _mgr->getRpcReadoutElement(id);
}

const RpcReadoutElement* RpcReadoutSet::readoutElement(int dbz, int dbp) const
{
  Identifier chid =  _helper->channelID(_id, dbz, dbp, 1, 0, 1);
  return readoutElement(chid);
}

} // namespace MuonGM
