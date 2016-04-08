/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibStreamCnvSvc/MuonCalibStreamTestAlg.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGate.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonRDO/CscRawDataContainer.h"

MuonCalibStreamTestAlg::MuonCalibStreamTestAlg(const std::string& name, ISvcLocator* pSvcLocator):AthAlgorithm(name,pSvcLocator)
{
	m_byCont=1;
	m_byColl=0;
	declareProperty("ByCont", m_byCont );
	declareProperty("ByColl", m_byColl );
}

MuonCalibStreamTestAlg::~MuonCalibStreamTestAlg()
{
}

StatusCode MuonCalibStreamTestAlg::initialize()
{

  ATH_MSG_INFO( "in initialize()" );

  // retrieve the active store
  ATH_CHECK( serviceLocator()->service("ActiveStoreSvc", m_activeStore) );

  // Initialize the IdHelper
  ATH_CHECK( detStore()->retrieve(m_mdtIdHelper,"MDTIDHELPER") );

  return StatusCode::SUCCESS;
}

StatusCode MuonCalibStreamTestAlg::execute()
{

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in execute()" << endreq;
  

  std::string csckey = "CSCRDO";
  const CscRawDataContainer * cscRdoCont;

  StatusCode csc_read = evtStore()->retrieve(cscRdoCont, csckey);
  if(csc_read.isFailure()) {
    log << MSG::WARNING << " Cannot retrieve CscRawDataContainer " << endreq;
    //return StatusCode::SUCCESS;
  }
  else
  {

    log <<MSG::INFO << "Retrieved CscRawDataContainer" << endreq;
    /*
    CscRawDataContainer::const_iterator cscCollItr = cscRdoCont->begin();
    CscRawDataContainer::const_iterator cscCollEnd = cscRdoCont->end();
    for(; cscCollItr != cscCollEnd; cscCollItr++)
    */
  }

  std::string	key = "MDT_DriftCircles";
  const Muon::MdtPrepDataContainer* mdtPrds;
  StatusCode sc_read = evtStore()->retrieve(mdtPrds, key);
  if (sc_read.isFailure()) {
    log << MSG::WARNING << " Cannot retrieve MdtPrepDataContainer " << endreq;
    //return StatusCode::SUCCESS;
    //return sc_read;
  }
  else {

    log << MSG::DEBUG <<"****** Container size: " << mdtPrds->size()<<endreq;

    if(m_byCont!=0) {
      Muon::MdtPrepDataContainer::const_iterator it = mdtPrds->begin();
      Muon::MdtPrepDataContainer::const_iterator it_end = mdtPrds->end();

      for( ; it!=it_end; ++it ){
        log << MSG::DEBUG <<"********** Collections size: " << (*it)->size()<<endreq;
        Muon::MdtPrepDataCollection::const_iterator cit_begin = (*it)->begin();
        Muon::MdtPrepDataCollection::const_iterator cit_end = (*it)->end();
        Muon::MdtPrepDataCollection::const_iterator cit = cit_begin;   
        for( ; cit!=cit_end;++cit ) {
          const Muon::MdtPrepData* mdt = (*cit);
          //Identifier id = mdt->identify();
          mdt->dump(log);
        }
      } 
    }
  }
  if(m_byColl!=0) {
    const DataHandle<Muon::MdtPrepDataCollection> mdtCollection;
    const DataHandle<Muon::MdtPrepDataCollection> lastColl;

    if (evtStore()->retrieve(mdtCollection,lastColl) == StatusCode::SUCCESS) 
    {
      log << MSG::DEBUG << "collections retrieved" << std::endl;
      for ( ; mdtCollection != lastColl ; ++mdtCollection ) 
      {
        Muon::MdtPrepDataCollection::const_iterator cit_begin = (mdtCollection)->begin();
        Muon::MdtPrepDataCollection::const_iterator cit_end = (mdtCollection)->end();
        Muon::MdtPrepDataCollection::const_iterator cit = cit_begin;   
        for( ; cit!=cit_end;++cit ) // first
        {
          const Muon::MdtPrepData* mdt = (*cit);
          mdt->dump(log);
        }
      }
    }
  }

  std::string	key2 = "RPC_Measurements";
  const Muon::RpcPrepDataContainer* rpcPrds;
  StatusCode sc_read2 = evtStore()->retrieve(rpcPrds, key2);
  if (sc_read2.isFailure()) {
    log << MSG::WARNING << " Cannot retrieve RpcPrepDataContainer " << endreq;
    //return sc_read2;
  }
  else {

    log << MSG::DEBUG <<"****** Container size: " << rpcPrds->size()<<endreq;
    if(m_byCont!=0) {
      Muon::RpcPrepDataContainer::const_iterator it = rpcPrds->begin();
      Muon::RpcPrepDataContainer::const_iterator it_end = rpcPrds->end();

      for( ; it!=it_end; ++it ){
        log << MSG::DEBUG <<"********** Collections size: " << (*it)->size()<<endreq;
        Muon::RpcPrepDataCollection::const_iterator cit_begin = (*it)->begin();
        Muon::RpcPrepDataCollection::const_iterator cit_end = (*it)->end();
        Muon::RpcPrepDataCollection::const_iterator cit = cit_begin;   
        for( ; cit!=cit_end;++cit ) {
          const Muon::RpcPrepData* rpc = (*cit);
          rpc->dump(log);
        }
      } 
    }
  }

  if(m_byColl!=0) {
    const DataHandle<Muon::RpcPrepDataCollection> rpcCollection;
    const DataHandle<Muon::RpcPrepDataCollection> lastColl;

    if (evtStore()->retrieve(rpcCollection,lastColl) == StatusCode::SUCCESS) 
    {
      log << MSG::DEBUG << "rpc collections retrieved" << std::endl;
      for ( ; rpcCollection != lastColl ; ++rpcCollection ) 
      {
        Muon::RpcPrepDataCollection::const_iterator cit_begin = (rpcCollection)->begin();
        Muon::RpcPrepDataCollection::const_iterator cit_end = (rpcCollection)->end();
        Muon::RpcPrepDataCollection::const_iterator cit = cit_begin;   
        for( ; cit!=cit_end;++cit ) // first
        {
          const Muon::RpcPrepData* rpc = (*cit);
          rpc->dump(log);
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode MuonCalibStreamTestAlg::finalize()
{
   return StatusCode::SUCCESS;
}
