/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTools/CaloNoiseToolDB.h"


//=== AttributeList
#include "CoralBase/Blob.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
//=== TileCalibBlobObjs
#include "CaloCondBlobObjs/CaloCondBlobFlt.h"
#include "CaloCondBlobObjs/CaloCondUtils.h"

#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandGauss.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"

#include <TMath.h>
#include <boost/math/special_functions/erf.hpp>


using CLHEP::RandGauss;

namespace {

// Minbias events per unit lumi
const double nMinbiasPerLumi = 2.3;

const double lumiPerNMinbias = 1. / nMinbiasPerLumi;

}

//////////////////////////////////////////////////

CaloNoiseToolDB::CaloNoiseToolDB(const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent) 
  : AthAlgTool(type, name, parent),
    m_cached(),
    m_CachedGetNoiseCDDE(nullptr),
    m_CachedGetNoiseCELL(nullptr),
    m_ncell(0),
    m_highestGain(),
    m_Nminbias(0),
    m_cacheValid(false),
    m_calo_id(nullptr),
    m_cacheUpdateCounter(0)
{
  declareInterface<ICaloNoiseTool>(this);
  declareInterface<ICalorimeterNoiseTool>(this);

  declareProperty("Luminosity",m_lumi0=0,"Luminosity in 10**33 units");
  declareProperty("CachedNoise",m_ReturnNoiseName="totalNoise","Cached Noise");
  declareProperty("FixGain",m_gain_from_joboption=-1);
  declareProperty("SpeedTwoGauss",m_speedTwoGauss=true);
  declareProperty("FolderNames",m_folderNames);
  declareProperty("LumiFolderName",m_lumiFolderName="/TRIGGER/LUMI/LBLESTONL");
  declareProperty("LArHVCellCorrTool",m_larHVCellCorrTool);
  declareProperty("RescaleForHV",m_rescaleForHV=false);
}
 
//////////////////////////////////////////////////

StatusCode 
CaloNoiseToolDB::initialize()
{

  m_cached = ICalorimeterNoiseTool::TOTALNOISE;

  if (m_folderNames.size()==0) {
    msg(MSG::ERROR) << "No database folder name give to read noise! Please set " << name() << ".FolderNames=[...]" << endreq;
    return StatusCode::FAILURE;
  }


  const IGeoModelSvc *geoModel=0;
  StatusCode sc = service("GeoModelSvc", geoModel);
  if(sc.isFailure())
  {
    msg(MSG::ERROR) << "Could not locate GeoModelSvc" << endreq;
    return sc;
  }

  // dummy parameters for the callback:
  int dummyInt=0;
  std::list<std::string> dummyList;

  if (geoModel->geoInitialized())
  {
    return geoInit(dummyInt,dummyList);
  }
  else
  {
    sc = detStore()->regFcn(&IGeoModelSvc::geoInit,
			  geoModel,
			  &CaloNoiseToolDB::geoInit,this);
    if(sc.isFailure())
    {
      msg(MSG::ERROR) << "Could not register geoInit callback" << endreq;
      return sc;
    }
  }

  

  return sc;
}


StatusCode 
CaloNoiseToolDB::finalize()
{
  msg(MSG::INFO) << "CaloNoiseToolDB final(), cleaning m_noiseBlobMap, size = " <<m_noiseBlobMap.size() <<endreq;
  msg(MSG::INFO) << "Cache was recomputed " <<  m_cacheUpdateCounter << " times" << endreq;

  //=== delete old CaloCondBlobFlt (which does not own the blob)
  std::map<SYSTEM, const CaloCondBlobFlt*>::iterator it = m_noiseBlobMap.begin();
  std::map<SYSTEM, const CaloCondBlobFlt*>::iterator it_e = m_noiseBlobMap.end();
  for (; it!=it_e; ++it ){
    delete it->second;
  }

  return StatusCode::SUCCESS; 

}
StatusCode
CaloNoiseToolDB::geoInit(IOVSVC_CALLBACK_ARGS)
{
  StatusCode sc;

  msg(MSG::INFO)
      << "CaloNoiseToolDB initialize() begin" 
      << endreq;

  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::  
  //retrieves helpers for Identifier

  //retrieve ID helpers 
  sc = detStore()->retrieve( m_caloIdMgr );
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to retrieve CaloIdMgr in LArHitEMap " << endreq;
   return StatusCode::FAILURE;
  }
  m_calo_id      = m_caloIdMgr->getCaloCell_ID();

  m_Nminbias = m_lumi0*nMinbiasPerLumi;

  m_highestGain[CaloCell_ID::LAREM]   = CaloGain::LARHIGHGAIN;
  m_highestGain[CaloCell_ID::LARHEC]  = CaloGain::LARMEDIUMGAIN;
  m_highestGain[CaloCell_ID::LARFCAL] = CaloGain::LARHIGHGAIN;
  m_highestGain[CaloCell_ID::TILE]    = CaloGain::TILEHIGHHIGH;

  // register callback for luminosity (only if not hardcoded from jobOptions)

  if (m_lumi0<0) {
    m_lumi0=0;
    if (detStore()->contains<CondAttrListCollection>(m_lumiFolderName)) {
      const DataHandle<CondAttrListCollection> lumiData;
      sc = detStore()->regFcn(&CaloNoiseToolDB::updateLumi, this , lumiData, m_lumiFolderName, true);
      if (sc.isFailure()) {
          msg(MSG::ERROR) << " cannot register callback for luminosity " << endreq;
          return StatusCode::FAILURE;
      }
      msg(MSG::INFO) << " Registered a callback for " << m_lumiFolderName << " Cool folder " << endreq;
    }
  }


  m_noiseAttrListColl.resize(m_folderNames.size());
  std::vector<std::string>::const_iterator fldr_it=m_folderNames.begin();
  std::vector<std::string>::const_iterator fldr_it_e=m_folderNames.end();
  unsigned i=0;
  for(;fldr_it!=fldr_it_e;++fldr_it) {
    const std::string& folderName=*fldr_it;
    
    msg(MSG::INFO) << "Registering callback on folder " << folderName << endreq;
    sc=detStore()->regFcn(&CaloNoiseToolDB::updateMap, this, const_cast<const DataHandle<CondAttrListCollection>&>(m_noiseAttrListColl[i++]), folderName, true);
     if (sc.isFailure()) {
       msg(MSG::ERROR) << " cannot register callback to folder " << folderName <<  endreq;
       return StatusCode::FAILURE;
     }
  }

  if (m_rescaleForHV) {
    sc=m_larHVCellCorrTool.retrieve();
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Failed to retrieve LArHVCellCorrTool " << m_larHVCellCorrTool << endreq;
      return StatusCode::FAILURE;
    }
    msg(MSG::INFO) << "Retrieved " << m_larHVCellCorrTool << " for on-the-fly noise rescaling" << endreq;
    sc=detStore()->regFcn(&ILArCellHVCorrTool::LoadCalibration, &(*m_larHVCellCorrTool),&CaloNoiseToolDB::clearCache, this);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Failed to install callback to LArHVCellCorrTool::LoadCalibration " << endreq;
      return StatusCode::FAILURE;
    }
    msg(MSG::INFO) << "Regestered callback on ILArCellHVCorrTool::LoadCalibration" << endreq;
  }

  msg(MSG::INFO) << "CaloNoiseToolDB geoInit() end" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode
