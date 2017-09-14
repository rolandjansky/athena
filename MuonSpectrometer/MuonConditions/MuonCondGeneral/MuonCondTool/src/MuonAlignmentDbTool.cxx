/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"
#include "SGTools/TransientAddress.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/GlobalUtilities.h"

#include "Identifier/IdentifierHash.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "PathResolver/PathResolver.h"
#include <fstream>
#include <string>

#include "MuonAlignmentData/ALinePar.h"
#include "MuonAlignmentData/BLinePar.h"
#include "MuonAlignmentData/CscInternalAlignmentPar.h"
#include "MuonAlignmentData/MdtAsBuiltPar.h"
#include "MuonAlignmentData/CorrContainer.h"

#include "MuonCondTool/MuonAlignmentDbTool.h" 
#include "MuonCondInterface/IMuonAlignmentDbTool.h"
#include "MuonCondSvc/MdtStringUtils.h"

#include <map>
#include "Identifier/Identifier.h"

//namespace MuonCalib 
//{      

MuonAlignmentDbTool::MuonAlignmentDbTool (const std::string& type,
                             const std::string& name,
                             const IInterface* parent)
  : AthAlgTool(type, name, parent), 
    m_detStore(0),
    m_IOVSvc(0),
    m_alineData(0),
    m_blineData(0),
    m_ilineData(0),
    m_asbuiltData(0),
    m_log( msgSvc(), name ),
    m_debug(false),
    m_verbose(false)  
{

  declareInterface<IMuonAlignmentDbTool>(this);

  m_alineDataLocation="A_LINE_CORR_updates";
  m_blineDataLocation="B_LINE_CORR_updates";
  m_ilineDataLocation="I_LINE_CORR_updates";
  m_asbuiltDataLocation="ASBUILT_CORR_updates";
  m_mdtIdHelper = 0;
  m_cscIdHelper = 0;
  m_rpcIdHelper = 0;
  m_tgcIdHelper = 0;
  m_geometryVersion = "";

  m_parlineFolder.clear();
  m_lastIOVread.clear();
  declareProperty("ParlineFolders",  m_parlineFolder);
  declareProperty("DumpALines",      m_dumpALines=false);
  declareProperty("DumpBLines",      m_dumpBLines=false);
  declareProperty("DumpILines",      m_dumpILines=false);
  declareProperty("DumpAsBuilt",     m_dumpAsBuilt=false);
  declareProperty("ILinesFromCondDB",m_ILinesFromDb=false);
  declareProperty("ALinesFile",      m_aLinesFile="");
  declareProperty("AsBuiltFile",     m_asBuiltFile="");
}

//StatusCode MuonAlignmentDbTool::updateAddress(SG::TransientAddress* tad)
StatusCode MuonAlignmentDbTool::updateAddress(StoreID::type /*storeID*/,
                                              SG::TransientAddress* tad,
                                              const EventContext& /*ctx*/)
{
  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;
  //   MsgStream log(msgSvc(), name());
    CLID clid        = tad->clID();
    std::string key  = tad->name();
    if ( 1324994866== clid && m_alineDataLocation == key)
    {
        if( m_debug )  m_log << MSG::DEBUG << "OK aline" << endmsg;
        return StatusCode::SUCCESS;
    }
    if (1124991377== clid && m_blineDataLocation == key)
    {
        if( m_debug )  m_log << MSG::DEBUG << "OK bline" << endmsg;
        return StatusCode::SUCCESS;
    }
    if (1285567354== clid && m_ilineDataLocation == key)
    {
        if( m_debug )  m_log << MSG::DEBUG << "OK iline" << endmsg;
        return StatusCode::SUCCESS;
    }
    if (1198729422== clid && m_asbuiltDataLocation == key)
    {
        if( m_debug )  m_log << MSG::DEBUG << "OK asbuilt" << endmsg;
        return StatusCode::SUCCESS;
    }
    return StatusCode::FAILURE;
}

StatusCode MuonAlignmentDbTool::initialize()
{ 
  //  MsgStream log(msgSvc(), name());
  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;

  m_log<<MSG::INFO<<"Initilalizing"<<endmsg;
  m_log << MSG::INFO << "In initialize ---- # of folders registered is "<<m_parlineFolder.size()<< endmsg;


  StatusCode sc = serviceLocator()->service("DetectorStore", m_detStore);
  if ( sc.isSuccess() ) {
    if( m_debug )  m_log << MSG::DEBUG << "Retrieved DetectorStore" << endmsg;
  }else{
    m_log << MSG::ERROR << "Failed to retrieve DetectorStore" << endmsg;
    return sc;
  }
  

  // Get interface to IOVSvc
  m_IOVSvc = 0;
  bool CREATEIF(true);
  sc = service( "IOVSvc", m_IOVSvc, CREATEIF );
  if ( sc.isFailure() )
  {
       m_log << MSG::ERROR << "Unable to get the IOVSvc" << endmsg;
       return StatusCode::FAILURE;
  }

  if(sc.isFailure()) return StatusCode::FAILURE;

  // *********************************************************************  
  // B Line for the moment are switched off....
  //log<<MSG::INFO<<"Call-back for B-lines..... maybe going to register"<<endmsg;
  //  sc=m_detStore->regFcn(&IMuonAlignmentDbTool::loadAlignBLine,
  //                        dynamic_cast<IMuonAlignmentDbTool*>(this),
  //                        parlineData, m_parlineFolder);
  //
  //log<<MSG::INFO<<"Call-back for B-lines registered"<<endmsg;
  //
  //if(sc.isFailure()) return StatusCode::FAILURE;
  // *********************************************************************  
  
  // *********************************************************************  
  // B Line for the moment are switched off....
  // m_detStore->regFcn(&IMuonAlignmentDbTool::loadAlignBLine,dynamic_cast<IMuonAlignmentDbTool*>(this),&MuonAlignmentDbSvc::loadAlignBLine,dynamic_cast<MuonAlignmentDbSvc*>(dbSvc));
  //log<<MSG::INFO<<"registered loadAlignBline"<<endmsg;
  // *********************************************************************  

  m_blineData = new BLineMapContainer();
  
  m_alineData = new ALineMapContainer() ;

  m_ilineData = new CscInternalAlignmentMapContainer();

  m_asbuiltData = new MdtAsBuiltMapContainer();
  if( m_debug )  m_log << MSG::DEBUG<<"MGM_alTool init time: pointer to A-line container =<"<<m_alineData<<"> to B-line container <"<<m_blineData<<"> to I-line container =<"<< m_ilineData << " to AsBuilt container =<" << m_asbuiltData <<endmsg;

  
  // record the containers in the detector store
  
  sc = m_detStore->record(m_alineData,m_alineDataLocation);
  if (sc == StatusCode::FAILURE) {
    m_log << MSG::ERROR << "Cannot record A Lines container in the detector store"
	<< endmsg;
    return sc;
  }
  else m_log << MSG::INFO << "A Lines container recorded in the detector store"<<endmsg;
           
  sc = m_detStore->record(m_blineData,m_blineDataLocation);
  if (sc == StatusCode::FAILURE) {
    m_log << MSG::ERROR << "Cannot record B Lines container in the detector store"
	<< endmsg;
    return sc;
  }
  else m_log << MSG::INFO << "B Lines container recorded in the detector store"<<endmsg;
  
  sc = m_detStore->record(m_asbuiltData,m_asbuiltDataLocation);
  if (sc == StatusCode::FAILURE) {
    m_log << MSG::ERROR << "Cannot record B Lines container in the detector store"
	<< endmsg;
    return sc;
  }
  else m_log << MSG::INFO << "As-Built container recorded in the detector store"<<endmsg;

  if( m_ILinesFromDb ) sc = m_detStore->record(m_ilineData,m_ilineDataLocation);
  if (sc == StatusCode::FAILURE && m_ILinesFromDb) {
    m_log << MSG::ERROR << "Cannot not record I-Lines container in the detector store"
	<< endmsg;
    return sc;
  }
  else m_log << MSG::INFO << "I-Lines container recorded in the detector store or skipped since not needed"<<endmsg;

  // Get the TransientAddress from DetectorStore and set "this" as the
  // AddressProvider
  SG::DataProxy* proxy = m_detStore->proxy(ClassID_traits<ALineMapContainer>::ID(), m_alineDataLocation);
  if (!proxy) {
      m_log << MSG::ERROR << "Unable to get the proxy for class ALineParContainer" << endmsg;
      return StatusCode::FAILURE;
  }
  else m_log << MSG::INFO << "proxy for class ALineParContainer found" << endmsg;



   IAddressProvider* addp = this;
   proxy->setProvider(addp, StoreID::DETECTOR_STORE);
   //   tad->setProvider(addp);
   if( m_debug )  m_log << MSG::DEBUG << "set address provider for ALineParContainer" << endmsg;

                                                                            

   proxy = m_detStore->proxy(ClassID_traits<BLineMapContainer>::ID(), m_blineDataLocation);
   if (!proxy) {
     m_log << MSG::ERROR << "Unable to get the proxy for class BLineParContainer" << endmsg;
     return StatusCode::FAILURE;
   }
   else m_log << MSG::INFO << "proxy for class BLineParContainer found" << endmsg;
   
   addp = this;
   proxy->setProvider(addp, StoreID::DETECTOR_STORE);
   //tad->setProvider(addp);
   if( m_debug )  m_log << MSG::DEBUG << "set address provider for BLineParContainer" << endmsg;

   proxy = m_detStore->proxy(ClassID_traits<CscInternalAlignmentMapContainer>::ID(), m_ilineDataLocation);
   if (!proxy && m_ILinesFromDb) {
     m_log << MSG::ERROR << "Unable to get the proxy for class CscInternalAlignmentMapContainer" << endmsg;
     return StatusCode::FAILURE;
   }
   else m_log << MSG::INFO << "proxy for class CscInternalAlignmentMapContainer found or not needed" << endmsg;
   
   addp = this;
   if( m_ILinesFromDb ) proxy->setProvider(addp, StoreID::DETECTOR_STORE);
   //tad->setProvider(addp);
   if( m_debug )  m_log << MSG::DEBUG << "set address provider for CscInternalAlignmentMapContainer" << endmsg;


   proxy = m_detStore->proxy(ClassID_traits<MdtAsBuiltMapContainer>::ID(), m_asbuiltDataLocation);
   if (!proxy) {
     m_log << MSG::ERROR << "Unable to get the proxy for class MdtAsBuiltParContainer" << endmsg;
     return StatusCode::FAILURE;
   }
   else m_log << MSG::INFO << "proxy for class MdtAsBuiltParContainer found" << endmsg;
   
   addp = this;
   proxy->setProvider(addp, StoreID::DETECTOR_STORE);
   //tad->setProvider(addp);
   if( m_debug )  m_log << MSG::DEBUG << "set address provider for MdtAsBuiltParContainer" << endmsg;

  return sc;
 
}


