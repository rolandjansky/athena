/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawUtils/LArRoI_Map.h" 
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"

#include "LArRawUtils/LArTT_Sampling.h" 

#include "CaloTTDetDescr/CaloTTDescriptor.h"
#include "CaloTTDetDescr/CaloTTDescrManager.h"
#include "CaloIdentifier/CaloLVL1_ID.h" 
#include "LArIdentifier/LArOnlineID.h" 

#include "LArTools/LArCablingService.h" 
#include "CaloTriggerTool/CaloTriggerTowerService.h" 

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/IIncidentSvc.h"

#include <math.h>
#include <fstream>

// This class converts a LAr offline ID and Online Identifier into a RoI id, 
// 

static const InterfaceID IID_ILArRoI_Map("LArRoI_Map", 1, 0);

const InterfaceID& LArRoI_Map::interfaceID( ) 
{ return IID_ILArRoI_Map; }

// default contructor 
LArRoI_Map::LArRoI_Map( const std::string& type, const std::string& name,
        const IInterface* parent ) 
  :  AthAlgTool(type,name,parent), m_print(false),m_validCache(false)
{

  declareInterface<LArRoI_Map>( this );

  // property 
  declareProperty("Print",m_print);
  declareProperty("LoadAtBegin",m_loadAtBegin=true);
  return ; 
} 

StatusCode LArRoI_Map::initialize()
{ 
  // FIXME
  // initTT_Hash(); 

  // intialize the internal map
  // currently, the best way is to get all the channels from
  // LArDetDescritor


  MsgStream log(msgSvc(), name());

  StoreGateSvc* detStore;
  if(StatusCode::SUCCESS != service("DetectorStore",detStore)){
    log << MSG::ERROR << " Can't get DetectorStore " << endreq;
    return StatusCode::FAILURE;
  }

  //  const DataHandle<CaloTTDescrManager> ttMan; 

  if(StatusCode::SUCCESS != detStore->retrieve(m_TT_ID) ) {
    log << MSG::ERROR << " Can't get CaloLVL1_ID " << endreq;
    return StatusCode::FAILURE;
  } 

  if(StatusCode::SUCCESS != detStore->retrieve(m_onlineID) ) {
    // temp FIXME 
    log << MSG::ERROR << " Can't get LArOnlineID " << endreq;
    return StatusCode::FAILURE;
  } 


  if(StatusCode::SUCCESS != detStore->retrieve(m_em_id) ) {
    log << MSG::ERROR << " Can't get LArEM_ID " << endreq;
    return StatusCode::FAILURE;
  } 

  if(StatusCode::SUCCESS != detStore->retrieve(m_hec_id) ) {
    log << MSG::ERROR << " Can't get LArHEC_ID " << endreq;
    return StatusCode::FAILURE;
  } 

  if(StatusCode::SUCCESS != detStore->retrieve(m_fcal_id) ) {
    log << MSG::ERROR << " Can't get LArFCAL_ID " << endreq;
    return StatusCode::FAILURE;
  } 

  IToolSvc* toolSvc;
  StatusCode sc   = service( "ToolSvc",toolSvc  );
  if(! sc.isSuccess()) { 
    log << MSG::ERROR << " Can't get ToolSvc" << endreq;
     return sc;
  }  

  sc =toolSvc->retrieveTool("LArCablingService",m_cablingSvc);
  if(!sc.isSuccess() ) {
    log << MSG::ERROR << " Can't get LArCablingService " << endreq;
    return sc;
  }

  sc =toolSvc->retrieveTool("CaloTriggerTowerService",m_ttSvc);
  if(!sc.isSuccess() ) {
    log << MSG::ERROR << " Can't get CaloTriggerTowerService " << endreq;
    return sc;
  }

  sc = detStore->regFcn(&LArCablingService::iovCallBack,&(*m_cablingSvc),
		                                   &LArRoI_Map::iovCallBack,
		                                   this,true) ; 
  if(!sc.isSuccess() ) {
    log << MSG::ERROR << " Can't regFcn to LArCablingService  " << endreq;
    return sc;
  }

  sc = detStore->regFcn(&CaloTriggerTowerService::iovCallBack,&(*m_ttSvc),
		                                   &LArRoI_Map::iovCallBack,
		                                   this,true) ; 

  if(!sc.isSuccess() ) {
    log << MSG::ERROR << " Can't regFcn to LArCablingService  " << endreq;
    return sc;
  }

 

  if (m_loadAtBegin) {
    log << MSG::DEBUG << "Setting callback function to load id map at begin of run" << endreq;
    // Incident Service: 
    IIncidentSvc* incSvc;
    sc = service("IncidentSvc", incSvc);
    if (sc.isFailure()) {
      log << MSG::ERROR << "Unable to retrieve pointer to IncidentSvc "
	  << endreq;
      return sc;
    }
    
    //start listening to "BeginRun". The incident should be fired AFTER the IOV callbacks and only once.
    const long priority=std::numeric_limits<long>::min(); //Very low priority
    incSvc->addListener(this, "BeginRun", priority ,false,true); //single-shot incident
  }
    
  log << MSG::INFO << " LArRoI_Map initialized " << endreq;
  return StatusCode::SUCCESS; 

}


