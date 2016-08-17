/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSim/LArFakeHitProducer.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IDataProviderSvc.h"


#include "LArSimEvent/LArHit.h"
#include "LArSimEvent/LArHitContainer.h"

#include "StoreGate/StoreGate.h" 


// Constructor
LArFakeHitProducer::LArFakeHitProducer(const std::string& name, ISvcLocator* pSvcLocator ) :
 LArHitProducer(name , pSvcLocator)
{
  
}
//Initialize method
StatusCode LArFakeHitProducer::hitConstruction(){

    ATH_MSG_DEBUG("LArFakeHitProducer "<< this->name()<<"hitConstrution" );

    LArHitContainer* cont = hitContainer(); 
    for (int i=1;i<100;++i)
    {
	Identifier id( (unsigned int)i ) ; 
	double t = i; 
	double e = i; 
	LArHit* h = new LArHit(id,e,t); 
	cont->push_back(h);         
    } 

    ATH_MSG_DEBUG(" Filled the container, size = "<< cont->size() );


    ATH_CHECK( StoreGate::instance().record(cont,"LArHitFake") );
    return StatusCode::SUCCESS;
}

