/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

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
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/GaudiException.h" 
#include "GaudiKernel/IToolSvc.h"

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
        AthReentrantAlgorithm(name, pSvcLocator),
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
        m_regSvc("IOVRegistrationSvc", name),
        m_streamer ("CondStream1")

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
void IOVDbTestAlg::waitForSecond() const {
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
  msg() << MSG::INFO << "IOVDbTestAlg::testCallBack callback invoked for keys: i = " << i << " ";
  for (std::list<std::string>::const_iterator itr=keys.begin(); itr!=keys.end(); ++itr) {
    msg() << *itr << " ";
  }
  msg() << endmsg;
  return  StatusCode::SUCCESS;
}


StatusCode IOVDbTestAlg::initialize(){
    ATH_MSG_DEBUG( "in initialize()" );

    // Get Output Stream tool for writing
    if (m_writeCondObjs) {
        m_streamer = "AthenaOutputStreamTool/" + m_streamName;
        ATH_CHECK( m_streamer.retrieve() );
    }
    
    // Get the IOVRegistrationSvc when needed
    if (m_regIOV) {
        ATH_CHECK( m_regSvc.retrieve() );
        ATH_MSG_DEBUG( "Found IOVRegistrationSvc " );
        ATH_MSG_INFO( "Tag to be used: " << m_tagID );
    }

    if (m_readInInit) {
        ATH_CHECK( readWithBeginRun() );
        ATH_MSG_DEBUG( "Read with BeginRun " );
    }

    // register callbacks for test of online change of constants
    if (!m_online) return StatusCode::SUCCESS;
    
    const DataHandle<IOVDbTestMDTEleMap> mdtelemap;
    ATH_CHECK( detStore()->regFcn(&IOVDbTestAlg::testCallBack, this, mdtelemap, "/IOVDbTest/IOVDbTestMDTEleMap") );
    ATH_MSG_INFO( "Registered callback for IOVDbTestAlg::testCallBack" );
   
    
    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode IOVDbTestAlg::readWithBeginRun(){
    ATH_MSG_INFO( "in readWithBeginRun()" );

    // As a result of the restructuring the EventIncident class (dropping the reference to EventInfo)
    // the old mechanism of overriding run&event&time is no longer working.
    // If we need this functionality, then we need to find a new way of implementing it.
    // For the time being this function simply fires a BeginRun incident using the EventContext, without overriding anything

    ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", name() );
    ATH_CHECK( incSvc.retrieve() );

    incSvc->fireIncident( Incident(name(), IncidentType::BeginRun, Gaudi::Hive::currentContext()) );

    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode IOVDbTestAlg::createCondObjects(const EventContext& ctx) const
{
    ATH_MSG_INFO ("in createCondObjects()");

    // Create IOVDbTestMDTEleMap
    IOVDbTestMDTEleMap* elemMap = new IOVDbTestMDTEleMap;
    unsigned long long timestamp = ctx.eventID().time_stamp();
    if (timestamp) 
        elemMap->set(ctx.eventID().time_stamp(),"mdt element map");
    else 
        elemMap->set(ctx.eventID().run_number(), ctx.eventID().event_number(), "mdt element map");
    
  
    // Must provide a key which is used as the name to create the folder
    ATH_CHECK( detStore()->record(elemMap, "/IOVDbTest/IOVDbTestMDTEleMap") );

    // Create IOVDbTestMDTEleMapColl
    IOVDbTestMDTEleMapColl* elemMapColl = new IOVDbTestMDTEleMapColl;

    // Add in 10 maps, set indices
    unsigned int offset = 0;
    if (m_createExtraChans) offset = 100;
    for (unsigned int i = 0; i < 10; ++i) {
        IOVDbTestMDTEleMap* elemMap = new IOVDbTestMDTEleMap;

        unsigned long long timestamp = ctx.eventID().time_stamp();
        if (timestamp) 
            elemMap->set(ctx.eventID().time_stamp() + 10*i, "mdt element map");
        else 
            elemMap->set(ctx.eventID().run_number() + i, ctx.eventID().event_number(), "mdt element map");
	
        elemMapColl->push_back(elemMap);
        elemMapColl->add(2*i+1+i + offset);
    }
    
    // Must provide a key which is used as the name to create the folder
    ATH_CHECK( detStore()->record(elemMapColl, "/IOVDbTest/IOVDbTestMDTEleMapColl") );
    
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
  
    ATH_CHECK( detStore()->record(amdbCorr, "/IOVDbTest/IOVDbTestAMDBCorrection") );

    // Create an attribute list

    // Create spec
    coral::AttributeListSpecification* attrSpec = new coral::AttributeListSpecification();
    attrSpec->extend("xPosition", "float");
    attrSpec->extend("id", "int");
    attrSpec->extend("name", "string");

    if (!attrSpec->size()) {
        ATH_MSG_ERROR (" Attribute list specification is empty");
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
    ATH_MSG_DEBUG( "Attribute list " << attrStr1.str() );

    ATH_CHECK( detStore()->record(attrList, "/IOVDbTest/IOVDbTestAttrList") );

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
        ATH_MSG_DEBUG( "Fancy Attribute list " << fanstr.str() );
        ATH_CHECK( detStore()->record(fanList, "/IOVDbTest/IOVDbTestFancyList") );
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
    ATH_MSG_DEBUG( "ChanNum " << chanNum << " Attribute list " << attrStr2.str() );
    attrListColl->add(chanNum, attrList0);

    coral::AttributeList attrList1(*attrSpec);
    attrList1["xPosition"].setValue((float)45.0);
    attrList1["id"].setValue((int)27);
    attrList1["name"].setValue(std::string("TestAttrList"));
    chanNum = 26;
    
    std::ostringstream attrStr3;
    attrList1.toOutputStream( attrStr3 );
    ATH_MSG_DEBUG( "ChanNum " << chanNum << " Attribute list " << attrStr3.str() );
    attrListColl->add(chanNum, attrList1);

    coral::AttributeList attrList2(*attrSpec);
    attrList2["xPosition"].setValue((float)55.0);
    attrList2["id"].setValue((int)37);
    attrList2["name"].setValue(std::string("TestAttrList"));
    chanNum = 36;
    
    std::ostringstream attrStr4;
    attrList2.toOutputStream( attrStr4 );
    ATH_MSG_DEBUG( "ChanNum " << chanNum << " Attribute list " << attrStr4.str() );
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
        ATH_MSG_DEBUG( "ChanNum " << chanNum << " Attribute list " << attrStr5.str() );
        attrListColl->add(chanNum, attrList3);

        // Add in new IOV with min run == 4
        IOVRange range(IOVTime(4, IOVTime::MINEVENT), IOVTime(IOVTime::MAXRUN, IOVTime::MAXEVENT));
        attrListColl->add(chanNum, range);
        ATH_MSG_DEBUG( "Add min : since " << range.start().run() << " " << range.start().event()
                       << " till " << range.stop().run() << " " << range.stop().event() );

        coral::AttributeList attrList4(*attrSpec);
        attrList4["xPosition"].setValue((float)75.0);
        attrList4["id"].setValue((int)57);
        attrList4["name"].setValue(std::string("TestAttrList"));
        chanNum = 56;
    
        std::ostringstream attrStr6;
        attrList4.toOutputStream( attrStr6 );
        ATH_MSG_DEBUG( "ChanNum " << chanNum << " Attribute list " << attrStr6.str() );
        attrListColl->add(chanNum, attrList4);

        // Add in new IOV with min run == 5
        IOVRange range1(IOVTime(5, IOVTime::MINEVENT), IOVTime(IOVTime::MAXRUN, IOVTime::MAXEVENT));
        attrListColl->add(chanNum, range1);
        ATH_MSG_DEBUG( "Add min : since " << range1.start().run() << " " << range1.start().event() << " till " << range1.stop().run() << " " << range1.stop().event() );
    }
    // add names to the channels if needed
    if (m_nameChans) {
      ATH_MSG_DEBUG( "Name channels in CondAttrListCollection" );
      for (CondAttrListCollection::const_iterator citr=attrListColl->begin();
	   citr!=attrListColl->end();++citr) {
	CondAttrListCollection::ChanNum chan=citr->first;
	std::ostringstream name;
	name << "Name_" << chan;
	attrListColl->add(chan,name.str());
      }
    }

    ATH_CHECK( detStore()->record(attrListColl, "/IOVDbTest/IOVDbTestAttrListColl") );

    return StatusCode::SUCCESS;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 


StatusCode IOVDbTestAlg::printCondObjects() const {
    ATH_MSG_INFO( "in printCondObjects()" );

    // IOVDbTestMDTEleMap
    const IOVDbTestMDTEleMap* elemMap;
    ATH_CHECK( detStore()->retrieve(elemMap, "/IOVDbTest/IOVDbTestMDTEleMap") );
    ATH_MSG_INFO( "Retrieved IOVDbTestMDTEleMap " );
    
    ATH_MSG_INFO( "Found " <<  elemMap->name() 
                  << " run "   <<  elemMap->runNumber() 
                  << " event " <<  elemMap->eventNumber()
                  << " time  " <<  elemMap->timeStamp() );



    // IOVDbTestAmdbCorrection
    const IOVDbTestAmdbCorrection* amdbCorr;
    ATH_CHECK( detStore()->retrieve(amdbCorr, "/IOVDbTest/IOVDbTestAMDBCorrection") );
    ATH_MSG_INFO ("Retrieved /IOVDbTest/IOVDbTestAMDBCorrection" );

    HepGeom::Point3D<double> trans = amdbCorr->getTranslation();
    HepGeom::Point3D<double> rot   = amdbCorr->getRotation();
  
    ATH_MSG_INFO( "Found " <<  amdbCorr->name() 
                  << " trans " << trans.x() << " " << trans.y() << " " << trans.z()
                  << " rot " <<  rot.x() << " " << rot.y() << " " << rot.z() );

    
    if (m_readNewTag) {
        // IOVDbTestAmdbCorrection
        const IOVDbTestAmdbCorrection* amdbCorr;
        ATH_CHECK( detStore()->retrieve(amdbCorr, "/IOVDbTest/IOVDbTestAMDBCorrection-NEWTAG") );
        ATH_MSG_INFO( "Retrieved /IOVDbTest/IOVDbTestAMDBCorrection-NEWTAG" );

        HepGeom::Point3D<double> trans = amdbCorr->getTranslation();
        HepGeom::Point3D<double> rot   = amdbCorr->getRotation();
  
        ATH_MSG_INFO( "Found " <<  amdbCorr->name() 
                      << " trans " << trans.x() << " " << trans.y() << " " << trans.z()
                      << " rot " <<  rot.x() << " " << rot.y() << " " << rot.z() );
    }
    

    const AthenaAttributeList*    attrList     = 0;
    const CondAttrListCollection* attrListColl = 0;

    if (!m_twoStepWriteReg && m_readWriteCool) {
	// AttrList
        ATH_CHECK( detStore()->retrieve(attrList, "/IOVDbTest/IOVDbTestAttrList") );
        ATH_MSG_DEBUG( "Retrieved IOVDbTestAttrList" );
  
	std::ostringstream attrStr1;
	attrList->print( attrStr1 );
	ATH_MSG_DEBUG( "Attribute list " << attrStr1.str() );

        if (m_readNewTag) {
            // AttrList
            ATH_CHECK( detStore()->retrieve(attrList, "/IOVDbTest/IOVDbTestAttrList-NEWTAG") );
            ATH_MSG_DEBUG( "Retrieved IOVDbTestAttrList-NEWTAG" );
  
            std::ostringstream attrStr1;
            attrList->print( attrStr1 );
            ATH_MSG_DEBUG( "Attribute list NEWTAG: " << attrStr1.str() );
        }

	// fancy attributelist
	if (m_fancylist) {
            ATH_MSG_DEBUG( detStore()->retrieve(attrList, "/IOVDbTest/IOVDbTestFancyList") );
            ATH_MSG_DEBUG( "Retrieved IOVDbTestFancyList" );
            std::ostringstream fanstr;
            attrList->print( fanstr );
            ATH_MSG_DEBUG( "Fancy Attribute list " << fanstr.str() );
            // for the blob type, check the actual data is correct
            const coral::Blob& blob=(*attrList)["FanBlob"].data<coral::Blob>();
            const unsigned char* p=static_cast<const unsigned char*>
                (blob.startingAddress());
            int nerr=0;
            for (int i=0;i<blob.size();++i,++p) if (*p!=(i % 256)) ++nerr;
            if (nerr>0) ATH_MSG_ERROR(  "Blob has " << nerr << 
                                        " data mismatches!" );
	}

	// AttrListColl
	ATH_CHECK( detStore()->retrieve(attrListColl, "/IOVDbTest/IOVDbTestAttrListColl") );
        ATH_MSG_DEBUG( "Retrieved IOVDbTestAttrListColl" );
  

	std::ostringstream attrStr2;
	
	// Loop over collection
	CondAttrListCollection::const_iterator first = attrListColl->begin();
	CondAttrListCollection::const_iterator last  = attrListColl->end();
	for (; first != last; ++first) {

            if (msgLvl (MSG::DEBUG)) {
              std::ostringstream attrStr1;
              (*first).second.toOutputStream( attrStr1 );
              msg() << MSG::DEBUG << "ChanNum " << (*first).first;
              // print out the name if present
              if (attrListColl->name_size()>0) {
                CondAttrListCollection::name_const_iterator 
                  nitr=attrListColl->chanNamePair((*first).first);
                if (nitr!=attrListColl->name_end())
                  msg() << MSG::DEBUG << " name " << nitr->second;
              }
              msg() << MSG::DEBUG << 
                " Attribute list " << attrStr1.str() << endmsg;
            }

            // Print out range if it exits
            CondAttrListCollection::ChanNum chanNum = (*first).first;
            CondAttrListCollection::iov_const_iterator iovIt = attrListColl->chanIOVPair(chanNum);
            if (iovIt != attrListColl->iov_end()) {
              const IOVRange& range = (*iovIt).second;
              if(range.start().isTimestamp()) {
                ATH_MSG_DEBUG( "Range timestamp : since " << range.start().timestamp()
                               << " till " << range.stop().timestamp() );
              }
              else {
                ATH_MSG_DEBUG( "Range R/E : since " << range.start().run() << " " 
                               << range.start().event()
                               << " till " << range.stop().run() << " " 
                               << range.stop().event() );
              }
	    }
	    else {
              ATH_MSG_DEBUG( "No range found " );
            }
	}

        // Simulation and digitization parameters:

	if ( detStore()->retrieve(attrList, "/Simulation/Parameters").isFailure() ) {
	    // May not have been added - just a warning
            ATH_MSG_WARNING( "Could not retrieve Simulation parameters" );
	}
        else {
          ATH_MSG_DEBUG( "Retrieved Simulation parameters" );
          std::ostringstream attrStr;
          attrList->print( attrStr );
          ATH_MSG_DEBUG( "Attribute list " << attrStr.str() );
	}
  
	if (detStore()->retrieve(attrList, "/Digitization/Parameters").isFailure()) {
            // May not have been added - just a warning
            ATH_MSG_WARNING( "Could not retrieve Digitization parameters" );
        }
        else {
            ATH_MSG_DEBUG( "Retrieved Digitization parameters" );
            std::ostringstream attrStr;
            attrList->print( attrStr );
            ATH_MSG_DEBUG( "Attribute list " << attrStr.str() );
	}
    }
    

    // IOVDbTestMDTEleMapColl

//    if (m_readWriteCool) {
	
    const IOVDbTestMDTEleMapColl* elemMapColl;
    ATH_CHECK( detStore()->retrieve(elemMapColl, "/IOVDbTest/IOVDbTestMDTEleMapColl") );
    ATH_MSG_INFO( "Retrieved IOVDbTestMDTEleMapColl " );
  
    // Make sure the channel vector is filled
    if (elemMapColl->size() != elemMapColl->chan_size()) {
        ATH_MSG_ERROR( "Must fill in channel numbers! Number of objects: " << elemMapColl->size()
                       << "  Number of channels: " << elemMapColl->chan_size() );
        return(StatusCode::FAILURE);       
    }
    // Print out IOVs if they are there
    bool hasIOVs = (elemMapColl->iov_size() == elemMapColl->size());
    IOVDbTestMDTEleMapColl::chan_const_iterator itChan = elemMapColl->chan_begin();
    IOVDbTestMDTEleMapColl::iov_const_iterator  itIOV  = elemMapColl->iov_begin();
    for (unsigned int i = 0; i < elemMapColl->size(); ++i, ++itChan) {
        const IOVDbTestMDTEleMap* elemMap = (*elemMapColl)[i];
        msg() << MSG::INFO << "Found " <<  elemMap->name() 
            << " run "   <<  elemMap->runNumber() 
            << " event " <<  elemMap->eventNumber()
            << " time  " <<  elemMap->timeStamp()
            << " channel " << (*itChan);
        if(hasIOVs) {
            msg() << MSG::INFO << " iov " <<  (*itIOV);
            ++itIOV;
        }
        msg() << MSG::INFO << endmsg;
    }
//    }
    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode IOVDbTestAlg::execute (const EventContext& ctx) const {

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

    if (msgLvl (MSG::DEBUG)) {
      msg() << MSG::DEBUG << "Event (run,ev,lb:time): [" << ctx.eventID().run_number() << "," << ctx.eventID().event_number();
      if (m_printLB) msg() << "," << ctx.eventID().lumi_block();
      msg()  << ":" << ctx.eventID().time_stamp() << "]" << endmsg;
    }

    if (m_writeCondObjs||m_noStream) {

        // We create the conditions objects only at run == 2, event == 5
        if (2 != ctx.eventID().run_number() || 5 != ctx.eventID().event_number()) {
            ATH_MSG_DEBUG( "Event NOT selected for creating conditions objects " );
            return StatusCode::SUCCESS;
        }
		
        ATH_MSG_DEBUG( "Creating condtions objects " );
        ATH_CHECK( createCondObjects(ctx) );

        //  Read objects from DetectorStore
        if(!m_noStream){
          ATH_CHECK( printCondObjects() );
	}
    }
    else {
	
        ATH_MSG_DEBUG( "Calling printCondObjects" <<m_online<< "\t"<<ctx.eventID().run_number()<<"\t"<<ctx.eventID().event_number() );

        //  Read objects from DetectorStore
        if (m_online && 2 == ctx.eventID().run_number() && 9 == ctx.eventID().event_number())
            waitForSecond();
        ATH_CHECK( printCondObjects() );
    }
    
    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode IOVDbTestAlg::finalize(){
    ATH_MSG_INFO( "in finalize()" );

    if (m_writeCondObjs) {
        // Stream out and register objects here
        ATH_MSG_DEBUG( "Stream out objects directly " );
        ATH_CHECK( streamOutCondObjects() );
        ATH_MSG_DEBUG( "Streamed out OK " );
    }
    if(m_regIOV) {
        ATH_CHECK( registerCondObjects() );
        ATH_MSG_DEBUG( "Register OK " );
    }
  
    return StatusCode::SUCCESS;
}



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode 
IOVDbTestAlg::streamOutCondObjects(){
    ATH_MSG_DEBUG( "entering streamOutCondObjects " );
    ATH_CHECK( m_streamer->connectOutput() );

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

    ATH_MSG_DEBUG( "Stream out for pairs:" );
    for (unsigned int i = 0; i < typeKeys.size(); ++i) {
      ATH_MSG_DEBUG( typeKeys[i].first << " " << typeKeys[i].second << " " );
    }
    
    ATH_CHECK( m_streamer->streamObjects(typeKeys) );
    ATH_CHECK( m_streamer->commitOutput() );

    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode 
IOVDbTestAlg::registerCondObjects(){
    ATH_MSG_DEBUG( "entering registerCondObject " );

    // Register the IOV DB with the conditions data written out
    std::string tag = "no tag";
    if (m_tagID!="") {
        tag = "tag MDTEleMap_" + m_tagID;
        ATH_CHECK( m_regSvc->registerIOV("IOVDbTestMDTEleMap", "MDTEleMap_"+m_tagID,m_run,IOVTime::MAXRUN,IOVTime::MINEVENT,IOVTime::MAXEVENT) );
    } else {
        ATH_CHECK( m_regSvc->registerIOV("IOVDbTestMDTEleMap", "") );
    }
    ATH_MSG_DEBUG( "registered IOVDbTestMDTEleMap with " << tag );

    // For IOVDbTestAmdbCorrection use time (in sec)
    uint64_t start=static_cast<long long>(m_regTime)*1000000000LL;
    uint64_t stop = IOVTime::MAXTIMESTAMP;
    tag = "no tag";
    if (m_tagID!="") {
        tag = "tag AmdbCorrection_" + m_tagID;
        ATH_CHECK( m_regSvc->registerIOV("IOVDbTestAmdbCorrection", "AmdbCorrection_"+m_tagID, start, stop) );
    } else {
        ATH_CHECK( m_regSvc->registerIOV("IOVDbTestAmdbCorrection", "", start, stop) );
    }
    ATH_MSG_DEBUG( "registered IOVDbTestAmdbCorrection with " << tag );
    if (m_readWriteCool) {

	// Can only write out AttrList's if this is NOT write and reg in two steps
	if (!m_twoStepWriteReg) { 
	    
	    // Using COOL, write out attrlist and collection of attrlists
            tag = "no tag";
            if (m_tagID!="") {
        	tag = "tag AttrList_" + m_tagID;
                ATH_CHECK( m_regSvc->registerIOV("AthenaAttributeList","/IOVDbTest/IOVDbTestAttrList","AttrList_"+m_tagID,m_run,IOVTime::MAXRUN,IOVTime::MINEVENT,IOVTime::MAXEVENT) );
	    } else {
                ATH_CHECK( m_regSvc->registerIOV("AthenaAttributeList","/IOVDbTest/IOVDbTestAttrList","") );
	    }
            ATH_MSG_DEBUG( "registered AthenaAttributeList with " << tag );
	    if (m_fancylist) {
                // register Fancy AttributeList
                tag = "no tag";
                if (m_tagID!="") {
                    tag = "tag FancyList_" + m_tagID;
                    ATH_CHECK( m_regSvc->registerIOV("AthenaAttributeList","/IOVDbTest/IOVDbTestFancyList","FancyList_"+m_tagID,m_run,IOVTime::MAXRUN,IOVTime::MINEVENT,IOVTime::MAXEVENT) );
                } else {
                    ATH_CHECK( m_regSvc->registerIOV("AthenaAttributeList", "/IOVDbTest/IOVDbTestFancyList","") );
                }
                ATH_MSG_DEBUG ( "registered Fancy AthenaAttributeList with " << tag );
	    }
	    // attrlist collection 
            tag = "no tag";
            if (m_tagID!="") {
                tag = "tag AttrListColl_" + m_tagID;
                ATH_CHECK( m_regSvc->registerIOV("CondAttrListCollection","AttrListColl_"+m_tagID,m_run,IOVTime::MAXRUN,IOVTime::MINEVENT,IOVTime::MAXEVENT) );
	    } else {
                ATH_CHECK( m_regSvc->registerIOV("CondAttrListCollection", "") );
	    }
            ATH_MSG_DEBUG( "registered CondAttrListCollection with " << tag );
	}
        
	// mdtMapColl
        tag = "no tag";
        if (m_tagID!="") {
            tag = "tag MDTEleMapColl_" + m_tagID;
            ATH_CHECK( m_regSvc->registerIOV("IOVDbTestMDTEleMapColl","MDTEleMapColl_"+m_tagID,m_run,IOVTime::MAXRUN,IOVTime::MINEVENT,IOVTime::MAXEVENT) );
	} else {
            ATH_CHECK( m_regSvc->registerIOV("IOVDbTestMDTEleMapColl","") );
	}
        ATH_MSG_DEBUG( "registered IOVDbTestMDTEleMapColl with " << tag );
    }

    return StatusCode::SUCCESS;

}

