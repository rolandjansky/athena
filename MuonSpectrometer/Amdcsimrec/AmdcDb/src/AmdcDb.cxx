/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ISvcLocator.h"
 
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

#include "AmdcAth/AmdcsimrecAthenaSvc.h"
#include "AmdcDb/AmdcDb.h"
#include "AmdcDb/AmdcDbSvc.h"
#include "AmdcDb/AmdcDbRecordset.h"
#include "AmdcDb/AmdcDbSvcMakerFromRDB.h"
#include "AmdcDb/AmdcDbSvcMakerFromAmdc.h"
 
/// Standard Constructor
AmdcDb::AmdcDb(const std::string& name,ISvcLocator* svc)
  : AthService(name,svc) 
  , m_emptyRecordset(new AmdcDbRecordset())
  , m_AmdcsimrecAthenaSvc ( "AmdcsimrecAthenaSvc",name )
{
  m_AmdcDbSvcFromAmdc = 0 ;
  m_AmdcDbSvcFromRDB  = 0 ;

  m_AMDBtag = "" ;
  m_UglyCodeOn = 0 ;

  m_FromAmdcEpsLengthMM = 4 ;
  m_FromAmdcEpsLengthCM = 5 ;
  m_FromAmdcEpsAngle    = 6 ;
  m_FromRDBEpsLengthMM = 4 ;
  m_FromRDBEpsLengthCM = 5 ;
  m_FromRDBEpsAngle    = 6 ;

  std::string DummyString = "AAAA" ;
  m_TabFromAmdcEpsLengthMM.push_back(DummyString) ; m_TabFromAmdcEpsLengthMM.clear() ;
  m_TabFromAmdcEpsLengthCM.push_back(DummyString) ; m_TabFromAmdcEpsLengthCM.clear() ;
  m_TabFromAmdcEpsAngle.push_back(DummyString)    ; m_TabFromAmdcEpsAngle.clear   () ;
  m_TabFromRDBEpsLengthMM.push_back(DummyString)  ; m_TabFromRDBEpsLengthMM.clear () ;
  m_TabFromRDBEpsLengthCM.push_back(DummyString)  ; m_TabFromRDBEpsLengthCM.clear () ;
  m_TabFromRDBEpsAngle.push_back(DummyString)     ; m_TabFromRDBEpsAngle.clear    () ;

  int DummyInt = 0 ;
  m_ValFromAmdcEpsLengthMM.push_back(DummyInt) ; m_ValFromAmdcEpsLengthMM.clear() ;
  m_ValFromAmdcEpsLengthCM.push_back(DummyInt) ; m_ValFromAmdcEpsLengthCM.clear() ;
  m_ValFromAmdcEpsAngle.push_back(DummyInt)    ; m_ValFromAmdcEpsAngle.clear   () ;
  m_ValFromRDBEpsLengthMM.push_back(DummyInt)  ; m_ValFromRDBEpsLengthMM.clear () ;
  m_ValFromRDBEpsLengthCM.push_back(DummyInt)  ; m_ValFromRDBEpsLengthCM.clear () ;
  m_ValFromRDBEpsAngle.push_back(DummyInt)     ; m_ValFromRDBEpsAngle.clear    () ;

  //Declare the properties
  declareProperty("AMDBtag", m_AMDBtag) ;
  declareProperty("UglyCodeOn", m_UglyCodeOn) ;
  
  declareProperty( "FromAmdcEpsLengthMM" , m_FromAmdcEpsLengthMM ) ;
  declareProperty( "FromAmdcEpsLengthCM" , m_FromAmdcEpsLengthCM ) ;
  declareProperty( "FromAmdcEpsAngle"    , m_FromAmdcEpsAngle    ) ;
  declareProperty( "FromRDBEpsLengthMM"  , m_FromRDBEpsLengthMM  ) ;
  declareProperty( "FromRDBEpsLengthCM"  , m_FromRDBEpsLengthCM  ) ;
  declareProperty( "FromRDBEpsAngle"     , m_FromRDBEpsAngle     ) ;

  declareProperty( "TabFromAmdcEpsLengthMM"  , m_TabFromAmdcEpsLengthMM ) ;
  declareProperty( "TabFromAmdcEpsLengthCM"  , m_TabFromAmdcEpsLengthCM ) ;
  declareProperty( "TabFromAmdcEpsAngle"     , m_TabFromAmdcEpsAngle    ) ;
  declareProperty( "TabFromRDBEpsLengthMM"   , m_TabFromRDBEpsLengthMM  ) ;
  declareProperty( "TabFromRDBEpsLengthCM"   , m_TabFromRDBEpsLengthCM  ) ;
  declareProperty( "TabFromRDBEpsAngle"      , m_TabFromRDBEpsAngle     ) ;
   
  declareProperty( "ValFromAmdcEpsLengthMM"  , m_ValFromAmdcEpsLengthMM ) ;
  declareProperty( "ValFromAmdcEpsLengthCM"  , m_ValFromAmdcEpsLengthCM ) ;
  declareProperty( "ValFromAmdcEpsAngle"     , m_ValFromAmdcEpsAngle    ) ;
  declareProperty( "ValFromRDBEpsLengthMM"   , m_ValFromRDBEpsLengthMM  ) ;
  declareProperty( "ValFromRDBEpsLengthCM"   , m_ValFromRDBEpsLengthCM  ) ;
  declareProperty( "ValFromRDBEpsAngle"      , m_ValFromRDBEpsAngle     ) ;
   

  declareProperty("AmdcsimrecAthenaSvc", m_AmdcsimrecAthenaSvc);
  
  m_detStore = 0 ;
}
 