void LArRoI_Map::handle(const Incident& /*inc*/)
{ 
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << " handle called " << endreq;

  initData().ignore(); 

  return ;
}

StatusCode LArRoI_Map::iovCallBack(IOVSVC_CALLBACK_ARGS) {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << " iovCallBack " << endreq;

  m_validCache = false; 

  // HACK: Force a call to the cabling service iov callback to make sure
  // it gets initialized early enough.
  int dum1 = 0;
  std::list<std::string> dum2 { "/LAR/Identifier/OnOffIdMap" };
  return m_cablingSvc->iovCallBack (dum1, dum2);
}


StatusCode LArRoI_Map::initData()
{ 

  // intialize the internal map

  MsgStream log(msgSvc(), name());

  if(m_validCache) {
    log<<MSG::DEBUG<<" in initData, but cache is valid"<<endreq;
    return StatusCode::SUCCESS;
  }

  log<<MSG::DEBUG<<" in initData, cache is not valid"<<endreq;

  m_offmap.clear(); 
  m_onlmap.clear(); 
  m_roimap.clear(); 

  const std::vector<Identifier>& emIds   = m_em_id->channel_ids();
  const std::vector<Identifier>& hecIds  = m_hec_id->channel_ids();
  const std::vector<Identifier>& fcalIds = m_fcal_id->channel_ids();

  int n_em   = 0 ; 
  int n_hec  = 0 ; 
  int n_fcal = 0 ; 

  // EM
  std::vector<Identifier>::const_iterator it = emIds.begin();
  std::vector<Identifier>::const_iterator it2 = emIds.end();
  for(; it!=it2;++it)
  {
    const Identifier& chan_id = *it ;
    addEntry(chan_id); 
    ++n_em; 
  } // end of EM 



  // HEC
  it = hecIds.begin();
  it2 = hecIds.end();
  for(; it!=it2;++it)
  {
    const Identifier& chan_id = *it ;
    addEntry(chan_id); 
    ++n_hec;
  } // end of HEC

  // FCAL
  it = fcalIds.begin();
  it2 = fcalIds.end();
  for(; it!=it2;++it)
  {
    const Identifier& chan_id = *it ;
    addEntry(chan_id); 
    ++n_fcal;
  } // end of FCAL

  m_validCache = true; 

  log << MSG::INFO << " LArRoI_Map data cache reset " << endreq;

  if(m_print){ 
    std::cout << " LArRoI_Map:  number of cells made in Map (EM, HEC, FCAL)" 
        << n_em <<" "<<n_hec<<" "<<n_fcal << std::endl; 
  } 

  if(m_print) print(); 


  return StatusCode::SUCCESS; 

}

