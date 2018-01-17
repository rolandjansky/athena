/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
 
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

//----------------------------------------------------------------//

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

/////////////////////////////////////////////////////////
#include "AmdcDb/AmdcDb.h"

//----------------------------------------------------------------//
#include "AmdcAth/AmdcsimrecAthenaSvc.h"

#include "AmdcDb/AmdcDbSvc.h"
#include "AmdcDb/AmdcDbRecordset.h"
#include "AmdcDb/AmdcDbSvcMakerFromRDB.h"
#include "AmdcDb/AmdcDbSvcMakerFromAmdc.h"
 
/// Standard Constructor
AmdcDb::AmdcDb(const std::string& name,ISvcLocator* svc):
  AthService(name,svc) ,
p_AmdcsimrecAthenaSvc ( "AmdcsimrecAthenaSvc",name )
{

  p_AmdcDbRecordsetEmptyOne = 0 ;

//Set Default values
  p_AmdcDbSvcFromAmdc = 0 ;
  p_AmdcDbSvcFromRDB  = 0 ;

  m_IsInitialized = false;
  m_IsUsable      = false;

  m_AMDBtag = "" ;
  m_UglyCodeOn = 0 ;

  m_FromAmdcEpsLengthMM = 4 ;
  m_FromAmdcEpsLengthCM = 5 ;
  m_FromAmdcEpsAngle    = 6 ;
  m_FromRDBEpsLengthMM = 4 ;
  m_FromRDBEpsLengthCM = 5 ;
  m_FromRDBEpsAngle    = 6 ;

  m_AmdcsimrecAthenaSvcUpdatedSvcDONE = false ;

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
   

  declareProperty("AmdcsimrecAthenaSvc", p_AmdcsimrecAthenaSvc);
  
  p_detStore = 0 ;

}
 
/// Standard Destructor
AmdcDb::~AmdcDb()  {
  delete p_AmdcDbSvcFromAmdc ;
  delete p_AmdcDbSvcFromRDB  ;
}
 
/// Service initialisation
StatusCode AmdcDb::initialize() {

  ATH_MSG_INFO( "Initialisation started     " ) ;

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
  sc = service("DetectorStore",p_detStore);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "DetectorStore service not found !" ) ;
    return StatusCode::FAILURE;
  }

//Retrieve p_AmdcsimrecAthenaSvc and set up call back
  if ( p_AmdcsimrecAthenaSvc.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve service " << p_AmdcsimrecAthenaSvc ) ;
    return StatusCode::FAILURE;
  } 
  ATH_MSG_INFO( "Retrieved service " << p_AmdcsimrecAthenaSvc ) ;

  if (p_AmdcsimrecAthenaSvc->InitializedSvc()) {
    ATH_MSG_INFO( "p_AmdcsimrecAthenaSvc->InitializedSvc() is true " ) ;
    m_AmdcsimrecAthenaSvcUpdatedSvcDONE = true ; 

  }else{
    ATH_MSG_INFO( "p_AmdcsimrecAthenaSvc->InitializedSvc() is false " ) ;

    sc=regFcnAmdcsimrecAthenaSvcUpdatedSvc();
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL("regFcnAmdcsimrecAthenaSvcUpdatedSvc failed" ) ;
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Done: regFcnAmdcsimrecAthenaSvcUpdatedSvc " ) ;

  }

//Do something now if possible
  if ( m_AmdcsimrecAthenaSvcUpdatedSvcDONE ){
    ATH_MSG_INFO( "m_AmdcsimrecAthenaSvcUpdatedSvcDONE found true in initialize " ) ;
      StatusCode sc = DoUpdatedSvc() ;
      if ( sc.isFailure() ) {
        ATH_MSG_FATAL( "DoUpdatedSvc failed" ) ; 
        return StatusCode::FAILURE;
      }
      m_IsInitialized = true;
  }
  
  ATH_MSG_INFO( "Initialisation ended     " ) ;
  return StatusCode::SUCCESS;

}
 