CaloNoiseToolDB::clearCache( IOVSVC_CALLBACK_ARGS ) {
  if (m_rescaleForHV && m_larHVCellCorrTool->updateOnLastCallback()) {
    msg(MSG::INFO) << "Change of HV situation encountered. Invalidating cached noise" << endreq;
    m_cacheValid=false;
  }
  return StatusCode::SUCCESS;
}


//______________________________________________________________________________________
StatusCode
CaloNoiseToolDB::updateLumi( IOVSVC_CALLBACK_ARGS )
{

  msg(MSG::INFO) << " in updateLumi() " << endreq;

  const CondAttrListCollection* attrListColl = 0;
  StatusCode sc = detStore()->retrieve(attrListColl, m_lumiFolderName);
  if (sc.isFailure() || !attrListColl) {
     msg(MSG::WARNING)  << "attrrListColl not found for " << m_lumiFolderName << endreq;
     return StatusCode::SUCCESS;
  }
  // Loop over collection
  CondAttrListCollection::const_iterator first = attrListColl->begin();
  CondAttrListCollection::const_iterator last  = attrListColl->end();
  for (; first != last; ++first) {
   if ((*first).first == 0) {
      std::ostringstream attrStr1;
      (*first).second.toOutputStream( attrStr1 );
      ATH_MSG_DEBUG("ChanNum " << (*first).first << " Attribute list " << attrStr1.str());
      const coral::AttributeList& attrList = (*first).second;
      if (attrList["LBAvInstLumi"].isNull()) {
          msg(MSG::WARNING) << " NULL Luminosity information in database ... set it to 0 " << endreq;
          m_lumi0 = 0.;
      } else {
        m_lumi0 = attrList["LBAvInstLumi"].data<float>() *1e-3;  // luminosity (from 10**30 units in db to 10*33 units)
      }
      if ( !(m_lumi0 == m_lumi0) ) {
        msg(MSG::WARNING) << " Luminosity is not a number.. " << m_lumi0 << "  ... set it to 0 " << endreq;
        m_lumi0=0.;
      }
      m_Nminbias = m_lumi0*nMinbiasPerLumi;
      m_cacheValid = false;    
      break;
   }
  }
  return StatusCode::SUCCESS;
}