LArRoI_Map::~LArRoI_Map( ) 
{


}


void LArRoI_Map::addEntry(const Identifier& id )  throw (LArID_Exception) 
{

// std::cout <<" addEntry id="<<m_TT_ID->show_to_string(id)<<std::endl; 

try{
 HWIdentifier sigId = m_cablingSvc->createSignalChannelID(id);
 Identifier     rId = m_ttSvc->whichTTID( id ) ;

 // convert the Full TT Identifier to hash FIXME 
 // TT_ID hashid; 
 IdentifierHash hashid; 
 IdContext IdCntx = m_TT_ID->layer_context(); 
 if( m_TT_ID->get_hash(rId,hashid,&IdCntx ) !=0 ) {
   std::cout<< " LArRoI_Map failed to get TT hash "<<std::endl; 
   std::string err(" LArRoI_Map:  failed to get TT hash "); 
   LArID_Exception except(err,1); 
   //throw except; 
 } 
 // TT id with sampling 

// LArTT_Sampling ttsam(hashid,sam); 
// TT_ID hashid_sam =ttsam.combinedId(); 

 TT_ID hashid_sam = (unsigned int) hashid ; 

 m_offmap[    id.get_identifier32().get_compact() ] = hashid_sam; 
 m_onlmap[ sigId.get_identifier32().get_compact() ] = hashid_sam; 

 // add entry for RoI to COLL_ID map. 
 // make an LArFEB_ID 
 // HWIdentifier febId = m_cablingSvc->createFEB_ID(sigId);
 HWIdentifier febId = m_onlineID->feb_Id(sigId);

 HWIdentifier robId =m_cablingSvc->getReadoutModuleID(febId); 

 COLL_ID_VEC& v = m_roimap[hashid_sam]; 

 if (v.size()==0){
   v.reserve(1);
   v.push_back(robId);  
 }
 else{   
   COLL_ID_VEC::const_iterator it = v.begin(); 
   COLL_ID_VEC::const_iterator it_end = v.end(); 
   bool found = false; 
   
   for(; it != it_end; ++it)
     if(robId == (*it) )   {
       found = true; 
       break;
     }
   if(!found) {
     v.push_back(robId);  
   }
 }
} catch (LArID_Exception& except)
{
  std::cout <<" Failed to get LArSignalChannelID in LArRoI_Map::addEntry "
  <<" id = "<< m_TT_ID->show_to_string(id) << std::endl;
  std::cout << (std::string)except <<std::endl; 
}

return ;

} 


// Given offline channel ID, find TT ID. 
LArRoI_Map::TT_ID LArRoI_Map::TrigTowerID(const Identifier& id) 
 const throw(LArID_Exception)
{ 
  
  if(!m_validCache){
    LArRoI_Map* roiMap=const_cast<LArRoI_Map*>(this);
    StatusCode sc = roiMap->initData();
    if(sc.isFailure()){ 
      std::string err(" LArRoI_Map:  cache not valid "); 
      LArID_Exception except(err,1); 
      throw except; 
    }
  }       
   
// calculate on the flight. too slow?? 
// LArSignalChannelID sigId = m_cablingSvc->createSignalChannelID(0,id);
// return TrigTowerID(sigId); 
  
  unsigned int idd = id.get_identifier32().get_compact(); 
  // std::cout <<" LArRoI_Map::TT, offline id = "<<(std::string)id<<" " <<std::hex<<idd<<std::endl;
  
  OFFMAP::const_iterator it = m_offmap.find(idd ); 
  if( it==m_offmap.end() ) 
  {
/*
     std::string err(" LArRoI_Map:  no entry for ID "); 
     err +=  id.getString();
     // std::cout <<err<<std::endl;
     LArID_Exception except(err,1); 
     throw except; 
*/

     MsgStream log(msgSvc(),name());
     log << MSG::DEBUG << " TrigTowerID failed, use hashmax + 1 " << endreq;

     return m_TT_ID->layer_hash_max()+1 ; 

  }

  return (*it).second; 

}


