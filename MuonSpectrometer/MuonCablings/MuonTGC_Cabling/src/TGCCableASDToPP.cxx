/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCCableASDToPP.h"
#include <fstream>
#include <sstream>
#include "GaudiKernel/StatusCode.h"

#include "MuonTGC_Cabling/TGCDatabaseASDToPP.h" 
#include "MuonTGC_Cabling/TGCChannelASDOut.h"
#include "MuonTGC_Cabling/TGCChannelPPIn.h"

namespace MuonTGC_Cabling {

// Constructor & Destructor
TGCCableASDToPP::TGCCableASDToPP(std::string filename)
  : TGCCable(TGCCable::ASDToPP),  
    m_tgcCablingDbTool("TGCCablingDbTool"), 
    m_ASD2PP_DIFF_12(0)
{
  initialize(filename);
}

TGCCableASDToPP::~TGCCableASDToPP(void)
{
  // MuonSpectrometer/MuonCablings/TGCcablingInterface/TGCcablingInterface/TGCIdBase.h defines  
  // enum ModuleType    {NoModuleType=-1,    WD,SD,WT,ST,WI,SI, MaxModuleType}; as of TGCcablingInterface-00-01-12.  
  // If this order is changed, these for loops should be revisited.  
  for(int side=0; side<TGCIdBase::MaxSideType; side++) { 
    for(int module=TGCIdBase::WD; module<=TGCIdBase::SI; module++) { 
      for(int region=TGCIdBase::Endcap; region<=TGCIdBase::Forward; region++) { 
	for(int sector=0;  
	    (module<=TGCIdBase::ST && ((region==TGCIdBase::Forward && sector<TGCId::NumberOfForwardSector) || // Big Wheel Forward  
				       (region==TGCIdBase::Endcap && sector<TGCId::NumberOfEndcapSector))) // Big Wheel Endcap  
	      || (module>=TGCIdBase::WI && sector<TGCId::NumberOfInnerSector); // EI/FI  
	    sector++) {  
	  TGCDatabaseASDToPP* db = dynamic_cast<TGCDatabaseASDToPP*>(getDatabase(side, region, sector, module)); 
	  // If the database is used by only this sector, it is deleted now.  
	  if(db && !(db->isCommon())) {  
	    delete db; 
	    db = 0; 
	  } 
	} 
      } 
    } 
  } 

  // Common databases are deleted.   
  for(int region=0; region<TGCIdBase::MaxRegionType; region++) { 
    for(int module=0; module<TGCIdBase::MaxModuleType; module++) { 
      delete m_commonDb[region][module]; // delete null pointer should be safe.   
      m_commonDb[region][module] = 0;  
    } 
  } 

  delete m_ASD2PP_DIFF_12;
  m_ASD2PP_DIFF_12 = 0;
}


void TGCCableASDToPP::initialize(std::string& filename)
{
  // Common database pointers are initialized 
  for(int region=0; region<TGCIdBase::MaxRegionType; region++) { 
    for(int module=0; module<TGCIdBase::MaxModuleType; module++) { 
      m_commonDb[region][module] = 0; 
    } 
  } 

  m_commonDb[TGCIdBase::Forward][TGCIdBase::WD] = new TGCDatabaseASDToPP(filename, "FWD"); 
  m_commonDb[TGCIdBase::Forward][TGCIdBase::SD] = new TGCDatabaseASDToPP(filename, "FSD"); 
  m_commonDb[TGCIdBase::Forward][TGCIdBase::WT] = new TGCDatabaseASDToPP(filename, "FWT"); 
  m_commonDb[TGCIdBase::Forward][TGCIdBase::ST] = new TGCDatabaseASDToPP(filename, "FST"); 
  
  m_commonDb[TGCIdBase::Endcap][TGCIdBase::WD] = new TGCDatabaseASDToPP(filename, "EWD"); 
  m_commonDb[TGCIdBase::Endcap][TGCIdBase::SD] = new TGCDatabaseASDToPP(filename, "ESD"); 
  m_commonDb[TGCIdBase::Endcap][TGCIdBase::WT] = new TGCDatabaseASDToPP(filename, "EWT"); 
  m_commonDb[TGCIdBase::Endcap][TGCIdBase::ST] = new TGCDatabaseASDToPP(filename, "EST"); 

  m_commonDb[TGCIdBase::Forward][TGCIdBase::WI] = new TGCDatabaseASDToPP(filename, "FWI"); 
  m_commonDb[TGCIdBase::Forward][TGCIdBase::SI] = new TGCDatabaseASDToPP(filename, "FSI"); 
  m_commonDb[TGCIdBase::Endcap][TGCIdBase::WI] = new TGCDatabaseASDToPP(filename, "EWI"); 
  m_commonDb[TGCIdBase::Endcap][TGCIdBase::SI] = new TGCDatabaseASDToPP(filename, "ESI"); 
  
  for(int side=0; side < TGCIdBase::MaxSideType; side++) { 
    for(int sector=0; sector < TGCId::NumberOfForwardSector; sector++) { 
      m_FWDdb[side][sector] = m_commonDb[TGCIdBase::Forward][TGCIdBase::WD];  
      m_FSDdb[side][sector] = m_commonDb[TGCIdBase::Forward][TGCIdBase::SD]; 
      m_FWTdb[side][sector] = m_commonDb[TGCIdBase::Forward][TGCIdBase::WT]; 
      m_FSTdb[side][sector] = m_commonDb[TGCIdBase::Forward][TGCIdBase::ST];  
    } 
    for(int sector=0; sector < TGCId::NumberOfEndcapSector; sector++) { 
      m_EWDdb[side][sector] = m_commonDb[TGCIdBase::Endcap][TGCIdBase::WD]; 
      m_ESDdb[side][sector] = m_commonDb[TGCIdBase::Endcap][TGCIdBase::SD];  
      m_EWTdb[side][sector] = m_commonDb[TGCIdBase::Endcap][TGCIdBase::WT];  
      m_ESTdb[side][sector] = m_commonDb[TGCIdBase::Endcap][TGCIdBase::ST];  
    } 
    for(int sector=0; sector < TGCId::NumberOfInnerSector; sector++) { 
      m_FWIdb[side][sector] = m_commonDb[TGCIdBase::Forward][TGCIdBase::WI];  
      m_FSIdb[side][sector] = m_commonDb[TGCIdBase::Forward][TGCIdBase::SI];  
      m_EWIdb[side][sector] = m_commonDb[TGCIdBase::Endcap][TGCIdBase::WI]; 
      m_ESIdb[side][sector] = m_commonDb[TGCIdBase::Endcap][TGCIdBase::SI]; 
    } 
  } 
}


StatusCode TGCCableASDToPP::updateDatabase()
{
  if(m_tgcCablingDbTool.retrieve().isFailure()) return StatusCode::FAILURE;
  std::vector<std::string> *tmp_ASD2PP_DIFF_12 = m_tgcCablingDbTool->giveASD2PP_DIFF_12();
  if(!tmp_ASD2PP_DIFF_12) return StatusCode::FAILURE;

  delete m_ASD2PP_DIFF_12;
  m_ASD2PP_DIFF_12 = new std::vector<std::string>;

  // Truncation saves initialization CPU time of about 30 ms. 
  for(std::vector<std::string>::const_iterator it = tmp_ASD2PP_DIFF_12->begin();
      it != tmp_ASD2PP_DIFF_12->end(); 
      it++) {
    if((*it).substr(0,1)=="/"||(*it).substr(0,1)=="*") continue; 
    m_ASD2PP_DIFF_12->push_back(*it);
  }
  delete tmp_ASD2PP_DIFF_12;
  tmp_ASD2PP_DIFF_12 = 0;

  for(int side=0; side<TGCIdBase::MaxSideType; side++) { 
    for(int sector=0; sector<TGCId::NumberOfForwardSector; sector++) { 
      StatusCode sc = updateIndividualDatabase(side, sector, "FWD", m_FWDdb[side][sector]);
      if(!sc.isSuccess()) return sc; 
      sc = updateIndividualDatabase(side, sector, "FSD", m_FSDdb[side][sector]); 
      if(!sc.isSuccess()) return sc; 
      sc = updateIndividualDatabase(side, sector, "FWT", m_FWTdb[side][sector]); 
      if(!sc.isSuccess()) return sc; 
      sc = updateIndividualDatabase(side, sector, "FST", m_FSTdb[side][sector]); 
      if(!sc.isSuccess()) return sc; 
    } 
    for(int sector=0; sector<TGCId::NumberOfEndcapSector; sector++) { 
      StatusCode sc = updateIndividualDatabase(side, sector, "EWD", m_EWDdb[side][sector]); 
      if(!sc.isSuccess()) return sc; 
      sc = updateIndividualDatabase(side, sector, "ESD", m_ESDdb[side][sector]); 
      if(!sc.isSuccess()) return sc; 
      sc = updateIndividualDatabase(side, sector, "EWT", m_EWTdb[side][sector]); 
      if(!sc.isSuccess()) return sc; 
      sc = updateIndividualDatabase(side, sector, "EST", m_ESTdb[side][sector]); 
      if(!sc.isSuccess()) return sc; 
    } 
    for(int sector=0; sector<TGCId::NumberOfInnerSector; sector++) { 
      StatusCode sc = updateIndividualDatabase(side, sector, "EWI", m_EWIdb[side][sector]); 
      if(!sc.isSuccess()) return sc; 
      sc = updateIndividualDatabase(side, sector, "ESI", m_ESIdb[side][sector]); 
      if(!sc.isSuccess()) return sc; 
      sc = updateIndividualDatabase(side, sector, "FWI", m_FWIdb[side][sector]); 
      if(!sc.isSuccess()) return sc; 
      sc = updateIndividualDatabase(side, sector, "FSI", m_FSIdb[side][sector]); 
      if(!sc.isSuccess()) return sc; 
    } 
  } 

  return StatusCode::SUCCESS;
}

StatusCode TGCCableASDToPP::getUpdateInfo(const int side,  
					  const int sector,  
					  const std::string blockname, 
					  std::vector<std::vector<int> >& info) 
{
  // clear info
  info.clear();
  
  std::vector<std::string>::const_iterator it   = m_ASD2PP_DIFF_12->begin();
  std::vector<std::string>::const_iterator it_e = m_ASD2PP_DIFF_12->end();
  std::string buf;
  int size = 0;

  // search block name
  while(it!=it_e) {
    buf = (*it);
    it++;

    if(buf.substr(0,1)=="/"||buf.substr(0,1)=="*") continue;
    if(buf.substr(0,blockname.size())==blockname) {
      std::istringstream line(buf);
      std::string temp;
      line >> temp >> size;
      break;
    }
  }
  
  // loop over entries of specified block
  while(it!=it_e) {
    buf = (*it);
    it++;

    if(buf.substr(0,1)=="/"||buf.substr(0,1)=="*") continue;
    if(buf.substr(0,1)=="E"||buf.substr(0,1)=="F") break;
    std::istringstream line(buf);
    std::vector<int> entry;
    int  t_side, t_sector;
    line >> t_side;
    line >> t_sector;
    bool isOK = false;
    if((t_side==side) && (t_sector==sector)) { 
      for(int i=2; i<8; i++) {
	int temp=-1;
	if(line >> temp) { 
	  entry.push_back(temp);
	} else {
	  break;
	}
	isOK = (i==7);
      }
      if(isOK) {
	info.push_back(entry);
      }
    }
  }

  return StatusCode::SUCCESS;
}



TGCDatabase* TGCCableASDToPP::getDatabase(const int side, 
					  const int region, 
					  const int sector,
					  const int module) const
{
  if(side<0 || side>=TGCIdBase::MaxSideType) return 0;
  if(sector<0) return 0;

  TGCDatabase* db=0;
  if(region==TGCIdBase::Endcap) {
    switch(module) {
    case TGCIdBase::WD :
      if(sector<TGCId::NumberOfEndcapSector) db = m_EWDdb[side][sector];
      break;
    case TGCIdBase::SD :
      if(sector<TGCId::NumberOfEndcapSector) db = m_ESDdb[side][sector];
      break;
    case TGCIdBase::WT :
      if(sector<TGCId::NumberOfEndcapSector) db = m_EWTdb[side][sector];
      break;
    case TGCIdBase::ST :
      if(sector<TGCId::NumberOfEndcapSector) db = m_ESTdb[side][sector];
      break;
    case TGCIdBase::WI :
      if(sector<TGCId::NumberOfInnerSector) db = m_EWIdb[side][sector];
      break;
    case TGCIdBase::SI :
      if(sector<TGCId::NumberOfInnerSector) db = m_ESIdb[side][sector];
      break;
    default:
      break;
    }
  } else if(region==TGCIdBase::Forward) {
    switch(module) {
    case TGCIdBase::WD :
      if(sector<TGCId::NumberOfForwardSector) db = m_FWDdb[side][sector];
      break;
    case TGCIdBase::SD :
      if(sector<TGCId::NumberOfForwardSector) db = m_FSDdb[side][sector];
      break;
    case TGCIdBase::WT :
      if(sector<TGCId::NumberOfForwardSector) db = m_FWTdb[side][sector];
      break;
    case TGCIdBase::ST :
      if(sector<TGCId::NumberOfForwardSector) db = m_FSTdb[side][sector];
      break;
    case TGCIdBase::WI :
      if(sector<TGCId::NumberOfInnerSector) db = m_FWIdb[side][sector];
      break;
    case TGCIdBase::SI :
      if(sector<TGCId::NumberOfInnerSector) db = m_FSIdb[side][sector];
      break;
    default:
      break;
    }
  }
  return db;
}

// reverse layers in Forward sector
const int TGCCableASDToPP::s_stripForward[] = {2,1,0,4,3,6,5,8,7};


TGCChannelId* TGCCableASDToPP::getChannel(const TGCChannelId* channelId,
					  bool orChannel) const {
  if(channelId) {
    if(channelId->getChannelIdType()==TGCIdBase::ASDOut)
      return getChannelOut(channelId,orChannel);
    if(channelId->getChannelIdType()==TGCIdBase::PPIn)
      return getChannelIn(channelId,orChannel);
  }
  return 0;
}

TGCChannelId* TGCCableASDToPP::getChannelIn(const TGCChannelId* ppin,
					   bool orChannel) const {
  if(orChannel) return 0;
  if(ppin->isValid()==false) return 0;
  
  TGCDatabase* databaseP = 
    getDatabase(ppin->getSideType(),
		ppin->getRegionType(),
		ppin->getSector(),
		ppin->getModuleType());
  
  if(databaseP==0) return 0;

  int indexOut[TGCDatabaseASDToPP::NIndexOut] = 
    {ppin->getId(), ppin->getBlock(), ppin->getChannel()};
  int i = databaseP->getIndexDBOut(indexOut);
  if(i<0) return 0;

  // ASD2PP.db is Backward connection  
  int layer = databaseP->getEntry(i,0);
  if(ppin->isStrip()) {
    if(!(ppin->isBackward())) {
      layer = s_stripForward[layer];
    }
  }
  int offset = (ppin->isWire()) ? 4 : 0;
  int channel = databaseP->getEntry(i,2+offset);

  // Endcap Triplet chamberId start from 1 in ASDOut
  int chamber = databaseP->getEntry(i,1);
  if(ppin->isEndcap() && ppin->isTriplet()) {
    chamber = chamber+1;
  }
  
  TGCChannelASDOut *asdout = new TGCChannelASDOut(ppin->getSideType(),
						  ppin->getSignalType(),
						  ppin->getRegionType(),
						  ppin->getSector(),
						  layer,
						  chamber,
						  channel);
  
  return asdout;
}

TGCChannelId* TGCCableASDToPP::getChannelOut(const TGCChannelId* asdout,
					     bool orChannel) const {
  if(orChannel) return 0;
  if(asdout->isValid()==false) return 0;

  const bool asdoutisStrip = asdout->isStrip();
  const bool asdoutisBackward = asdout->isBackward();
  const bool asdoutisEndcap = asdout->isEndcap();
  const bool asdoutisTriplet = asdout->isTriplet();
  const int asdoutLayer = asdout->getLayer();
  const int asdoutChamber = asdout->getChamber();
  const int asdoutChannel = asdout->getChannel();

  TGCDatabase* databaseP =
    getDatabase(asdout->getSideType(),
		asdout->getRegionType(),
		asdout->getSector(),
		asdout->getModuleType());

  if(databaseP==0) return 0;
  
  TGCChannelPPIn* ppin = 0;
  const int MaxEntry = databaseP->getMaxEntry();
  for(int i=0; i<MaxEntry; i++) {
    // ASD2PP.db is Backward connection
    int layer = asdoutLayer;
    if(asdoutisStrip) {
      if(!asdoutisBackward) {
	layer = s_stripForward[layer];
      }
    }
    
    int elecChannel = asdoutChannel;

    // Endcap Triplet chamberId start from 1 in ASDOut
    int chamber = asdoutChamber;
    if(asdoutisEndcap&&asdoutisTriplet)
      chamber = chamber-1;
    int offset = (asdout->isWire())? 4 : 0;  
    if(databaseP->getEntry(i,0)==layer&&
       databaseP->getEntry(i,1)==chamber&&
       databaseP->getEntry(i,2+offset)==elecChannel)
      {
	int id = databaseP->getEntry(i,3);
	int block = databaseP->getEntry(i,4);
	int channel = databaseP->getEntry(i,5);
	
	ppin = new TGCChannelPPIn(asdout->getSideType(),
				  asdout->getModuleType(),
				  asdout->getRegionType(),
				  asdout->getSector(),
				  id,
				  block,
				  channel);
	break;
      } 
  }
  return ppin;
}  

StatusCode TGCCableASDToPP::updateIndividualDatabase(const int side,  
						     const int sector, 
						     const std::string blockname, 
						     TGCDatabase*& database) { 
  TGCDatabaseASDToPP* db = dynamic_cast<TGCDatabaseASDToPP*>(database); 
  if(!db) return StatusCode::FAILURE; 
  
  std::vector<std::vector<int> > info; 
  
  StatusCode sc = getUpdateInfo(side, sector, blockname, info); 
  if(!sc.isSuccess()) return sc;
  
  size_t info_size = info.size(); 
  if(!info_size) return StatusCode::SUCCESS; 

  if(db->isCommon()) { 
    db = new TGCDatabaseASDToPP(*db, false); // false means this database is not commonly used.  
    if(!db) return StatusCode::FAILURE;

    database = db; 
  } 

  for(size_t i=0; i<info_size; i++) { 
    db->update(info[i]); 
  } 

  return StatusCode::SUCCESS;
} 

} //end of namespace
