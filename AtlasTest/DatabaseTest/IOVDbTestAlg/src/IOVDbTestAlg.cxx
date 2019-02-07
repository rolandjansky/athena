/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IOVDbTestAlg.cxx,v 1.39 2009-03-30 12:10:15 ivukotic Exp $

#include "IOVDbTestAlg/IOVDbTestAlg.h"

// IOVDbTest includes
#include "RegistrationServices/IIOVRegistrationSvc.h"
#include "IOVDbTestConditions/IOVDbTestMDTEleMap.h"
#include "IOVDbTestConditions/IOVDbTestMDTEleMapColl.h"
#include "IOVDbTestConditions/IOVDbTestAmdbCorrection.h"

// Athena includes
#include "StoreGate/StoreGate.h"
#include "AthenaKernel/IAthenaOutputStreamTool.h"

// Gaudi includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/GaudiException.h" 
#include "GaudiKernel/IToolSvc.h"

// Event Incident 
#include "EventInfo/EventIncident.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

// AttributeList
#include "CoralBase/Attribute.h"
#include "CoralBase/Blob.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

// for online testing
#include <sys/ipc.h>
#include <sys/msg.h>


/////////////////////////////////////////////////////////////////////////////

IOVDbTestAlg::IOVDbTestAlg(const std::string& name, ISvcLocator* pSvcLocator) :
        AthAlgorithm(name, pSvcLocator),
        m_writeCondObjs(false),
        m_regIOV(false),
        m_readWriteCool(false),
        m_twoStepWriteReg(false),
        m_createExtraChans(false),
        m_readInInit(false),
        m_writeOnlyCool(false),
        m_fancylist(false),
        m_printLB(false),
        m_writeNewTag(false),
        m_readNewTag(false),
	m_noStream(false),
        m_regTime(0),
        m_streamName("CondStream1"),
        m_tagID(""),
        m_sgSvc(0),
        m_detStore(0),
        m_evt(0),
        m_regSvc(0),
        m_streamer(0)