// Given online Channel ID, find TT ID. 
LArRoI_Map::TT_ID LArRoI_Map::TrigTowerID(const HWIdentifier& sigId)
const throw(LArID_Exception)
{

  if(!m_validCache){
    LArRoI_Map* roiMap=const_cast<LArRoI_Map*>(this);
    StatusCode sc = roiMap->initData();
    if(sc.isFailure()){ 
      std::string err(" LArRoI_Map:  cache not valid "); 
      LArID_Exception except(err,1); 
      throw except; 
    }
  }       


// calculate on the flight. too slow?? 
// LArTTChannelID ttid =  m_cablingSvc->whichTTChannelID( sigId) ;
// return m_cablingSvc->cnvToIdentifier(ttid); 

  ONLMAP::const_iterator it = m_onlmap.find( sigId.get_identifier32().get_compact() ); 
  if(it==m_onlmap.end() ) 
  {
/* 
     std::string err(" LArRoI_Map::TrigTowerID(const HWIdentifier&):  no entry for ID "); 
     // err +=  id.getString() 
     LArID_Exception except(err,1); 
     throw except; 
*/
     MsgStream log(msgSvc(),name() );
     log << MSG::DEBUG << " TrigTowerID failed, use hashmax + 1 " << endreq;
     return m_TT_ID->layer_hash_max()+1 ; 

  }

  return (*it).second; 

}


const LArRoI_Map::COLL_ID_VEC& LArRoI_Map::CollectionID
               (const LArRoI_Map::TT_ID & RoIid ) const 
{

  if(!m_validCache){
    LArRoI_Map* roiMap=const_cast<LArRoI_Map*>(this);
    StatusCode sc = roiMap->initData();
    if(sc.isFailure()){ 
      std::string err(" LArRoI_Map:  cache not valid "); 
      LArID_Exception except(err,1); 
      throw except; 
    }
  }       

// Given an hash index, find the all the ROB collections.
   ROIMAP::const_iterator it = m_roimap.find(RoIid); 
   if(it !=m_roimap.end()) {
	return (*it).second; 
   }

/*
   // It is expected to have request that may not be in the map. 
   // Not all TTs have all samplings.  For example, at eta>1.8, there is no 
   // presampler. 

   LArTT_Sampling ttSam(RoIid); 
   std::cout <<" LArRoI_Map::CollectionID: can not find RoI" << std::endl;
   std::cout <<" TT id = "<<(unsigned int)ttSam.TT_id()<<" sam=" 
	     << ttSam.sampling()<<endl;
*/    
   MsgStream log(msgSvc(),name());
   log << MSG::DEBUG << " CollectionID failed, empty vector " << endreq;
   static COLL_ID_VEC v ; 
   return v; 
}


bool  LArRoI_Map::validId
               (const LArRoI_Map::TT_ID & RoIid ) const 
{

  if(!m_validCache){
    LArRoI_Map* roiMap=const_cast<LArRoI_Map*>(this);
    StatusCode sc = roiMap->initData();
    if(sc.isFailure()){ 
      std::string err(" LArRoI_Map:  cache not valid "); 
      LArID_Exception except(err,1); 
      throw except; 
    }
  }       

   ROIMAP::const_iterator it = m_roimap.find(RoIid); 
   return it !=m_roimap.end() ; 
}