//_______________________________________________________________________________________
StatusCode
CaloNoiseToolDB::updateMap( IOVSVC_CALLBACK_ARGS_K(keys) )
{
  std::set<unsigned> channelsFound;
  std::list<std::string>::const_iterator itr;
  for (itr=keys.begin(); itr!=keys.end(); ++itr) {
    msg(MSG::INFO) <<  " updateMap callback for key " << *itr << endreq;
    const CondAttrListCollection* noiseAttrListColl;
    StatusCode sc=detStore()->retrieve(noiseAttrListColl,*itr);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Failed to retrieve CondAttrListCollection with key " << *itr << endreq;
      return sc;
    }

    //=== loop over collection (all cool channels)
    CondAttrListCollection::const_iterator iColl = noiseAttrListColl->begin();
    CondAttrListCollection::const_iterator last  = noiseAttrListColl->end();
    for (; iColl != last; ++iColl) {
      ATH_MSG_DEBUG("Working on folder " << *itr << " Channel " << iColl->first);
      const coral::Blob& blob = (iColl->second)["CaloCondBlob16M"].data<coral::Blob>();
      if (blob.size()<1) {
	ATH_MSG_DEBUG("Empty blob in folder " << *itr << ", channel " << iColl->first << ". Ignored.");
	continue;
      }

      if (channelsFound.insert(iColl->first).second==false) {
	msg(MSG::ERROR) << "Channel " << iColl->first << " encountered twice during this callback! Don't know what to do." << endreq;
	return StatusCode::FAILURE;
      }
      //=== COOL channel number is system id
      SYSTEM sysId = static_cast<SYSTEM>(iColl->first);
      
      //=== delete old CaloCondBlobFlt (which does not own the blob)
      std::map<SYSTEM, const CaloCondBlobFlt*>::iterator iOld = m_noiseBlobMap.find(sysId);
      if(iOld != m_noiseBlobMap.end()){
        delete iOld->second;
      }
      
      //=== Get new CaloCondBlobFlt instance, interpreting current BLOB
      
      const CaloCondBlobFlt* flt = CaloCondBlobFlt::getInstance(blob);
    
      //=== store new pointer in map
      m_noiseBlobMap[sysId] = flt;
      
    }//end iColl
  }
  
  //if (m_noiseBlobMap.size()==7) {
  //   this->updateCache();
  //}
  m_cacheValid=false;
  return StatusCode::SUCCESS;
}
//_______________________________________________________________________________________
void 
CaloNoiseToolDB::updateCache()
  //
  // This function is called when /CALO/Noise/CellNoise goes out of IOV.
  // It ensures that the pointers to the CaloCondBlobFlt objects stored in 
  // the different COOL channels are also up to date. 
  //
{
 msg(MSG::INFO) << " in update Cache " << endreq;

 if (m_noiseBlobMap.size()!=7) {
   msg(MSG::ERROR) << "Not all necessary COOL channels loaded yet. Got only channels [";
   for (auto& mapitem : m_noiseBlobMap) 
     msg(MSG::ERROR) << mapitem.first << " ";
   msg(MSG::ERROR) << "]" << endreq;
   //throw .... 
   return;
 }

  // Cache, what noise to return
  m_CachedGetNoiseCDDE=NULL;
  if(m_ReturnNoiseName=="electronicNoise") {
    msg(MSG::INFO) << "Will cache electronic noise" << endreq;
    m_CachedGetNoiseCDDE=&CaloNoiseToolDB::elecNoiseRMS;
    m_CachedGetNoiseCELL=&CaloNoiseToolDB::elecNoiseRMS;
    m_cached = ICalorimeterNoiseTool::ELECTRONICNOISE;
  }
  if(m_ReturnNoiseName=="pileupNoise") {
    msg(MSG::INFO) << "Will cache pileupNoise noise" << endreq;
    m_CachedGetNoiseCDDE=&CaloNoiseToolDB::pileupNoiseRMS;
    m_CachedGetNoiseCELL=&CaloNoiseToolDB::pileupNoiseRMS;
    m_cached = ICalorimeterNoiseTool::PILEUPNOISE;
  }
  if(m_ReturnNoiseName=="totalNoise") {
    msg(MSG::INFO) << "Will cache totalNoise noise" << endreq;
    m_CachedGetNoiseCDDE=&CaloNoiseToolDB::totalNoiseRMS;
    m_CachedGetNoiseCELL=&CaloNoiseToolDB::totalNoiseRMS;
    m_cached = ICalorimeterNoiseTool::TOTALNOISE;
  }
  if(m_CachedGetNoiseCDDE==NULL ||
     m_CachedGetNoiseCELL==NULL) {
    msg(MSG::ERROR) << "Unknown noise !" << endreq;
  }


  // total number of cells in calorimeter
  m_ncell=m_calo_id->calo_cell_hash_max();

  // Number of cells per system in EM Barrel and EndCap (indexed by SYSTEM, only first NUM_EM_SYS SYSTEMs)
  // This is not used later, only for printing
  int ncellEM[NUM_EM_SYS];

  std::fill_n(ncellEM, NUM_EM_SYS, 0);
  std::fill_n(m_firstSysHash, NUM_EM_SYS, IdentifierHash());

  IdentifierHash cellHash, cellHashEnd;
  m_calo_id->calo_cell_hash_range(CaloCell_ID::LAREM, cellHash, cellHashEnd);
  for (; cellHash != cellHashEnd; cellHash += 1) {
    Identifier id = m_calo_id->cell_id(cellHash);
    int sys = 0;
    if (m_calo_id->is_em_barrel(id)) {
      sys = (m_calo_id->pos_neg(id) > 0) ? EMBZPOS : EMBZNEG;
    } else if (m_calo_id->is_em_endcap(id)) {
      sys = (m_calo_id->pos_neg(id) > 0) ? EMECZPOS : EMECZNEG;
    }
    if (not m_firstSysHash[sys].is_valid()) m_firstSysHash[sys] = cellHash;
    ++ ncellEM[sys];
  }

  if (m_cacheUpdateCounter==0) {
    msg(MSG::INFO) << " starting cell hash in EM calo " << m_firstSysHash[EMECZNEG] << " " << m_firstSysHash[EMBZNEG]  << " " << m_firstSysHash[EMBZPOS] << " " << m_firstSysHash[EMECZPOS] << endreq;
    msg(MSG::INFO) << " number of cells in EM calo " << ncellEM[EMECZNEG] << " " << ncellEM[EMBZNEG]  << " " << ncellEM[EMBZPOS] << " " << ncellEM[EMECZPOS] << endreq;
    msg(MSG::INFO) << " number from Cools " <<  m_noiseBlobMap[EMECZNEG]->getNChans() << " " << m_noiseBlobMap[EMBZNEG]->getNChans() << " " 
                   << m_noiseBlobMap[EMBZPOS]->getNChans() << " " << m_noiseBlobMap[EMECZPOS]->getNChans() << endreq;
  }
  const int MaxGains = 4;  // make sure that code below does not exceed this size
  // m_noise is quite large and memory layout is important; profiling shows that
  // [MaxGains][m_ncell] ordering makes fewer cache misses than opposite order.
  m_noise.resize(boost::extents[MaxGains][m_ncell]);

  bool tile_not_found = true;
  
  msg(MSG::INFO) << " m_lumi0 " << m_lumi0 << endreq;
  float lumi = m_lumi0;
  for (int i=0;i<m_ncell;i++) {
      IdentifierHash idHash = i;
      int ngain=3;
      bool isTile=m_calo_id->is_tile(idHash);
      if (isTile) {
          ngain=4;
          if (tile_not_found) {
              tile_not_found = false;
              if (this->checkObjLength(i) < 5) {
                  msg(MSG::WARNING) 
                      << "Old CALO DB detected, double gaussian noise option has no effect" << endreq;
              } else {
                  msg(MSG::INFO) 
                      << "New CALO DB detected, double gaussian noise option can be used" << endreq;
              }
          }
      }
      for (int igain=0;igain<ngain;igain++) {
         CaloGain::CaloGain caloGain=CaloGain::INVALIDGAIN;;
         if (!isTile) caloGain = static_cast<CaloGain::CaloGain> (igain);
         else {
           if (igain==0) caloGain=CaloGain::TILELOWLOW;
           if (igain==1) caloGain=CaloGain::TILELOWHIGH;
           if (igain==2) caloGain=CaloGain::TILEHIGHLOW;
           if (igain==3) caloGain=CaloGain::TILEHIGHHIGH;
         }
         if (m_cached==ICalorimeterNoiseTool::TOTALNOISE) {
	   float hvcorr=1.0;
	   if (m_rescaleForHV && !isTile) hvcorr=m_larHVCellCorrTool->getCorrection(m_calo_id->cell_id(i));
           float noise = this->getDBNoise(i,caloGain,lumi,hvcorr);
           m_noise[igain][i] = noise;
           if (i==0 &&  m_cacheUpdateCounter==0) {
            msg(MSG::INFO) << " NoiseDB parameters for first cell at gain " << igain << endreq;
            msg(MSG::INFO) << " a  " << this->getA(i,caloGain) << endreq;
            msg(MSG::INFO) << " b  " << this->getB(i,caloGain) << endreq;
            msg(MSG::INFO) << " lumi " << lumi << endreq;
            msg(MSG::INFO) << " noise " << noise << endreq;
	    if (m_rescaleForHV)  msg(MSG::INFO) << " HV corr " << hvcorr << endreq;
           }
         }
         if (m_cached==ICalorimeterNoiseTool::ELECTRONICNOISE) {
           float noise = this->getA(i,caloGain);
	   if (m_rescaleForHV) {
	     const Identifier id= m_calo_id->cell_id(i); //convert hash to identifier  
	     noise*=m_larHVCellCorrTool->getCorrection(id); 
	   }
           m_noise[igain][i] = noise;
         }
         if (m_cached==ICalorimeterNoiseTool::PILEUPNOISE) {
           float noise = this->getB(i,caloGain)*sqrt(lumi);
           m_noise[igain][i] = noise;
         }
      }
  }
  m_cacheValid = true; 
  m_cacheUpdateCounter++;
}

//_______________________________________________________________________________________
float 
CaloNoiseToolDB::getDBNoise(unsigned int cellHash,
			    CaloGain::CaloGain caloGain, 
			    float lumi,
			    float hvcorr) const
{
 unsigned int subHash;
 SYSTEM sysId = this->caloSystem(cellHash,subHash);
 return this->getDBNoise(sysId, subHash, caloGain, lumi, hvcorr);
}


