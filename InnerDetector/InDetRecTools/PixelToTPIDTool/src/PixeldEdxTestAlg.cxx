/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelToTPIDTool/PixeldEdxTestAlg.h"

// IOVDbTest includes
#include "RegistrationServices/IIOVRegistrationSvc.h"

// Athena includes
#include "StoreGate/StoreGate.h"
#include "AthenaKernel/IAthenaOutputStreamTool.h"

// Gaudi includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/GaudiException.h" 
#include "GaudiKernel/IToolSvc.h"

// Event Info 
#include "EventInfo/EventIncident.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"

// AttributeList
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include <fstream>

#include "PathResolver/PathResolver.h"

/////////////////////////////////////////////////////////////////////////////

PixeldEdxTestAlg::PixeldEdxTestAlg(const std::string& name, ISvcLocator* pSvcLocator) :
        Algorithm(name, pSvcLocator),
        m_streamName("CondStream1"),
        m_tagID(""),
        m_sgSvc(0),
        m_detStore(0),
        m_regSvc(0),
        m_streamer(0)

{
    declareProperty("CalibrationFile", m_filename = "mcpar_signed_234.txt");
    declareProperty("TagID",             m_tagID);
    declareProperty("StreamName",        m_streamName);

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

PixeldEdxTestAlg::~PixeldEdxTestAlg()
{ }


StatusCode PixeldEdxTestAlg::testCallBack( IOVSVC_CALLBACK_ARGS_P( /*I*/,keys) ) { 
  // print out the keys we were given (for info)
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "PixeldEdxTestAlg::testCallBack callback invoked for keys:";
  for (std::list<std::string>::const_iterator itr=keys.begin(); itr!=keys.end(); ++itr) {
    log << *itr << " ";
  }
  log << endreq;
  return  StatusCode::SUCCESS;
}


StatusCode PixeldEdxTestAlg::initialize(){
    //StatusCode sc;
    MsgStream log(msgSvc(), name());
    log <<MSG::DEBUG <<"in initialize()" <<endreq;

    // Storegate
    StatusCode sc = service("StoreGateSvc", m_sgSvc);
    if (sc.isFailure()) {
	log << MSG::ERROR << "Unable to get the StoreGateSvc" << endreq;
	return sc;
    }

    // locate the conditions store ptr to it.
    sc = service("DetectorStore", m_detStore);
    if (!sc.isSuccess() || 0 == m_detStore)  {
	log <<MSG::ERROR <<"Could not find DetStore" <<endreq;
	return StatusCode::FAILURE;
    }

    // Get Output Stream tool for writing
	IToolSvc* toolSvc = 0;// Pointer to Tool Service
	sc = service("ToolSvc", toolSvc);
	if (sc.isFailure()) {
	    log << MSG::ERROR
		<< " Tool Service not found "
		<< endreq;
	    return StatusCode::FAILURE;
	}
	sc = toolSvc->retrieveTool("AthenaPoolOutputStreamTool", m_streamName, m_streamer);
	if (sc.isFailure()) {
	    log << MSG::INFO
		<< "Unable to find AthenaOutputStreamTool" 
		<< endreq;
	    return StatusCode::FAILURE;
	}  
sc = service("IOVRegistrationSvc", m_regSvc);
	        if (sc.isFailure()) {
	            log << MSG::INFO
                << "Unable to find IOVRegistrationSvc "
	                << endreq;
	            return StatusCode::FAILURE;
	        } 
	        else {
	            log << MSG::DEBUG << "Found IOVRegistrationSvc "  << endreq;
	        }
	        log  << MSG::INFO
	             << "Tag to be used: " << m_tagID
	             << endreq;
    
	sc = readWithBeginRun();
	if (sc.isFailure()) {
	    log << MSG::INFO
		<< "Unable to find read with BeginRun "
		<< endreq;
	    return StatusCode::FAILURE;
	}  
	else {
	    log << MSG::DEBUG << "Read with BeginRun "  << endreq;
	}

    
    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode PixeldEdxTestAlg::readWithBeginRun(){
    StatusCode status;
    MsgStream log(msgSvc(), name());
    log <<MSG::INFO <<"in readWithBeginRun()" <<endreq;

    // Get Run/Event/Time from EventSelector

    IProperty* propertyServer(0); 
    // Access EventSelector to check if run/event/time are being
    // explicitly set. This may be true for simulation
    status = serviceLocator()->service("EventSelector", propertyServer); 
    if (status != StatusCode::SUCCESS ) {
	log << MSG::ERROR 
	    << " Cannot get EventSelector " 
	    << endreq; 
	return status ;
    }

    // Get run/event/time if OverrideRunNumber flag is set
    BooleanProperty 	boolProperty("OverrideRunNumber", false);
    status = propertyServer->getProperty(&boolProperty);
    if (!status.isSuccess()) {
	log << MSG::ERROR << "unable to get OverrideRunNumber flag: found " 
	    << boolProperty.value()
	    << endreq;
	return status;
    }

    uint32_t event, run;
    uint64_t time;
    if (boolProperty.value()) {
	// Overriding run number, get run/event/time from EventSelector
	IntegerProperty  intProp("RunNumber", 0);
	status = propertyServer->getProperty(&intProp);
	if (!status.isSuccess()) {
	    log << MSG::ERROR << "unable to get RunNumber: found " 
		<< intProp.value()
		<< endreq;
	    return status;
	}
	else {
	    run = intProp.value();
	}
	intProp = IntegerProperty("FirstEvent", 0);
	status = propertyServer->getProperty(&intProp);
	if (!status.isSuccess()) {
	    log << MSG::ERROR << "unable to get event number: found " 
		<< intProp.value()
		<< endreq;
	    return status;
	}
	else {
	    event = intProp.value();
	}
	intProp = IntegerProperty("InitialTimeStamp", 0);
	status = propertyServer->getProperty(&intProp);
	if (!status.isSuccess()) {
	    log << MSG::ERROR << "unable to get time stamp: found " 
		<< intProp.value()
		<< endreq;
	    return status;
	}
	else {
	    time = intProp.value();
	}
    }
    else {
	log << MSG::DEBUG << "Override run number NOT set" << endreq;
	return StatusCode::SUCCESS;
    }

    // Now send BeginRun incident
    IIncidentSvc* incSvc;
    status = service( "IncidentSvc", incSvc );
    if (status.isFailure()) {
	log << MSG::ERROR << "Unable to get the IncidentSvc" << endreq;
	return status;
    }

    EventInfo evt(new EventID(run, event, time), new EventType);
    EventIncident evtInc(evt, name(), "BeginRun");
    incSvc->fireIncident( evtInc );

    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode PixeldEdxTestAlg::createCondObjects(){
    StatusCode sc;
    MsgStream log(msgSvc(), name());
    log <<MSG::INFO <<"in createCondObjects()" <<endreq;

      std::string file_name = PathResolver::find_file (m_filename, "DATAPATH");
  if (file_name.size()==0) {
    log << MSG::FATAL << "Could not find dEdx calibration file" << m_filename << endreq;
    return StatusCode::FAILURE;
  }
  std::ifstream f(file_name.c_str());

 
  std::string fit_type;
  std::string fun_type;
  std::string bb_type;
  f >> fit_type;
  f >> fun_type;
  f >> bb_type;

  std::string datastring;
  datastring=fit_type+" "+fun_type+" "+bb_type;
  if (fit_type=="0"){
    for (int i=0;i<NCLASS;i++){
      for (int j=0;j<NPAR;j++){
        std::string tmpstring;
	f >> tmpstring;
        if (tmpstring!="") datastring+=" "+tmpstring;
      }
    }
  } else {
    for (int i=0;i<2*NCLASS;i++){
      for (int j=0;j<NPAR;j++){
        std::string tmpstring;
	f >> tmpstring;
        if (tmpstring!="") datastring+=" "+tmpstring;
      }
    }
  } 

    // Create an attribute list collection
  
    // Use existing spec
    //CondAttrListCollection* attrListColl = new CondAttrListCollection(true);
    coral::AttributeListSpecification* attrSpec = new coral::AttributeListSpecification(); 
    attrSpec->extend("data", "string"); 

    // Add attr list
    AthenaAttributeList *attrList0=new AthenaAttributeList(*attrSpec);
    (*attrList0)["data"].setValue(datastring);
    //attrListColl->add(1, attrList0);


    sc = m_detStore->record(attrList0, "/PIXEL/PixdEdx");
    if (sc.isFailure()) {
	log <<MSG::ERROR <<"Could not record PixeldEdxCalib" <<endreq;
	return( StatusCode::FAILURE);
    }
    


    return StatusCode::SUCCESS;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 


StatusCode PixeldEdxTestAlg::printCondObjects(){
    StatusCode sc;
    MsgStream log(msgSvc(), name());
    log <<MSG::INFO <<"in printCondObjects()" <<endreq;


    const AthenaAttributeList*    attrList     = 0;

	// AttrList
	sc = m_detStore->retrieve(attrList, "/PIXEL/PixdEdx");
	if (sc.isFailure()) {
	    log <<MSG::ERROR <<"Could not retrieve PixdEdx" <<endreq;
	    // Using COOL, is failure
	    return( StatusCode::FAILURE);
	}
	if (0 == attrList) {
	    log <<MSG::ERROR <<"IOVDbTestAttrList ptr is 0" <<endreq;
	    return( StatusCode::FAILURE);
	}
	else {
	    log <<MSG::DEBUG <<"Retrieved IOVDbTestAttrList" <<endreq;
	}
  
	std::ostringstream attrStr1;
	attrList->print( attrStr1 );
	log << MSG::DEBUG << "Attribute list " << attrStr1.str() << endreq;



    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode PixeldEdxTestAlg::execute() {

    // There are different scenario for conditions data:
    // 
    //  A) Calculating and writing conditions
    //
    //    1) Loop over events and accumulate "averages" 
    //
    //    2) At the desired moment, e.g. after N events or at the end
    //       of the job, calculate the conditions data to be written
    //       out by creating the corresponding objects and store in the
    //       DetectorStore
    //
    //    3) Write out objects with the IAthenaOutputStreamTool - done
    //       in finalize.
    //
    //    4) Finally, one must "register" the objects written out in
    //       the IOV DB. This writes and IOV and a ref to each object,
    //       and is done in the finalize method.
    //
    //  B) Reading back in conditions data to analyse it
    //
    //    1) Aside from specifying the correct jobOptions, this is
    //       simply done by doing a standard StoreGate retrieve from
    //       the DetectorStore.


    MsgStream log(msgSvc(), name());
    log <<MSG::DEBUG <<" in execute()" <<endreq;


		
        log << MSG::DEBUG << "Creating condtions objects " << endreq;
        StatusCode sc = createCondObjects();
        if (sc.isFailure()) {
            log <<MSG::ERROR <<"Could not create cond objects" <<endreq;
            return( StatusCode::FAILURE);
        }

        //  Read objects from DetectorStore
        sc = printCondObjects();
        if (sc.isFailure()) {
            log <<MSG::ERROR <<"Could not print out cond objects" <<endreq;
            return( StatusCode::FAILURE);
        }
    
    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode PixeldEdxTestAlg::finalize(){
    MsgStream log(msgSvc(), name());
    log <<MSG::INFO <<"in finalize()" <<endreq;

        // Stream out and register objects here
        log << MSG::DEBUG << "Stream out objects directly " << endreq;
        StatusCode sc = streamOutCondObjects();
        if (sc.isFailure()) {
            log <<MSG::ERROR <<"Could not stream out objects" <<endreq;
            return( StatusCode::FAILURE);
        }
        log << MSG::DEBUG << "Streamed out OK " << endreq;
        sc = registerCondObjects();
       if (sc.isFailure()) {
        log <<MSG::ERROR <<"Could not register objects" <<endreq;
        return( StatusCode::FAILURE);
     }
    log << MSG::DEBUG << "Register OK " << endreq;  
    return StatusCode::SUCCESS;
}



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode 
PixeldEdxTestAlg::streamOutCondObjects(){
    // Get the messaging service, print where you are
    MsgStream log(messageService(), name());

    log << MSG::DEBUG << "entering streamOutCondObjects "  << endreq;
    StatusCode sc = m_streamer->connectOutput();
    if (sc.isFailure()) {
	log <<MSG::ERROR <<"Could not connect stream to output" <<endreq;
	return( StatusCode::FAILURE);
    }

/*    int npairs = 1;
    int index = 0;
    IAthenaOutputStreamTool::TypeKeyPairs  typeKeys(npairs);
	IAthenaOutputStreamTool::TypeKeyPair   dedxPair("PixeldEdxCalib", "");
	typeKeys[index] = dedxPair;
	++index;

    log << MSG::DEBUG <<"Stream out for pairs:" <<endreq;
    for (unsigned int i = 0; i < typeKeys.size(); ++i) {
        log << MSG::DEBUG << typeKeys[i].first << " " << typeKeys[i].second << " " 
            << endreq;
    }
    
    sc = m_streamer->streamObjects(typeKeys);
    if (sc.isFailure()) {
	log <<MSG::ERROR <<"Could not stream out PixeldEdxCalib" <<endreq;
	return( StatusCode::FAILURE);
    }
  */  
    sc = m_streamer->commitOutput();
    if (sc.isFailure()) {
	log <<MSG::ERROR <<"Could not commit output stream" <<endreq;
	return( StatusCode::FAILURE);
    }

    return StatusCode::SUCCESS;
}


StatusCode
PixeldEdxTestAlg::registerCondObjects(){
    // Get the messaging service, print where you are
    MsgStream log(messageService(), name());

    log << MSG::DEBUG << "entering registerCondObject "  << endreq;

    // Register the IOV DB with the conditions data written out
    StatusCode sc;


            // Using COOL, write out attrlist and collection of attrlists
            std::string tag = "no tag";
            if (m_tagID!="") {
                tag = m_tagID;
                sc = m_regSvc->registerIOV("AthenaAttributeList","/PIXEL/PixdEdx",m_tagID,IOVTime::MINRUN,IOVTime::MAXRUN,IOVTime::MINEVENT,IOVTime::MAXEVENT);
            } else {
                sc = m_regSvc->registerIOV("AthenaAttributeList","/PIXEL/PixdEdx","");
            }

            if (sc.isFailure()) {
                log <<MSG::ERROR <<"Could not register in IOV DB for AthenaAttributeList" <<endreq;
                return( StatusCode::FAILURE);
            }
            log << MSG::DEBUG << "registered AthenaAttributeList with " << tag << endreq;

    return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 


