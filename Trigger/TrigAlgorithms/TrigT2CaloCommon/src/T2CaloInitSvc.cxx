/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"

//#include "GaudiKernel/ISvcFactory.h"
//#include "GaudiKernel/SvcFactory.h"

#include "CaloDetDescr/CaloDetDescrManager.h"

#include "TrigT2CaloCommon/T2CaloInitSvc.h"

using namespace std;

// Factory for instantiation of service objects 
//static SvcFactory<T2CaloInitSvc> s_factory; 
//const ISvcFactory& T2CaloInitSvcFactory = s_factory; 

T2CaloInitSvc::T2CaloInitSvc(const std::string& name,ISvcLocator* sl) :
    AthService(name,sl) 
{
}

// queryInterface 
StatusCode T2CaloInitSvc::queryInterface(const InterfaceID& riid, void** ppvIF) 
{ 
  if(IT2CaloInitSvc::interfaceID().versionMatch(riid) ) 
    { 
        *ppvIF = (T2CaloInitSvc*)this; 
    } else { 
        MsgStream log(messageService(), name());
        log << MSG::DEBUG << name() << " cannot found the interface!" <<endmsg;
        return AthService::queryInterface(riid, ppvIF); 
    }
    return StatusCode::SUCCESS;
} 

StatusCode T2CaloInitSvc::initialize()
{
    StatusCode sc;

    MsgStream log(messageService(), name());
    log << MSG::DEBUG << name() << ": Start of run initialisation" << endmsg;

    sc = AthService::initialize();
    if ( sc.isFailure() ) return sc;
    
    //ISvcLocator* svcLoc = Gaudi::svcLocator( );
    StoreGateSvc* detStore = 0;
    const CaloDetDescrManager* theMgr;

    //sc = svcLoc->service( "DetectorStore", detStore );
    sc = service( "DetectorStore", detStore );
    if ( sc.isSuccess( ) ) {
      sc = detStore->retrieve( theMgr );    
      if(sc.isSuccess( )) {
	log << MSG::DEBUG << name() << ": successfully retrived CaloDetDescrManager" << endmsg;
      }
      else {
        log << MSG::ERROR << name() << ": failed to retrive CaloDetDescrManager" << endmsg;
        return sc;
      }
      
    }
    else {
      log << MSG::ERROR << name() << ": Could not locate DetectorStore" << endmsg;
      return sc;
    }

    return StatusCode::SUCCESS;
}

StatusCode T2CaloInitSvc::finalize() 
{
  //delete MuLUT::s_instance;
    return StatusCode::SUCCESS;
} 

