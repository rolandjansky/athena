/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ****************************************************************************************
//
// To perform cell per cell pedestal shift correction
//  taking the pedestal shift in MeV from the database /CALO/Pedestal/CellPedestal folder
//
// G.Unal    9 may 2008 ....  first version
//
// ****************************************************************************************

#include "CaloCellCorrection/CaloCellPedestalCorr.h"

#include "CaloEvent/CaloCell.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

//=== AttributeList
#include "CoralBase/Blob.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
//=== CaloCondBlod
#include "CaloCondBlobObjs/CaloCondBlobFlt.h"
#include "CaloCondBlobObjs/CaloCondUtils.h"

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"


#include "CaloIdentifier/CaloCell_ID.h"

// ======================================================
// Constructor

CaloCellPedestalCorr::CaloCellPedestalCorr(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent)
  : CaloCellCorrection(type, name, parent),
   m_caloCoolIdTool("CaloCoolIdTool"),m_caloLumiBCIDTool(""),m_isMC(false)
{
 declareInterface<CaloCellCorrection>(this);
 declareProperty("Luminosity",m_lumi0=0,"Luminosity in 10**33 units");
 declareProperty("CaloCoolIdTool",m_caloCoolIdTool,"Tool for Calo cool Id");
 declareProperty("FolderName",m_folderName="/CALO/Pedestal/CellPedestal");
 declareProperty("LumiFolderName",m_lumiFolderName="/TRIGGER/LUMI/LBLESTONL");
 declareProperty("LumiBCIDTool",m_caloLumiBCIDTool,"Tool for BCID pileup offset average correction");
 declareProperty("isMC",m_isMC,"Data/MC flag");
 m_lastIt=m_noiseBlobMap.begin();
}

//========================================================
// Initialize

StatusCode CaloCellPedestalCorr::initialize()
{
 MsgStream  log(msgSvc(),name());
 log << MSG::INFO << " in CaloCellPedestalCorr::initialize() " << endreq;

 StoreGateSvc* detStore;
 StatusCode sc = service ( "DetectorStore" , detStore ) ;
 if (sc.isFailure()) {
    log << MSG::ERROR << " Unable to get DetectorStore " << endreq; 
    return StatusCode::FAILURE;
 }

 sc=detStore->retrieve(m_cellId, "CaloCell_ID");
 if (sc.isFailure()) {
   log << MSG::ERROR << " Unable to get CaloCell_ID" << endreq; 
   return StatusCode::FAILURE;
 }
  

  if (m_lumi0<0) {
    if (detStore->contains<CondAttrListCollection>(m_lumiFolderName)) {
      const DataHandle<CondAttrListCollection> lumiData;
      sc = detStore->regFcn(&CaloCellPedestalCorr::updateLumi, this , lumiData, m_lumiFolderName, true);
      if (sc.isFailure()) {
          log << MSG::ERROR << " cannot register callback for luminosity " << endreq;
          return StatusCode::FAILURE;
      }
      log << MSG::INFO << " Registered a callback for " << m_lumiFolderName << " Cool folder " << endreq;
    }
    m_lumi0=0;
  }


  if (!m_isMC) {
    //=== Register callback for this data handle
    sc=detStore->regFcn(&CaloCellPedestalCorr::updateMap, this, m_noiseAttrListColl, m_folderName);
    if (sc.isFailure()) {
      log << MSG::ERROR << " cannot register callback " << endreq;
      return StatusCode::FAILURE;
    }
    log << MSG::INFO << " registered a callback for " << m_folderName << " folder " << endreq;

    sc = m_caloCoolIdTool.retrieve();
    if (sc.isFailure()) {
       log << MSG::ERROR << " cannot retrieve CaloCoolIdTool" << endreq;
       return StatusCode::FAILURE;
    }
  }

  if (!m_caloLumiBCIDTool.empty() ) {
    sc = m_caloLumiBCIDTool.retrieve();
    if (sc.isFailure()) {
       log <<MSG::ERROR << " cannot retrieve LumiBCIDTool" << endreq;
       return StatusCode::FAILURE;
    }
  }
  
  log << MSG::INFO << "CaloCellPedestalCorr initialize() end" << endreq;

  return StatusCode::SUCCESS;

}