StatusCode MuonAlignmentDbTool::loadParameters(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  StatusCode sc = StatusCode::SUCCESS;
  //MsgStream log(msgSvc(), name());
  m_log << MSG::INFO << "entering loadParameters" << endmsg;

  if (m_geometryVersion == "")
  {
      const MuonGM::MuonDetectorManager* muonMgr;
      if (StatusCode::SUCCESS != m_detStore->retrieve(muonMgr))
      {
          m_log << MSG::WARNING << "Failed to retrieve the MuonDetectorManager - will assume geometry version R.03.03" << endmsg;
          m_geometryVersion = "R.03.03";
      }
      else {
          m_geometryVersion = muonMgr->geometryVersion();
          m_log << MSG::INFO << "geometry version from the MuonDetectorManager = "<<m_geometryVersion<< endmsg;
      }
  }
  else m_log << MSG::INFO << "geometry version already know (from the MuonDetectorManager) = "<<m_geometryVersion<< endmsg;
  
  
  if (m_mdtIdHelper == 0)
  {
      sc = m_detStore->retrieve(m_mdtIdHelper, "MDTIDHELPER" );
      if (!sc.isSuccess()) {
          m_log << MSG::ERROR << "Can't retrieve MdtIdHelper" << endmsg;
          return sc;
      }
      else m_log << MSG::INFO << "MdtIdHelper retrieved from the DetectorStore" << endmsg;
  }
  else if( m_debug )  m_log << MSG::DEBUG << "MdtIdHelper already initialized" << endmsg;
  if (m_cscIdHelper == 0)
  {
      sc = m_detStore->retrieve(m_cscIdHelper, "CSCIDHELPER" );
      if (!sc.isSuccess()) {
	m_log << MSG::ERROR << "Can't retrieve CscIdHelper" << endmsg;
          return sc;
      }
      else m_log << MSG::INFO << "CscIdHelper retrieved from the DetectorStore" << endmsg;
  }
  else if( m_debug )  m_log << MSG::DEBUG << "CscIdHelper already initialized" << endmsg;
  if (m_rpcIdHelper == 0)
  {
      sc = m_detStore->retrieve(m_rpcIdHelper, "RPCIDHELPER" );
      if (!sc.isSuccess()) {
          m_log << MSG::ERROR << "Can't retrieve RpcIdHelper" << endmsg;
          return sc;
      }
      else m_log << MSG::INFO << "RpcIdHelper retrieved from the DetectorStore" << endmsg;
  }
  else if( m_debug )  m_log << MSG::DEBUG << "RpcIdHelper already initialized" << endmsg;
  if (m_tgcIdHelper == 0)
  {
      sc = m_detStore->retrieve(m_tgcIdHelper, "TGCIDHELPER" );
      if (!sc.isSuccess()) {
          m_log << MSG::ERROR << "Can't retrieve TgcIdHelper" << endmsg;
          return sc;
      }
      else m_log << MSG::INFO << "TgcIdHelper retrieved from the DetectorStore" << endmsg;
  }
  else if( m_debug )  m_log << MSG::DEBUG << "TgcIdHelper already initialized" << endmsg;


  // reinitialize
  // retreive and remove the old collection 
  sc = m_detStore->retrieve( m_alineData, m_alineDataLocation );
  if(sc.isSuccess())  {
      m_log << MSG::INFO << "Previous A-line Container found in the DetStore <" << m_alineData <<">"<< endmsg;
      sc = m_detStore->remove( m_alineData );
      if(sc.isSuccess()) {
	m_log << MSG::INFO << "A-line Container at <" << m_alineData << "> removed "<<endmsg;
      }
      else m_log << MSG::INFO << "  I tried to remove an A-line - but then I remembered I don't know where they go. So it failed "
	       << endmsg;
  }
  else {
    if (m_alineData) {
        m_log << MSG::INFO << "Previous A-line Container not in the DetStore but pointer not NULL <" << m_alineData <<">"<< endmsg;
        delete m_alineData;
    }
  }
  m_alineData=0;
  
  sc = m_detStore->retrieve( m_blineData, m_blineDataLocation );
  if(sc.isSuccess())  {
      m_log << MSG::INFO << "Previous B-line Container found in the DetStore <" << m_blineData <<">"<< endmsg;
      sc = m_detStore->remove( m_blineData );
      if(sc.isSuccess()) {
	m_log << MSG::INFO << "B-line Container at <" << m_blineData << "> removed "<<endmsg;
      }
      else m_log << MSG::INFO << " wow - what is a b line , I wish I knew because then I could have removed it "<<endmsg;
  }
  else {
    if (m_blineData) {
        m_log << MSG::INFO << "Previous B-line Container not in the DetStore but pointer not NULL <" << m_blineData <<">"<< endmsg;
        delete m_blineData;
    }	
  }
  m_blineData=0;

  sc = m_detStore->retrieve( m_ilineData, m_ilineDataLocation );
  if(sc.isSuccess())  {
      m_log << MSG::INFO << "Previous I-line Container found in the DetStore <" << m_ilineData <<">"<< endmsg;
      if( m_ILinesFromDb ) {
        sc = m_detStore->remove( m_ilineData );
        if(sc.isSuccess()) {
	  m_log << MSG::INFO << "I-line Container at <" << m_ilineData << "> removed "<<endmsg;
        }
        else m_log << MSG::INFO << " wow - what is a i line , I wish I knew because then I could have removed it "<<endmsg;
      }
      else m_log << MSG::INFO << "Previous I-line Container kept in DetStore since reading from DB disabled"<<endmsg;
  }
  else {
    if (m_ilineData) {
        m_log << MSG::INFO << "Previous I-line Container not in the DetStore but pointer not NULL <" << m_ilineData <<">"<< endmsg;
        delete m_ilineData;
    }	
    if (!m_ILinesFromDb) sc = StatusCode::SUCCESS;
  }
  m_ilineData=0;

  sc = m_detStore->retrieve( m_asbuiltData, m_asbuiltDataLocation );
  if(sc.isSuccess())  {
      m_log << MSG::INFO << "Previous As-Built Container found in the DetStore <" << m_asbuiltData <<">"<< endmsg;
      sc = m_detStore->remove( m_asbuiltData );
      if(sc.isSuccess()) {
	m_log << MSG::INFO << "As-Built Container at <" << m_asbuiltData << "> removed "<<endmsg;
      }
      else m_log << MSG::INFO << " wow - what is as built , I wish I knew because then I could have removed it "<<endmsg;
  }
  else {
    if (m_blineData) {
        m_log << MSG::INFO << "Previous As-Built Container not in the DetStore but pointer not NULL <" << m_asbuiltData <<">"<< endmsg;
        delete m_asbuiltData;
    }	
  }
  m_asbuiltData=0;
  
  m_alineData = new ALineMapContainer() ;
  m_log << MSG::INFO<<"New ALineMapContainer pointer "<<(uintptr_t)m_alineData<<endmsg;
  m_blineData = new BLineMapContainer() ;
  m_log << MSG::INFO<<"New BLineMapContainer pointer "<<(uintptr_t)m_blineData<<endmsg;
  m_ilineData = new CscInternalAlignmentMapContainer() ;
  m_log << MSG::INFO<<"New CscInternalAlignmentMapContainer pointer "<<(uintptr_t)m_ilineData<<endmsg;
  m_asbuiltData = new MdtAsBuiltMapContainer() ;
  m_log << MSG::INFO<<"New AsBuiltMapContainer pointer "<<(uintptr_t)m_asbuiltData<<endmsg;

  if (keys.empty()) 
  {
      m_log << MSG::INFO << "LoadParameters triggered without IoV keys " << endmsg;
      m_log << MSG::INFO << "# of folders registered is "<<m_parlineFolder.size()<< endmsg;
      int ifc=0;
      int nLoadedFolders = 0;
      for (std::vector<std::string>::const_iterator itf=m_parlineFolder.begin();
           itf!=m_parlineFolder.end(); ++itf )
      {
          ifc++;
          std::string currentFolderName = *itf;
          m_log << MSG::INFO <<"Current Folder Name (*itf)="<<currentFolderName<<" at index "<<ifc<<endmsg;
	  if( m_debug ) m_log << MSG::DEBUG <<"Before reading ALineMapContainer pointer "<<(uintptr_t)m_alineData<<endmsg;
          if(currentFolderName.find("ILINES") != std::string::npos ) {
            if( m_ILinesFromDb ) sc = loadAlignILines(*itf);
            else {
              m_log << MSG::INFO << "DB folder for I-Lines given in job options however loading disabled " << endmsg;
              sc = StatusCode::SUCCESS;
              nLoadedFolders--;
            }
          }
          else if(currentFolderName.find("ASBUILTPARAMS") != std::string::npos) sc = loadAlignAsBuilt(*itf);
          else sc = loadAlignABLines(*itf);
          if (sc == StatusCode::RECOVERABLE)
              m_log << MSG::WARNING
                  << "loadAlignABLines couldn't find folder "<<currentFolderName<<endmsg;
	  else if (sc != StatusCode::SUCCESS)
              m_log << MSG::WARNING
                  << "Something wrong in loadAlignALine for folder "<<currentFolderName<<endmsg;
	  else nLoadedFolders++;
      }
      m_log << MSG::INFO << "# of folders found and loaded is "<<nLoadedFolders<< endmsg;
      if( m_debug ) m_log << MSG::DEBUG <<"end of loop over folders ---- ALineMapContainer pointer "<<(uintptr_t)m_alineData<<endmsg;
  }
  else
  {
      m_log << MSG::INFO << "LoadParameters has been triggered for the following keys " << endmsg;
      std::list<std::string>::const_iterator itr;
      for (itr=keys.begin(); itr!=keys.end(); ++itr)
      {
          m_log << MSG::INFO <<"key "<< *itr <<" at index I= "<<I<<endmsg;
      }
      for (itr=keys.begin(); itr!=keys.end(); ++itr)
      {
          m_log << MSG::INFO <<"Current key "<< *itr <<" at index I= "<<I<<endmsg;
          int ifc=0;
          for (std::vector<std::string>::const_iterator itf=m_parlineFolder.begin();
               itf!=m_parlineFolder.end(); ++itf )
          {
              ifc++;
              std::string currentFolderName = *itf;
              m_log << MSG::VERBOSE <<"Current Folder Name (*itf)="<<currentFolderName<<" at index "<<ifc<<endmsg;
              if(*itr==*itf) {
                  m_log << MSG::INFO <<"Matching Folder Name "<<currentFolderName<<endmsg;
                  if(currentFolderName.find("ILINES") != std::string::npos) {
                    if( m_ILinesFromDb ) sc = loadAlignILines(*itf);
                    else { 
                      m_log << MSG::INFO << "DB folder for I-Lines given in job options however loading disabled " << endmsg;
                      sc = StatusCode::SUCCESS;
                    }
                  }
                  else if(currentFolderName.find("ASBUILTPARAMS") != std::string::npos) sc = loadAlignAsBuilt(*itf);
                  else sc = loadAlignABLines(*itf);
                  if (sc != StatusCode::SUCCESS)
                      m_log << MSG::INFO
                          << "Something wrong in loadAlignABLines: " << *itr << endmsg;
              }
          }
      }
  }
  
  // finally record ABLineContainer
  if ((m_detStore->record( m_alineData, m_alineDataLocation )).isFailure()) return StatusCode::FAILURE;
  m_log << MSG::INFO<<"New A-line container recoded in the DetStore with key "<<m_alineDataLocation<<endmsg;
  if ((m_detStore->record( m_blineData, m_blineDataLocation )).isFailure()) return StatusCode::FAILURE;
  m_log << MSG::INFO<<"New B-line container recoded in the DetStore with key "<<m_blineDataLocation<<endmsg;
  if( m_ILinesFromDb ) {
    if ((m_detStore->record( m_ilineData, m_ilineDataLocation )).isFailure()) return StatusCode::FAILURE;
    m_log << MSG::INFO<<"New I-line container recoded in the DetStore with key "<<m_ilineDataLocation<<endmsg;
  }
  else m_log << MSG::INFO<<"I-line not recorded from this tool since functionality disabled."<<endmsg;
  if ((m_detStore->record( m_asbuiltData, m_asbuiltDataLocation )).isFailure()) return StatusCode::FAILURE;
  m_log << MSG::INFO<<"New As-built container recoded in the DetStore with key "<<m_asbuiltDataLocation<<endmsg;
  
  m_log << MSG::INFO <<"LoadParameters done !"<< endmsg;
  
  return sc;
    
}