StatusCode AmdcDb::regFcnAmdcsimrecAthenaSvcUpdatedSvc()
{

  StatusCode sc = p_detStore->regFcn(
                         &AmdcsimrecAthenaSvc::UpdatedSvc,(&*p_AmdcsimrecAthenaSvc),
                         &AmdcDb::AmdcsimrecAthenaSvcUpdatedSvc,this,true
                         );
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Unable to register callback on AmdcDb::AmdcsimrecAthenaSvcUpdatedSvc from AmdcsimrecAthenaSvc::UpdatedSvc " ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Done: Register callback on AmdcDb::AmdcsimrecAthenaSvcUpdatedSvc from AmdcsimrecAthenaSvc::UpdatedSvc" ) ;
  sc = p_detStore->regFcn(
              &AmdcDb::AmdcsimrecAthenaSvcUpdatedSvc,this,
              &IRDBAccessSvcWithUpdate::UpdatedSvc,dynamic_cast<IRDBAccessSvcWithUpdate*>(this),true
              );
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Unable to register callback on AmdcDb::UpdatedSvc from AmdcDb::AmdcsimrecAthenaSvcUpdatedSvc " ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Done: Register callback on AmdcDb::UpdatedSvc from AmdcDb::AmdcsimrecAthenaSvcUpdatedSvc " ) ;

  return StatusCode::SUCCESS;
  
}
StatusCode AmdcDb::AmdcsimrecAthenaSvcUpdatedSvc(IOVSVC_CALLBACK_ARGS)
{
  ATH_MSG_INFO( "AmdcsimrecAthenaSvcUpdatedSvc called     " ) ;  

  if ( !(p_AmdcsimrecAthenaSvc->UsableSvc()) ) {
    ATH_MSG_INFO( "BUT p_AmdcsimrecAthenaSvc found NOT usable yet  " ) ;  
    return StatusCode::SUCCESS;
  }else{
    m_AmdcsimrecAthenaSvcUpdatedSvcDONE = true ;
    ATH_MSG_INFO( "AND p_AmdcsimrecAthenaSvc found usable   " ) ;  
  }


  StatusCode sc = DoUpdatedSvc() ;
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "DoUpdatedSvc failed" ) ; 
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}
StatusCode AmdcDb::DoUpdatedSvc()
{
  if ( !m_AmdcsimrecAthenaSvcUpdatedSvcDONE ){
    ATH_MSG_INFO( "DoUpdatedSvc() called BUT m_AmdcsimrecAthenaSvcUpdatedSvcDONE is false    " ) ;
    return StatusCode::SUCCESS;
  }

//Set Db Keys
  IRDBAccessSvc* pIRDBAccessSvc;
  StatusCode sc = service("RDBAccessSvc",pIRDBAccessSvc);
  if (!sc.isSuccess()) {
    ATH_MSG_FATAL( "Unable to get RDBAccessSvc." ) ;
    return StatusCode::FAILURE;
  }
  
//Get Amdc geometry
  Amdcsimrec* pAmdcsimrec = p_AmdcsimrecAthenaSvc->GetAmdcsimrec();
   
//Set p_AmdcDbSvcFromAmdc
  delete p_AmdcDbSvcFromAmdc ;
  p_AmdcDbSvcFromAmdc = new AmdcDbSvc();
  AmdcDbSvcMakerFromAmdc aAmdcDbSvcMakerFromAmdc;
  aAmdcDbSvcMakerFromAmdc.SetUglyCodeOn(m_UglyCodeOn);
  aAmdcDbSvcMakerFromAmdc.SetEpsLengthMM(m_FromAmdcEpsLengthMM);
  aAmdcDbSvcMakerFromAmdc.SetEpsLengthCM(m_FromAmdcEpsLengthCM);
  aAmdcDbSvcMakerFromAmdc.SetEpsAngle(m_FromAmdcEpsAngle);
  int ItemMax =0 ; 
  ItemMax =  m_TabFromAmdcEpsLengthMM.size() ; for (int Item=0; Item<ItemMax ; Item++) aAmdcDbSvcMakerFromAmdc.SetEpsLengthMM( m_TabFromAmdcEpsLengthMM[Item] , m_ValFromAmdcEpsLengthMM[Item] ) ;
  ItemMax =  m_TabFromAmdcEpsLengthCM.size() ; for (int Item=0; Item<ItemMax ; Item++) aAmdcDbSvcMakerFromAmdc.SetEpsLengthCM( m_TabFromAmdcEpsLengthCM[Item] , m_ValFromAmdcEpsLengthCM[Item] ) ;
  ItemMax =  m_TabFromAmdcEpsAngle.size()    ; for (int Item=0; Item<ItemMax ; Item++) aAmdcDbSvcMakerFromAmdc.SetEpsAngle   ( m_TabFromAmdcEpsAngle[Item]    , m_ValFromAmdcEpsAngle[Item]    ) ; 
  
  
  aAmdcDbSvcMakerFromAmdc.Set(pAmdcsimrec,p_AmdcDbSvcFromAmdc);
 
//Set p_AmdcDbSvcFromRDB
  delete p_AmdcDbSvcFromRDB  ;
  p_AmdcDbSvcFromRDB = new AmdcDbSvc();
  AmdcDbSvcMakerFromRDB aAmdcDbSvcMakerFromRDB;
  aAmdcDbSvcMakerFromRDB.SetEpsLengthMM(m_FromRDBEpsLengthMM);
  aAmdcDbSvcMakerFromRDB.SetEpsLengthCM(m_FromRDBEpsLengthCM);
  aAmdcDbSvcMakerFromRDB.SetEpsAngle(m_FromRDBEpsAngle);
  aAmdcDbSvcMakerFromRDB.Set(m_detectorKey,m_detectorNode,pIRDBAccessSvc,p_AmdcDbSvcFromRDB);
  ItemMax =  m_TabFromRDBEpsLengthMM.size() ; for (int Item=0; Item<ItemMax ; Item++) aAmdcDbSvcMakerFromRDB.SetEpsLengthMM( m_TabFromRDBEpsLengthMM[Item] , m_ValFromRDBEpsLengthMM[Item] ) ;
  ItemMax =  m_TabFromRDBEpsLengthCM.size() ; for (int Item=0; Item<ItemMax ; Item++) aAmdcDbSvcMakerFromRDB.SetEpsLengthCM( m_TabFromRDBEpsLengthCM[Item] , m_ValFromRDBEpsLengthCM[Item] ) ;
  ItemMax =  m_TabFromRDBEpsAngle.size()    ; for (int Item=0; Item<ItemMax ; Item++) aAmdcDbSvcMakerFromRDB.SetEpsAngle   ( m_TabFromRDBEpsAngle[Item]    , m_ValFromRDBEpsAngle[Item]    ) ; 

  m_IsUsable      = true ;
  
  return StatusCode::SUCCESS;
}