//______________________________________________________________________________________
StatusCode
CaloCellPedestalCorr::updateLumi( IOVSVC_CALLBACK_ARGS )
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << " in updateLumi() " << endreq;

  StoreGateSvc* detStore;
  StatusCode sc = service ( "DetectorStore" , detStore ) ;
  if (sc.isFailure()) {
    log << MSG::ERROR << " Unable to get DetectorStore " << endreq; 
    return StatusCode::FAILURE;
  }

  const CondAttrListCollection* attrListColl = 0;
  sc = detStore->retrieve(attrListColl, m_lumiFolderName);
  if (sc.isFailure() || !attrListColl) {
     log << MSG::WARNING  << "attrrListColl not found for " << m_lumiFolderName << endreq;
     return StatusCode::SUCCESS;
  }
  // Loop over collection
  CondAttrListCollection::const_iterator first = attrListColl->begin();
  CondAttrListCollection::const_iterator last  = attrListColl->end();
  for (; first != last; ++first) {
   if ((*first).first == 0) {
      std::ostringstream attrStr1;
      (*first).second.toOutputStream( attrStr1 );
      log << MSG::DEBUG << "ChanNum " << (*first).first <<
          " Attribute list " << attrStr1.str() << endreq;
      const coral::AttributeList& attrList = (*first).second;
      if (attrList["LBAvInstLumi"].isNull()) {
          log << MSG::WARNING << " NULL Luminosity information in database ... set it to 0 " << endreq;
          m_lumi0 = 0.;
      } else {
        m_lumi0 = attrList["LBAvInstLumi"].data<float>() *1e-3;  // luminosity (from 10**30 units in db to 10*33 units)
      }
      break;
   }
  }
  if ( !(m_lumi0 == m_lumi0) ) {
     log << MSG::WARNING << " Luminosity not a number ?  m_lumi0 " << m_lumi0 << "   ... set it to 0 " << endreq;
     m_lumi0 = 0.;
  }
  log << MSG::INFO << " Luminosity " << m_lumi0 << endreq;
  return StatusCode::SUCCESS;
}



// ===============================================================================

StatusCode CaloCellPedestalCorr::updateMap(IOVSVC_CALLBACK_ARGS_K(keys) )
{

  MsgStream  log(msgSvc(),name());

  log << MSG::INFO << " in updateMap ";
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
    unsigned int sysId = static_cast<unsigned int>(iColl->first);
    
    //=== delete old CaloCondBlobFlt (which does not own the blob)
    std::map<unsigned int, const CaloCondBlobFlt*>::iterator iOld = m_noiseBlobMap.find(sysId);
    if(iOld != m_noiseBlobMap.end()){
      delete iOld->second;
    }
    
    //=== Get new CaloCondBlobFlt instance, interpreting current BLOB
    const coral::Blob& blob = (iColl->second)["CaloCondBlob16M"].data<coral::Blob>();
    const CaloCondBlobFlt* flt = CaloCondBlobFlt::getInstance(blob);

    //=== store new pointer in map
    m_noiseBlobMap[sysId] = flt;
    
  }//end iColl
  m_lastIt=m_noiseBlobMap.begin();
  return StatusCode::SUCCESS;
}

// ============================================================================

void CaloCellPedestalCorr::MakeCorrection(CaloCell* theCell)
{
  const unsigned int cellHash=m_cellId->calo_cell_hash(theCell->ID());

  float pedestal=0.;

  if (!m_isMC) {
    unsigned int subHash;
    const unsigned int iCool = m_caloCoolIdTool->getCoolChannelId(cellHash,subHash);
    //std::cout << "Got iCool=" << iCool << " subhash=" << subHash << std::endl;
    if (m_lastIt->first!=iCool) {
      m_lastIt=m_noiseBlobMap.find(iCool);
    }
    //The following checks would make sense but were obmitted to speed up execution:
    //1. m_lastIt!=m_noiseBlobMap.end() eg, if iCool exists
    //2. subHash < flt->getNChans()
    const CaloCondBlobFlt* const flt = m_lastIt->second;
    const unsigned int dbGain = CaloCondUtils::getDbCaloGain(theCell->gain());
    pedestal = flt->getCalib(subHash, dbGain, m_lumi0);
  }

  if (!m_caloLumiBCIDTool.empty() ) {
       pedestal = pedestal + m_caloLumiBCIDTool->average(theCell,0);
  }

  theCell->addEnergy(-pedestal);
}