StatusCode MuonAlignmentDbTool::loadAlignALine(IOVSVC_CALLBACK_ARGS_P(/*I*/,/*keys*/))
{return StatusCode::SUCCESS;}
StatusCode MuonAlignmentDbTool::loadAlignBLine(IOVSVC_CALLBACK_ARGS_P(/*I*/,/*keys*/))
{return StatusCode::SUCCESS;}
StatusCode MuonAlignmentDbTool::loadAlignILine(IOVSVC_CALLBACK_ARGS_P(/*I*/,/*keys*/))
{return StatusCode::SUCCESS;}
StatusCode MuonAlignmentDbTool::loadAlignAsBuilt(IOVSVC_CALLBACK_ARGS_P(/*I*/,/*keys*/))
{return StatusCode::SUCCESS;}

StatusCode MuonAlignmentDbTool::loadAlignABLines(std::string folderName) 
{

   MsgStream log(msgSvc(), name());
   StatusCode sc=StatusCode::SUCCESS;
   log << MSG::INFO << "Load alignment parameters from DB folder <"<<folderName<<">"<< endmsg;
   if( m_debug ) log << MSG::DEBUG << " ----- ALineMapContainer pointer "<<(uintptr_t)m_alineData<<endmsg;

   // retreive the collection of strings read out from the DB
   const CondAttrListCollection * atrc;
   sc=m_detStore->retrieve(atrc,folderName);
   if(sc.isFailure())  {
     log << MSG::WARNING 
         << "could not retreive the CondAttrListCollection from DB folder " 
         << folderName << endmsg;
     return StatusCode::RECOVERABLE;
   }
   else
       log<<MSG::INFO<<" CondAttrListCollection from DB folder have been obtained with size "<< atrc->size() <<endmsg;
   if( m_debug ) log << MSG::DEBUG << " data found ----- ALineMapContainer pointer "<<(uintptr_t)m_alineData<<endmsg;

   bool hasBLine = true;
   if (folderName.find("TGC")!=std::string::npos)  {
       hasBLine = false;
       log << MSG::INFO << "No BLines decoding will be attempted for folder named "<<folderName<<endmsg;
   }
   
   

   // unpack the strings in the collection and update the 
   // ALlineContainer in TDS
   int nLines = 0;
   int nDecodedLines = 0;
   int nNewDecodedALines = 0;
   int nNewDecodedBLines = 0;
   CondAttrListCollection::const_iterator itr;
   for (itr = atrc->begin(); itr != atrc->end(); ++itr)
   {
    const coral::AttributeList& atr=itr->second;
    std::string data;
    data=*(static_cast<const std::string*>((atr["data"]).addressOfData()));
    
    if( m_debug ) log << MSG::DEBUG << "Data load is " << data << " FINISHED HERE "<<endmsg;
    
    // Check the first word to see if it is a correction
    std::string type;
    
    //Parse corrections
    std::string since_str;
    std::string till_str;
    std::string delimiter = "\n";
      
    
      std::vector<std::string> lines;
      MuonCalib::MdtStringUtils::tokenize(data,lines,delimiter);
      for (unsigned int i=0; i<lines.size();i++)
      {
        ++nLines;
	//std::cout<<"scanning CLOB --- current line = "<<nLines<<std::endl;
	//	ALinePar* newALine = new ALinePar();
	std::string blobline = lines[i];
	
	std::string delimiter = ":";
	std::vector<std::string> tokens;
	MuonCalib::MdtStringUtils::tokenize(blobline,tokens,delimiter);
	type = tokens[0];
	//Parse line
	if (type.find("#")==0) {
	  //skip it
	  continue;
	}
	//std::cout << type ;
	if (type.find("Header")==0) {
	  std::string delimiter = "|";
	  std::vector<std::string> tokens;
	  MuonCalib::MdtStringUtils::tokenize(blobline,tokens,delimiter);
	  since_str = tokens[1];
	  till_str = tokens[2];
	  //std::cout << "Since: " << since_str << std::endl;
	  //std::cout << "Till: " << till_str << std::endl;
	}
	
	if (type.find("IOV")==0) 
	{
	    //std::cout<<" trovato IOV"<<std::endl;
	    std::string delimiter = " ";
	    std::vector<std::string> tokens;
	    MuonCalib::MdtStringUtils::tokenize(blobline,tokens,delimiter);
	    if (m_verbose){
		for (unsigned int i=0; i<tokens.size(); i++) 
		{	
		    log << MSG::VERBOSE <<tokens[i] <<" | ";
		}
	    }	    
	    int ival = 1;
	    long int iovThisBlob=0;
	    long int lastIOV = getLastIOVforThisFolder(folderName);

	    std::string str_iovThisBlob = tokens[ival];
	    sscanf(str_iovThisBlob.c_str(),"%80ld",&iovThisBlob);
	    log << MSG::INFO <<"Data read from folder "<< folderName
		  <<" have IoV = "<<iovThisBlob<<" to be compared with latest IOVread for the same folder = "<<lastIOV<<endmsg;
	    if (iovThisBlob == lastIOV) 
	    {
		log << MSG::INFO <<"Call back for  "<< folderName
		      <<" detected without a real IOV transition: IOV registered in this data "<<iovThisBlob
		      <<" to be compared with latest IOVread for the same folder = "
		      <<lastIOV<<endmsg<<"Ignoring this call back for folder "<<folderName<<endmsg;
		return StatusCode::SUCCESS;
	    }
	    log << MSG::INFO <<"Call back for  "<< folderName
		  <<" detected with a real IOV transition: IOV registered in this data "<<iovThisBlob
		  <<" to be compared with latest IOVread for the same folder = "
		  <<lastIOV<<endmsg;
	    setLastIOVforThisFolder(folderName, iovThisBlob);
	    
	}
	
	if (type.find("Corr")==0) {
//#: Corr line is counter typ,  jff,  jzz, job,                         * Chamber information 
//#:                       svalue,  zvalue, tvalue,  tsv,  tzv,  ttv,   * A lines 
//#:                       bz, bp, bn, sp, sn, tw, pg, tr, eg, ep, en   * B lines 
//#:                       chamber                                      * Chamber name 
//.... example
//Corr: EMS  4   1  0     2.260     3.461    28.639 -0.002402 -0.002013  0.000482    -0.006    -0.013 -0.006000  0.000000  0.000000     0.026    -0.353  0.000000  0.070000  0.012000    -0.012    EMS1A08

	    //std::cout<<" trovato Corr"<<std::endl;
	  std::string delimiter = " ";
	  std::vector<std::string> tokens;
	  MuonCalib::MdtStringUtils::tokenize(blobline,tokens,delimiter);
          if( m_verbose ) {
	      log << MSG::VERBOSE << "Parsing Line = ";
	      for (unsigned int i=0; i<tokens.size(); i++) log << MSG::VERBOSE <<tokens[i] <<" | ";
	      log<<endmsg;
	  }	  
	  bool thisRowHasBLine = true;
	  if (tokens.size()<15)
	    {
	      // only A-lines ..... old COOL blob convention for barrel 
	      thisRowHasBLine = false;
	      if( m_verbose )log << MSG::VERBOSE <<"(old COOL blob convention for barrel) skipping B-line decoding "<<endmsg;
	    }

	  // Start parsing
	  int ival=1;

	  // Station Component identification 
	  int jff; 
	  int jzz;
	  int job;
	  std::string stationType = tokens[ival++];	  
	  std::string jff_str = tokens[ival++]; 
	  sscanf(jff_str.c_str(),"%80d",&jff);
	  std::string jzz_str = tokens[ival++];
	  sscanf(jzz_str.c_str(),"%80d",&jzz);
	  std::string job_str = tokens[ival++];
	  sscanf(job_str.c_str(),"%80d",&job);
	  
	  // A-line
	  float s;
	  float z;
	  float t;
	  float ths;
	  float thz;
	  float tht;	  
	  std::string s_str = tokens[ival++];
	  sscanf(s_str.c_str(),"%80f",&s);
	  std::string z_str = tokens[ival++];
	  sscanf(z_str.c_str(),"%80f",&z);
	  std::string t_str = tokens[ival++];
	  sscanf(t_str.c_str(),"%80f",&t);
	  std::string ths_str = tokens[ival++];
	  sscanf(ths_str.c_str(),"%80f",&ths);
	  std::string thz_str = tokens[ival++];
	  sscanf(thz_str.c_str(),"%80f",&thz);
	  std::string tht_str = tokens[ival++];
	  sscanf(tht_str.c_str(),"%80f",&tht);

	  // B-line
	  float bz, bp, bn, sp, sn, tw, pg, tr, eg, ep, en;
	  std::string ChamberHwName="";
	  
	  if (hasBLine && thisRowHasBLine)
	  {	      
	  std::string tmp_str = tokens[ival++];
	  sscanf(tmp_str.c_str(),"%80f",&bz);
	  tmp_str = tokens[ival++];
	  sscanf(tmp_str.c_str(),"%80f",&bp);
	  tmp_str = tokens[ival++];
	  sscanf(tmp_str.c_str(),"%80f",&bn);
	  tmp_str = tokens[ival++];
	  sscanf(tmp_str.c_str(),"%80f",&sp);
	  tmp_str = tokens[ival++];
	  sscanf(tmp_str.c_str(),"%80f",&sn);
	  tmp_str = tokens[ival++];
	  sscanf(tmp_str.c_str(),"%80f",&tw);
	  tmp_str = tokens[ival++];
	  sscanf(tmp_str.c_str(),"%80f",&pg);
	  tmp_str = tokens[ival++];
	  sscanf(tmp_str.c_str(),"%80f",&tr);
	  tmp_str = tokens[ival++];
	  sscanf(tmp_str.c_str(),"%80f",&eg);
	  tmp_str = tokens[ival++];
	  sscanf(tmp_str.c_str(),"%80f",&ep);
	  tmp_str = tokens[ival++];
	  sscanf(tmp_str.c_str(),"%80f",&en);	  

	  // ChamberName (hardware convention)
	  ChamberHwName = tokens[ival++];
	  }
	  
	  if( m_verbose ) log << MSG::VERBOSE <<"Station type "  << stationType <<endmsg;
	  if( m_verbose ) log << MSG::VERBOSE <<"jff / jzz / job "  <<jff <<" / "<< jzz<<" / "<<job;
	  if (hasBLine) {
	    if( m_verbose ) log <<MSG::VERBOSE << " HardwareChamberName "<<ChamberHwName<<endmsg;
	    else if( m_verbose ) log  <<MSG::VERBOSE <<endmsg;
	  }
	  if( m_verbose ) log << MSG::VERBOSE <<"A-line: s,z,t "  << s <<" "<< z <<" "<< t;
	  if( m_verbose ) log << MSG::VERBOSE <<" ts,tz,tt "  << ths <<" "<< thz <<" "<< tht <<endmsg;
	  if (hasBLine) {
	    if (thisRowHasBLine) {
	      if( m_verbose ) log << MSG::VERBOSE <<"B-line:  bz,bp,bn "<<bz<<" "<<bp<<" "<<bn;
	      if( m_verbose ) log << MSG::VERBOSE <<" sp,sn "<<sp<<" "<<sn<<" tw,pg,tr "<<tw<<" "<<pg<<" "<<tr
		  <<" eg,ep,en "  <<eg<<" "<<ep<<" "<<en<<endmsg;
            }
	    else if( m_verbose ) log << MSG::VERBOSE <<"No B-line found"<<endmsg; 
	  }
	  
	  int stationName = m_mdtIdHelper->stationNameIndex(stationType);
	  Identifier id;
	  if( m_verbose ) log << MSG::VERBOSE<< "stationName  " << stationName << endmsg;
	  if (stationType.substr(0,1)=="T") 
	  {
	      // tgc case
	      int stPhi = MuonGM::stationPhiTGC(stationType, jff, jzz, m_geometryVersion);
	      int stEta = 1;
	      if (jzz<0) stEta = -1;			 
	      if (job != 0) {
		  // this should become the default now 
		  stEta=job;
		  if (jzz<0) stEta = -stEta;
	      }
	      id = m_tgcIdHelper->elementID(stationName, stEta, stPhi);
              if( m_verbose ) log << MSG::VERBOSE<< "identifier being assigned is " <<m_tgcIdHelper->show_to_string(id)<< endmsg;
	  }
	  else if (stationType.substr(0,1)=="C") 
	  {
	      // csc case
	      id = m_cscIdHelper->elementID(stationName, jzz, jff);
              if( m_verbose ) log << MSG::VERBOSE<< "identifier being assigned is " <<m_cscIdHelper->show_to_string(id)<< endmsg;
	  }
	  else if (stationType.substr(0,3)=="BML" && abs(jzz)==7) 
	  {
	      // rpc case
	      id = m_rpcIdHelper->elementID(stationName, jzz, jff, 1);
              if( m_verbose ) log << MSG::VERBOSE<< "identifier being assigned is " <<m_rpcIdHelper->show_to_string(id)<< endmsg;
	  }
	  else
          {
              id = m_mdtIdHelper->elementID(stationName, jzz, jff);
              if( m_verbose ) log << MSG::VERBOSE<< "identifier being assigned is " <<m_mdtIdHelper->show_to_string(id)<< endmsg;
	  }
          

          // new Aline
          ++nDecodedLines;
          ++nNewDecodedALines;
          ALinePar* newALine = new ALinePar();
	  newALine->setAmdbId(stationType, jff, jzz, job);
	  newALine->setParameters(s,z,t,ths,thz,tht);
	  newALine->isNew(true);	  
	  iALineMap ialine;
	  if((ialine = m_alineData->find(id)) != m_alineData->end())
          {
              log << MSG::WARNING<< "More than one (A-line) entry in folder "<<folderName<<" for  "
                  << stationType<<" at Jzz/Jff "<<jzz<<"/"<< jff <<" --- keep the latest one"<< endmsg;
              m_alineData->erase(id);
              --nNewDecodedALines;
          }
	  m_alineData->insert(std::make_pair(id,(ALinePar*)newALine));

          if (hasBLine && thisRowHasBLine)
	  {
	      // new Bline
	      ++nNewDecodedBLines;
	      BLinePar* newBLine = new BLinePar();
	      newBLine->setAmdbId(stationType, jff, jzz, job);
	      newBLine->setParameters(bz, bp, bn, sp, sn, tw, pg, tr, eg, ep, en);
	      newBLine->isNew(true);
	      iBLineMap ibline;
	      if((ibline = m_blineData->find(id)) != m_blineData->end())
	      {
		  log << MSG::WARNING<< "More than one (B-line) entry in folder "<<folderName<<" for  "
		      << stationType<<" at Jzz/Jff "<<jzz<<"/"<< jff <<" --- keep the latest one"<< endmsg;
		  m_blineData->erase(id);
		  --nNewDecodedBLines;
	      }
	      m_blineData->insert(std::make_pair(id,(BLinePar*)newBLine));
	  }
	}
      }
   }
  if( m_debug ) log << MSG::DEBUG<<"In folder <"<<folderName<<"> # lines/decodedLines/newDecodedALines/newDecodedBLines= "
       <<nLines<<"/"<<nDecodedLines<<"/"<<nNewDecodedALines<<"/"<<nNewDecodedBLines<<endmsg;

   // set A-lines from ASCII file
   if (m_aLinesFile!="" && (int)m_alineData->size()>0 ) setALinesFromAscii();
   
   // dump A-lines to log file
   if (m_dumpALines && (int)m_alineData->size()>0) dumpALines(folderName);
   
   // dump B-lines to log file
   if (m_dumpBLines && (int)m_blineData->size()>0) dumpBLines(folderName);
   
   if ( m_asBuiltFile!="" ) setAsBuiltFromAscii();

   if( m_verbose ) m_log << MSG::VERBOSE << "Collection CondAttrListCollection CLID "
       << atrc->clID() << endmsg;
   
   IOVRange range;
   if ((m_IOVSvc->getRange(1238547719, folderName, range)).isFailure()) return StatusCode::FAILURE;
   if( m_debug) log << MSG::DEBUG <<"IOVRange for these data is <"<<range<<">"<<endmsg;
   

// ss:01/07/2008 not clear if this is needed at all 
   SG::DataProxy* proxy = m_detStore->proxy(ClassID_traits<ALineMapContainer>::ID(), m_alineDataLocation);
    if (!proxy) {
      log << MSG::ERROR << "Unable to get the proxy for class ALineParContainer" << endmsg;
      return StatusCode::FAILURE;
    }
    IAddressProvider* addp = this;
    //    tad->setProvider(addp);
    proxy->setProvider(addp, StoreID::DETECTOR_STORE);
    if( m_debug ) log << MSG::DEBUG<< "set address provider for ALineParContainer" << endmsg;
   
   return  sc; 
}

