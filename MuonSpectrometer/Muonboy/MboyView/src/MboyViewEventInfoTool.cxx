/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "AthenaKernel/IIOVDbSvc.h"
#include "AthenaKernel/IOVRange.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"
#include "EventInfo/EventID.h"
 
#include "GeoModelInterfaces/IGeoModelSvc.h"

/////////////////////////////////////////////////////////
#include "MboyView/MboyViewEventInfoTool.h"

#include "MboyAthEvt/MuonRecDigiStoreTDS.h"
#include "MboyAthEvt/MuonRecDigiStoreAth.h"

using namespace std;

static const InterfaceID IID_IMboyViewEventInfoTool("MboyViewEventInfoTool", 1, 0);

const InterfaceID& MboyViewEventInfoTool::interfaceID()
{
  return IID_IMboyViewEventInfoTool;
}

MboyViewEventInfoTool::MboyViewEventInfoTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p),
 p_MboyViewAGDD2GeoSwitchesTool       ( "MboyViewAGDD2GeoSwitchesTool/ConfiguredMboyViewAGDD2GeoSwitchesTool"            ) 
{

  declareInterface<MboyViewEventInfoTool>(this);

  declareProperty("MboyViewAGDD2GeoSwitchesTool"       , p_MboyViewAGDD2GeoSwitchesTool       ) ; 

//-----------------------------------------------------------------
// AGDD2GeoSwitches to be Viewed

  declareProperty("ViewAGDD2GeoSwitches"                                   , m_ViewAGDD2GeoSwitches                         = 1                                    ) ;
  declareProperty("SIM"                                   , m_SIM                         = 0                                    ) ;

  m_IsFirstEvent = 1 ; 

  m_KounterErrorMessage    =  0; 
  m_KounterErrorMessageMax = 10;   

}

MboyViewEventInfoTool::~MboyViewEventInfoTool(){}

StatusCode MboyViewEventInfoTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" ) ;
    return( StatusCode::FAILURE );
  }

  ATH_MSG_INFO( "================================" ) ;
  ATH_MSG_INFO( "=Proprieties are       " ) ;
  ATH_MSG_INFO( "=*ViewAGDD2GeoSwitches                           " << m_ViewAGDD2GeoSwitches                  ) ;
  ATH_MSG_INFO( "=*SIM                                            " << m_SIM                  ) ;
  ATH_MSG_INFO( "================================" ) ;

  if (m_ViewAGDD2GeoSwitches==1){
    if ( p_MboyViewAGDD2GeoSwitchesTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL( "Failed to retrieve tool " << p_MboyViewAGDD2GeoSwitchesTool ) ;
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Retrieved tool " << p_MboyViewAGDD2GeoSwitchesTool  ) ;
  }
  ATH_MSG_INFO( "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;

}

StatusCode MboyViewEventInfoTool::finalize(){return StatusCode::SUCCESS;}

StatusCode MboyViewEventInfoTool::ViewEventInfo(
               std::string /*MuonRecDigiStoreTDSLocation*/,
               int ViewAthIds,
               int ATCOSM,
               std::ofstream* pOut
){

  StatusCode sc = StatusCode::SUCCESS;

//Access
  const DataHandle<EventInfo> TheEventInfo;
  sc = evtStore()->retrieve(TheEventInfo);
  if ( sc.isFailure() ) {
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage){
      ATH_MSG_INFO( " retrieve TheEventInfo failed" ) ;
    }
    return( StatusCode::SUCCESS );
  }


//First Event actions
  if (m_IsFirstEvent == 1 ){
    m_IsFirstEvent = 0 ; 
    std::string AtlasVersion = GetGeoModelAtlasVersion() ;
    *pOut << "GEO "
          << AtlasVersion
          << std::endl;
    *pOut << "SIM "
          << m_SIM
          << std::endl;

    putConditionsTags(*pOut);

//  View AGDD2GeoSwitches
    if (m_ViewAGDD2GeoSwitches==1){
      if (p_MboyViewAGDD2GeoSwitchesTool) {
        sc = p_MboyViewAGDD2GeoSwitchesTool->ViewAGDD2GeoSwitches(
                                                   pOut
                                                  );
        if (sc.isFailure()){
          ATH_MSG_FATAL( " ViewAGDD2GeoSwitches failed " ) ;
          return( StatusCode::FAILURE );
        }
      }
    }

  }

 
//Dump
  *pOut << "NEW EVT "
        << TheEventInfo->event_ID()->event_number() << " "
        << TheEventInfo->event_ID()->run_number()   << " "
        << ViewAthIds << " "
        << ATCOSM << " "
	<< TheEventInfo->event_ID()->time_stamp()
        << std::endl;

  return StatusCode::SUCCESS;

}

std::string MboyViewEventInfoTool::GetGeoModelAtlasVersion()
{

  StatusCode sc;

  std::string ToBeReturned = "NoAtlasVersion" ;

  const IGeoModelSvc * pIGeoModelSvc;
  sc = service ("GeoModelSvc",pIGeoModelSvc);
  if (!sc.isSuccess()) {
    ATH_MSG_WARNING( "Unable to get pIGeoModelSvc." ) ;
    return ToBeReturned;
  }

  return pIGeoModelSvc->atlasVersion();
 
}

void MboyViewEventInfoTool::putConditionsTags(std::ostream& out) const {

  IIOVDbSvc* iovDbSvc(0);
  IOVRange range;
  string tag, foldername;
  bool retrieved;
  unsigned long long bytesRead;
  float readTime;
  bool ret;

  if (service("IOVDbSvc", iovDbSvc).isFailure()) {
    ATH_MSG_WARNING("Could not get IOVDbSvc. CDB TAG output disabled.");
    return;
  }

  vector<string> keys = iovDbSvc->getKeyList();
  for (vector<string>::const_iterator it(keys.begin()), end(keys.end()); it!=end; ++it) {
    const string& key = *it;
    ret = iovDbSvc->getKeyInfo(key, foldername, tag, range, retrieved, bytesRead, readTime);
    if (!ret) {
      ATH_MSG_ERROR("Unable to get info for key " << key << ". CDB TAG output disabled.");
      continue;
    }
    out << "CDB TAG " << foldername << " " << tag << endl;
  }

}
