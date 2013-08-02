/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCcabling/TGCCableASDToPP.h"

namespace LVL1TGCCabling8 {

// Constructor & Destructor
TGCCableASDToPP::TGCCableASDToPP (std::string filename)
  : TGCCable(TGCCable::ASDToPP)
{
  database[TGCIdBase::Endcap][TGCIdBase::WD] = 
    new TGCDatabaseASDToPP(filename,"EWD");
  database[TGCIdBase::Endcap][TGCIdBase::WT] = 
    new TGCDatabaseASDToPP(filename,"EWT");
  database[TGCIdBase::Endcap][TGCIdBase::SD] =
    new TGCDatabaseASDToPP(filename,"ESD");
  database[TGCIdBase::Endcap][TGCIdBase::ST] =
    new TGCDatabaseASDToPP(filename,"EST");
  database[TGCIdBase::Endcap][TGCIdBase::WI] =
    new TGCDatabaseASDToPP(filename,"EWI");
  database[TGCIdBase::Endcap][TGCIdBase::SI] =
    new TGCDatabaseASDToPP(filename,"ESI");
  database[TGCIdBase::Forward][TGCIdBase::WD] = 
    new TGCDatabaseASDToPP(filename,"FWD");
  database[TGCIdBase::Forward][TGCIdBase::WT] =
    new TGCDatabaseASDToPP(filename,"FWT");
  database[TGCIdBase::Forward][TGCIdBase::SD] =
    new TGCDatabaseASDToPP(filename,"FSD");
  database[TGCIdBase::Forward][TGCIdBase::ST] = 
    new TGCDatabaseASDToPP(filename,"FST");
  database[TGCIdBase::Forward][TGCIdBase::WI] =
    new TGCDatabaseASDToPP(filename,"FWI");
  database[TGCIdBase::Forward][TGCIdBase::SI] = 
    new TGCDatabaseASDToPP(filename,"FSI");
}
  
TGCCableASDToPP::~TGCCableASDToPP (void)
{
  delete database[TGCIdBase::Endcap][TGCIdBase::WD];
  delete database[TGCIdBase::Endcap][TGCIdBase::WT];
  delete database[TGCIdBase::Endcap][TGCIdBase::SD];
  delete database[TGCIdBase::Endcap][TGCIdBase::ST];
  delete database[TGCIdBase::Endcap][TGCIdBase::WI];
  delete database[TGCIdBase::Endcap][TGCIdBase::SI];
  delete database[TGCIdBase::Forward][TGCIdBase::WD];
  delete database[TGCIdBase::Forward][TGCIdBase::WT];
  delete database[TGCIdBase::Forward][TGCIdBase::SD];
  delete database[TGCIdBase::Forward][TGCIdBase::ST];
  delete database[TGCIdBase::Forward][TGCIdBase::WI];
  delete database[TGCIdBase::Forward][TGCIdBase::SI];
}

// reverse layers in Forward sector
const int TGCCableASDToPP::stripForward[] = {2,1,0,4,3,6,5,8,7};
  
TGCChannelId* TGCCableASDToPP::getChannel (const TGCChannelId* channelId,
					   bool orChannel) const {
  if(channelId){
    if(channelId->getChannelIdType()==TGCChannelId::ASDOut)
      return getChannelOut(channelId,orChannel);
    if(channelId->getChannelIdType()==TGCChannelId::PPIn)
      return getChannelIn(channelId,orChannel);
  }
  return 0;
}

TGCChannelId* TGCCableASDToPP::getChannelIn (const TGCChannelId* ppin,
					     bool orChannel) const {
  if(orChannel) return 0;
  if(ppin->isValid()==false) return 0;

  int ppinSector = ppin->getSector();
  bool ppinisAside = ppin->isAside();
  bool isBackwardChamber = 
    ( ppinisAside && (ppinSector%2==0) ) ||
    (!ppinisAside && (ppinSector%2==1) );

  TGCDatabase* databaseP = 
    database[ppin->getRegionType()][ppin->getModuleType()];

  int indexOut[TGCDatabaseASDToPP::NIndexOut] = 
    {ppin->getId(), ppin->getBlock(), ppin->getChannel()};
  int i = databaseP->getIndexDBOut(indexOut);
  if(i<0) return 0;

  // ASD2PP.db is backward connection
  int layer = databaseP->getEntry(i,0);
  if(ppin->isStrip() && !isBackwardChamber){
    layer = stripForward[layer];
  } 

  int offset = (ppin->isWire()) ? 4 : 0;
  int channel = databaseP->getEntry(i,2+offset);

  // Endcap Triplet chamberId start from 1 in ASDOut
  int chamber = databaseP->getEntry(i,1);
  if(ppin->isEndcap() && ppin->isTriplet())
    chamber = chamber+1;

  TGCChannelASDOut* asdout = 
    new TGCChannelASDOut(ppin->getSideType(),
			 ppin->getSignalType(),
			 ppin->getRegionType(),
			 ppinSector,
			 layer,
			 chamber,
			 channel);

  return asdout;
}

TGCChannelId* TGCCableASDToPP::getChannelOut (const TGCChannelId* asdout,
					      bool orChannel) const {
  if(orChannel) return 0;
  if(asdout->isValid()==false) return 0;

  TGCDatabase* databaseP =
    database[asdout->getRegionType()][asdout->getModuleType()];
  
  TGCChannelPPIn* ppin = 0;
  int MaxEntry = databaseP->getMaxEntry();
  for(int i=0; i<MaxEntry; i++){
    // ASD2PP.db is backward connection
    bool isBackwardChamber = ( asdout->isAside() && (asdout->getSector()%2 == 0) ) ||
      (!asdout->isAside() && (asdout->getSector()%2 == 1) );
    int layer = asdout->getLayer();
    if(asdout->isStrip()&&!isBackwardChamber){
      layer = stripForward[layer];
    }

    int elecChannel = asdout->getChannel();

    // Endcap Triplet chamberId start from 1 in ASDOut
    int chamber = asdout->getChamber();
    if(asdout->isEndcap()&&asdout->isTriplet())
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

} //end of namespace
