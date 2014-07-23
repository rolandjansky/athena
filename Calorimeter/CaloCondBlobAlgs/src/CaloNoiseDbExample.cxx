/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloCondBlobAlgs/CaloNoiseDbExample.h"
//#include "CaloInterface/ICaloNoiseTool.h"

//=== Athena includes
#include "StoreGate/StoreGate.h"

//=== Gaudi includes
#include "GaudiKernel/MsgStream.h"

//=== Event Info 
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

//=== AttributeList
#include "CoralBase/Blob.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

//=== TileCalibBlobObjs
#include "CaloCondBlobObjs/CaloCondBlobFlt.h"
#include "CaloCondBlobObjs/CaloCondUtils.h"

//
//_______________________________________________________________________________________
CaloNoiseDbExample::CaloNoiseDbExample(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),
  m_sgSvc(0),
  m_evt(0),
  m_calo_id(0)
{
  declareProperty("FolderName"   , m_foldername="/CALO/Ofl/Noise/CellNoise");
}

//
//_______________________________________________________________________________________
CaloNoiseDbExample::~CaloNoiseDbExample()
{ 
}

//
//_______________________________________________________________________________________
StatusCode 
CaloNoiseDbExample::initialize()
{
  MsgStream log(msgSvc(), name());
  log <<MSG::DEBUG <<"in initialize()" <<endreq;
  
  //=== Storegate
  StatusCode sc = service("StoreGateSvc", m_sgSvc);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Unable to get the StoreGateSvc" << endreq;
    return sc;
  }
  
  //=== DetectorStore
  StoreGateSvc* detStore(0);
  sc = service("DetectorStore", detStore);
  if (!sc.isSuccess() || 0 == detStore)  {
    log <<MSG::ERROR <<"Could not find DetStore" <<endreq;
    return StatusCode::FAILURE;
  }

  //retrieve ID helpers 
   
  sc = detStore->retrieve( m_caloIdMgr );
  if (sc.isFailure()) {
    log << MSG::ERROR << "Unable to retrieve CaloIdMgr " << endreq;
    return StatusCode::FAILURE;
  }
  m_calo_id      = m_caloIdMgr->getCaloCell_ID();
  
  
  //=== Bind noise DataHandle to COOL folder
  log << MSG::INFO << "Folder name " << m_foldername << endreq;
 
  //   std::string folderName("/CALO/Ofl/Noise/CellNoise");
  std::string folderName(m_foldername);
  /* OBSOLETE
     if(detStore->regHandle(m_noiseAttrListColl, folderName).isFailure()){
     log << MSG::ERROR << "Could not register handle to " << folderName << endreq;
     return StatusCode::FAILURE;
     }
  */
  //=== Register callback for this data handle

  
  if(detStore->regFcn(&CaloNoiseDbExample::updateMap,this,m_noiseAttrListColl,folderName).isFailure()){
    log << MSG::ERROR << "Could not register updateMap() callback" << endreq;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}


//
//_______________________________________________________________________________________
StatusCode 
CaloNoiseDbExample::execute() 
{
  
  MsgStream log(msgSvc(), name());
  log <<MSG::DEBUG <<" in execute()" <<endreq;
  
  //=== print run/evt/lbn/time info for each event
  StatusCode sc = m_sgSvc->retrieve(m_evt);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "could not get event info " 
	<< endreq;
    return( StatusCode::FAILURE);
  }
  else {
    log << MSG::DEBUG << "Event: [" << m_evt->event_ID()->run_number()
	<< "," << m_evt->event_ID()->event_number();
    log << "," << m_evt->event_ID()->lumi_block();
    log    << ":" << m_evt->event_ID()->time_stamp()
	   << "]" << endreq;
  }
  

  //=======================================
  //=== Print out some noise values
  //=======================================
  log << MSG::DEBUG << "Calling printSomeInfo" << endreq;
  sc = printSomeInfo(); 
  if (sc.isFailure()) {
    log <<MSG::ERROR <<"Could not print out some info" <<endreq;
    return( StatusCode::FAILURE);
  }
  
  return StatusCode::SUCCESS;
}