StatusCode MuonAlignmentDbTool::loadAlignILines(std::string folderName) 
{

   MsgStream log(msgSvc(), name());
   StatusCode sc=StatusCode::SUCCESS;
   log << MSG::INFO << "Load alignment parameters from DB folder <"<<folderName<<">"<< endmsg;
   if( m_debug ) log << MSG::DEBUG << " ----- ILineMapContainer pointer "<<(uintptr_t)m_ilineData<<endmsg;

   // retreive the collection of strings read out from the DB
   const CondAttrListCollection * atrc;
   sc=m_detStore->retrieve(atrc,folderName);
   if(sc.isFailure())  {
     log << MSG::WARNING 
         << "could not retreive the CondAttrListCollection from DB folder " 
         << folderName << endmsg;
     return StatusCode::RECOVERABLE;
   }
   else
       log<<MSG::INFO<<" CondAttrListCollection from DB folder have been obtained with size "<< atrc->size() <<endmsg;
   if( m_debug ) log << MSG::DEBUG << " data found ----- ILineMapContainer pointer "<<(uintptr_t)m_ilineData<<endmsg;

   // unpack the strings in the collection and update the 
   // ALlineContainer in TDS
   int nLines = 0;
   int nDecodedLines = 0;
   int nNewDecodedILines = 0;
   CondAttrListCollection::const_iterator itr;
   for (itr = atrc->begin(); itr != atrc->end(); ++itr)
   {
    const coral::AttributeList& atr=itr->second;
    std::string data;
    data=*(static_cast<const std::string*>((atr["data"]).addressOfData()));
    
    if( m_debug ) log << MSG::DEBUG << "Data load is " << data << " FINISHED HERE "<<endmsg;
    
    // Check the first word to see if it is a correction
    std::string type;
    
    //Parse corrections
    std::string since_str;
    std::string till_str;
    std::string delimiter = "\n";
      
    
      std::vector<std::string> lines;
      MuonCalib::MdtStringUtils::tokenize(data,lines,delimiter);
      for (unsigned int i=0; i<lines.size();i++)
      {
        ++nLines;
	//std::cout<<"scanning CLOB --- current line = "<<nLines<<std::endl;
	//	CscInternalAlignmentPar* newILine = new CscInternalAlignmentPar();
	std::string blobline = lines[i];
	
	std::string delimiter = ":";
	std::vector<std::string> tokens;
	MuonCalib::MdtStringUtils::tokenize(blobline,tokens,delimiter);
	type = tokens[0];
	//Parse line
	if (type.find("#")==0) {
	  //skip it
	  continue;
	}
	//std::cout << type ;
	if (type.find("Header")==0) {
	  std::string delimiter = "|";
	  std::vector<std::string> tokens;
	  MuonCalib::MdtStringUtils::tokenize(blobline,tokens,delimiter);
	  since_str = tokens[1];
	  till_str = tokens[2];
	  //std::cout << "Since: " << since_str << std::endl;
	  //std::cout << "Till: " << till_str << std::endl;
	}
	
	if (type.find("IOV")==0) 
	{
	    //std::cout<<" trovato IOV"<<std::endl;
	    std::string delimiter = " ";
	    std::vector<std::string> tokens;
	    MuonCalib::MdtStringUtils::tokenize(blobline,tokens,delimiter);
	    if (m_verbose){
		for (unsigned int i=0; i<tokens.size(); i++) 
		{	
		    log << MSG::VERBOSE <<tokens[i] <<" | ";
		}
	    }	    
	    int ival = 1;
	    long int iovThisBlob=0;
	    long int lastIOV = getLastIOVforThisFolder(folderName);

	    std::string str_iovThisBlob = tokens[ival];
	    sscanf(str_iovThisBlob.c_str(),"%80ld",&iovThisBlob);
	    log << MSG::INFO <<"Data read from folder "<< folderName
		  <<" have IoV = "<<iovThisBlob<<" to be compared with latest IOVread for the same folder = "<<lastIOV<<endmsg;
	    if (iovThisBlob == lastIOV) 
	    {
		log << MSG::INFO <<"Call back for  "<< folderName
		      <<" detected without a real IOV transition: IOV registered in this data "<<iovThisBlob
		      <<" to be compared with latest IOVread for the same folder = "
		      <<lastIOV<<endmsg<<"Ignoring this call back for folder "<<folderName<<endmsg;
		return StatusCode::SUCCESS;
	    }
	    log << MSG::INFO <<"Call back for  "<< folderName
		  <<" detected with a real IOV transition: IOV registered in this data "<<iovThisBlob
		  <<" to be compared with latest IOVread for the same folder = "
		  <<lastIOV<<endmsg;
	    setLastIOVforThisFolder(folderName, iovThisBlob);
	    
	}
	
	if (type.find("Corr")==0) {
        //# Amdb like clob for ilines using geometry tag ISZT-R06-02 
        //# ISZT_DATA_ID VERS TYP JFF JZZ JOB JLAY TRAS TRAZ TRAT ROTS ROTZ ROTT
        //
        //.... example
        //Corr:  CSL 1 -1 3 1 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000

	  std::string delimiter = " ";
	  std::vector<std::string> tokens;
	  MuonCalib::MdtStringUtils::tokenize(blobline,tokens,delimiter);
          if( m_verbose ) {
	      log << MSG::VERBOSE << "Parsing Line = ";
	      for (unsigned int i=0; i<tokens.size(); i++) log << MSG::VERBOSE <<tokens[i] <<" | ";
	      log<<endmsg;
	  }	  

	  // Start parsing
	  int ival=1;

	  // Station Component identification 
	  int jff; 
	  int jzz;
	  int job;
	  int jlay;
	  std::string stationType = tokens[ival++];	  
	  std::string jff_str = tokens[ival++]; 
	  sscanf(jff_str.c_str(),"%80d",&jff);
	  std::string jzz_str = tokens[ival++];
	  sscanf(jzz_str.c_str(),"%80d",&jzz);
	  std::string job_str = tokens[ival++];
	  sscanf(job_str.c_str(),"%80d",&job);
          std::string jlay_str = tokens[ival++];
	  sscanf(jlay_str.c_str(),"%80d",&jlay);
	  
	  // I-line
	  float tras;
	  float traz;
	  float trat;
	  float rots;
	  float rotz;
          float rott;	
	  std::string tras_str = tokens[ival++];
	  sscanf(tras_str.c_str(),"%80f",&tras);
	  std::string traz_str = tokens[ival++];
	  sscanf(traz_str.c_str(),"%80f",&traz);
	  std::string trat_str = tokens[ival++];
	  sscanf(trat_str.c_str(),"%80f",&trat);
	  std::string rots_str = tokens[ival++];
	  sscanf(rots_str.c_str(),"%80f",&rots);
	  std::string rotz_str = tokens[ival++];
	  sscanf(rotz_str.c_str(),"%80f",&rotz);
	  std::string rott_str = tokens[ival++];
	  sscanf(rott_str.c_str(),"%80f",&rott);

	  if( m_verbose ) log << MSG::VERBOSE <<"Station type "  << stationType <<endmsg;
	  if( m_verbose ) log << MSG::VERBOSE <<"jff / jzz / job / jlay "  <<jff <<" / "<< jzz<<" / "<< job<<" / "<<jlay ;
	  if( m_verbose ) log << MSG::VERBOSE <<"I-line: tras,traz,trat "  << tras <<" "<< traz <<" "<< trat;
	  if( m_verbose ) log << MSG::VERBOSE <<" rots,rotz,rott "  << rots <<" "<< rotz <<" "<< rott <<endmsg;
	  
	  int stationName = m_cscIdHelper->stationNameIndex(stationType);
	  Identifier id;
	  if( m_verbose ) log << MSG::VERBOSE<< "stationName  " << stationName << endmsg;
	  if (stationType.substr(0,1)=="C") 
	  {
              // csc case
              int chamberLayer = 2;
              if (job != 3) log<<MSG::WARNING<<"job = "<<job<<" is not 3 => chamberLayer should be 1 - not existing ! setting 2"<<endmsg;
              id = m_cscIdHelper->channelID(stationType, jzz, jff, chamberLayer, jlay, 0, 1);
              if( m_verbose ) log << MSG::VERBOSE<< "identifier being assigned is " <<m_cscIdHelper->show_to_string(id)<< endmsg;
	  }
	  else { 
              log << MSG::ERROR<< "There is a non CSC chamber in the list of CSC internal alignment parameters."<< endmsg;
	  }

          // new Iline
          ++nDecodedLines;
          ++nNewDecodedILines;
          CscInternalAlignmentPar* newILine = new CscInternalAlignmentPar();
	  newILine->setAmdbId(stationType, jff, jzz, job, jlay);
	  newILine->setParameters(tras,traz,trat,rots,rotz,rott);
	  newILine->isNew(true);	  
	  iCscInternalAlignmentMap iiline;
	  if((iiline = m_ilineData->find(id)) != m_ilineData->end())
          {
              log << MSG::WARNING<< "More than one (I-line) entry in folder "<<folderName<<" for  "
                  << stationType<<" at Jzz/Jff/Jlay "<<jzz<<"/"<< jff<<"/"<< jlay <<" --- keep the latest one"<< endmsg;
              m_ilineData->erase(id);
              --nNewDecodedILines;
          }
	  m_ilineData->insert(std::make_pair(id,(CscInternalAlignmentPar*)newILine));

	}
      }
   }
  if( m_debug ) log << MSG::DEBUG<<"In folder <"<<folderName<<"> # lines/decodedLines/newDecodedILines= "
       <<nLines<<"/"<<nDecodedLines<<"/"<<nNewDecodedILines<<"/"<<endmsg;

   // set I-lines from ASCII file TBA
   // this option is currently coded in MuonGeoModel and should perhaps be migrated here
   
   // dump I-lines to log file TBA
   if (m_dumpILines && (int)m_ilineData->size()>0) dumpILines(folderName);
   
   if( m_verbose ) log << MSG::VERBOSE << "Collection CondAttrListCollection CLID "
       << atrc->clID() << endmsg;
   
   IOVRange range;
   if ((m_IOVSvc->getRange(1238547719, folderName, range)).isFailure()) return StatusCode::FAILURE;
   if( m_debug) log << MSG::DEBUG <<"IOVRange for these data is <"<<range<<">"<<endmsg;
   

// ss:01/07/2008 not clear if this is needed at all 
   SG::DataProxy* proxy = m_detStore->proxy(ClassID_traits<CscInternalAlignmentMapContainer>::ID(), m_ilineDataLocation);
    if (!proxy) {
      log << MSG::ERROR << "Unable to get the proxy for class ILineParContainer" << endmsg;
      return StatusCode::FAILURE;
    }
    IAddressProvider* addp = this;
    //    tad->setProvider(addp);
    proxy->setProvider(addp, StoreID::DETECTOR_STORE);
    if( m_debug ) log << MSG::DEBUG<< "set address provider for ILineParContainer" << endmsg;
   
   return  sc; 
}