//
float
CaloNoiseToolDB::getA(unsigned int cellHash, CaloGain::CaloGain caloGain) const
{
 unsigned int subHash;
 SYSTEM sysId = this->caloSystem(cellHash,subHash);
 return this->getA(sysId,subHash,caloGain);
}

//
float
CaloNoiseToolDB::getB(unsigned int cellHash, CaloGain::CaloGain caloGain) const
{
 unsigned int subHash;
 SYSTEM sysId = this->caloSystem(cellHash,subHash);
 return this->getB(sysId,subHash,caloGain);
}

//
float
CaloNoiseToolDB::getC(unsigned int cellHash, CaloGain::CaloGain caloGain) const
{
 unsigned int subHash;
 SYSTEM sysId = this->caloSystem(cellHash,subHash);
 return this->getC(sysId,subHash,caloGain);
}

//
float
CaloNoiseToolDB::getD(unsigned int cellHash, CaloGain::CaloGain caloGain) const
{
 unsigned int subHash;
 SYSTEM sysId = this->caloSystem(cellHash,subHash);
 return this->getD(sysId,subHash,caloGain);
}

//
float
CaloNoiseToolDB::getE(unsigned int cellHash, CaloGain::CaloGain caloGain) const
{
 unsigned int subHash;
 SYSTEM sysId = this->caloSystem(cellHash,subHash);
 return this->getE(sysId,subHash,caloGain);
}

CaloNoiseToolDB::SYSTEM CaloNoiseToolDB::caloSystem(unsigned int cellHash, unsigned int& subHash) const
{
  int subCalo;
  subHash = m_calo_id->subcalo_cell_hash (cellHash, subCalo);
  if (subCalo == CaloCell_ID::LAREM) { // EM calo
    for (int i = 0; i != NUM_EM_SYS-1; ++ i) {
      if (cellHash < m_firstSysHash[i+1].value()) {
        subHash = cellHash - m_firstSysHash[i].value();
        return SYSTEM(i);
      }
    }
    subHash = cellHash - m_firstSysHash[NUM_EM_SYS-1].value();
    return SYSTEM(NUM_EM_SYS-1);
  } else if (subCalo == CaloCell_ID::LARHEC) { // HEC
    return HEC;
  } else if (subCalo == CaloCell_ID::LARFCAL) { // Fcal
    return FCAL;
  } else {
    if (subCalo != CaloCell_ID::TILE) {
      std::cout << " something wrong. either LAr not tile..." << std::endl;
    }
    return TILE;
  }
}



//_______________________________________________________________________________________
float 
CaloNoiseToolDB::getA(SYSTEM sysId, unsigned int cellHash, CaloGain::CaloGain caloGain) const
{
  const CaloCondBlobFlt* const flt = m_noiseBlobMap.find(sysId)->second;
  unsigned int dbGain = CaloCondUtils::getDbCaloGain(caloGain);
  return flt->getData(cellHash,dbGain,0);
}

//
//_______________________________________________________________________________________
float 
CaloNoiseToolDB::getB(SYSTEM sysId, unsigned int cellHash, CaloGain::CaloGain caloGain) const
{
  const CaloCondBlobFlt* const flt = m_noiseBlobMap.find(sysId)->second;
  unsigned int dbGain = CaloCondUtils::getDbCaloGain(caloGain);
  return flt->getData(cellHash,dbGain,1);
}

//
//_______________________________________________________________________________________
float 
CaloNoiseToolDB::getC(SYSTEM sysId, unsigned int cellHash, CaloGain::CaloGain caloGain) const
{
  const CaloCondBlobFlt* const flt = m_noiseBlobMap.find(sysId)->second;
  unsigned int dbGain = CaloCondUtils::getDbCaloGain(caloGain);
  return flt->getData(cellHash,dbGain,2);
}

//
//_______________________________________________________________________________________
float 
CaloNoiseToolDB::getD(SYSTEM sysId, unsigned int cellHash, CaloGain::CaloGain caloGain) const
{
  const CaloCondBlobFlt* const flt = m_noiseBlobMap.find(sysId)->second;
  unsigned int dbGain = CaloCondUtils::getDbCaloGain(caloGain);
  return flt->getData(cellHash,dbGain,3);
}

//
//_______________________________________________________________________________________
float 
CaloNoiseToolDB::getE(SYSTEM sysId, unsigned int cellHash, CaloGain::CaloGain caloGain) const
{
  const CaloCondBlobFlt* const flt = m_noiseBlobMap.find(sysId)->second;
  unsigned int dbGain = CaloCondUtils::getDbCaloGain(caloGain);
  return flt->getData(cellHash,dbGain,4);
}

//
int 
CaloNoiseToolDB::checkObjLength(unsigned int cellHash) const
{
 unsigned int subHash;
 SYSTEM sysId = this->caloSystem(cellHash,subHash);
 const CaloCondBlobFlt* const flt = m_noiseBlobMap.find(sysId)->second;
 return flt->getObjSizeUint32();
}

//_______________________________________________________________________________________
float 
CaloNoiseToolDB::getDBNoise(SYSTEM sysId, 
			    unsigned int cellHash, 
			    CaloGain::CaloGain caloGain, 
			    float lumi,
			    float hvcorr) const
{
  const CaloCondBlobFlt* const flt = m_noiseBlobMap.find(sysId)->second;
  unsigned int dbGain = CaloCondUtils::getDbCaloGain(caloGain);
  //return flt->getCalib(cellHash, dbGain, lumi);
  const float a=flt->getData(cellHash,dbGain,0)*hvcorr;
  const float b=flt->getData(cellHash,dbGain,1);

  if (flt->getObjVersion()!=1) 
    msg(MSG::ERROR) << "Encountered unexpected object version " << flt->getObjVersion() << endreq;

 
  return std::sqrt(a*a + b*b*lumi);
}