/// Standard Destructor
AmdcDb::~AmdcDb()  {
  delete m_AmdcDbSvcFromAmdc ;
  delete m_AmdcDbSvcFromRDB  ;
}
 
/// Service initialisation
StatusCode AmdcDb::initialize() {

  ATH_MSG_INFO( "Initialisation started" ) ;

  StatusCode sc=AthService::initialize();
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Service::initialize() failed" ) ;
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO( "================================" ) ;
  ATH_MSG_INFO( "=Proprieties are     " ) ;
  ATH_MSG_INFO( "= AMDBtag                  " << m_AMDBtag              ) ;
  ATH_MSG_INFO( "= UglyCodeOn               " << m_UglyCodeOn           ) ;
  ATH_MSG_INFO( "= FromAmdcEpsLengthMM      " << m_FromAmdcEpsLengthMM  ) ;
  ATH_MSG_INFO( "= FromAmdcEpsLengthCM      " << m_FromAmdcEpsLengthCM  ) ;
  ATH_MSG_INFO( "= FromAmdcEpsAngle         " << m_FromAmdcEpsAngle     ) ;
  ATH_MSG_INFO( "= FromRDBEpsLengthMM       " << m_FromRDBEpsLengthMM   ) ;
  ATH_MSG_INFO( "= FromRDBEpsLengthCM       " << m_FromRDBEpsLengthCM   ) ;
  ATH_MSG_INFO( "= FromRDBEpsAngle          " << m_FromRDBEpsAngle      ) ;
  ATH_MSG_INFO( "= " ) ;
  int ItemMax =0 ; 
  ItemMax =  m_TabFromAmdcEpsLengthMM.size() ; for (int Item=0; Item<ItemMax ; Item++)  ATH_MSG_INFO( "= Multi FromAmdcEpsLengthMM " << m_TabFromAmdcEpsLengthMM[Item] << " " << m_ValFromAmdcEpsLengthMM[Item] ) ;
  ItemMax =  m_TabFromAmdcEpsLengthCM.size() ; for (int Item=0; Item<ItemMax ; Item++)  ATH_MSG_INFO( "= Multi FromAmdcEpsLengthCM " << m_TabFromAmdcEpsLengthCM[Item] << " " << m_ValFromAmdcEpsLengthCM[Item] ) ;
  ItemMax =  m_TabFromAmdcEpsAngle.size()    ; for (int Item=0; Item<ItemMax ; Item++)  ATH_MSG_INFO( "= Multi FromAmdcEpsAngle    " << m_TabFromAmdcEpsAngle[Item]    << " " << m_ValFromAmdcEpsAngle[Item]    ) ;
  ATH_MSG_INFO( "= " ) ;
  ItemMax =  m_TabFromRDBEpsLengthMM.size()  ; for (int Item=0; Item<ItemMax ; Item++)  ATH_MSG_INFO( "= Multi FromRDBEpsLengthMM  " << m_TabFromRDBEpsLengthMM[Item]  << " " << m_ValFromRDBEpsLengthMM[Item]  ) ;
  ItemMax =  m_TabFromRDBEpsLengthCM.size()  ; for (int Item=0; Item<ItemMax ; Item++)  ATH_MSG_INFO( "= Multi FromRDBEpsLengthCM  " << m_TabFromRDBEpsLengthCM[Item]  << " " << m_ValFromRDBEpsLengthCM[Item]  ) ;
  ItemMax =  m_TabFromRDBEpsAngle.size()     ; for (int Item=0; Item<ItemMax ; Item++)  ATH_MSG_INFO( "= Multi FromRDBEpsAngle     " << m_TabFromRDBEpsAngle[Item]     << " " << m_ValFromRDBEpsAngle[Item]     ) ;

  ATH_MSG_INFO( "================================" ) ;

//Set Db Keys
  m_detectorKey  = m_AMDBtag ;
  m_detectorNode = "MuonSpectrometer"  ;
  
  ATH_MSG_INFO( "Keys are  (key) "  << m_detectorKey << " (node) " << m_detectorNode ) ;

//Set pointer on DetectorStore 
  sc = service("DetectorStore",m_detStore);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "DetectorStore service not found !" ) ;
    return StatusCode::FAILURE;
  }