StatusCode MuonAlignmentDbTool::loadAlignAsBuilt(std::string folderName) 
{

   StatusCode sc=StatusCode::SUCCESS;
   ATH_MSG_INFO( "Load alignment parameters from DB folder <"<<folderName<<">" );
   ATH_MSG_DEBUG( " ----- MdtAsBuiltMapContainer pointer "<<(uintptr_t)m_asbuiltData );

   // retreive the collection of strings read out from the DB
   const CondAttrListCollection * atrc;
   sc=m_detStore->retrieve(atrc,folderName);
   if(sc.isFailure())  {
     ATH_MSG_WARNING( "could not retreive the CondAttrListCollection from DB folder " 
                      << folderName  );
     return StatusCode::RECOVERABLE;
   }
   else
     ATH_MSG_INFO(" CondAttrListCollection from DB folder have been obtained with size "<< atrc->size()  );
   ATH_MSG_DEBUG( " data found ----- AsBuiltMapContainer pointer "<<(uintptr_t)m_ilineData );

   // unpack the strings in the collection and update the 
   // AsBuiltContainer in TDS
   int nLines = 0;
   int nDecodedLines = 0;
   int nNewDecodedAsBuilt = 0;
   MdtAsBuiltPar xPar;
   xPar.isNew(true);
   CondAttrListCollection::const_iterator itr;
   for (itr = atrc->begin(); itr != atrc->end(); ++itr)
   {
    const coral::AttributeList& atr=itr->second;
    std::string data;
    data=*(static_cast<const std::string*>((atr["data"]).addressOfData()));
    
    ATH_MSG_DEBUG( "Data load is " << data << " FINISHED HERE " );
    
    // Check the first word to see if it is a correction
    std::string type;
    
    //Parse corrections
    std::string delimiter = "\n";
    
    std::vector<std::string> lines;
    MuonCalib::MdtStringUtils::tokenize(data,lines,delimiter);
    for (unsigned int i=0; i<lines.size();i++)
    {
      ++nLines;
      //std::cout<<"scanning CLOB --- current line = "<<nLines<<std::endl;
      //	CscInternalAlignmentPar* newILine = new CscInternalAlignmentPar();
      std::string blobline = lines[i];

      std::string delimiter = ":";
      std::vector<std::string> tokens;
      MuonCalib::MdtStringUtils::tokenize(blobline,tokens,delimiter);
      type = tokens[0];
      //Parse line
      if (type.find("#")==0) {
         //skip it
         continue;
      }
	
      if (type.find("Corr")==0) {
	if (!xPar.setFromAscii(blobline)) {
          ATH_MSG_ERROR( "Unable to parse AsBuilt params from Ascii line: " << blobline  );
          continue;
	}

        std::string stationType="XXX";
        int jff = 0;
        int jzz = 0;
        int job = 0;
        xPar.getAmdbId(stationType, jff, jzz, job);
	Identifier id = m_mdtIdHelper->elementID(stationType, jzz, jff);

	ATH_MSG_VERBOSE("Station type jff jzz "  << stationType  << jff << " " << jzz  );
        ++nDecodedLines;
        ++nNewDecodedAsBuilt;
        ciMdtAsBuiltMap iasbuild;
        if((iasbuild = m_asbuiltData->find(id)) != m_asbuiltData->end())
        {
          ATH_MSG_WARNING( "More than one (As-built) entry in folder "<<folderName<<" for  "
                           << stationType<<" at Jzz/Jff "<<jzz<<"/"<< jff<<" --- keep the latest one" );
          m_asbuiltData->erase(id);
          --nNewDecodedAsBuilt;
        }
	m_asbuiltData->insert(std::make_pair(id,new MdtAsBuiltPar(xPar)));
      }
    }
   }
   ATH_MSG_DEBUG("In folder <"<<folderName<<"> # lines/decodedLines/newDecodedILines= "
                 <<nLines<<"/"<<nDecodedLines<<"/"<<nNewDecodedAsBuilt<<"/" );

   // dump I-lines to m_log file TBA
   if (m_dumpAsBuilt && (int)m_asbuiltData->size()>0) dumpAsBuilt(folderName);
   
   ATH_MSG_VERBOSE( "Collection CondAttrListCollection CLID " << atrc->clID() );
   
   IOVRange range;
   if ((m_IOVSvc->getRange(1238547719, folderName, range)).isFailure()) return StatusCode::FAILURE;
   ATH_MSG_DEBUG("IOVRange for these data is <"<<range<<">" );
   

// ss:01/07/2008 not clear if this is needed at all 
   SG::DataProxy* proxy = m_detStore->proxy(ClassID_traits<MdtAsBuiltMapContainer>::ID(), m_asbuiltDataLocation);
    if (!proxy) {
      ATH_MSG_ERROR( "Unable to get the proxy for class ILineParContainer"  );
      return StatusCode::FAILURE;
    }
    IAddressProvider* addp = this;
    //    tad->setProvider(addp);
    proxy->setProvider(addp, StoreID::DETECTOR_STORE);
    ATH_MSG_DEBUG( "set address provider for MdtAsBuiltParContainer"  );
   
   return  sc; 
}

    
std::vector<std::string> MuonAlignmentDbTool::abLineFolderNames() const
{return m_parlineFolder;}
std::string MuonAlignmentDbTool::aLineFolderName() const
{return *m_parlineFolder.begin();}
std::string MuonAlignmentDbTool::bLineFolderName() const
{return *m_parlineFolder.begin();}
std::string MuonAlignmentDbTool::iLineFolderName() const
{return *m_parlineFolder.begin();}
std::string MuonAlignmentDbTool::asBuiltFolderName() const
{return *m_parlineFolder.begin();}