//////////////////////////////////////////////////
// functions for the getNoise interface
float
CaloNoiseToolDB::getNoise(const CaloDetDescrElement* caloDDE, CalorimeterNoiseType type)
{
  if (!m_cacheValid) this->updateCache(); 
  float Nminbias=-1;
  if( m_Nminbias>0 )
    Nminbias=m_Nminbias;
  
  CaloGain::CaloGain igain = m_highestGain[caloDDE->getSubCalo()];
  if( m_gain_from_joboption>=0 &&
      (type == ICalorimeterNoiseTool::ELECTRONICNOISE ||
       type == ICalorimeterNoiseTool::PILEUPNOISE ||
       type == ICalorimeterNoiseTool::TOTALNOISE ))
    igain = static_cast<CaloGain::CaloGain>(m_gain_from_joboption);
  
  switch(type) {
  case ICalorimeterNoiseTool::JOBOPTION:
    return (this->*m_CachedGetNoiseCDDE)(caloDDE,igain,Nminbias,
					 ICaloNoiseToolStep::CELLS);
    break;
  case ICalorimeterNoiseTool::ELECTRONICNOISE_HIGHESTGAIN:
  case ICalorimeterNoiseTool::ELECTRONICNOISE:
    return elecNoiseRMS(caloDDE,igain,Nminbias,
			ICaloNoiseToolStep::CELLS);
    break;
  case ICalorimeterNoiseTool::PILEUPNOISE_HIGHESTGAIN:
  case ICalorimeterNoiseTool::PILEUPNOISE:
    return pileupNoiseRMS(caloDDE,igain,Nminbias,
			  ICaloNoiseToolStep::CELLS);
    break;
  case ICalorimeterNoiseTool::TOTALNOISE_HIGHESTGAIN:
  case ICalorimeterNoiseTool::TOTALNOISE:
    return totalNoiseRMS(caloDDE,igain,Nminbias,
			 ICaloNoiseToolStep::CELLS);
    break;
    
  default:
    return 0.;
    break;
  }
}

float
CaloNoiseToolDB::getNoise(const CaloCell* caloCell, CalorimeterNoiseType type)
{

  if (!m_cacheValid) this->updateCache(); 
  const CaloDetDescrElement* caloDDE = caloCell->caloDDE();
  float Nminbias=-1;
  if( m_Nminbias>0 )
    Nminbias=m_Nminbias;
  
  CaloGain::CaloGain igain = caloCell->gain();
  
  if( m_gain_from_joboption>=0 &&
      (type == ICalorimeterNoiseTool::ELECTRONICNOISE ||
       type == ICalorimeterNoiseTool::PILEUPNOISE ||
       type == ICalorimeterNoiseTool::TOTALNOISE ))
    igain = static_cast<CaloGain::CaloGain>(m_gain_from_joboption);
  
  switch(type) {
  case ICalorimeterNoiseTool::JOBOPTION:
    return (this->*m_CachedGetNoiseCELL)(caloCell,igain,Nminbias,
					 ICaloNoiseToolStep::CELLS);
    break;
  case ICalorimeterNoiseTool::ELECTRONICNOISE_HIGHESTGAIN:
    // overwrite iGain with highest gain for that detector and
    // continue into normal code (NO break !!)
    igain=m_highestGain[caloDDE->getSubCalo()];
  case ICalorimeterNoiseTool::ELECTRONICNOISE:
    return elecNoiseRMS(caloDDE,igain,Nminbias,
			ICaloNoiseToolStep::CELLS);
    break;
    
  case ICalorimeterNoiseTool::PILEUPNOISE_HIGHESTGAIN:
    // overwrite iGain with highest gain for that detector and
    // continue into normal code (NO break !!)
    igain=m_highestGain[caloDDE->getSubCalo()];
    //  NOTE: igain is not used at the moment for pileupnoise
  case ICalorimeterNoiseTool::PILEUPNOISE:
    return pileupNoiseRMS(caloDDE,igain,Nminbias,
			  ICaloNoiseToolStep::CELLS);
    break;
    
  case ICalorimeterNoiseTool::TOTALNOISE_HIGHESTGAIN:
    // overwrite iGain with highest gain for that detector and
    // continue into normal code (NO break !!)
    igain=m_highestGain[caloDDE->getSubCalo()];
    //  NOTE: igain is only used for the electronics noise at the moment
  case ICalorimeterNoiseTool::TOTALNOISE:
    return totalNoiseRMS(caloDDE,igain,Nminbias,
			 ICaloNoiseToolStep::CELLS);
    break;
    
  default:
    return 0.;
    break;
  }
}

float
CaloNoiseToolDB::getNoise(const CaloCell* caloCell, float energy, CalorimeterNoiseType type)
{

  if (!m_cacheValid) this->updateCache(); 
  const CaloDetDescrElement* caloDDE = caloCell->caloDDE();
  float Nminbias=-1;
  if( m_Nminbias>0 )
    Nminbias=m_Nminbias;
  
  CaloGain::CaloGain igain = caloCell->gain();
  
  if( m_gain_from_joboption>=0 &&
      (type == ICalorimeterNoiseTool::ELECTRONICNOISE ||
       type == ICalorimeterNoiseTool::PILEUPNOISE ||
       type == ICalorimeterNoiseTool::TOTALNOISE ))
    igain = static_cast<CaloGain::CaloGain>(m_gain_from_joboption);
  
  switch(type) {
  case ICalorimeterNoiseTool::JOBOPTION:
    return (this->*m_CachedGetNoiseCELL)(caloCell,igain,Nminbias,
					 ICaloNoiseToolStep::CELLS);
    break;
  case ICalorimeterNoiseTool::ELECTRONICNOISE_HIGHESTGAIN:
    // overwrite iGain with highest gain for that detector and
    // continue into normal code (NO break !!)
    igain=m_highestGain[caloDDE->getSubCalo()];
  case ICalorimeterNoiseTool::ELECTRONICNOISE:
    return elecNoiseRMS(caloDDE,igain,Nminbias,energy,
			ICaloNoiseToolStep::CELLS);
    break;
    
  case ICalorimeterNoiseTool::PILEUPNOISE_HIGHESTGAIN:
    // overwrite iGain with highest gain for that detector and
    // continue into normal code (NO break !!)
    igain=m_highestGain[caloDDE->getSubCalo()];
    //  NOTE: igain is not used at the moment for pileupnoise
  case ICalorimeterNoiseTool::PILEUPNOISE:
    return pileupNoiseRMS(caloDDE,igain,Nminbias,
			  ICaloNoiseToolStep::CELLS);
    break;
    
  case ICalorimeterNoiseTool::TOTALNOISE_HIGHESTGAIN:
    // overwrite iGain with highest gain for that detector and
    // continue into normal code (NO break !!)
    igain=m_highestGain[caloDDE->getSubCalo()];
    //  NOTE: igain is only used for the electronics noise at the moment
  case ICalorimeterNoiseTool::TOTALNOISE:
    return totalNoiseRMS(caloDDE,igain,Nminbias,energy,
			 ICaloNoiseToolStep::CELLS);
    break;
    
  default:
    return 0.;
    break;
  }
}