//Retrieve m_AmdcsimrecAthenaSvc and set up call back
  if ( m_AmdcsimrecAthenaSvc.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve service " << m_AmdcsimrecAthenaSvc ) ;
    return StatusCode::FAILURE;
  } 
  ATH_MSG_INFO( "Retrieved service " << m_AmdcsimrecAthenaSvc ) ;

  ATH_CHECK(DoUpdatedSvc());
  
  ATH_MSG_INFO( "Initialisation ended     " ) ;
  return StatusCode::SUCCESS;

}

StatusCode AmdcDb::DoUpdatedSvc()
{

//Set Db Keys
  IRDBAccessSvc* pIRDBAccessSvc;
  StatusCode sc = service("RDBAccessSvc",pIRDBAccessSvc);
  if (!sc.isSuccess()) {
    ATH_MSG_FATAL( "Unable to get RDBAccessSvc." ) ;
    return StatusCode::FAILURE;
  }
  
//Get Amdc geometry
  Amdcsimrec* pAmdcsimrec = m_AmdcsimrecAthenaSvc->GetAmdcsimrec();
   
//Set m_AmdcDbSvcFromAmdc
  delete m_AmdcDbSvcFromAmdc ;
  m_AmdcDbSvcFromAmdc = new AmdcDbSvc();
  AmdcDbSvcMakerFromAmdc aAmdcDbSvcMakerFromAmdc;
  aAmdcDbSvcMakerFromAmdc.SetUglyCodeOn(m_UglyCodeOn);
  aAmdcDbSvcMakerFromAmdc.SetEpsLengthMM(m_FromAmdcEpsLengthMM);
  aAmdcDbSvcMakerFromAmdc.SetEpsLengthCM(m_FromAmdcEpsLengthCM);
  aAmdcDbSvcMakerFromAmdc.SetEpsAngle(m_FromAmdcEpsAngle);
  int ItemMax =0 ; 
  ItemMax =  m_TabFromAmdcEpsLengthMM.size() ; for (int Item=0; Item<ItemMax ; Item++) aAmdcDbSvcMakerFromAmdc.SetEpsLengthMM( m_TabFromAmdcEpsLengthMM[Item] , m_ValFromAmdcEpsLengthMM[Item] ) ;
  ItemMax =  m_TabFromAmdcEpsLengthCM.size() ; for (int Item=0; Item<ItemMax ; Item++) aAmdcDbSvcMakerFromAmdc.SetEpsLengthCM( m_TabFromAmdcEpsLengthCM[Item] , m_ValFromAmdcEpsLengthCM[Item] ) ;
  ItemMax =  m_TabFromAmdcEpsAngle.size()    ; for (int Item=0; Item<ItemMax ; Item++) aAmdcDbSvcMakerFromAmdc.SetEpsAngle   ( m_TabFromAmdcEpsAngle[Item]    , m_ValFromAmdcEpsAngle[Item]    ) ; 
  
  
  aAmdcDbSvcMakerFromAmdc.Set(pAmdcsimrec,m_AmdcDbSvcFromAmdc);
 
//Set m_AmdcDbSvcFromRDB
  delete m_AmdcDbSvcFromRDB  ;
  m_AmdcDbSvcFromRDB = new AmdcDbSvc();
  AmdcDbSvcMakerFromRDB aAmdcDbSvcMakerFromRDB;
  aAmdcDbSvcMakerFromRDB.SetEpsLengthMM(m_FromRDBEpsLengthMM);
  aAmdcDbSvcMakerFromRDB.SetEpsLengthCM(m_FromRDBEpsLengthCM);
  aAmdcDbSvcMakerFromRDB.SetEpsAngle(m_FromRDBEpsAngle);
  aAmdcDbSvcMakerFromRDB.Set(m_detectorKey,m_detectorNode,pIRDBAccessSvc,m_AmdcDbSvcFromRDB);
  ItemMax =  m_TabFromRDBEpsLengthMM.size() ; for (int Item=0; Item<ItemMax ; Item++) aAmdcDbSvcMakerFromRDB.SetEpsLengthMM( m_TabFromRDBEpsLengthMM[Item] , m_ValFromRDBEpsLengthMM[Item] ) ;
  ItemMax =  m_TabFromRDBEpsLengthCM.size() ; for (int Item=0; Item<ItemMax ; Item++) aAmdcDbSvcMakerFromRDB.SetEpsLengthCM( m_TabFromRDBEpsLengthCM[Item] , m_ValFromRDBEpsLengthCM[Item] ) ;
  ItemMax =  m_TabFromRDBEpsAngle.size()    ; for (int Item=0; Item<ItemMax ; Item++) aAmdcDbSvcMakerFromRDB.SetEpsAngle   ( m_TabFromRDBEpsAngle[Item]    , m_ValFromRDBEpsAngle[Item]    ) ; 

  return StatusCode::SUCCESS;
}

