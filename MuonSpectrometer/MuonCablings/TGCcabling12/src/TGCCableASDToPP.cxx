/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCcabling12/TGCCableASDToPP.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace LVL1TGCCabling12 {

// Constructor & Destructor
TGCCableASDToPP::TGCCableASDToPP (std::string  filename)
  : TGCCable(TGCCable::ASDToPP)
{
  initialize(filename);
}

TGCCableASDToPP::~TGCCableASDToPP (void)
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

}


void TGCCableASDToPP::initialize(std::string& filename)
{
  // Common database pointers are initialized
  for(int region=0; region<TGCIdBase::MaxRegionType; region++) {
    for(int module=0; module<TGCIdBase::MaxModuleType; module++) {
      m_commonDb[region][module] = 0;
    }
  }

  // DEBUG
  //std::cerr << "TGCCableASDToPP::initialize" << std::endl; 

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
      FWDdb[side][sector] = m_commonDb[TGCIdBase::Forward][TGCIdBase::WD]; 
      FSDdb[side][sector] = m_commonDb[TGCIdBase::Forward][TGCIdBase::SD];
      FWTdb[side][sector] = m_commonDb[TGCIdBase::Forward][TGCIdBase::WT];
      FSTdb[side][sector] = m_commonDb[TGCIdBase::Forward][TGCIdBase::ST]; 
    }
    for(int sector=0; sector < TGCId::NumberOfEndcapSector; sector++) {
      EWDdb[side][sector] = m_commonDb[TGCIdBase::Endcap][TGCIdBase::WD];
      ESDdb[side][sector] = m_commonDb[TGCIdBase::Endcap][TGCIdBase::SD]; 
      EWTdb[side][sector] = m_commonDb[TGCIdBase::Endcap][TGCIdBase::WT]; 
      ESTdb[side][sector] = m_commonDb[TGCIdBase::Endcap][TGCIdBase::ST]; 
    }
    for(int sector=0; sector < TGCId::NumberOfInnerSector; sector++) {
      FWIdb[side][sector] = m_commonDb[TGCIdBase::Forward][TGCIdBase::WI]; 
      FSIdb[side][sector] = m_commonDb[TGCIdBase::Forward][TGCIdBase::SI]; 
      EWIdb[side][sector] = m_commonDb[TGCIdBase::Endcap][TGCIdBase::WI];
      ESIdb[side][sector] = m_commonDb[TGCIdBase::Endcap][TGCIdBase::SI];
    }
  }

}


void  TGCCableASDToPP::updateDatabase(std::string&  filename)
{
  for(int side=0; side<TGCIdBase::MaxSideType; side++) {
    for(int sector=0; sector<TGCId::NumberOfForwardSector; sector++) {
      updateIndividualDatabase(filename, side, sector, "FWD", FWDdb[side][sector]);
      updateIndividualDatabase(filename, side, sector, "FSD", FSDdb[side][sector]);
      updateIndividualDatabase(filename, side, sector, "FWT", FWTdb[side][sector]);
      updateIndividualDatabase(filename, side, sector, "FST", FSTdb[side][sector]);
    }
    for(int sector=0; sector<TGCId::NumberOfEndcapSector; sector++) {
      updateIndividualDatabase(filename, side, sector, "EWD", EWDdb[side][sector]);
      updateIndividualDatabase(filename, side, sector, "ESD", ESDdb[side][sector]);
      updateIndividualDatabase(filename, side, sector, "EWT", EWTdb[side][sector]);
      updateIndividualDatabase(filename, side, sector, "EST", ESTdb[side][sector]);
    }
    for (int sector=0; sector<TGCId::NumberOfInnerSector; sector++) {
      updateIndividualDatabase(filename, side, sector, "EWI", EWIdb[side][sector]);
      updateIndividualDatabase(filename, side, sector, "ESI", ESIdb[side][sector]);
      updateIndividualDatabase(filename, side, sector, "FWI", FWIdb[side][sector]);
      updateIndividualDatabase(filename, side, sector, "FSI", FSIdb[side][sector]);
    }
  }
}

void  TGCCableASDToPP::getUpdateInfo(const std::string filename,
				     const int side, 
				     const int sector, 
				     const std::string blockname,
				     std::vector<std::vector<int> >& info)
{
  // clear info
  info.clear();
  
  // 
  std::ifstream file(filename.c_str());
  std::string buf;
  int  size = 0;
  // search block name
  while(getline(file,buf)){
    if(buf.substr(0,1)=="/"||buf.substr(0,1)=="*") continue;
    if(buf.substr(0,blockname.size())==blockname) {
      std::istringstream line(buf);
      std::string temp;
      line >> temp >> size;
      break;
    }
  }
  
  // loop over entries of specified block
  while(getline(file,buf)){
    if(buf.substr(0,1)=="/"||buf.substr(0,1)=="*") continue;
    if(buf.substr(0,1)=="E"||buf.substr(0,1)=="F") break;
    std::istringstream line(buf);
    std::vector<int> entry;
    int  t_side, t_sector;
    line >> t_side;
    line >> t_sector;
    bool isOK = false;
    if ( (t_side == side) && (t_sector == sector) ) { 
      for(int i=2; i<8; i++){
	int temp=-1;
	if (line >> temp) { 
	  entry.push_back(temp);
	} else {
	  break;
	}
	isOK = (i == 7);
      }
      if ( isOK) {
	info.push_back(entry);
      }
    }
  }

  //
  file.close();
}