float 
CaloNoiseToolDB::calcSig(double e, double sigma1, double ratio, double sigma2) {
//Luca: code to return the C.L. of the double gaussian with
//3 parameters.
//sigma1, sigma2 and ratio are the 3 noise parameters
// e is the input energy of which you want to calculated the
//significance
  float valid_range[2]={0.9,7.5};
  
  if((sigma1 == 0. && sigma2 == 0.) || e == 0.) return 0.;
  if(sigma1 == 0.) return e/sigma2;
  if((ratio  == 0.) || sigma2 == 0.) return e/sigma1;
  double x1 = e/sigma1;
  double x2 = e/sigma2;

  if (m_speedTwoGauss) {
    float wide_gauss_sigma = std::min(fabs(x1),fabs(x2));
    if(wide_gauss_sigma > valid_range[1]) return wide_gauss_sigma;
    float narrow_gauss_sigma= std::max(fabs(x1),fabs(x2));
    if(narrow_gauss_sigma < valid_range[0]) return narrow_gauss_sigma;
  }

  double y1= TMath::Erf(invsqrt2*x1);
  double y2= TMath::Erf(invsqrt2*x2);

  double z = ( y1*sigma1 + ratio*y2*sigma2 )/( sigma1 + ratio*sigma2);

  //return the C.L. probability (with sign!)
  //  return z;
 
  // if instead you want to return the sigma-equivalent C.L.
  // (with sign!) use the following line
  if (std::abs(z) < 1.0) {
    return sqrt2*boost::math::erf_inv(z);
  }
  return 0.0;

}

//========================  USER INTERFACES ===================================


//////////////////////////////////////////////////

float 
CaloNoiseToolDB::elecNoiseRMS(const CaloCell* caloCell, 
		 	    const int /* step */)
{
  if (!m_cacheValid) this->updateCache();
  Identifier id = caloCell->ID();
  int i = (int)(m_calo_id->calo_cell_hash(id));
  if (i>m_ncell) return -1;
  CaloGain::CaloGain calogain = caloCell->gain();
  if (m_cached==ICalorimeterNoiseTool::ELECTRONICNOISE && m_cacheValid) {
    int igain = CaloCondUtils::getDbCaloGain(calogain);
    return m_noise[igain][i];
  }
  else 
    if (m_rescaleForHV && !caloCell->caloDDE()->is_tile())
      return m_larHVCellCorrTool->getCorrection(id) * this->getA(i,calogain);
    else
      return this->getA(i,calogain);
}

//////////////////////////////////////////////////

float 
CaloNoiseToolDB::elecNoiseRMS(const CaloCell* caloCell, 
			    const float /* Nminbias */, const int step)
{
  return this->elecNoiseRMS(caloCell,step);
}

//////////////////////////////////////////////////

float
CaloNoiseToolDB::elecNoiseRMSHighestGain(const CaloCell* caloCell, 
				       const int /* step */)
{    
 if (!m_cacheValid) this->updateCache(); 
 Identifier id = caloCell->ID();
 int i = (int)(m_calo_id->calo_cell_hash(id));
 if (i>m_ncell) return -1;
 const CaloDetDescrElement* caloDDE = caloCell->caloDDE();
 int subcalo = caloDDE->getSubCalo();
 if (subcalo < 0 || subcalo >= m_nCalos) return -1;
 CaloGain::CaloGain highestgain=m_highestGain[subcalo];
 if (m_rescaleForHV && !caloDDE->is_tile())
   return m_larHVCellCorrTool->getCorrection(id) * this->getA(i,highestgain);
 else
   return this->getA(i,highestgain);
}

//////////////////////////////////////////////////

float
CaloNoiseToolDB::elecNoiseRMSHighestGain(const CaloCell* caloCell, 
				         const float /* Nminbias */, const int step)
{   
 return this->elecNoiseRMSHighestGain(caloCell,step);
 
}

//////////////////////////////////////////////////

float
CaloNoiseToolDB::elecNoiseRMSHighestGain(const CaloDetDescrElement* caloDDE, 
				       const int step)
{    
  CaloGain::CaloGain highestGain=m_highestGain[caloDDE->getSubCalo()];
  return this->elecNoiseRMS(caloDDE,highestGain,-1,step);    
}

//////////////////////////////////////////////////

float
CaloNoiseToolDB::elecNoiseRMSHighestGain(const CaloDetDescrElement* caloDDE, 
				       const float Nminbias, const int step)
{    
  CaloGain::CaloGain highestGain=m_highestGain[caloDDE->getSubCalo()];
  return this->elecNoiseRMS(caloDDE,highestGain,Nminbias,step);    
}

//////////////////////////////////////////////////

float 
CaloNoiseToolDB::elecNoiseRMS(const CaloDetDescrElement* caloDDE, 
			    const CaloGain::CaloGain gain,
			    const float /* Nminbias */, 
			    const int /* step */)
{  
  if (!m_cacheValid) this->updateCache();
  int i = (int)(caloDDE->calo_hash());
  if (i>m_ncell) return -1;
  if (m_cached==ICalorimeterNoiseTool::ELECTRONICNOISE && m_cacheValid) {
    int igain = CaloCondUtils::getDbCaloGain(gain);
    return m_noise[igain][i];
  }
  else
    if (m_rescaleForHV &&  !caloDDE->is_tile())
      return m_larHVCellCorrTool->getCorrection(caloDDE->identify()) * this->getA(i,gain);
    else
      return this->getA(i,gain);
}

//////////////////////////////////////////////////

float 
CaloNoiseToolDB::elecNoiseRMS(const CaloDetDescrElement* caloDDE, 
			    const CaloGain::CaloGain gain,
			    const float /* Nminbias */, 
			    const float energy, 
			    const int /* step */)
{  
  if (!m_cacheValid) this->updateCache();
  int i = (int)(caloDDE->calo_hash());
  if (i>m_ncell) return -1;
  unsigned int subHash;
  SYSTEM sysId = this->caloSystem(i,subHash);
  float hvcorr=1.0;
  if (sysId!=TILE && m_rescaleForHV) {
    hvcorr=m_larHVCellCorrTool->getCorrection(caloDDE->identify());
  }
  const CaloCondBlobFlt* const flt = m_noiseBlobMap.find(sysId)->second;
  unsigned int dbGain = CaloCondUtils::getDbCaloGain(gain);
  if (flt->getObjSizeUint32() > 4) { // need at least 5 elements in a vector for 2G noise
    //This clause is only executed for Tile -> no LAr HV Correction
    double sigma1 = flt->getData(subHash,dbGain,2);
    double sigma2 = flt->getData(subHash,dbGain,3);
    double ratio  = flt->getData(subHash,dbGain,4);
    float n_sigma = this->calcSig(energy, sigma1, ratio, sigma2);
    float equiv_noise = (n_sigma != 0.) ? fabs(energy/n_sigma) : 0.0;
    return equiv_noise;
  } else {
    if (m_cached==ICalorimeterNoiseTool::ELECTRONICNOISE && m_cacheValid) {
      return m_noise[dbGain][i];
    }
    else {
      return flt->getData(subHash,dbGain,0)*hvcorr;
    }
  }
}