const ALineMapContainer*
MuonAlignmentDbTool::ALineContainer() const
{return m_alineData;}
const BLineMapContainer*
MuonAlignmentDbTool::BLineContainer() const
{return m_blineData;}
const CscInternalAlignmentMapContainer*
MuonAlignmentDbTool::ILineContainer() const
{return m_ilineData;}
const MdtAsBuiltMapContainer*
MuonAlignmentDbTool::AsBuiltContainer() const
{return m_asbuiltData;}

int MuonAlignmentDbTool::stationPhiTGC(std::string stName, int fi, int zi_input) const
{
    // this function is defined to use as input fi in th erange 0 - N
    fi-=1;

    // remember that if this changes the one in MuonGeoModel/MuonChamber has to change 
    std::string stName3 = stName.substr(0,3);
    int stphi = 0;

    int zi = abs(zi_input);
    
    if (stName3 == "T4E")
    {
        switch (fi)
        {
        case 0: stphi = zi;
            break;
        case 1: stphi = zi+3;
            break;
        case 2: stphi = zi+6;
            break;
        case 3: stphi = zi+9-1; //zi are 2 and 3 (counting do not tart by 1)
            break;
        case 4: stphi = zi+11;
            break;
        case 5: stphi = zi+14;
            break;
        case 6: stphi = zi+16;
            break;
        case 7: stphi = zi+19;
            break;
        default: stphi = 0;
        }
        // minumum stPhi at phi 0
        stphi = stphi - 1;
        if (stphi <1) stphi = stphi + 21;
    }
    else 
    {
        int nch = 3;
        if ( (stName).substr(2,1) == "E" ) nch=6;
        int fioff = abs(zi);
        if (fioff>3 && (stName).substr(2,1) == "F")  fioff = fioff-3;
        // minumum stPhi at phi 0
        if ((stName).substr(2,1) == "F") fioff = fioff -1;
        else fioff = fioff -2;
        stphi = fi*nch + fioff;
        if (stphi<1) {
            if ((stName).substr(2,1) == "F") stphi = stphi + 24;
            else stphi = stphi + 48;
        }
    }

    return stphi;    
}