//
//_______________________________________________________________________________________
StatusCode 
CaloNoiseDbExample::finalize()
{
  MsgStream log(msgSvc(), name());
  log <<MSG::INFO <<"in finalize()" <<endreq;
  return StatusCode::SUCCESS;
}



//
//_______________________________________________________________________________________
StatusCode 
CaloNoiseDbExample::printSomeInfo()
{
  MsgStream log(msgSvc(), name());
  log <<MSG::INFO <<"In printSomeInfo()" <<endreq;

  int ncell=m_calo_id->calo_cell_hash_max();
  log << MSG::INFO << " start loop over Calo cells " << ncell << endreq;  
  
  //=== print some value
  CaloCondUtils::SYSTEM sysId = CaloCondUtils::TILE;

  const CaloCondBlobFlt* const flt = m_noiseBlobMap.find(sysId)->second;
  log << MSG::INFO  << "ObjSize="<< flt->getObjSizeUint32() << endreq;

  //unsigned int cellHash       = 0;
  //     TILELOWHIGH =-15 ,
  //     TILEHIGHLOW  = -12,
  //     TILEHIGHHIGH = -11,
  //     TILEONELOW   =-4,
  //     TILEONEHIGH  =-3,
  
  for (int i=0;i<ncell;i++) {
    IdentifierHash idHash=i;
    int subCalo;
    IdentifierHash idSubHash = m_calo_id->subcalo_cell_hash (idHash, subCalo);
    Identifier id=m_calo_id->cell_id(idHash);
    if (m_calo_id->is_tile(id)) {

      //    const CaloDetDescrElement* calodde = m_calodetdescrmgr->get_element(id);
    
      int ngain = 4;
      for (int igain=0;igain<ngain;igain++) {

	CaloGain::CaloGain gain = CaloGain::TILELOWHIGH;
	if (igain==0) gain=CaloGain::TILELOWLOW;
	if (igain==1) gain=CaloGain::TILELOWHIGH;
	if (igain==2) gain=CaloGain::TILEHIGHLOW;
	if (igain==3) gain=CaloGain::TILEHIGHHIGH;

	double lumi = 100.;
	float val = getNoise(sysId, idSubHash, gain, lumi);
  
	float vala = getA(sysId, idSubHash, gain);
	float valb = getB(sysId, idSubHash, gain);
	float valc = getC(sysId, idSubHash, gain);
	float vald = getD(sysId, idSubHash, gain);
	float vale = getE(sysId, idSubHash, gain);
	log << MSG::INFO << "Value a = " << vala  << " b = " << valb << " c = " << valc << " d = " << vald << " e = " << vale <<" totalnoise: " << val << endreq;
	log << MSG::INFO << "Noise for sysId " << sysId << " IdHash " << idHash << " idSubHash " << idSubHash << " gain " << gain << " lumi=" <<lumi << endreq;
      }
    }
  }
  return StatusCode::SUCCESS;
}

//
//_______________________________________________________________________________________
float 
CaloNoiseDbExample::getA(CaloCondUtils::SYSTEM sysId, unsigned int cellHash, 
			 CaloGain::CaloGain caloGain) const
{
  MsgStream log(msgSvc(), name());
  const CaloCondBlobFlt* const flt = m_noiseBlobMap.find(sysId)->second;
  unsigned int dbGain = CaloCondUtils::getDbCaloGain(caloGain);
  return flt->getData(cellHash,dbGain,0);
}

//
//_______________________________________________________________________________________
float 
CaloNoiseDbExample::getB(CaloCondUtils::SYSTEM sysId, unsigned int cellHash, 
			 CaloGain::CaloGain caloGain) const
{
  const CaloCondBlobFlt* const flt = m_noiseBlobMap.find(sysId)->second;
  unsigned int dbGain = CaloCondUtils::getDbCaloGain(caloGain);
  return flt->getData(cellHash,dbGain,1);
}


