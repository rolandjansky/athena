/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SimplePOSIXTimeKeeperSvc.h"
using namespace std;

SimplePOSIXTimeKeeperSvc::~SimplePOSIXTimeKeeperSvc(){}

SimplePOSIXTimeKeeperSvc::SimplePOSIXTimeKeeperSvc(const std::string& name,
						   ISvcLocator* svc)
  : POSIXTimeKeeper(), AthService(name,svc), m_allocTime(999999999)
{
  declareProperty("AllocTime",  m_allocTimeProp=99999999);
  m_allocTimeProp.declareUpdateHandler(&SimplePOSIXTimeKeeperSvc::decodeAllocTime, this);
}

StatusCode  
SimplePOSIXTimeKeeperSvc::queryInterface(const InterfaceID& riid, 
					 void** ppvInterface) 
{
    if ( ITimeKeeper::interfaceID().versionMatch(riid) )    {
        *ppvInterface = (ITimeKeeper*)this;
    }
    else  {
	// Interface is not directly available: try out a base class
	return AthService::queryInterface(riid, ppvInterface);
    }
    addRef();
    return StatusCode::SUCCESS;
}

void 
SimplePOSIXTimeKeeperSvc::decodeAllocTime(Gaudi::Details::PropertyBase&) {
  m_allocTime = 100 * m_allocTimeProp;
}


time_t
SimplePOSIXTimeKeeperSvc::allocTime() const {
  return m_allocTime;
}

bool
SimplePOSIXTimeKeeperSvc::nextIter() {
  bool doNext(TimeKeeper::nextIter());
  //we override to put out a DEBUG message per event or an INFO message at
  //the end
  if ((msg().level() == MSG::INFO && timeOver()) ||
      msg().level()  < MSG::INFO) {
    msg() << msg().level() << *this << endmsg;
  }
  //FIXME       log.level()  < MSG::INFO)  log << *this << endmsg;
  return doNext;
}


StatusCode 
SimplePOSIXTimeKeeperSvc::initialize()
{
  ATH_MSG_INFO ("Initializing " << name()
		<< " - package version " << PACKAGE_VERSION);
  return StatusCode::SUCCESS;
}

StatusCode 
SimplePOSIXTimeKeeperSvc::finalize()
{
  ATH_MSG_INFO (*this);
  return StatusCode::SUCCESS;
}