void MuonAlignmentDbTool::setALinesFromAscii() const
{
  MsgStream log(msgSvc(), name());
  log.setLevel(msgLevel());

  log << MSG::INFO << " Set alignment constants from text file "<< m_aLinesFile << endmsg;

  std::ifstream infile;
  infile.open(m_aLinesFile.c_str());
  
  char line[512] ;
  if( m_debug ) log << MSG::DEBUG << "reading file" << endmsg;
  
  while( infile.getline(line,512) ) {
    
    std::istringstream is(line) ;
    
    char AlineMarker[2];
    std::string name; int jff,jzz,obj;
    float tras,traz,trat,rots,rotz,rott;
    is >> AlineMarker >> name >> jff >> jzz >> obj
       >> tras >> traz >> trat >> rots >> rotz >> rott;
    
    if( m_debug ) log << MSG::DEBUG << "read line: "<< line << endmsg;
    
    if( AlineMarker[0] == '\0') {
      if( m_debug ) log << MSG::DEBUG << "read empty line!" << endmsg; 
    }
    else {
      
      // loop through A-line container and find the correct one
      std::string testStationType;
      int testJff,testJzz,testJob;
      ALinePar* ALine(0);
      for (ciALineMap cialine = m_alineData->begin(); cialine != m_alineData->end(); ++cialine) {
	(*cialine).second->getAmdbId(testStationType,testJff,testJzz,testJob);
	if (testStationType==name &&
	    testJff        == jff &&
	    testJzz        == jzz) {
	  ALine = (*cialine).second;
	  break;
	}
      }

      // set parameter if you found it
      if (ALine) 
	ALine->setParameters(tras,traz,trat,rots,rotz,rott);      
    }
  }
  return;
}