bool AmdcDb::UsableSvc()      {return m_IsUsable     ;}
bool AmdcDb::InitializedSvc() {return m_IsInitialized;}
StatusCode AmdcDb::UpdatedSvc(IOVSVC_CALLBACK_ARGS)
{
  ATH_MSG_INFO( "UpdatedSvc done     " ) ;
  return StatusCode::SUCCESS;
}
 
/// Service finalisation
StatusCode AmdcDb::finalize(){return StatusCode::SUCCESS;}
 
StatusCode AmdcDb::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if (IID_IRDBAccessSvc == riid)
    *ppvInterface = (IRDBAccessSvc*)this;
  else 
    return Service::queryInterface(riid, ppvInterface);

  addRef();
  return StatusCode::SUCCESS;
}
 
const IRDBRecordset* AmdcDb::getRecordset(const std::string& node,
					  const std::string& tag,
					  const std::string& /*tag2node*/ ,
				          const std::string& /*connName*/)
{
 if ( p_AmdcDbRecordsetEmptyOne == 0 ) p_AmdcDbRecordsetEmptyOne = new AmdcDbRecordset();
 if ( tag=="RDB"  ) {
   const IRDBRecordset* pIRDBRecordset = p_AmdcDbSvcFromRDB->getRecordset(node);
   if (pIRDBRecordset == 0) return p_AmdcDbRecordsetEmptyOne;
   return pIRDBRecordset;
 }
 if ( tag=="Amdc"  ) {
   const IRDBRecordset* pIRDBRecordset = p_AmdcDbSvcFromAmdc->getRecordset(node);
   if (pIRDBRecordset == 0) return p_AmdcDbRecordsetEmptyOne;
   return pIRDBRecordset;
 }
 return p_AmdcDbRecordsetEmptyOne;
}

//Functions
//  of IRDBAccessSvc Not implemented
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
IRDBRecordset_ptr AmdcDb::getRecordsetPtr(const std::string& node,
                                          const std::string& /*tag*/,
                                          const std::string& /*tag2node*/ ,
                                          const std::string& /*connName*/)
{
    IRDBRecordset* c_ptr = const_cast<IRDBRecordset*>((this)->getRecordset(node,"Amdc"));
    IRDBRecordset_ptr rec = IRDBRecordset_ptr(c_ptr);
    return rec;
}
RDBTagDetails AmdcDb::getTagDetails(const std::string& /*tag*/,
   	                                    const std::string& /*connName*/ )
{
 RDBTagDetails tagDetails;
 return tagDetails;
}                                            