TGCDatabase*  TGCCableASDToPP::getDatabase(const int side, 
					   const int region, 
					   const int sector,
					   const int module) const
{
  if(side<0 || side>=TGCIdBase::MaxSideType) return 0;
  if(sector<0) return 0; 

  TGCDatabase* db=0;
  if (region == TGCIdBase::Endcap ){
    switch(module){
    case TGCIdBase::WD :
      if(sector<TGCId::NumberOfEndcapSector) db = EWDdb[side][sector];
      break;
    case TGCIdBase::SD :
      if(sector<TGCId::NumberOfEndcapSector) db = ESDdb[side][sector];
      break;
    case TGCIdBase::WT :
      if(sector<TGCId::NumberOfEndcapSector) db = EWTdb[side][sector];
      break;
    case TGCIdBase::ST :
      if(sector<TGCId::NumberOfEndcapSector) db = ESTdb[side][sector];
      break;
    case TGCIdBase::WI :
      if(sector<TGCId::NumberOfInnerSector) db = EWIdb[side][sector];
      break;
    case TGCIdBase::SI :
      if(sector<TGCId::NumberOfInnerSector) db = ESIdb[side][sector];
      break;
    default:
      break;
    }
  } else if (region == TGCIdBase::Forward ){
    switch(module){
    case TGCIdBase::WD :
      if(sector<TGCId::NumberOfForwardSector) db = FWDdb[side][sector];
      break;
    case TGCIdBase::SD :
      if(sector<TGCId::NumberOfForwardSector) db = FSDdb[side][sector];
      break;
    case TGCIdBase::WT :
      if(sector<TGCId::NumberOfForwardSector) db = FWTdb[side][sector];
      break;
    case TGCIdBase::ST :
      if(sector<TGCId::NumberOfForwardSector) db = FSTdb[side][sector];
      break;
    case TGCIdBase::WI :
      if(sector<TGCId::NumberOfInnerSector) db = FWIdb[side][sector];
      break;
    case TGCIdBase::SI :
      if(sector<TGCId::NumberOfInnerSector) db = FSIdb[side][sector];
      break;
    default:
      break;
    }
  }
  return db;
}

// reverse layers in Forward sector
const int TGCCableASDToPP::stripForward[] = {2,1,0,4,3,6,5,8,7};


TGCChannelId* TGCCableASDToPP::getChannel (const TGCChannelId* channelId,
					   bool orChannel) const {
  if(channelId){
    if(channelId->getChannelIdType()==TGCIdBase::ASDOut)
      return getChannelOut(channelId,orChannel);
    if(channelId->getChannelIdType()==TGCIdBase::PPIn)
      return getChannelIn(channelId,orChannel);
  }
  return 0;
}

TGCChannelId*TGCCableASDToPP::getChannelIn (const TGCChannelId* ppin,
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
      layer = stripForward[layer];
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

TGCChannelId* TGCCableASDToPP::getChannelOut (const TGCChannelId* asdout,
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
		asdout->getModuleType() );

  if (databaseP ==0) return 0;
  
  TGCChannelPPIn* ppin = 0;
  const int MaxEntry = databaseP->getMaxEntry();
  for(int i=0; i<MaxEntry; i++){
    // ASD2PP.db is Backward connection
    int layer = asdoutLayer;
    if( asdoutisStrip) {
      if ( !asdoutisBackward ) {
	layer = stripForward[layer];
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

void TGCCableASDToPP::updateIndividualDatabase(const std::string filename,
					       const int side, 
					       const int sector,
					       const std::string blockname,
					       TGCDatabase*& database) {
  TGCDatabaseASDToPP* db = dynamic_cast<TGCDatabaseASDToPP*>(database);
  if(!db) return;

  std::vector<std::vector<int> > info;

  getUpdateInfo(filename, side, sector, blockname, info);

  size_t info_size = info.size();
  if(!info_size) return;
  
  if(db->isCommon()) {
    db = new TGCDatabaseASDToPP(*db, false); // false means this database is not commonly used. 
    database = db;
  }
  for (size_t i=0; i<info_size; i++) {
    db->update(info[i]);
  }
}

} //end of namespace