void LArRoI_Map::print()
{

  MsgStream log(msgSvc(), name());

  StoreGateSvc* detStore;
  if(StatusCode::SUCCESS != service("DetectorStore",detStore)){
    log<<MSG::ERROR <<  " Can't get ToolSvc " << endreq;
    return  ; 
  }

  const CaloTTDescrManager* ttMan; 

  if(StatusCode::SUCCESS != detStore->retrieve(ttMan) ) {
    log<<MSG::ERROR <<  " Can't CaloTTDescrManager " << endreq;
    return;
  } 


  int ncount=0; 
  // print TT map to files, EM and HAD separately. 
  std::vector<std::ofstream*> ttmapEM; 
  ttmapEM.push_back(new std::ofstream("TrigTowerMapEM_0.txt")) ;
  ttmapEM.push_back(new std::ofstream("TrigTowerMapEM_1.txt")) ;
  ttmapEM.push_back(new std::ofstream("TrigTowerMapEM_2.txt")) ;
  ttmapEM.push_back(new std::ofstream("TrigTowerMapEM_3.txt")) ;

  std::vector<std::ofstream*> ttmapHEC; 
  ttmapHEC.push_back(new std::ofstream("TrigTowerMapHEC_0.txt")) ;
  ttmapHEC.push_back(new std::ofstream("TrigTowerMapHEC_1.txt")) ;
  ttmapHEC.push_back(new std::ofstream("TrigTowerMapHEC_2.txt")) ;
  ttmapHEC.push_back(new std::ofstream("TrigTowerMapHEC_3.txt")) ;

  std::vector<std::ofstream*> ttmapFCAL; 
  ttmapFCAL.push_back(new std::ofstream("TrigTowerMapFCAL_0.txt")) ;
  ttmapFCAL.push_back(new std::ofstream("TrigTowerMapFCAL_1.txt")) ;

  // now print the eta phi region for each TT 
//  std::cout <<" TT-ID , HASH,  SAMP,  ETAMIN,  ETAMAX, PHIMIN, PHIMAX  "<<std::endl ;
  *ttmapEM[0] <<" TT-ID , HASH, SAMP, ETAMIN, ETAMAX, PHIMIN, PHIMAX, ROBIDs "<<std::endl ;
  *ttmapEM[1] <<" TT-ID , HASH, SAMP, ETAMIN, ETAMAX, PHIMIN, PHIMAX, ROBIDs "<<std::endl ;
  *ttmapEM[2] <<" TT-ID , HASH, SAMP, ETAMIN, ETAMAX, PHIMIN, PHIMAX, ROBIDs "<<std::endl ;
  *ttmapEM[3] <<" TT-ID , HASH, SAMP, ETAMIN, ETAMAX, PHIMIN, PHIMAX, ROBIDs "<<std::endl ;
  *ttmapHEC[0] <<" TT-ID , HASH, SAMP, ETAMIN, ETAMAX, PHIMIN, PHIMAX, ROBIDs "<<std::endl ;
  *ttmapHEC[1] <<" TT-ID , HASH, SAMP, ETAMIN, ETAMAX, PHIMIN, PHIMAX, ROBIDs "<<std::endl ;
  *ttmapHEC[2] <<" TT-ID , HASH, SAMP, ETAMIN, ETAMAX, PHIMIN, PHIMAX, ROBIDs "<<std::endl ;
  *ttmapHEC[3] <<" TT-ID , HASH, SAMP, ETAMIN, ETAMAX, PHIMIN, PHIMAX, ROBIDs "<<std::endl ;
  *ttmapFCAL[0] <<" TT-ID , HASH, SAMP, ETAMIN, ETAMAX, PHIMIN, PHIMAX, ROBIDs "<<std::endl ;
  *ttmapFCAL[1] <<" TT-ID , HASH, SAMP, ETAMIN, ETAMAX, PHIMIN, PHIMAX, ROBIDs "<<std::endl ;

//  CaloTTDescrManager* ttman = CaloTTDescrManager::instance(); 
//  if(!ttman->is_initialized()) ttman->initialize(); 

  CaloTTDescrManager::calo_descr_const_iterator it2 =
	ttMan->calo_descriptors_begin();	
  CaloTTDescrManager::calo_descr_const_iterator it2_end =
	ttMan->calo_descriptors_end();	
  // cout <<" Number of descriptors "<<ttman->calo_descriptors_size()<<std::endl;

  std::ofstream* ttmap;

  for(; it2!=it2_end;++it2){
	const CaloTTDescriptor* descr = (*it2); 
	// print them out.
	Identifier reg_id = descr->identify(); 
	int sam = m_TT_ID->sampling(reg_id); 	

  	float e1,e2,p1,p2 ;
	float deta = descr->deta() ; 
	float dphi = descr->dphi() ; 
        float sg = descr->sign_eta(); 

	e1 = descr->eta_min() ; 
	e2 = e1+deta; 

        //cout <<" region = " <<  reg_id.getString() 
	//     <<" neta = "   << descr->nEta() 
	//     <<" nphi = "   << descr->nPhi() 
	//     <<std::endl; 
	for(int eta=0;eta<descr->nEta();++eta)
	{ 
	  p1 = descr->phiMin() ; 
	  p2 = p1+dphi; 
	  for(int phi=0;phi<descr->nPhi();++phi)
	  { 
           int nlay = 4 ; 
           if( fabs(e1)>=3.2) { 
              nlay = 2; 
           }
	   for(int layer=0;layer<nlay;++layer)
	   { 

            if(sam==0) {
  	       ttmap = ttmapEM[layer]; 
            }else 
            if( fabs(e1)>=3.2) { 
             ttmap = ttmapFCAL[layer]; 
            }
            else {
	    ttmap = ttmapHEC[layer]; 
            } 


	    Identifier idTT = m_TT_ID->tower_id(reg_id, eta,phi); 
	    Identifier id = m_TT_ID->layer_id(idTT, layer); 

	    IdentifierHash hashid;	

            IdContext IdCntx = m_TT_ID->layer_context(); 
	    m_TT_ID->get_hash(id, hashid, &IdCntx ) ; 

	    const COLL_ID_VEC& vColl= CollectionID( (unsigned int) hashid); 
            if(vColl.size() == 0 ) 
             {

		std::cerr<<" WARNING: No Mapping to ROBID for this TT "<<std::endl;
		std::cerr<<" sg,sam,eta,phi,layer,etamin,phimin  =  " 
	       << sg<<" " 
	       << sam<<" " 
               << eta<<" " 
               << phi<<" " 
               << layer<<" " 
               << e1 << " "
               << p1 << " "
               <<std::endl;

             } else 	    
             {
	       *ttmap <<  id.getString() << " " << hashid 
	       << "  " << sam <<  "  "
	       << "  " << layer <<  "  "
	       <<e1*sg << "  " 
	       <<e2*sg << "  "
	       <<p1    << "  "
	       <<p2   <<  "  " 
	       <<std::hex<<vColl[0].get_compact() <<std::dec<< std::endl; 
	       ++ncount; 

	     }		

	    }
	    p1 = p2 ; 
	    p2 = p1+dphi; 
           }
	  e1 = e2 ; 
	  e2 = e1+deta; 
	} 
  }

  log<< MSG::DEBUG <<" Total number of TTs = "<<ncount<< endreq;

  delete ttmapEM[0];
  delete ttmapEM[1];
  delete ttmapEM[2];
  delete ttmapEM[3];
  
  delete ttmapHEC[0];
  delete ttmapHEC[1];
  delete ttmapHEC[2];
  delete ttmapHEC[3];


  delete ttmapFCAL[0];
  delete ttmapFCAL[1];


  // print sizes
  ROIMAP::const_iterator it  = m_roimap.begin();
  ROIMAP::const_iterator it_e  = m_roimap.end();
  
  log<<MSG::INFO<< " size of roimap "<<m_roimap.size()<< endreq;
    
  for (;it!=it_e;++it){
    log<<MSG::INFO<< " tt id "<< (*it).first << " size = "<<(*it).second.size()<<endreq;
  }

  return ;

}

