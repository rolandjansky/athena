/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasAlg/AthenaHepMCInterface.h"
#include "G4AtlasAlg/TruthHepMCEventConverter.h"

#include "HepMC/GenEvent.h"
#include "FadsKinematics/FadsGeneratorT.h"
#include "FadsKinematics/GeneratorCenter.h"

#include "StoreGate/StoreGate.h"
#include "StoreGate/DataHandle.h"

// to permit access to StoreGate
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "GeneratorObjects/McEventCollection.h"

FADS::FadsGeneratorT<AthenaHepMCInterface> gen("AthenaHepMCInterface");

AthenaHepMCInterface::AthenaHepMCInterface(std::string s):FADS::FadsGenerator(s),p_sgSvc(0),m_log(0)
{
	FADS::GeneratorCenter::GetGeneratorCenter()->DeleteHepMCEvent(false);
	FADS::GeneratorCenter::GetGeneratorCenter()->SetHepMCEventConverter(
						new TruthHepMCEventConverter);
}
AthenaHepMCInterface::~AthenaHepMCInterface()
{
	log() << MSG::DEBUG << "\b the AthenaHepMCInterface is being destroyed "<<"\n"
          << " should you wish to use it again type: "<<"\n"
          << " /Generator/Select AthenaHepMCInterface "<<"\n"<<endreq;
	if (m_log) delete m_log;
}
void AthenaHepMCInterface::Initialize()
{}
void AthenaHepMCInterface::Terminate()
{}
HepMC::GenEvent* AthenaHepMCInterface::GenerateAnEvent()
{
	// Lazy initialization
	if (!p_sgSvc){
    	log() << MSG::DEBUG <<
	    " standard interface to StoreGate for retrieving HepMC events"<<endreq;
    	ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
	    StatusCode sc=svcLocator->service("StoreGateSvc",p_sgSvc);
    	if (sc.isFailure())
 		{
    	    log() << MSG::WARNING <<"AthenaHepMCInterface could not access StoreGate!!"<<endreq;
	    }
	}

	const DataHandle<McEventCollection> mcCollptr;
	StatusCode status=p_sgSvc->retrieve(mcCollptr,"GEN_EVENT");
	if (status.isSuccess() ) 
	{
		if (0==mcCollptr->size()){
			log() << MSG::WARNING << "No event in MC Event Collection!" << endreq;
			return 0;
		}

		McEventCollection::const_iterator it = mcCollptr->begin();
		
		// getting only the first event here.
		HepMC::GenEvent *p_evt = (*it);
		return p_evt;

  	} 
	else 
	{
    		log() << MSG::WARNING << "no McEventCollection found."  << endreq;
  	}
	return 0;
}

MsgStream AthenaHepMCInterface::log()
{
    if (m_log) return *m_log;
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    IMessageSvc* p_msgSvc = 0;
    if (svcLocator->service("MessageSvc", p_msgSvc).isFailure() || !p_msgSvc)
        std::cout << "AthenaHepMCInterface: Trouble getting the message service.  Should never happen.  Will crash now." << std::endl;
    m_log = new MsgStream(p_msgSvc,"AthenaHepMCInterface");
    return *m_log;
}