void MuonAlignmentDbTool::setAsBuiltFromAscii() const
{
  ATH_MSG_INFO (" Set alignment constants from text file "<< m_asBuiltFile);

  std::ifstream fin(m_asBuiltFile.c_str());
  std::string line;
  MdtAsBuiltPar xPar;
  xPar.isNew(true);
  int count = 0;
  while(getline(fin, line)) {
    if (line.find("Corr:")==0) {
      if (!xPar.setFromAscii(line)) {
        ATH_MSG_ERROR( "Unable to parse AsBuilt params from Ascii line: " << line  );
	} else {
        std::string stName="XXX";
        int jff = 0;
        int jzz = 0;
        int job = 0;
        xPar.getAmdbId(stName, jff, jzz, job);
        Identifier id = m_mdtIdHelper->elementID(stName, jzz, jff);
        if (!id.is_valid()) {
          ATH_MSG_ERROR( "Invalid MDT identifiers: sta=" << stName << " eta=" << jzz << " phi=" << jff  );
           continue;
        }
        iMdtAsBuiltMap ci = m_asbuiltData->begin();
        if((ci = m_asbuiltData->find(id)) != m_asbuiltData->end())
        {
          ATH_MSG_DEBUG( "Updating extisting entry in AsBuilt container for Station " <<stName<<" at Jzz/Jff "<<jzz<<"/"<< jff  );
          ATH_MSG_DEBUG( "That is strange since it's read from ASCII so this station is listed twice!"  );
          MdtAsBuiltPar* oldAsBuilt =  (*ci).second;
          m_asbuiltData->erase(id);
          delete oldAsBuilt; oldAsBuilt=0;
        } else {
          ATH_MSG_DEBUG( "New entry in AsBuilt container for Station "
                         <<stName<<" at Jzz/Jff "<<jzz<<"/"<< jff<<" --- in the container with key "<< m_mdtIdHelper->show_to_string(id) );
        }
	  m_asbuiltData->insert(std::make_pair(id,new MdtAsBuiltPar(xPar)));
	  ++count;
	}
    }
  }
  ATH_MSG_INFO( "Parsed AsBuilt parameters: " << count  );

  return;
}

void MuonAlignmentDbTool::dumpALines(const std::string& folderName)
{
    //  MsgStream m_log(msgSvc(), name());
   m_log.setLevel(msgLevel());

  m_log << MSG::INFO << "dumping A-lines for folder "<<folderName<<endmsg;
  
  if( m_debug ) m_log  << MSG::DEBUG << "A "
	    << "type jff jzz job s(cm)  z(cm)  t(cm)  ths(rad)  thz(rad)  tht(rad)  ID"<< endmsg;
  
  for (ciALineMap cialine = m_alineData->begin(); cialine != m_alineData->end(); ++cialine) {
    Identifier ALineId = (*cialine).first;
    ALinePar* ALine = (*cialine).second;
    std::string stationType;
    int jff,jzz,job;
    ALine->getAmdbId(stationType,jff,jzz,job);
    float s,z,t,ths,thz,tht;
    ALine->getParameters(s,z,t,ths,thz,tht);
    
    if( m_debug ) m_log << MSG::DEBUG <<"A " << std::setiosflags(std::ios::fixed|std::ios::right)
	      << std::setw(4) << stationType  <<" " 
	      << std::setw(2) << jff  <<"  " 
	      << std::setw(2) << jzz  <<" " 
	      << std::setw(2) << job  <<" "
	      << std::setw(6) << std::setprecision(3) <<  s   <<" "  // here cm !
	      << std::setw(6) << std::setprecision(3) <<  z   <<" "  // here cm !
	      << std::setw(6) << std::setprecision(3) <<  t   <<" "  // here cm !
	      << std::setw(6) << std::setprecision(6) << ths  <<" " 
	      << std::setw(6) << std::setprecision(6) << thz  <<" " 
	      << std::setw(6) << std::setprecision(6) << tht  <<" "
	      << ALineId << endmsg;
  }
  //std::cout<<std::endl;
}

void MuonAlignmentDbTool::dumpBLines(const std::string& folderName)
{
    //  MsgStream m_log(msgSvc(), name());
   m_log.setLevel(msgLevel());

  m_log << MSG::INFO << "dumping B-lines for folder "<<folderName<<endmsg;
  if( m_debug ) m_log << MSG::DEBUG << "B "
	    << "type jff jzz job bs       bp        bn        sp        sn        tw        pg        tr        eg        ep        en        ID"
		      << endmsg;;
  
  for (ciBLineMap cibline = m_blineData->begin(); cibline != m_blineData->end(); ++cibline) {
    Identifier BLineId = (*cibline).first;
    BLinePar* BLine = (*cibline).second;
    std::string stationType;
    int jff,jzz,job;
    BLine->getAmdbId(stationType,jff,jzz,job);
    float bs,bp,bn,sp,sn,tw,pg,tr,eg,ep,en;
    BLine->getParameters(bs,bp,bn,sp,sn,tw,pg,tr,eg,ep,en);
    
    if( m_debug ) m_log << MSG::DEBUG <<"B " << std::setiosflags(std::ios::fixed|std::ios::right)
	      << std::setw(4) << stationType  <<" " 
	      << std::setw(2) << jff  <<" " 
	      << std::setw(2) << jzz  <<" " 
	      << std::setw(2) << job  <<"  "
	      << std::setw(6) << std::setprecision(6) <<  bs  <<" "
	      << std::setw(6) << std::setprecision(6) <<  bp  <<" "
	      << std::setw(6) << std::setprecision(6) <<  bn  <<" "
	      << std::setw(6) << std::setprecision(6) <<  sp  <<" " 
	      << std::setw(6) << std::setprecision(6) <<  sn  <<" " 
	      << std::setw(6) << std::setprecision(6) <<  tw  <<" "
	      << std::setw(6) << std::setprecision(6) <<  pg  <<" "
	      << std::setw(6) << std::setprecision(6) <<  tr  <<" "
	      << std::setw(6) << std::setprecision(6) <<  eg  <<" "
	      << std::setw(6) << std::setprecision(6) <<  ep  <<" "
	      << std::setw(6) << std::setprecision(6) <<  en  <<" "
	      << BLineId << endmsg;
  }
  //std::cout<<std::endl;
}

void MuonAlignmentDbTool::dumpILines(const std::string& folderName)
{
    //  MsgStream m_log(msgSvc(), name());
   m_log.setLevel(msgLevel());

  m_log << MSG::INFO << "dumping I-lines for folder "<<folderName<<endmsg;
  if( m_debug ) m_log << MSG::DEBUG << "I "
	    << "\ttype\tjff\tjzz\tjob\tjlay\ttras\ttraz\ttrat\trots\trotz\trott"
		      << endmsg;;
  
  for (ciCscInternalAlignmentMap ciiline = m_ilineData->begin(); ciiline != m_ilineData->end(); ++ciiline) {
    Identifier ILineId = (*ciiline).first;
    CscInternalAlignmentPar* ILine = (*ciiline).second;
    std::string stationType;
    int jff,jzz,job,jlay;
    ILine->getAmdbId(stationType,jff,jzz,job,jlay);
    float tras,traz,trat,rots,rotz,rott;
    ILine->getParameters(tras,traz,trat,rots,rotz,rott);
    
    if( m_debug ) m_log << MSG::DEBUG <<"I\t" << std::setiosflags(std::ios::fixed|std::ios::right)
	      << std::setw(4) << stationType  <<"\t" 
	      << std::setw(2) << jff  <<"\t" 
	      << std::setw(2) << jzz  <<"\t" 
	      << std::setw(2) << job  <<"\t"
	      << std::setw(2) << jlay  <<"\t"
	      << std::setw(6) << std::setprecision(6) <<  tras  <<"\t"
	      << std::setw(6) << std::setprecision(6) <<  traz  <<"\t"
	      << std::setw(6) << std::setprecision(6) <<  trat  <<"\t"
	      << std::setw(6) << std::setprecision(6) <<  rots  <<"\t" 
	      << std::setw(6) << std::setprecision(6) <<  rotz  <<"\t" 
	      << std::setw(6) << std::setprecision(6) <<  rott  <<"\t"
	      << ILineId << endmsg;
  }
  //std::cout<<std::endl;
}

void MuonAlignmentDbTool::dumpAsBuilt(const std::string& folderName)
{
    //  MsgStream m_log(msgSvc(), name());
   m_log.setLevel(msgLevel());

  m_log << MSG::INFO << "dumping As-build for folder "<<folderName<<endmsg;
  //std::cout<<std::endl;
}

void MuonAlignmentDbTool::setLastIOVforThisFolder(std::string& folderName, long int iov)  
{
    if (m_lastIOVread.find(folderName)!=m_lastIOVread.end()) 
    {
	m_lastIOVread[folderName]=iov;       
    }
    else m_lastIOVread.insert(std::pair<std::string, long int>(folderName, iov));
}

long int MuonAlignmentDbTool::getLastIOVforThisFolder(std::string& folderName) 
{
    long int lastIOV = -1;
    if (m_lastIOVread.find(folderName)!=m_lastIOVread.end()) 
    {
	lastIOV = m_lastIOVread[folderName];
	if (m_debug) m_log << MSG::DEBUG <<"For folder named <" <<folderName<<"> latest IOV loaded was: "<<lastIOV<<endmsg;
    }
    else       
    {
	if (m_debug) m_log << MSG::DEBUG <<"For folder named <" <<folderName<<"> no previously loaded IOV exists"<<endmsg;
    }
    

    return lastIOV;
}


