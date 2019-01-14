/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCcabling/TGCCableInPP.h"
#include "TGCcabling/TGCChannelSLBIn.h"

namespace LVL1TGCCabling8 {

// Constructor & Destructor
TGCCableInPP::TGCCableInPP (std::string filename)
  : TGCCable(TGCCable::InPP)
{
  m_database[TGCIdBase::Endcap][TGCIdBase::WD] = new TGCDatabaseInPP(filename,"EWD");
  m_database[TGCIdBase::Endcap][TGCIdBase::WT] = new TGCDatabaseInPP(filename,"EWT");
  m_database[TGCIdBase::Endcap][TGCIdBase::SD] = new TGCDatabaseInPP(filename,"ESD");
  m_database[TGCIdBase::Endcap][TGCIdBase::ST] = new TGCDatabaseInPP(filename,"EST");
  m_database[TGCIdBase::Endcap][TGCIdBase::WI] = new TGCDatabaseInPP(filename,"EWI");
  m_database[TGCIdBase::Endcap][TGCIdBase::SI] = new TGCDatabaseInPP(filename,"ESI");
  m_database[TGCIdBase::Forward][TGCIdBase::WD] = new TGCDatabaseInPP(filename,"FWD");
  m_database[TGCIdBase::Forward][TGCIdBase::WT] = new TGCDatabaseInPP(filename,"FWT");
  m_database[TGCIdBase::Forward][TGCIdBase::SD] = new TGCDatabaseInPP(filename,"FSD");
  m_database[TGCIdBase::Forward][TGCIdBase::ST] = new TGCDatabaseInPP(filename,"FST");
  m_database[TGCIdBase::Forward][TGCIdBase::WI] = new TGCDatabaseInPP(filename,"FWI");
  m_database[TGCIdBase::Forward][TGCIdBase::SI] = new TGCDatabaseInPP(filename,"FSI");
}
  
TGCCableInPP::~TGCCableInPP (void)
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


TGCChannelId* TGCCableInPP::getChannel (const TGCChannelId* channelId,
					bool orChannel) const {
  if(channelId){
    if(channelId->getChannelIdType()==TGCChannelId::PPIn)
      return getChannelOut(channelId,orChannel);
    if(channelId->getChannelIdType()==TGCChannelId::PPOut)
      return getChannelIn(channelId,orChannel);
  }
  return 0;
}
  
TGCChannelId* TGCCableInPP::getChannelIn (const TGCChannelId* ppout,
					  bool orChannel) const {
  if(ppout->isValid()==false) return 0;

  TGCDatabase* databaseP = 
    m_database[ppout->getRegionType()][ppout->getModuleType()];
  
  int indexIn[TGCDatabaseInPP::NIndexIn] = 
    {ppout->getId(), ppout->getBlock(), ppout->getChannel()};
  int i = databaseP->getIndexDBIn(indexIn);
  if(i<0) return 0;

  int id, block, channel;
  bool found = false;
  
  if(orChannel==false){
    // first channel
    id = databaseP->getEntry(i,3);
    block = databaseP->getEntry(i,4);
    channel = databaseP->getEntry(i,5);
    
    if(id==-1&&block==-1&&channel==-1) return 0;

    found = true;
  } else {
    // ored channel
    if(databaseP->getEntrySize(i)==9) {
      id = databaseP->getEntry(i,6);
      block = databaseP->getEntry(i,7);
      channel = databaseP->getEntry(i,8);
      found = true;
    } 
  }

  if(!found) return 0;

  TGCChannelPPIn* ppin = 
    new TGCChannelPPIn(ppout->getSideType(),
		       ppout->getModuleType(),
		       ppout->getRegionType(),
		       ppout->getSector(), 
		       id,
		       block,
		       channel);

  return ppin;
}

TGCChannelId* TGCCableInPP::getChannelOut (const TGCChannelId* ppin,
					   bool orChannel) const {
  if(ppin->isValid()==false) return 0;

  TGCDatabase* databaseP =
    m_database[ppin->getRegionType()][ppin->getModuleType()];

  TGCChannelPPOut* ppout = 0;
  int MaxEntry = databaseP->getMaxEntry();
  for(int i=0; i<MaxEntry; i++){
    if((databaseP->getEntry(i,3)==ppin->getId()&&
        databaseP->getEntry(i,4)==ppin->getBlock()&&
        databaseP->getEntry(i,5)==ppin->getChannel())
       ||
       (databaseP->getEntrySize(i)==9&&
        databaseP->getEntry(i,6)==ppin->getId()&&
        databaseP->getEntry(i,7)==ppin->getBlock()&&
        databaseP->getEntry(i,8)==ppin->getChannel()))
    {
      int id = databaseP->getEntry(i,0);
      int block = databaseP->getEntry(i,1);
      int channel = databaseP->getEntry(i,2);

      //TGCChannelSLBIn::CellType cellType = TGCChannelSLBIn::NoCellType;
      int channelInSLB = -1;
      bool adjacent = false;
      TGCIdBase::ModuleType moduleType = ppin->getModuleType();
      if(block==0||block==2){//C,D
	int lengthOfC = TGCChannelSLBIn::getLengthOfSLB(moduleType,
						       TGCChannelSLBIn::CellC);
	int lengthOfD = TGCChannelSLBIn::getLengthOfSLB(moduleType,
						       TGCChannelSLBIn::CellD);
	if(channel<lengthOfD){
	  int adjacentOfD = TGCChannelSLBIn::getAdjacentOfSLB(moduleType,
						       TGCChannelSLBIn::CellD);
	  //cellType = TGCChannelSLBIn::CellD;
	  channelInSLB = channel;
	  if(channelInSLB<adjacentOfD||channelInSLB>=lengthOfD-adjacentOfD)
	    adjacent = true;
	} else {
	  int adjacentOfC = TGCChannelSLBIn::getAdjacentOfSLB(moduleType,
						       TGCChannelSLBIn::CellC);
	  //cellType = TGCChannelSLBIn::CellC;
	  channelInSLB = channel-lengthOfD;
	  if(channelInSLB<adjacentOfC||channelInSLB>=lengthOfC-adjacentOfC)
	    adjacent = true;
	}
      }
      if(block==1||block==3){//A,B
	int lengthOfA = TGCChannelSLBIn::getLengthOfSLB(moduleType,
						       TGCChannelSLBIn::CellA);
	int lengthOfB = TGCChannelSLBIn::getLengthOfSLB(moduleType,
						       TGCChannelSLBIn::CellB);
	if(channel<lengthOfB){
	  int adjacentOfB = TGCChannelSLBIn::getAdjacentOfSLB(moduleType,
						       TGCChannelSLBIn::CellB);
	  //cellType = TGCChannelSLBIn::CellB;
	  channelInSLB = channel;
	  if(channelInSLB<adjacentOfB||channelInSLB>=lengthOfB-adjacentOfB)
	    adjacent = true;
	} else {
	  int adjacentOfA = TGCChannelSLBIn::getAdjacentOfSLB(moduleType,
						       TGCChannelSLBIn::CellA);
	  //cellType = TGCChannelSLBIn::CellA;
	  channelInSLB = channel-lengthOfB;
	  if(channelInSLB<adjacentOfA||channelInSLB>=lengthOfA-adjacentOfA)
	    adjacent = true;
	}
      }
      
      if(adjacent==orChannel){
	ppout = new TGCChannelPPOut(ppin->getSideType(),
				    ppin->getModuleType(),
				    ppin->getRegionType(),
				    ppin->getSector(),
				    id,
				    block,
				    channel);
	break;
      }
    } 
  }
  
  return ppout;
}  

} //end of namespace
