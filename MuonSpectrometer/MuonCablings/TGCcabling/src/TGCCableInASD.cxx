/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCcabling/TGCCableInASD.h"

namespace LVL1TGCCabling8 {

// Constructor & Destructor
TGCCableInASD::TGCCableInASD (std::string filename)
  : TGCCable(TGCCable::InASD)
{
  m_database[TGCIdBase::Endcap][TGCIdBase::WD] = 
    new TGCDatabaseASDToPP(filename,"EWD");
  m_database[TGCIdBase::Endcap][TGCIdBase::WT] = 
    new TGCDatabaseASDToPP(filename,"EWT");
  m_database[TGCIdBase::Endcap][TGCIdBase::SD] =
    new TGCDatabaseASDToPP(filename,"ESD");
  m_database[TGCIdBase::Endcap][TGCIdBase::ST] =
    new TGCDatabaseASDToPP(filename,"EST");
  m_database[TGCIdBase::Endcap][TGCIdBase::WI] =
    new TGCDatabaseASDToPP(filename,"EWI");
  m_database[TGCIdBase::Endcap][TGCIdBase::SI] =
    new TGCDatabaseASDToPP(filename,"ESI");
  m_database[TGCIdBase::Forward][TGCIdBase::WD] = 
    new TGCDatabaseASDToPP(filename,"FWD");
  m_database[TGCIdBase::Forward][TGCIdBase::WT] =
    new TGCDatabaseASDToPP(filename,"FWT");
  m_database[TGCIdBase::Forward][TGCIdBase::SD] =
    new TGCDatabaseASDToPP(filename,"FSD");
  m_database[TGCIdBase::Forward][TGCIdBase::ST] = 
    new TGCDatabaseASDToPP(filename,"FST");
  m_database[TGCIdBase::Forward][TGCIdBase::WI] =
    new TGCDatabaseASDToPP(filename,"FWI");
  m_database[TGCIdBase::Forward][TGCIdBase::SI] = 
    new TGCDatabaseASDToPP(filename,"FSI");
}
  
TGCCableInASD::~TGCCableInASD (void)
{
  delete m_database[TGCIdBase::Endcap][TGCIdBase::WD];
  delete m_database[TGCIdBase::Endcap][TGCIdBase::WT];
  delete m_database[TGCIdBase::Endcap][TGCIdBase::SD];
  delete m_database[TGCIdBase::Endcap][TGCIdBase::ST];
  delete m_database[TGCIdBase::Endcap][TGCIdBase::WI];
  delete m_database[TGCIdBase::Endcap][TGCIdBase::SI];
  delete m_database[TGCIdBase::Forward][TGCIdBase::WD];
  delete m_database[TGCIdBase::Forward][TGCIdBase::WT];
  delete m_database[TGCIdBase::Forward][TGCIdBase::SD];
  delete m_database[TGCIdBase::Forward][TGCIdBase::ST];
  delete m_database[TGCIdBase::Forward][TGCIdBase::WI];
  delete m_database[TGCIdBase::Forward][TGCIdBase::SI];
}

TGCChannelId* TGCCableInASD::getChannel (const TGCChannelId* channelId,
					 bool orChannel) const {
  if(channelId){
    if(channelId->getChannelIdType()==TGCChannelId::ASDIn)
      return getChannelOut(channelId,orChannel);
    if(channelId->getChannelIdType()==TGCChannelId::ASDOut)
      return getChannelIn(channelId,orChannel);
  }
  return 0;
}

TGCChannelId*TGCCableInASD::getChannelIn (const TGCChannelId* asdout,
					  bool orChannel) const {
  if(orChannel) return 0;
  if(asdout->isValid()==false) return 0;

  TGCDatabase* databaseP = 
    m_database[asdout->getRegionType()][asdout->getModuleType()];
  
  TGCChannelASDIn* asdin = 0;

  // sector ASDIn [1..48, 1..24], ASDOut [0..47, 0..23]
  int sector;
  if(asdout->isEndcap()&&!asdout->isInner()){
    sector = asdout->getSector()-1;
    if(sector<=0) sector += 48;
  } else {
    sector = asdout->getSector();
    if(sector<=0) sector += 24;
  }

  // chamber ASDIn [1(F),1,2,3,4,5(E)], ASDOut [0(F),4,3,2,1,0(E)]
  int chamber;
  if(asdout->isEndcap()&&!asdout->isInner()){
    chamber = 5-asdout->getChamber();
  } else {
    chamber = asdout->getChamber()+1;
  }
  
  int channel=-1;
  // channel ASDIn [1..32(S),1..n(W chamber)], ASDOut [0..31(S),n..0(W sector)]
  if(asdout->isWire()){
    // Endcap Triplet chamberId start from 1 in ASDOut
    int dbChamber = asdout->getChamber();
    if(asdout->isEndcap()&&asdout->isTriplet()) dbChamber = dbChamber-1;
    int indexIn[TGCDatabaseASDToPP::NIndexIn] = 
      {asdout->getLayer(), dbChamber, asdout->getChannel()};
    int i = databaseP->getIndexDBIn(indexIn);
    if(i<0) return 0;

    channel = databaseP->getEntry(i,7)+1;

  } else {
    // definition of Backward Chamber is consistnet with 12.X.0 release
    bool isBackwardChamber = ( asdout->isAside() && (asdout->getSector()%2 == 0) ) ||
                             (!asdout->isAside() && (asdout->getSector()%2 == 1) );
    if( (isBackwardChamber && asdout->isAside() )||
        (!isBackwardChamber && asdout->isCside()) ){
      channel = 32-asdout->getChannel();
    } else {
      channel = asdout->getChannel()+1;
    }
  }
  if(channel==-1) return 0;

  asdin = new TGCChannelASDIn(asdout->getSideType(),
			      asdout->getSignalType(),
			      asdout->getRegionType(),
			      sector,
			      asdout->getLayer(),
			      chamber,
			      channel);

  return asdin;
}

TGCChannelId* TGCCableInASD::getChannelOut (const TGCChannelId* asdin,
					    bool orChannel) const {
  if(orChannel) return 0;
  if(asdin->isValid()==false) return 0;

  TGCDatabase* databaseP =
    m_database[asdin->getRegionType()][asdin->getModuleType()];
  
  TGCChannelASDOut* asdout = 0;

  // sector ASDIn [1..48, 1..24], ASDOut [0..47, 0..23]
  int sector;
  if(asdin->isEndcap()&&!asdin->isInner()){
    sector = (asdin->getSector()+1)%48;
  } else {
    sector = asdin->getSector()%24;
  }
  
  // chamber ASDIn [1(F),1,2,3,4,5(E)], ASDOut [0(F),4,3,2,1,0(E)]
  int chamber;
  if(asdin->isEndcap()&&!asdin->isInner()){
    chamber = 5-asdin->getChamber();
  } else {
    chamber = asdin->getChamber()-1;
  }
  
  int channel=-1;
  // channel ASDIn [1..32(S),1..n(W chamber)], ASDOut [0..31(S),n..0(W sector)]
  if(asdin->isWire()){
    int MaxEntry = databaseP->getMaxEntry();
    for(int i=0; i<MaxEntry; i++){
      // Endcap Triplet chamberId start from 1 in ASDOut
      int dbChamber = chamber;
      if(asdin->isEndcap()&&asdin->isTriplet())
	dbChamber = dbChamber-1;
      
      int dbChannel = asdin->getChannel()-1;
      if(databaseP->getEntry(i,0)==asdin->getLayer()&&
	 databaseP->getEntry(i,1)==dbChamber&&
	 databaseP->getEntry(i,7)==dbChannel)
	{
	  channel = databaseP->getEntry(i,6);
	  break;
	}
    }
  } else {
    // definition of Backward Chamber is consistnet with 12.X.0 release
    bool isBackwardChamber;
    if (asdin->isEndcap() && !asdin->isInner()) {
      isBackwardChamber = ( asdin->isAside() && (asdin->getSector()%2 == 1) ) ||
                          ( asdin->isCside() && (asdin->getSector()%2 == 0) );
    } else {
     isBackwardChamber = ( asdin->isAside() && (asdin->getSector()%2 == 0) ) ||
                         ( asdin->isCside() && (asdin->getSector()%2 == 1) );
    }
    if( (isBackwardChamber && asdin->isAside()  )||
        (!isBackwardChamber && asdin->isCside() )   ){
        channel = 32-asdin->getChannel();
    } else {
        channel = asdin->getChannel()-1;
    }
  }
  if(channel==-1) return 0;

  asdout = new TGCChannelASDOut(asdin->getSideType(),
				asdin->getSignalType(),
				asdin->getRegionType(),
				sector,
				asdin->getLayer(),
				chamber,
				channel);

  return asdout;
}  


} //end of namespace
