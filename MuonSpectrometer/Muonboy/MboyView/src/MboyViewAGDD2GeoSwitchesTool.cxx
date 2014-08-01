/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

/////////////////////////////////////////////////////////
#include "MboyView/MboyViewAGDD2GeoSwitchesTool.h"

static const InterfaceID IID_IMboyViewAGDD2GeoSwitchesTool("MboyViewAGDD2GeoSwitchesTool", 1, 0);

const InterfaceID& MboyViewAGDD2GeoSwitchesTool::interfaceID()
{
  return IID_IMboyViewAGDD2GeoSwitchesTool;
}

MboyViewAGDD2GeoSwitchesTool::MboyViewAGDD2GeoSwitchesTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p),
p_IGeoModelSvc(0)
{

  declareInterface<MboyViewAGDD2GeoSwitchesTool>(this);

  m_KounterErrorMessage    =  0; 
  m_KounterErrorMessageMax = 10;   


}

MboyViewAGDD2GeoSwitchesTool::~MboyViewAGDD2GeoSwitchesTool(){}

// Initialize
StatusCode MboyViewAGDD2GeoSwitchesTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" ) ;
    return( StatusCode::FAILURE );
  }

  sc = service ("GeoModelSvc",p_IGeoModelSvc);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "GeoModelSvc service not found !" ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Found GeoModelSvc ") ;

  if(p_IGeoModelSvc->geoInitialized()) {

    ATH_MSG_INFO( "      p_IGeoModelSvc->geoInitialized() true "  ) ;

    sc = initializeFromGeomodel();
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL("initializeFromGeomodel failed" ) ;
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Done: initializeFromGeomodel " ) ;

  }else{

    ATH_MSG_INFO( "      p_IGeoModelSvc->geoInitialized() false  "  ) ;

   sc = regFcninitializeFromGeomodel();
   if ( sc.isFailure() ) {
     ATH_MSG_FATAL("regFcninitializeFromGeomodel failed" ) ;
     return StatusCode::FAILURE;
   }
   ATH_MSG_INFO( "Done: regFcninitializeFromGeomodel " ) ;

  }
  
  ATH_MSG_INFO( "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;

}

StatusCode MboyViewAGDD2GeoSwitchesTool::finalize(){return StatusCode::SUCCESS;}

StatusCode MboyViewAGDD2GeoSwitchesTool::ViewAGDD2GeoSwitches(
               std::ofstream* pOut
){

  *pOut << "A2G" << " "
        <<  m_AGDD2GeoSwitches.size()
        << std::endl;

  for (unsigned int Item = 0; Item < m_AGDD2GeoSwitches.size(); Item++) {
    std::string TheString = m_AGDD2GeoSwitches[Item] ;   
    *pOut << "A2G" 
          << " " <<  TheString
          << " " <<  Item
          << std::endl;
  }
  
  return StatusCode::SUCCESS;
  
}

StatusCode MboyViewAGDD2GeoSwitchesTool::regFcninitializeFromGeomodel() 
{

  ATH_MSG_INFO("----> regFcninitializeFromGeomodel is called" ) ; 

  StatusCode sc = detStore()->regFcn(
                          &IGeoModelSvc::align, &*p_IGeoModelSvc,
                          &MboyViewAGDD2GeoSwitchesTool::initializeFromGeomodelCallback, this
                         );
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Unable to register callback on MboyViewAGDD2GeoSwitchesTool::initializeFromGeomodelCallback from IGeoModelSvc::align" ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Done: Register callback on MboyViewAGDD2GeoSwitchesTool::initializeFromGeomodelCallback from IGeoModelSvc::align" ) ;

  return StatusCode::SUCCESS;
  
}

StatusCode MboyViewAGDD2GeoSwitchesTool::initializeFromGeomodelCallback(IOVSVC_CALLBACK_ARGS) 
{

  ATH_MSG_INFO("----> initializeFromGeomodelCallback is called" ) ; 
  
  StatusCode sc = initializeFromGeomodel();
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL("initializeFromGeomodel failed" ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Done: initializeFromGeomodel" ) ;

  return StatusCode::SUCCESS;
}

StatusCode MboyViewAGDD2GeoSwitchesTool::initializeFromGeomodel()
{

  ATH_MSG_INFO("----> initializeFromGeomodel is called" ) ; 

  std::string AtlasVersion = p_IGeoModelSvc->atlasVersion();
  std::string MuonVersion  = p_IGeoModelSvc->muonVersionOverride();

  m_detectorKey  = MuonVersion.empty() ? AtlasVersion : MuonVersion;
  m_detectorNode = MuonVersion.empty() ? "ATLAS" : "MuonSpectrometer";
  if ( MuonVersion == "CUSTOM"){
    m_detectorKey  = AtlasVersion ;
    m_detectorNode = "ATLAS"  ;
  } 
  
  StatusCode sc = initializeFromOracle() ;
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "initializeFromOracle failed" ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Done: initializeFromOracle" ) ;

  return StatusCode::SUCCESS;

}

StatusCode MboyViewAGDD2GeoSwitchesTool::initializeFromOracle()
{

  ATH_MSG_INFO("----> initializeFromOracle is called" ) ; 

  IRDBAccessSvc* pIRDBAccessSvc;
  StatusCode sc = service("RDBAccessSvc",pIRDBAccessSvc);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "Unable to get RDBAccessSvc." ) ;
    return StatusCode::FAILURE;
  }

  pIRDBAccessSvc->connect();

  ATH_MSG_INFO( "      Keys are  (key) "  << m_detectorKey  
      << " (node) " << m_detectorNode 
      ) ;

//Retrieve AGDD2GeoSwitches data
  std::string agdd2geoVersion = pIRDBAccessSvc->getChildTag("AGDD2GeoSwitches",m_detectorKey,m_detectorNode);
  m_AGDD2GeoSwitches.clear();
  if(!agdd2geoVersion.empty()) {

    ATH_MSG_INFO( "      agdd2geoVersion is NOT empty " ) ;

    std::string TheKEYNAME;
    int TheKEYVALUE; 
    const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset("AGDD2GeoSwitches",m_detectorKey,m_detectorNode);
    for(unsigned int i=0; i<pIRDBRecordset->size(); i++) {
      const IRDBRecord* record = (*pIRDBRecordset)[i];
      TheKEYNAME = record->getString("KEYNAME");
      TheKEYVALUE = record->getInt("KEYVALUE");
      if ( TheKEYVALUE == 1 ){
        ATH_MSG_INFO( "        Add to m_AGDD2GeoSwitches " << TheKEYNAME ) ;
        m_AGDD2GeoSwitches.push_back(TheKEYNAME);
      }
    }

  }else{

    ATH_MSG_INFO( "      agdd2geoVersion is empty " ) ;

  }
  ATH_MSG_INFO( "      AGDD2GeoSwitches data collected " ) ;

  pIRDBAccessSvc->shutdown();

  return StatusCode::SUCCESS;

}