{
    declareProperty("WriteCondObjs",     m_writeCondObjs);
    declareProperty("RegisterIOV",       m_regIOV);
    declareProperty("ReadWriteCool",     m_readWriteCool);
    declareProperty("WriteOnlyCool",     m_writeOnlyCool);
    declareProperty("TwoStepWriteReg",   m_twoStepWriteReg);
    declareProperty("StreamName",        m_streamName);
    declareProperty("CreateExtraChanns", m_createExtraChans);
    declareProperty("NameChanns",        m_nameChans);
    declareProperty("ReadInInit",        m_readInInit);
    declareProperty("FancyList",         m_fancylist);
    declareProperty("PrintLB",           m_printLB);
    declareProperty("WriteNewTag",       m_writeNewTag);
    declareProperty("ReadNewTag",        m_readNewTag);
    declareProperty("RegTime",           m_regTime);  // Register time in sec
    declareProperty("TagID",             m_tagID);
    declareProperty("run",               m_run);
    declareProperty("online",            m_online);
    declareProperty("NoStream",          m_noStream);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

IOVDbTestAlg::~IOVDbTestAlg()
{ }

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
void IOVDbTestAlg::waitForSecond(){
    struct mymsgbuf {
        long mtype;
        char mtext[80];
    };

    key_t key;
    int   msgqueue_id;
    struct mymsgbuf qbuf;
	
    /* Create unique key via call to ftok() */
    key = ftok(".", 'm');
	
    /* Open the queue */
    while((msgqueue_id = msgget(key, 0660)) == -1) {
        printf("waiting for message here.\n");
        sleep(3);
    }
    printf("Recieving a message ...\n");
	
    qbuf.mtype = 123;
    msgrcv(msgqueue_id, (struct msgbuf *)(&qbuf), 80, 123, 0);
    
    printf("Type: %ld Text: %s\n", qbuf.mtype, qbuf.mtext);
	
    msgctl(msgqueue_id, IPC_RMID, 0); // clearing the message
	
}

StatusCode IOVDbTestAlg::testCallBack( IOVSVC_CALLBACK_ARGS_P( i, keys) ) { 
  // print out the keys we were given (for info)
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "IOVDbTestAlg::testCallBack callback invoked for keys: i = " << i << " ";
  for (std::list<std::string>::const_iterator itr=keys.begin(); itr!=keys.end(); ++itr) {
    log << *itr << " ";
  }
  log << endmsg;
  return  StatusCode::SUCCESS;
}


StatusCode IOVDbTestAlg::initialize(){
    //StatusCode sc;
    MsgStream log(msgSvc(), name());
    log <<MSG::DEBUG <<"in initialize()" <<endmsg;

    // Storegate
    StatusCode sc = service("StoreGateSvc", m_sgSvc);
    if (sc.isFailure()) {
	log << MSG::ERROR << "Unable to get the StoreGateSvc" << endmsg;
	return sc;
    }

    // locate the conditions store ptr to it.
    sc = service("DetectorStore", m_detStore);
    if (!sc.isSuccess() || 0 == m_detStore)  {
	log <<MSG::ERROR <<"Could not find DetStore" <<endmsg;
	return StatusCode::FAILURE;
    }

    // Get Output Stream tool for writing
    if (m_writeCondObjs) {
	IToolSvc* toolSvc = 0;// Pointer to Tool Service
	StatusCode sc = service("ToolSvc", toolSvc);
	if (sc.isFailure()) {
	    log << MSG::ERROR
		<< " Tool Service not found "
		<< endmsg;
	    return StatusCode::FAILURE;
	}
	sc = toolSvc->retrieveTool("AthenaOutputStreamTool", m_streamName, m_streamer);
	if (sc.isFailure()) {
	    log << MSG::INFO
		<< "Unable to find AthenaOutputStreamTool" 
		<< endmsg;
	    return StatusCode::FAILURE;
	}  
    }
    
    // Get the IOVRegistrationSvc when needed
    if (m_regIOV) {
	sc = service("IOVRegistrationSvc", m_regSvc);
	if (sc.isFailure()) {
	    log << MSG::INFO
		<< "Unable to find IOVRegistrationSvc "
		<< endmsg;
	    return StatusCode::FAILURE;
	}  
	else {
	    log << MSG::DEBUG << "Found IOVRegistrationSvc "  << endmsg;
	}
        log  << MSG::INFO 
             << "Tag to be used: " << m_tagID 
             << endmsg;
    }

    if (m_readInInit) {
	sc = readWithBeginRun();
	if (sc.isFailure()) {
	    log << MSG::INFO
		<< "Unable to find read with BeginRun "
		<< endmsg;
	    return StatusCode::FAILURE;
	}  
	else {
	    log << MSG::DEBUG << "Read with BeginRun "  << endmsg;
	}
    }

    // register callbacks for test of online change of constants
    if (!m_online) return StatusCode::SUCCESS;
    
    const DataHandle<IOVDbTestMDTEleMap> mdtelemap;
    if (StatusCode::SUCCESS==m_detStore->regFcn(&IOVDbTestAlg::testCallBack, this, mdtelemap, "/IOVDbTest/IOVDbTestMDTEleMap")) {
        log << MSG::INFO << "Registered callback for IOVDbTestAlg::testCallBack" << endmsg;
    } else {
        log << MSG::ERROR << "Register callback failed" << endmsg;
        return StatusCode::FAILURE;
    }
    
    
    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode IOVDbTestAlg::readWithBeginRun(){
    StatusCode status;
    MsgStream log(msgSvc(), name());
    log <<MSG::INFO <<"in readWithBeginRun()" <<endmsg;

    // As a result of the restructuring the EventIncident class (dropping the reference to EventInfo)
    // the old mechanism of overriding run&event&time is no longer working.
    // If we need this functionality, then we need to find a new way of implementing it.
    // For the time being this function simply fires a BeginRun incident using the EventContext, without overriding anything

    ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", name() );
    ATH_CHECK( incSvc.retrieve() );

    EventIncident evtInc(name(), "BeginRun",getContext());
    incSvc->fireIncident( evtInc );

    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode IOVDbTestAlg::createCondObjects(){
    StatusCode sc;
    MsgStream log(msgSvc(), name());
    log <<MSG::INFO <<"in createCondObjects()" <<endmsg;

    // Create IOVDbTestMDTEleMap
    IOVDbTestMDTEleMap* elemMap = new IOVDbTestMDTEleMap;
    unsigned long long timestamp = m_evt->event_ID()->time_stamp();
    if (timestamp) 
        elemMap->set(m_evt->event_ID()->time_stamp(),"mdt element map");
    else 
        elemMap->set(m_evt->event_ID()->run_number(), m_evt->event_ID()->event_number(), "mdt element map");
    
  
    // Must provide a key which is used as the name to create the folder
    sc = m_detStore->record(elemMap, "/IOVDbTest/IOVDbTestMDTEleMap");
    if (sc.isFailure()) {
        log <<MSG::ERROR <<"Could not record IOVDbTestMDTEleMap" <<endmsg;
        return( StatusCode::FAILURE);
    }

    // Create IOVDbTestMDTEleMapColl
    IOVDbTestMDTEleMapColl* elemMapColl = new IOVDbTestMDTEleMapColl;

    // Add in 10 maps, set indices
    unsigned int offset = 0;
    if (m_createExtraChans) offset = 100;
    for (unsigned int i = 0; i < 10; ++i) {
        IOVDbTestMDTEleMap* elemMap = new IOVDbTestMDTEleMap;

        unsigned long long timestamp = m_evt->event_ID()->time_stamp();
        if (timestamp) 
            elemMap->set(m_evt->event_ID()->time_stamp() + 10*i, "mdt element map");
        else 
            elemMap->set(m_evt->event_ID()->run_number() + i, m_evt->event_ID()->event_number(), "mdt element map");
	
        elemMapColl->push_back(elemMap);
        elemMapColl->add(2*i+1+i + offset);
    }
    
    // Must provide a key which is used as the name to create the folder
    sc = m_detStore->record(elemMapColl, "/IOVDbTest/IOVDbTestMDTEleMapColl");
    if (sc.isFailure()) {
        log <<MSG::ERROR <<"Could not record IOVDbTestMDTEleMapColl" <<endmsg;
        return( StatusCode::FAILURE);
    }
    
    // Create IOVDbTestAmdbCorrection
    IOVDbTestAmdbCorrection* amdbCorr =   new IOVDbTestAmdbCorrection;
    HepGeom::Point3D<double> x(1.0, 2.0, 3.0);
    HepGeom::Point3D<double> y(4.0, 5.0, 6.0);
    if (m_writeNewTag) {
        // writing with new tag, set to different values
        x = HepGeom::Point3D<double>(11.0, 22.0, 33.0);
        y = HepGeom::Point3D<double>(44.0, 55.0, 66.0);
    }
    amdbCorr->set(x, y, "amdb correction");
  
    sc = m_detStore->record(amdbCorr, "/IOVDbTest/IOVDbTestAMDBCorrection");
    if (sc.isFailure()) {
        log <<MSG::ERROR <<"Could not record IOVDbTestAmdbCorrection" <<endmsg;
        return( StatusCode::FAILURE);
    }

    // Create an attribute list

    // Create spec
    coral::AttributeListSpecification* attrSpec = new coral::AttributeListSpecification();
    attrSpec->extend("xPosition", "float");
    attrSpec->extend("id", "int");
    attrSpec->extend("name", "string");

    if (!attrSpec->size()) {
        log << MSG::ERROR << " Attribute list specification is empty" <<endmsg;
        return(StatusCode::FAILURE);
    } 

    // FIX this 
    //std::ostringstream attrStr;
    //attrSpec->print( attrStr );
    //log << MSG::DEBUG << "Attribute spec " << attrStr.str() << endmsg;

    AthenaAttributeList* attrList = new AthenaAttributeList(*attrSpec);
    (*attrList)["xPosition"].setValue((float)m_run);
    (*attrList)["id"].setValue((int)7);
    (*attrList)["name"].setValue(std::string("TestAttrList"));
    if (m_writeNewTag) {
        // writing with new tag, set to different values
        (*attrList)["xPosition"].setValue((float)125.0);
        (*attrList)["id"].setValue((int)27);
        (*attrList)["name"].setValue(std::string("TestAttrListNEWTAG"));
    }
    std::ostringstream attrStr1;
    // FIXME
    attrList->toOutputStream( attrStr1 );
    // attrList->print(std::cout);
    log << MSG::DEBUG << "Attribute list " << attrStr1.str() << endmsg;

    sc = m_detStore->record(attrList, "/IOVDbTest/IOVDbTestAttrList");
    if (sc.isFailure()) {
        log <<MSG::ERROR <<"Could not record IOVDbTestAttrList" <<endmsg;
        return( StatusCode::FAILURE);
    }

    // optionally create a second 'fancy' attributelist testing more datatypes
    // including bool, CLOB and BLOB types
    if (m_fancylist) {
        coral::AttributeListSpecification* fanSpec = new coral::AttributeListSpecification();
        fanSpec->extend("FanBool","bool");
        fanSpec->extend("FanInt","int");
        fanSpec->extend("FanUInt","unsigned int");
        fanSpec->extend("FanI64","long long");
        fanSpec->extend("FanU64","unsigned long long");
        fanSpec->extend("FanFloat","float");
        fanSpec->extend("FanDouble","double");
        fanSpec->extend("FanSmallString","string");
        fanSpec->extend("FanBigString","string");
        fanSpec->extend("FanBlob","blob");
        AthenaAttributeList* fanList=new AthenaAttributeList(*fanSpec);
        // set values, note new style access methods
        (*fanList)["FanBool"].data<bool>()=true;
        (*fanList)["FanInt"].data<int>()=-12345;
        (*fanList)["FanUInt"].data<unsigned int>()=12345;
        (*fanList)["FanI64"].data<long long>()=-98765432100LL;
        (*fanList)["FanU64"].data<unsigned long long>()=98765432100LL;
        (*fanList)["FanFloat"].data<float>()=1.2345;
        (*fanList)["FanDouble"].data<double>()=1.23456789;
        (*fanList)["FanSmallString"].data<std::string>()="small string";
        (*fanList)["FanBigString"].data<std::string>()="potentially long string";
        // special construction to set blob type
        coral::Blob& blob=(*fanList)["FanBlob"].data<coral::Blob>();
        unsigned int blobsize=2000;
        blob.resize(blobsize);
        unsigned char* p=static_cast<unsigned char*>(blob.startingAddress());
        for (unsigned int i=0;i<blobsize;++i,++p) *p=(i % 256);
        // print out attributelist
        std::ostringstream fanstr;
        fanList->toOutputStream(fanstr);
        log << MSG::DEBUG << "Fancy Attribute list " << fanstr.str() << endmsg;
        sc = m_detStore->record(fanList, "/IOVDbTest/IOVDbTestFancyList");
        if (sc.isFailure()) {
            log <<MSG::ERROR <<"Could not record IOVDbTestFancyList" <<endmsg;
            return( StatusCode::FAILURE);
        }
    }

    // Create an attribute list collection
  
    // Use existing spec
    CondAttrListCollection* attrListColl = new CondAttrListCollection(true);

    // Add three attr lists
    coral::AttributeList attrList0(*attrSpec);
    attrList0["xPosition"].setValue((float)35.0);
    attrList0["id"].setValue((int)17);
    attrList0["name"].setValue(std::string("TestAttrList"));
    CondAttrListCollection::ChanNum chanNum = 16;
    
    std::ostringstream attrStr2;
    attrList0.toOutputStream( attrStr2 );
    log << MSG::DEBUG << "ChanNum " << chanNum << " Attribute list " << attrStr2.str() << endmsg;
    attrListColl->add(chanNum, attrList0);

    coral::AttributeList attrList1(*attrSpec);
    attrList1["xPosition"].setValue((float)45.0);
    attrList1["id"].setValue((int)27);
    attrList1["name"].setValue(std::string("TestAttrList"));
    chanNum = 26;
    
    std::ostringstream attrStr3;
    attrList1.toOutputStream( attrStr3 );
    log << MSG::DEBUG << "ChanNum " << chanNum << " Attribute list " << attrStr3.str() << endmsg;
    attrListColl->add(chanNum, attrList1);

    coral::AttributeList attrList2(*attrSpec);
    attrList2["xPosition"].setValue((float)55.0);
    attrList2["id"].setValue((int)37);
    attrList2["name"].setValue(std::string("TestAttrList"));
    chanNum = 36;
    
    std::ostringstream attrStr4;
    attrList2.toOutputStream( attrStr4 );
    log << MSG::DEBUG << "ChanNum " << chanNum << " Attribute list " << attrStr4.str() << endmsg;
    attrListColl->add(chanNum, attrList2);

    if(m_createExtraChans) {
        // Two more channels
        coral::AttributeList attrList3(*attrSpec);
        attrList3["xPosition"].setValue((float)65.0);
        attrList3["id"].setValue((int)47);
        attrList3["name"].setValue(std::string("TestAttrList"));
        chanNum = 46;
    
        std::ostringstream attrStr5;
        attrList3.toOutputStream( attrStr5 );
        log << MSG::DEBUG << "ChanNum " << chanNum << " Attribute list " << attrStr5.str() << endmsg;
        attrListColl->add(chanNum, attrList3);

        // Add in new IOV with min run == 4
        IOVRange range(IOVTime(4, IOVTime::MINEVENT), IOVTime(IOVTime::MAXRUN, IOVTime::MAXEVENT));
        attrListColl->add(chanNum, range);
        log << MSG::DEBUG <<"Add min : since " << range.start().run() << " " << range.start().event()
            << " till " << range.stop().run() << " " << range.stop().event() << endmsg;

        coral::AttributeList attrList4(*attrSpec);
        attrList4["xPosition"].setValue((float)75.0);
        attrList4["id"].setValue((int)57);
        attrList4["name"].setValue(std::string("TestAttrList"));
        chanNum = 56;
    
        std::ostringstream attrStr6;
        attrList4.toOutputStream( attrStr6 );
        log << MSG::DEBUG << "ChanNum " << chanNum << " Attribute list " << attrStr6.str() << endmsg;
        attrListColl->add(chanNum, attrList4);

        // Add in new IOV with min run == 5
        IOVRange range1(IOVTime(5, IOVTime::MINEVENT), IOVTime(IOVTime::MAXRUN, IOVTime::MAXEVENT));
        attrListColl->add(chanNum, range1);
        log << MSG::DEBUG <<"Add min : since " << range1.start().run() << " " << range1.start().event() << " till " << range1.stop().run() << " " << range1.stop().event() << endmsg;
    }
    // add names to the channels if needed
    if (m_nameChans) {
      log << MSG::DEBUG << "Name channels in CondAttrListCollection" << endmsg;
      for (CondAttrListCollection::const_iterator citr=attrListColl->begin();
	   citr!=attrListColl->end();++citr) {
	CondAttrListCollection::ChanNum chan=citr->first;
	std::ostringstream name;
	name << "Name_" << chan;
	attrListColl->add(chan,name.str());
      }
    }

    sc = m_detStore->record(attrListColl, "/IOVDbTest/IOVDbTestAttrListColl");
    if (sc.isFailure()) {
	log <<MSG::ERROR <<"Could not record IOVDbTestAttrListColl" <<endmsg;
	return( StatusCode::FAILURE);
    }
    


    return StatusCode::SUCCESS;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 


StatusCode IOVDbTestAlg::printCondObjects(){
    StatusCode sc;
    MsgStream log(msgSvc(), name());
    log <<MSG::INFO <<"in printCondObjects()" <<endmsg;

    // IOVDbTestMDTEleMap
    const IOVDbTestMDTEleMap* elemMap;
    sc = m_detStore->retrieve(elemMap, "/IOVDbTest/IOVDbTestMDTEleMap");
    if (sc.isFailure()) {
    	log <<MSG::ERROR <<"Could not find IOVDbTestMDTEleMap" <<endmsg;
    	return( StatusCode::FAILURE);
    }
    else {
	log << MSG::INFO << "Retrieved IOVDbTestMDTEleMap " 
	    << endmsg;
    }
    if (0 == elemMap) {
	log <<MSG::ERROR <<"IOVDbTestMDTEleMap ptr is 0" <<endmsg;
	return( StatusCode::FAILURE);
    }
    
    log << MSG::INFO << "Found " <<  elemMap->name() 
        << " run "   <<  elemMap->runNumber() 
    	<< " event " <<  elemMap->eventNumber()
    	<< " time  " <<  elemMap->timeStamp()
    	<< endmsg;



    // IOVDbTestAmdbCorrection
    const IOVDbTestAmdbCorrection* amdbCorr;
    sc = m_detStore->retrieve(amdbCorr, "/IOVDbTest/IOVDbTestAMDBCorrection");
    if (sc.isFailure()) {
	log <<MSG::ERROR <<"Could not find IOVDbTestAmdbCorrection" <<endmsg;
	return( StatusCode::FAILURE);
    }
    log << MSG::INFO << "Retrieved /IOVDbTest/IOVDbTestAMDBCorrection" << endmsg;
    if (0 == amdbCorr) {
	log <<MSG::ERROR <<"IOVDbTestAmdbCorrection ptr is 0" << endmsg;
	return( StatusCode::FAILURE);
    }

    HepGeom::Point3D<double> trans = amdbCorr->getTranslation();
    HepGeom::Point3D<double> rot   = amdbCorr->getRotation();
  
    log << MSG::INFO << "Found " <<  amdbCorr->name() 
	<< " trans " << trans.x() << " " << trans.y() << " " << trans.z()
	<< " rot " <<  rot.x() << " " << rot.y() << " " << rot.z()
	<< endmsg;

    
    if (m_readNewTag) {
        // IOVDbTestAmdbCorrection
        const IOVDbTestAmdbCorrection* amdbCorr;
        sc = m_detStore->retrieve(amdbCorr, "/IOVDbTest/IOVDbTestAMDBCorrection-NEWTAG");
        if (sc.isFailure()) {
            log <<MSG::ERROR <<"Could not find IOVDbTestAmdbCorrection" <<endmsg;
            return( StatusCode::FAILURE);
        }
        log << MSG::INFO << "Retrieved /IOVDbTest/IOVDbTestAMDBCorrection-NEWTAG" << endmsg;
        if (0 == amdbCorr) {
            log <<MSG::ERROR <<"IOVDbTestAmdbCorrection ptr is 0" <<endmsg;
            return( StatusCode::FAILURE);
        }

        HepGeom::Point3D<double> trans = amdbCorr->getTranslation();
        HepGeom::Point3D<double> rot   = amdbCorr->getRotation();
  
        log << MSG::INFO << "Found " <<  amdbCorr->name() 
            << " trans " << trans.x() << " " << trans.y() << " " << trans.z()
            << " rot " <<  rot.x() << " " << rot.y() << " " << rot.z()
            << endmsg;
    }
    

    const AthenaAttributeList*    attrList     = 0;
    const CondAttrListCollection* attrListColl = 0;

    if (!m_twoStepWriteReg && m_readWriteCool) {
	// AttrList
	sc = m_detStore->retrieve(attrList, "/IOVDbTest/IOVDbTestAttrList");
	if (sc.isFailure()) {
	    log <<MSG::ERROR <<"Could not retrieve IOVDbTestAttrList" <<endmsg;
	    // Using COOL, is failure
	    return( StatusCode::FAILURE);
	}
	if (0 == attrList) {
	    log <<MSG::ERROR <<"IOVDbTestAttrList ptr is 0" <<endmsg;
	    return( StatusCode::FAILURE);
	}
	else {
	    log <<MSG::DEBUG <<"Retrieved IOVDbTestAttrList" <<endmsg;
	}
  
	std::ostringstream attrStr1;
	attrList->print( attrStr1 );
	log << MSG::DEBUG << "Attribute list " << attrStr1.str() << endmsg;

        if (m_readNewTag) {
            // AttrList
            sc = m_detStore->retrieve(attrList, "/IOVDbTest/IOVDbTestAttrList-NEWTAG");
            if (sc.isFailure()) {
                log <<MSG::ERROR <<"Could not retrieve IOVDbTestAttrList-NEWTAG" <<endmsg;
                // Using COOL, is failure
                return( StatusCode::FAILURE);
            }
            if (0 == attrList) {
                log <<MSG::ERROR <<"IOVDbTestAttrList ptr is 0" <<endmsg;
                return( StatusCode::FAILURE);
            }
            else {
                log <<MSG::DEBUG <<"Retrieved IOVDbTestAttrList-NEWTAG" <<endmsg;
            }
  
            std::ostringstream attrStr1;
            attrList->print( attrStr1 );
            log << MSG::DEBUG << "Attribute list NEWTAG: " << attrStr1.str() << endmsg;
        }

	// fancy attributelist
	if (m_fancylist) {
            sc = m_detStore->retrieve(attrList, "/IOVDbTest/IOVDbTestFancyList");
            if (sc.isFailure()) {
                log <<MSG::ERROR <<"Could not retrieve IOVDbTestFancyList" <<endmsg;	    return( StatusCode::FAILURE);
            }
            if (0 == attrList) {
                log <<MSG::ERROR <<"IOVDbTestFancyList ptr is 0" <<endmsg;
                return( StatusCode::FAILURE);
            }
            else {
                log <<MSG::DEBUG <<"Retrieved IOVDbTestFancyList" <<endmsg;
            }
            std::ostringstream fanstr;
            attrList->print( fanstr );
            log << MSG::DEBUG << "Fancy Attribute list " << fanstr.str() << endmsg;
            // for the blob type, check the actual data is correct
            const coral::Blob& blob=(*attrList)["FanBlob"].data<coral::Blob>();
            const unsigned char* p=static_cast<const unsigned char*>
                (blob.startingAddress());
            int nerr=0;
            for (int i=0;i<blob.size();++i,++p) if (*p!=(i % 256)) ++nerr;
            if (nerr>0) log << MSG::ERROR << "Blob has " << nerr << 
                            " data mismatches!" << endmsg;
	}

	// AttrListColl
	sc = m_detStore->retrieve(attrListColl, "/IOVDbTest/IOVDbTestAttrListColl");
	if (sc.isFailure()) {
	    log <<MSG::ERROR <<"Could not retrieve IOVDbTestAttrListColl" <<endmsg;
	    // Using COOL, is failure
	    return( StatusCode::FAILURE);
	}
	if (0 == attrListColl) {
	    log <<MSG::ERROR <<"IOVDbTestAttrListColl ptr is 0" <<endmsg;
	    return( StatusCode::FAILURE);
	}
	else {
	    log <<MSG::DEBUG <<"Retrieved IOVDbTestAttrListColl" <<endmsg;
	}
  

	std::ostringstream attrStr2;
	
	// Loop over collection
	CondAttrListCollection::const_iterator first = attrListColl->begin();
	CondAttrListCollection::const_iterator last  = attrListColl->end();
	for (; first != last; ++first) {

	    std::ostringstream attrStr1;
	    (*first).second.toOutputStream( attrStr1 );
	    log << MSG::DEBUG << "ChanNum " << (*first).first;
	    // print out the name if present
	    if (attrListColl->name_size()>0) {
	      CondAttrListCollection::name_const_iterator 
		nitr=attrListColl->chanNamePair((*first).first);
	      if (nitr!=attrListColl->name_end())
		log << MSG::DEBUG << " name " << nitr->second;
	    }
	    log << MSG::DEBUG << 
	      " Attribute list " << attrStr1.str() << endmsg;
	    // Print out range if it exits
	    CondAttrListCollection::ChanNum chanNum = (*first).first;
	    CondAttrListCollection::iov_const_iterator iovIt = attrListColl->chanIOVPair(chanNum);
	    if (iovIt != attrListColl->iov_end()) {
		const IOVRange& range = (*iovIt).second;
		if(range.start().isTimestamp()) {
		    log << MSG::DEBUG <<"Range timestamp : since " << range.start().timestamp()
			<< " till " << range.stop().timestamp() << endmsg;
		}
		else {
		    log << MSG::DEBUG <<"Range R/E : since " << range.start().run() << " " 
			<< range.start().event()
			<< " till " << range.stop().run() << " " 
			<< range.stop().event() << endmsg;
		}
	    }
	    else {
		log << MSG::DEBUG <<"No range found " << endmsg;
	    }
	}

        // Simulation and digitization parameters:

	sc = m_detStore->retrieve(attrList, "/Simulation/Parameters");
	if (sc.isFailure()) {
	    // May not have been added - just a warning
	    log <<MSG::WARNING <<"Could not retrieve Simulation parameters" <<endmsg;
	}
        else {
            if (0 == attrList) {
                log <<MSG::ERROR <<"IOVDbTestAttrList ptr is 0" <<endmsg;
                return( StatusCode::FAILURE);
            }
	    log <<MSG::DEBUG <<"Retrieved Simulation parameters" <<endmsg;
            std::ostringstream attrStr;
            attrList->print( attrStr );
            log << MSG::DEBUG << "Attribute list " << attrStr.str() << endmsg;
	}
  
	sc = m_detStore->retrieve(attrList, "/Digitization/Parameters");
	if (sc.isFailure()) {
	    // May not have been added - just a warning
	    log <<MSG::WARNING <<"Could not retrieve Digitization parameters" <<endmsg;
	}
        else {
            if (0 == attrList) {
                log <<MSG::ERROR <<"IOVDbTestAttrList ptr is 0" <<endmsg;
                return( StatusCode::FAILURE);
            }
	    log <<MSG::DEBUG <<"Retrieved Digitization parameters" <<endmsg;
            std::ostringstream attrStr;
            attrList->print( attrStr );
            log << MSG::DEBUG << "Attribute list " << attrStr.str() << endmsg;
	}
  



    }
    

    // IOVDbTestMDTEleMapColl

//    if (m_readWriteCool) {
	
    const IOVDbTestMDTEleMapColl* elemMapColl;
    sc = m_detStore->retrieve(elemMapColl, "/IOVDbTest/IOVDbTestMDTEleMapColl");
    if (sc.isFailure()) {
        log <<MSG::ERROR <<"Could not find IOVDbTestMDTEleMapColl" <<endmsg;
        return( StatusCode::FAILURE);
    }
    else {
        log << MSG::INFO << "Retrieved IOVDbTestMDTEleMapColl " 
            << endmsg;
    }
    if (0 == elemMapColl->size()) {
        log <<MSG::ERROR <<"IOVDbTestMDTEleMapColl size is 0" <<endmsg;
        return( StatusCode::FAILURE);
    }
  
    // Make sure the channel vector is filled
    if (elemMapColl->size() != elemMapColl->chan_size()) {
        log << MSG::ERROR << "Must fill in channel numbers! Number of objects: " << elemMapColl->size()
            << "  Number of channels: " << elemMapColl->chan_size()
            << endmsg;
        return(StatusCode::FAILURE);       
    }
    // Print out IOVs if they are there
    bool hasIOVs = (elemMapColl->iov_size() == elemMapColl->size());
    IOVDbTestMDTEleMapColl::chan_const_iterator itChan = elemMapColl->chan_begin();
    IOVDbTestMDTEleMapColl::iov_const_iterator  itIOV  = elemMapColl->iov_begin();
    for (unsigned int i = 0; i < elemMapColl->size(); ++i, ++itChan) {
        const IOVDbTestMDTEleMap* elemMap = (*elemMapColl)[i];
        log << MSG::INFO << "Found " <<  elemMap->name() 
            << " run "   <<  elemMap->runNumber() 
            << " event " <<  elemMap->eventNumber()
            << " time  " <<  elemMap->timeStamp()
            << " channel " << (*itChan);
        if(hasIOVs) {
            log << MSG::INFO << " iov " <<  (*itIOV);
            ++itIOV;
        }
        log << MSG::INFO << endmsg;
    }
//    }
    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode IOVDbTestAlg::execute() {

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
    log <<MSG::DEBUG <<" in execute()" <<endmsg;

    StatusCode sc = m_sgSvc->retrieve(m_evt);
    if ( sc.isFailure() ) {
        log << MSG::ERROR << "could not get event info " << endmsg;
        return( StatusCode::FAILURE);     
    }
    else {
        log << MSG::DEBUG << "Event (run,ev,lb:time): [" << m_evt->event_ID()->run_number() << "," << m_evt->event_ID()->event_number();
        if (m_printLB) log << "," << m_evt->event_ID()->lumi_block();
        log    << ":" << m_evt->event_ID()->time_stamp() << "]" << endmsg;
    }

    if (m_writeCondObjs||m_noStream) {

        // We create the conditions objects only at run == 2, event == 5
        if (2 != m_evt->event_ID()->run_number() || 5 != m_evt->event_ID()->event_number()) {
            log << MSG::DEBUG << "Event NOT selected for creating conditions objects " << endmsg;
            return StatusCode::SUCCESS;
        }
		
        log << MSG::DEBUG << "Creating condtions objects " << endmsg;
        sc = createCondObjects();
        if (sc.isFailure()) {
            log <<MSG::ERROR <<"Could not create cond objects" <<endmsg;
            return( StatusCode::FAILURE);
        }

        //  Read objects from DetectorStore
        if(!m_noStream){
          sc = printCondObjects();
          if (sc.isFailure()) {
              log <<MSG::ERROR <<"Could not print out cond objects" <<endmsg;
              return( StatusCode::FAILURE);
          }
	}
    }
    else {
	
        log << MSG::DEBUG << "Calling printCondObjects" <<m_online<< "\t"<<m_evt->event_ID()->run_number()<<"\t"<<m_evt->event_ID()->event_number()<< endmsg;

        //  Read objects from DetectorStore
        if (m_online && 2 == m_evt->event_ID()->run_number() && 9 == m_evt->event_ID()->event_number())
            waitForSecond();
        sc = printCondObjects();
        if (sc.isFailure()) {
            log <<MSG::ERROR <<"Could not print out cond objects" <<endmsg;
            return( StatusCode::FAILURE);
        }
    }
    
    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode IOVDbTestAlg::finalize(){
    MsgStream log(msgSvc(), name());
    log <<MSG::INFO <<"in finalize()" <<endmsg;

    if (m_writeCondObjs) {
        // Stream out and register objects here
        log << MSG::DEBUG << "Stream out objects directly " << endmsg;
        StatusCode sc = streamOutCondObjects();
        if (sc.isFailure()) {
            log <<MSG::ERROR <<"Could not stream out objects" <<endmsg;
            return( StatusCode::FAILURE);
        }
        log << MSG::DEBUG << "Streamed out OK " << endmsg;
    }
    if(m_regIOV) {
        StatusCode sc = registerCondObjects();
        if (sc.isFailure()) {
            log <<MSG::ERROR <<"Could not register objects" <<endmsg;
            return( StatusCode::FAILURE);
        }
        log << MSG::DEBUG << "Register OK " << endmsg;
    }
  
    return StatusCode::SUCCESS;
}



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode 
IOVDbTestAlg::streamOutCondObjects(){
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());

    log << MSG::DEBUG << "entering streamOutCondObjects "  << endmsg;
    StatusCode sc = m_streamer->connectOutput();
    if (sc.isFailure()) {
	log <<MSG::ERROR <<"Could not connect stream to output" <<endmsg;
	return( StatusCode::FAILURE);
    }

    int npairs = 3;
    int index = 0;
    IAthenaOutputStreamTool::TypeKeyPairs  typeKeys(npairs);
    if (!m_writeOnlyCool) {
	IAthenaOutputStreamTool::TypeKeyPair   mdtPair("IOVDbTestMDTEleMap", "");
	typeKeys[index] = mdtPair;
	++index;
	IAthenaOutputStreamTool::TypeKeyPair   amdbCorr("IOVDbTestAmdbCorrection", "");
	typeKeys[index] = amdbCorr;
	++index;
	IAthenaOutputStreamTool::TypeKeyPair   mdtCollPair("IOVDbTestMDTEleMapColl", "");
	typeKeys[index] = mdtCollPair;
	++index;
    }

    log << MSG::DEBUG <<"Stream out for pairs:" <<endmsg;
    for (unsigned int i = 0; i < typeKeys.size(); ++i) {
        log << MSG::DEBUG << typeKeys[i].first << " " << typeKeys[i].second << " " 
            << endmsg;
    }
    
    sc = m_streamer->streamObjects(typeKeys);
    if (sc.isFailure()) {
	log <<MSG::ERROR <<"Could not stream out IOVDbTestMDTEleMap and IOVDbTestAmdbCorrection" <<endmsg;
	return( StatusCode::FAILURE);
    }
    
    sc = m_streamer->commitOutput();
    if (sc.isFailure()) {
	log <<MSG::ERROR <<"Could not commit output stream" <<endmsg;
	return( StatusCode::FAILURE);
    }

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode 
IOVDbTestAlg::registerCondObjects(){
    // Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());

    log << MSG::DEBUG << "entering registerCondObject "  << endmsg;

    // Register the IOV DB with the conditions data written out
    StatusCode sc;
    std::string tag = "no tag";
    if (m_tagID!="") {
        tag = "tag MDTEleMap_" + m_tagID;
        sc = m_regSvc->registerIOV("IOVDbTestMDTEleMap", "MDTEleMap_"+m_tagID,m_run,IOVTime::MAXRUN,IOVTime::MINEVENT,IOVTime::MAXEVENT);
    } else {
        sc = m_regSvc->registerIOV("IOVDbTestMDTEleMap", "");
    }
    if (sc.isFailure()) {
        log <<MSG::ERROR <<"Could not register in IOV DB for IOVDbTestMDTEleMap" <<endmsg;
        return( StatusCode::FAILURE);
    }
    log << MSG::DEBUG << "registered IOVDbTestMDTEleMap with " << tag << endmsg;

    // For IOVDbTestAmdbCorrection use time (in sec)
    uint64_t start=static_cast<long long>(m_regTime)*1000000000LL;
    uint64_t stop = IOVTime::MAXTIMESTAMP;
    tag = "no tag";
    if (m_tagID!="") {
        tag = "tag AmdbCorrection_" + m_tagID;
        sc = m_regSvc->registerIOV("IOVDbTestAmdbCorrection", "AmdbCorrection_"+m_tagID, start, stop);
    } else {
        sc = m_regSvc->registerIOV("IOVDbTestAmdbCorrection", "", start, stop);
    }
    if (sc.isFailure()) {
        log <<MSG::ERROR <<"Could not register in IOV DB for IOVDbTestAmdbCorrection" <<endmsg;
        return( StatusCode::FAILURE);
    }
    log << MSG::DEBUG << "registered IOVDbTestAmdbCorrection with " << tag << endmsg;
    if (m_readWriteCool) {

	// Can only write out AttrList's if this is NOT write and reg in two steps
	if (!m_twoStepWriteReg) { 
	    
	    // Using COOL, write out attrlist and collection of attrlists
            tag = "no tag";
            if (m_tagID!="") {
        	tag = "tag AttrList_" + m_tagID;
                sc = m_regSvc->registerIOV("AthenaAttributeList","/IOVDbTest/IOVDbTestAttrList","AttrList_"+m_tagID,m_run,IOVTime::MAXRUN,IOVTime::MINEVENT,IOVTime::MAXEVENT);
	    } else {
        	sc = m_regSvc->registerIOV("AthenaAttributeList","/IOVDbTest/IOVDbTestAttrList","");
	    }
	    if (sc.isFailure()) {
                log <<MSG::ERROR <<"Could not register in IOV DB for AthenaAttributeList" <<endmsg;
                return( StatusCode::FAILURE);
	    }
            log << MSG::DEBUG << "registered AthenaAttributeList with " << tag << endmsg;
	    if (m_fancylist) {
                // register Fancy AttributeList
                tag = "no tag";
                if (m_tagID!="") {
                    tag = "tag FancyList_" + m_tagID;
                    sc = m_regSvc->registerIOV("AthenaAttributeList","/IOVDbTest/IOVDbTestFancyList","FancyList_"+m_tagID,m_run,IOVTime::MAXRUN,IOVTime::MINEVENT,IOVTime::MAXEVENT);
                } else {
                    sc = m_regSvc->registerIOV("AthenaAttributeList", "/IOVDbTest/IOVDbTestFancyList","");
                }
                if (sc.isFailure()) {
                    log <<MSG::ERROR <<"Could not register in IOV DB for AthenaAttributeList" <<endmsg;
                    return( StatusCode::FAILURE);
                }
                log << MSG::DEBUG << "registered Fancy AthenaAttributeList with " << tag << endmsg;
	    }
	    // attrlist collection 
            tag = "no tag";
            if (m_tagID!="") {
                tag = "tag AttrListColl_" + m_tagID;
                sc = m_regSvc->registerIOV("CondAttrListCollection","AttrListColl_"+m_tagID,m_run,IOVTime::MAXRUN,IOVTime::MINEVENT,IOVTime::MAXEVENT);
	    } else {
                sc = m_regSvc->registerIOV("CondAttrListCollection", "");
	    }
	    if (sc.isFailure()) {
		log <<MSG::ERROR <<"Could not register in IOV DB for CondAttrListCollection" <<endmsg;
		return( StatusCode::FAILURE);
	    }
            log << MSG::DEBUG << "registered CondAttrListCollection with " << tag << endmsg;
	}
        
	// mdtMapColl
        tag = "no tag";
        if (m_tagID!="") {
            tag = "tag MDTEleMapColl_" + m_tagID;
            sc = m_regSvc->registerIOV("IOVDbTestMDTEleMapColl","MDTEleMapColl_"+m_tagID,m_run,IOVTime::MAXRUN,IOVTime::MINEVENT,IOVTime::MAXEVENT);
	} else {
            sc = m_regSvc->registerIOV("IOVDbTestMDTEleMapColl","");
	}
	if (sc.isFailure()) {
	    log <<MSG::ERROR <<"Could not register in IOV DB for IOVDbTestMDTEleMapColl" <<endmsg;
	    return( StatusCode::FAILURE);
	}
        log << MSG::DEBUG << "registered IOVDbTestMDTEleMapColl with " << tag << endmsg;
    }

    return StatusCode::SUCCESS;

}