//////////////////////////////////////////////////

std::vector<float> 
CaloNoiseToolDB::elecNoiseRMS3gains(const CaloDetDescrElement* caloDDE, 
				  const int step)
{  
  std::vector<float> sigma;
  for(int igain=0;igain<CaloGain::LARNGAIN;++igain)
    sigma.push_back(this->elecNoiseRMS(caloDDE,static_cast<CaloGain::CaloGain>(igain),-1,step));
  return sigma;

}

//////////////////////////////////////////////////

std::vector<float> 
CaloNoiseToolDB::elecNoiseRMS3gains(const CaloDetDescrElement* caloDDE, 
				  const float /* NMinBias */,const int step)
{  
  return this->elecNoiseRMS3gains(caloDDE,step);
}

//////////////////////////////////////////////////

VectorContainer* 
CaloNoiseToolDB::elecNoiseRMSContainer(const int &iCalo) 
{
  msg(MSG::WARNING) << " elecNoiseRMSContainer not implemented " << iCalo << endreq ;
  return 0;
}

//////////////////////////////////////////////////
// new interfaces to enable caching of the pointer to member

float 
CaloNoiseToolDB::elecNoiseRMS(const CaloCell* theCell,
			    const CaloGain::CaloGain /*gain*/,
			    const float Nminbias, 
			    const int step) {
  return this->elecNoiseRMS(theCell,Nminbias,step);
}

float 
CaloNoiseToolDB::pileupNoiseRMS(const CaloCell* theCell,
			      const CaloGain::CaloGain /*gain*/,
			      const float Nminbias, 
			      const int /*step*/) {
  return this->pileupNoiseRMS(theCell,Nminbias);
}

float 
CaloNoiseToolDB::totalNoiseRMS(const CaloCell* theCell,
			     const CaloGain::CaloGain /*gain*/,
			     const float Nminbias, 
			     const int /* step */) {
   return this->totalNoiseRMS(theCell,Nminbias);
}

float 
CaloNoiseToolDB::pileupNoiseRMS(const CaloDetDescrElement* caloDDE, 
			      const CaloGain::CaloGain gain,
			      const float Nminbias, 
			      const int /*step*/) {
  if (!m_cacheValid) this->updateCache();
  int i = (int)(caloDDE->calo_hash());
  if (i>m_ncell) return -1;
  if (m_cached==ICalorimeterNoiseTool::PILEUPNOISE && (Nminbias == m_Nminbias || Nminbias<0) && m_cacheValid) {
    int igain = CaloCondUtils::getDbCaloGain(gain);
    return m_noise[igain][i];
  }
  else {
    float lumi;
    if (Nminbias>0) {
      lumi=Nminbias*lumiPerNMinbias;
    }
    else {
      lumi=m_lumi0;
    }
    return (this->getB(i,gain)*sqrt(lumi));
  }

}

float 
CaloNoiseToolDB::totalNoiseRMS(const CaloDetDescrElement* caloDDE, 
			     const CaloGain::CaloGain gain,
			     const float Nminbias, 
			     const int /*step*/) {
  if (!m_cacheValid) this->updateCache();
  int i = (int)(caloDDE->calo_hash());
  if (i>m_ncell) return -1;
  if (m_cached==ICalorimeterNoiseTool::TOTALNOISE && (Nminbias == m_Nminbias || Nminbias<0) && m_cacheValid) {
    int igain = CaloCondUtils::getDbCaloGain(gain);
    return m_noise[igain][i];
  } else {
    float lumi = (Nminbias>0) ? Nminbias*lumiPerNMinbias : m_lumi0;

    float hvcorr=1.0;
    if (m_rescaleForHV && !caloDDE->is_tile()) hvcorr=m_larHVCellCorrTool->getCorrection(caloDDE->identify()); 

    return this->getDBNoise(i,gain,lumi,hvcorr);
  }
}

float 
CaloNoiseToolDB::totalNoiseRMS(const CaloDetDescrElement* caloDDE, 
			     const CaloGain::CaloGain gain,
			     const float Nminbias, 
			     const float energy, 
			     const int /*step*/) {
  //  std::cout << "totalNoiseRMS e " << energy << " gain " << gain << std::endl;
  if (!m_cacheValid) this->updateCache(); 
  int i = (int)(caloDDE->calo_hash());
  if (i>m_ncell) return -1;
  float lumi = (Nminbias>0) ? Nminbias*lumiPerNMinbias : m_lumi0;
  float x;
  float a = elecNoiseRMS(caloDDE,gain,Nminbias,energy,
			 ICaloNoiseToolStep::CELLS); //Includes HV correction is applicable
  float b = this->getB(i,gain);

  int objver = m_noiseBlobMap[TILE]->getObjVersion();
  if(objver==1){
    //=== Total noise parameterized as
    //=== Sigma**2 = a**2 + b**2 * Lumi
    x = std::sqrt( a*a + b*b*lumi );
  }
  else if (objver==2) {
    //== parameterization for pedestal = a + b*Lumi
    x = a+b*lumi;
  }
  else{
    throw CaloCond::VersionConflict("CaloNoiseToolDB::totalNoiseRMS ",objver);
  }
  return x;
}
//////////////////////////////////////////////////

float 
CaloNoiseToolDB::pileupNoiseRMS(const CaloCell* caloCell, const float Nminbias)
{
  if (!m_cacheValid) this->updateCache();
  Identifier id = caloCell->ID();
  int i = (int)(m_calo_id->calo_cell_hash(id));
  if (i>m_ncell) return -1;
  CaloGain::CaloGain calogain = caloCell->gain();
  if (m_cached==ICalorimeterNoiseTool::PILEUPNOISE && (Nminbias == m_Nminbias || Nminbias<0) && m_cacheValid) {
    int igain = CaloCondUtils::getDbCaloGain(calogain);
    return m_noise[igain][i];
  }
  else {
    float lumi;
    if (Nminbias>0) {
      lumi=Nminbias*lumiPerNMinbias;
    }
    else {
       lumi=m_lumi0;
    }
    return (this->getB(i,calogain)*sqrt(lumi));
  }
}

//////////////////////////////////////////////////