//
//_______________________________________________________________________________________
float 
CaloNoiseDbExample::getC(CaloCondUtils::SYSTEM sysId, unsigned int cellHash, 
			 CaloGain::CaloGain caloGain) const
{
  const CaloCondBlobFlt* const flt = m_noiseBlobMap.find(sysId)->second;
  unsigned int dbGain = CaloCondUtils::getDbCaloGain(caloGain);
  return flt->getData(cellHash,dbGain,2);
}


//
//_______________________________________________________________________________________
float 
CaloNoiseDbExample::getD(CaloCondUtils::SYSTEM sysId, unsigned int cellHash, 
			 CaloGain::CaloGain caloGain) const
{
  const CaloCondBlobFlt* const flt = m_noiseBlobMap.find(sysId)->second;
  unsigned int dbGain = CaloCondUtils::getDbCaloGain(caloGain);
  return flt->getData(cellHash,dbGain,3);
}


//
//_______________________________________________________________________________________
float 
CaloNoiseDbExample::getE(CaloCondUtils::SYSTEM sysId, unsigned int cellHash, 
			 CaloGain::CaloGain caloGain) const
{
  const CaloCondBlobFlt* const flt = m_noiseBlobMap.find(sysId)->second;
  unsigned int dbGain = CaloCondUtils::getDbCaloGain(caloGain);
  return flt->getData(cellHash,dbGain,4);
}

//
//_______________________________________________________________________________________
float 
CaloNoiseDbExample::getNoise(CaloCondUtils::SYSTEM sysId, 
			     unsigned int cellHash, 
			     CaloGain::CaloGain caloGain, 
			     float lumi) const
{
  const CaloCondBlobFlt* const flt = m_noiseBlobMap.find(sysId)->second;
  unsigned int dbGain = CaloCondUtils::getDbCaloGain(caloGain);
  return flt->getCalib(cellHash, dbGain, lumi);
}

//
//_______________________________________________________________________________________
StatusCode 
CaloNoiseDbExample::updateMap( IOVSVC_CALLBACK_ARGS_K(keys) )
//
// This function is called when /CALO/V01/NOISE goes out of IOV.
// It ensures that the pointers to the CaloCondBlobFlt objects stored in 
// the different COOL channels are also up to date. 
//
{
  MsgStream log(msgSvc(), name());
  log <<MSG::INFO <<"In updateMap(), has been triggered by: " <<endreq;
  
  std::list<std::string>::const_iterator itr;
  for (itr=keys.begin(); itr!=keys.end(); ++itr) {
    log << *itr << " ";
  }
  log << endreq;

  //=== loop over collection (all cool channels)
  CondAttrListCollection::const_iterator iColl = m_noiseAttrListColl->begin();
  CondAttrListCollection::const_iterator last  = m_noiseAttrListColl->end();
  for (; iColl != last; ++iColl) {
    
    //=== COOL channel number is system id
    CaloCondUtils::SYSTEM sysId = static_cast<CaloCondUtils::SYSTEM>(iColl->first);
    
    //=== delete old CaloCondBlobFlt (which does not own the blob)
    std::map<CaloCondUtils::SYSTEM, const CaloCondBlobFlt*>::iterator iOld = m_noiseBlobMap.find(sysId);
    if(iOld != m_noiseBlobMap.end()){
      delete iOld->second;
    }
    
    //=== Get new CaloCondBlobFlt instance, interpreting current BLOB
    const coral::Blob& blob = (iColl->second)["CaloCondBlob16M"].data<coral::Blob>();
    const CaloCondBlobFlt* flt = CaloCondBlobFlt::getInstance(blob);

    //=== store new pointer in map
    m_noiseBlobMap[sysId] = flt;
    
  }//end iColl
  
  return StatusCode::SUCCESS;
}