StatusCode AmdcDb::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if (IID_IRDBAccessSvc == riid)
    *ppvInterface = (IRDBAccessSvc*)this;
  else 
    return Service::queryInterface(riid, ppvInterface);

  addRef();
  return StatusCode::SUCCESS;
}

IRDBRecordset_ptr AmdcDb::getRecordsetPtr(const std::string& node,
                                          const std::string& tag,
                                          const std::string& /*tag2node*/ ,
                                          const std::string& /*connName*/)
{
  if(tag=="RDB") {
    return m_AmdcDbSvcFromRDB->getRecordset(node);
  }
  else if(tag=="Amdc") {
    return m_AmdcDbSvcFromAmdc->getRecordset(node);
  }
  else {
    return m_emptyRecordset;
  }
}

// Functions of IRDBAccessSvc Not implemented
bool AmdcDb::connect   (const std::string& /*connName*/ ) { std::cout << " AmdcDb::connect Not implemented    " << std::endl ; return true; }

bool AmdcDb::disconnect(const std::string& /*connName*/ ) { std::cout << " AmdcDb::disconnect Not implemented " << std::endl ; return true; }

bool AmdcDb::shutdown  (const std::string& /*connName*/ ) { std::cout << " AmdcDb::shutdown Not implemented   " << std::endl ; return true; }

std::string AmdcDb::getChildTag(const std::string& /*childNode*/  ,
			        const std::string& /*parentTag*/  ,
			        const std::string& /*parentNode*/ ,
			        const std::string& /*connName*/   )
{ std::cout << " AmdcDb::getChildTag Not implemented " << std::endl ; std::string ToBeReturned = "Not Implemented"; return ToBeReturned; }

std::unique_ptr<IRDBQuery> AmdcDb::getQuery(const std::string& /*node*/     ,
                            const std::string& /*tag*/      ,
                            const std::string& /*tag2node*/ ,
                            const std::string& /*connName*/ ){ std::cout << " AmdcDb::getQuery Not implemented " << std::endl ; return 0; }

void AmdcDb::getTagDetails(RDBTagDetails& /*tagDetails*/,
                           const std::string& /*tag*/,
                           const std::string& /*connName*/ )
{
}                                            
