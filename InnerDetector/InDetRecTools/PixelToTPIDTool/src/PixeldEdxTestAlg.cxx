/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelToTPIDTool/PixeldEdxTestAlg.h"

// IOVDbTest includes
#include "RegistrationServices/IIOVRegistrationSvc.h"

// Athena includes
#include "StoreGate/StoreGate.h"
#include "AthenaKernel/IAthenaOutputStreamTool.h"

// Gaudi includes
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/GaudiException.h" 
#include "GaudiKernel/ThreadLocalContext.h"

// AttributeList
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "AthenaBaseComps/AthCheckMacros.h"

#include <fstream>

#include "PathResolver/PathResolver.h"

/////////////////////////////////////////////////////////////////////////////

PixeldEdxTestAlg::PixeldEdxTestAlg(const std::string& the_name, ISvcLocator* pSvcLocator) :
        AthAlgorithm(the_name, pSvcLocator),
        m_tagID(""),
        m_regSvc("IOVRegistrationSvc", name() ),
        m_streamer("CondStream1")
{
    declareProperty("CalibrationFile", m_filename = "mcpar_signed_234.txt");
    declareProperty("TagID",             m_tagID);
    declareProperty("StreamName",        m_streamer);

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

PixeldEdxTestAlg::~PixeldEdxTestAlg()
{ }


namespace {
  MsgStream &operator << (MsgStream &log, const std::list<std::string> &keys) {
    for (std::list<std::string>::const_iterator itr=keys.begin(); itr!=keys.end(); ++itr) {
      log << *itr << " ";
    }
    return log;
  }
}

StatusCode PixeldEdxTestAlg::testCallBack( IOVSVC_CALLBACK_ARGS_P( /*I*/,keys) ) { 
  // print out the keys we were given (for info)
  ATH_MSG_INFO(  "PixeldEdxTestAlg::testCallBack callback invoked for keys:" << keys );
  return  StatusCode::SUCCESS;
}


StatusCode PixeldEdxTestAlg::initialize(){
    ATH_CHECK( AthAlgorithm::initialize() );
    //StatusCode sc;
    ATH_MSG_DEBUG ( "in initialize()"  );

    ATH_CHECK( m_regSvc.retrieve() );
    ATH_MSG_DEBUG ( "Found IOVRegistrationSvc "   );

    // Get Output Stream tool for writing
    ATH_CHECK( m_streamer.retrieve() );

    ATH_MSG_INFO ( "Tag to be used: " << m_tagID  );
    
    StatusCode sc( readWithBeginRun() );
    if (sc.isFailure()) {
      ATH_MSG_INFO ( "Unable to find read with BeginRun "  );
      return StatusCode::FAILURE;
    }  
    else {
      ATH_MSG_DEBUG ( "Read with BeginRun "   );
    }
    
    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode PixeldEdxTestAlg::readWithBeginRun(){
    ATH_MSG_INFO ( "in readWithBeginRun()"  );

    // As a result of the restructuring the EventIncident class (dropping the reference to EventInfo)
    // the old mechanism of overriding run&event&time is no longer working.
    // If we need this functionality, then we need to find a new way of implementing it.
    // For the time being this function simply fires a BeginRun incident using the EventContext, without overriding anything

    ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", name() );
    ATH_CHECK( incSvc.retrieve() );

    incSvc->fireIncident( Incident(name(), IncidentType::BeginRun, getContext()) );

    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode PixeldEdxTestAlg::createCondObjects(){
    ATH_MSG_INFO ( "in createCondObjects()"  );

      std::string file_name = PathResolver::find_file (m_filename, "DATAPATH");
  if (file_name.size()==0) {
    ATH_MSG_FATAL ( "Could not find dEdx calibration file" << m_filename  );
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


    StatusCode sc = detStore()->record(attrList0, "/PIXEL/PixdEdx");
    if (sc.isFailure()) {
	ATH_MSG_ERROR ( "Could not record PixeldEdxCalib"  );
	return( StatusCode::FAILURE);
    }
    
    return StatusCode::SUCCESS;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 


StatusCode PixeldEdxTestAlg::printCondObjects(){
    StatusCode sc;
    ATH_MSG_INFO ( "in printCondObjects()"  );


    const AthenaAttributeList*    attrList     = 0;

	// AttrList
        sc = detStore()->retrieve(attrList, "/PIXEL/PixdEdx");
	if (sc.isFailure()) {
	    ATH_MSG_ERROR ( "Could not retrieve PixdEdx"  );
	    // Using COOL, is failure
	    return( StatusCode::FAILURE);
	}
	if (0 == attrList) {
	    ATH_MSG_ERROR ( "IOVDbTestAttrList ptr is 0"  );
	    return( StatusCode::FAILURE);
	}
	else {
	    ATH_MSG_DEBUG ( "Retrieved IOVDbTestAttrList"  );
	}
  
	std::ostringstream attrStr1;
	attrList->print( attrStr1 );
	ATH_MSG_DEBUG ( "Attribute list " << attrStr1.str()  );



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


    ATH_MSG_DEBUG ( " in execute()"  );


		
        ATH_MSG_DEBUG ( "Creating condtions objects "  );
        StatusCode sc = createCondObjects();
        if (sc.isFailure()) {
            ATH_MSG_ERROR ( "Could not create cond objects"  );
            return( StatusCode::FAILURE);
        }

        //  Read objects from DetectorStore
        sc = printCondObjects();
        if (sc.isFailure()) {
            ATH_MSG_ERROR ( "Could not print out cond objects"  );
            return( StatusCode::FAILURE);
        }
    
    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode PixeldEdxTestAlg::finalize(){
    ATH_MSG_INFO ( "in finalize()"  );

        // Stream out and register objects here
        ATH_MSG_DEBUG ( "Stream out objects directly "  );
        StatusCode sc = streamOutCondObjects();
        if (sc.isFailure()) {
            ATH_MSG_ERROR ( "Could not stream out objects"  );
            return( StatusCode::FAILURE);
        }
        ATH_MSG_DEBUG ( "Streamed out OK "  );
        sc = registerCondObjects();
       if (sc.isFailure()) {
        ATH_MSG_ERROR ( "Could not register objects"  );
        return( StatusCode::FAILURE);
     }
    ATH_MSG_DEBUG ( "Register OK "  );  
    return StatusCode::SUCCESS;
}



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode 
PixeldEdxTestAlg::streamOutCondObjects(){
    // Get the messaging service, print where you are

    ATH_MSG_DEBUG ( "entering streamOutCondObjects "   );
    StatusCode sc = m_streamer->connectOutput();
    if (sc.isFailure()) {
	ATH_MSG_ERROR ( "Could not connect stream to output"  );
	return( StatusCode::FAILURE);
    }

/*    int npairs = 1;
    int index = 0;
    IAthenaOutputStreamTool::TypeKeyPairs  typeKeys(npairs);
	IAthenaOutputStreamTool::TypeKeyPair   dedxPair("PixeldEdxCalib", "");
	typeKeys[index] = dedxPair;
	++index;

    ATH_MSG_DEBUG ( "Stream out for pairs:"  );
    for (unsigned int i = 0; i < typeKeys.size(); ++i) {
        ATH_MSG_DEBUG ( typeKeys[i].first << " " << typeKeys[i].second << " "  );
    }
    
    sc = m_streamer->streamObjects(typeKeys);
    if (sc.isFailure()) {
	ATH_MSG_ERROR ( "Could not stream out PixeldEdxCalib"  );
	return( StatusCode::FAILURE);
    }
  */  
    sc = m_streamer->commitOutput();
    if (sc.isFailure()) {
	ATH_MSG_ERROR ( "Could not commit output stream"  );
	return( StatusCode::FAILURE);
    }

    return StatusCode::SUCCESS;
}


StatusCode
PixeldEdxTestAlg::registerCondObjects(){
    // Get the messaging service, print where you are
    ATH_MSG_DEBUG ( "entering registerCondObject "   );

    // Register the IOV DB with the conditions data written out
    StatusCode sc;

            ATH_CHECK( m_regSvc.retrieve() );

            // Using COOL, write out attrlist and collection of attrlists
            std::string tag = "no tag";
            if (m_tagID!="") {
                tag = m_tagID;
                sc = m_regSvc->registerIOV("AthenaAttributeList","/PIXEL/PixdEdx",m_tagID,IOVTime::MINRUN,IOVTime::MAXRUN,IOVTime::MINEVENT,IOVTime::MAXEVENT);
            } else {
                sc = m_regSvc->registerIOV("AthenaAttributeList","/PIXEL/PixdEdx","");
            }

            if (sc.isFailure()) {
                ATH_MSG_ERROR ( "Could not register in IOV DB for AthenaAttributeList"  );
                return( StatusCode::FAILURE);
            }
            ATH_MSG_DEBUG ( "registered AthenaAttributeList with " << tag  );

    return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 