float 
CaloNoiseToolDB::pileupNoiseRMS(const CaloDetDescrElement* caloDDE, 
			      const float Nminbias)
{
  if (!m_cacheValid) this->updateCache();
  int subcalo = caloDDE->getSubCalo();
  if (subcalo < 0 || subcalo >= m_nCalos) return -1;
  CaloGain::CaloGain highestGain=m_highestGain[subcalo];
  int i = (int)(caloDDE->calo_hash());
  if (i>m_ncell) return -1;
  if (m_cached==ICalorimeterNoiseTool::PILEUPNOISE && (Nminbias == m_Nminbias || Nminbias<0) && m_cacheValid) {
    int igain = CaloCondUtils::getDbCaloGain(highestGain);
    return m_noise[igain][i];
  }
  else {
    float lumi;
    if (Nminbias>0) {
      lumi=Nminbias*lumiPerNMinbias;
    }
    else {
     lumi=m_lumi0;
    }
    return (this->getB(i,highestGain)*sqrt(lumi));
  }

}

//////////////////////////////////////////////////

float 
CaloNoiseToolDB::totalNoiseRMS(const CaloCell* caloCell, const float Nminbias)  {

  return this->totalNoiseRMS(caloCell->caloDDE(),caloCell->gain(),Nminbias);
/*
{
  if (!m_cacheValid) this->updateCache();
  Identifier id = caloCell->ID();
  int i = (int)(m_calo_id->calo_cell_hash(id));
  if (i>m_ncell) return -1;
  CaloGain::CaloGain calogain = caloCell->gain();
  if (m_cached==ICalorimeterNoiseTool::TOTALNOISE && (Nminbias == m_Nminbias || Nminbias<0) && m_cacheValid) {
    int igain=CaloCondUtils::getDbCaloGain(calogain);
    return m_noise[igain][i];
  } else {
    float lumi = (Nminbias>0) ? Nminbias*lumiPerNMinbias : m_lumi0;
    return this->getDBNoise(i,calogain,lumi);
  }

*/
}

//////////////////////////////////////////////////

float 
CaloNoiseToolDB::totalNoiseRMS(const CaloDetDescrElement* caloDDE, 
			       const CaloGain::CaloGain gain, 
			       const float Nminbias)
{
  if (!m_cacheValid) this->updateCache();
  int i = (int)(caloDDE->calo_hash());
  if (i>m_ncell) return -1;
  if (m_cached==ICalorimeterNoiseTool::TOTALNOISE && (Nminbias == m_Nminbias || Nminbias<0) && m_cacheValid) {
    int igain = CaloCondUtils::getDbCaloGain(gain);
    return m_noise[igain][i];
  } else {
    float lumi = (Nminbias>0) ? Nminbias*lumiPerNMinbias : m_lumi0;
    
    float hvcorr=1.0;
    if (m_rescaleForHV && !caloDDE->is_tile()) hvcorr=m_larHVCellCorrTool->getCorrection(caloDDE->identify()); 
    return this->getDBNoise(i,gain,lumi,hvcorr);
  }
  
}

//////////////////////////////////////////////////

float 
CaloNoiseToolDB::totalNoiseRMSHighestGain(const CaloCell* caloCell, 
					  const float Nminbias)
{
  return this->totalNoiseRMSHighestGain(caloCell->caloDDE(),Nminbias);
}

//////////////////////////////////////////////////

float 
CaloNoiseToolDB::totalNoiseRMSHighestGain(const CaloDetDescrElement* caloDDE, 
					const float Nminbias)
{
  CaloGain::CaloGain highestGain=m_highestGain[caloDDE->getSubCalo()];
  return this->totalNoiseRMS(caloDDE,highestGain,Nminbias);
}


CaloGain::CaloGain CaloNoiseToolDB::estimatedGain(const CaloCell* /* caloCell */,
                                 const int & /*step */) {
 msg(MSG::WARNING) << "  CaloNoiseToolDB::estimatedGain returns INVALIDGAIN " << endreq;
 return CaloGain::INVALIDGAIN;
}
CaloGain::CaloGain CaloNoiseToolDB::estimatedGain(const CaloCell* /* caloCell */,
                                 const CaloDetDescrElement* /* caloDDE */,
                                 const int & /*step */){
 msg(MSG::WARNING) << "  CaloNoiseToolDB::estimatedGain returns INVALIDGAIN " << endreq;
 return CaloGain::INVALIDGAIN;
}
CaloGain::CaloGain CaloNoiseToolDB::estimatedGain(const CaloDetDescrElement* /* caloDDE */,
                                 const float& /* energy */,
                                 const int & /* step */) {
 msg(MSG::WARNING) << "  CaloNoiseToolDB::estimatedGain returns INVALIDGAIN " << endreq;
 return CaloGain::INVALIDGAIN;
}

StatusCode CaloNoiseToolDB::LoadCalibration(IOVSVC_CALLBACK_ARGS) {
 msg(MSG::WARNING) << "  CaloNoiseToolDB::LoadCalibration dummy method " << endreq;
 return StatusCode::SUCCESS;
}


// GU: below are temporary implementations, waiting for non-gaussian noise description in DB (at least for Tiles)

/////////////////////////////////////////////////////////////////////////

bool CaloNoiseToolDB::isEOverNSigma(const CaloCell* caloCell, float sigmaCut ,
                     CalorimeterNoiseSymmetryHandling /* symmetryHandling */,
                     CalorimeterNoiseType type)
{
  float noise_cut;
  const CaloDetDescrElement* caloDDE = caloCell->caloDDE();
  if(caloDDE->getSubCalo()==CaloCell_ID::TILE) {
    noise_cut = fabs(sigmaCut*this->getNoise(caloCell, (float) caloCell->e(), type));
  } else {
    noise_cut = fabs(sigmaCut*this->getNoise(caloCell, type));
  }
  float energy = fabs(caloCell->e());
  return (energy>noise_cut);
}

////////////////////////////////////////////////////////////////////

float CaloNoiseToolDB::getRandomisedE(const CaloCell* caloCell , CLHEP::HepRandomEngine* engine, CalorimeterNoiseType type)
{
  double rms = this->getNoise(caloCell,type);
  return RandGauss::shoot(engine,0.,rms);
}

//////////////////////////////////////////////////////////////////////////////////////

float CaloNoiseToolDB::getEffectiveSigma(const CaloCell* caloCell,
                     CalorimeterNoiseSymmetryHandling /*symmetryHandling*/,
                     CalorimeterNoiseType type )
{
  const CaloDetDescrElement* caloDDE = caloCell->caloDDE();
  float eqnoise;
  if(caloDDE->getSubCalo()==CaloCell_ID::TILE) {
    eqnoise = this->getNoise(caloCell, (float) caloCell->e(), type);
  } else {
    eqnoise = this->getNoise(caloCell, type);
  }
  return eqnoise;
     // simple implementation for gaussian noise
  //     return this->getNoise(caloCell,type);
}
