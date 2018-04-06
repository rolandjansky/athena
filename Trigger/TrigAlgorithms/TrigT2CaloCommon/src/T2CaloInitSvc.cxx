/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"

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
        ATH_MSG_DEBUG( name() << " cannot found the interface!"  );
        return AthService::queryInterface(riid, ppvIF); 
    }
    return StatusCode::SUCCESS;
} 

StatusCode T2CaloInitSvc::initialize()
{
    ATH_MSG_DEBUG( name() << ": Start of run initialisation"  );

    ATH_CHECK( AthService::initialize() );
    
    //ISvcLocator* svcLoc = Gaudi::svcLocator( );
    StoreGateSvc* detStore = 0;
    const CaloDetDescrManager* theMgr;

    //sc = svcLoc->service( "DetectorStore", detStore );
    ATH_CHECK( service( "DetectorStore", detStore ) );
    ATH_CHECK( detStore->retrieve( theMgr ) );
    ATH_MSG_DEBUG( name() << ": successfully retrived CaloDetDescrManager"  );

    return StatusCode::SUCCESS;
}

StatusCode T2CaloInitSvc::finalize() 
{
  //delete MuLUT::s_instance;
    return StatusCode::SUCCESS;
} 

