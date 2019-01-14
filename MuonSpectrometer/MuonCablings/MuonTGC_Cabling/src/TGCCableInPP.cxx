/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCCableInPP.h"

#include "MuonTGC_Cabling/TGCDatabaseInPP.h" 
#include "MuonTGC_Cabling/TGCChannelPPIn.h"
#include "MuonTGC_Cabling/TGCChannelPPOut.h"
#include "MuonTGC_Cabling/TGCChannelSLBIn.h"

namespace MuonTGC_Cabling {

// Constructor & Destructor
TGCCableInPP::TGCCableInPP(std::string filename)
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
  
TGCCableInPP::~TGCCableInPP(void)
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


TGCChannelId* TGCCableInPP::getChannel(const TGCChannelId* channelId,
				       bool orChannel) const {
  if(channelId){
    if(channelId->getChannelIdType()==TGCIdBase::PPIn)
      return getChannelOut(channelId,orChannel);
    if(channelId->getChannelIdType()==TGCIdBase::PPOut)
      return getChannelIn(channelId,orChannel);
  }
  return 0;
}
  
TGCChannelId* TGCCableInPP::getChannelIn(const TGCChannelId* ppout,
					 bool orChannel) const {
  if(ppout->isValid()==false) return 0;
  
  TGCIdBase::ModuleType moduleType = ppout->getModuleType();
  
  int ndatabaseP = 1;
  TGCDatabase* databaseP[2];
  databaseP[0] = m_database[ppout->getRegionType()][moduleType];
  // EI/FI
  //  wire(TGCIdBase::WI) and strip(TGCIdBase::SI) of a chamber
  //  use the same SLB chip
  //  The SLB chip is treated as TGCIdBase::WI in TGCCableSLBToSSW.cxx
  if(moduleType==TGCIdBase::WI) {
    databaseP[1] = m_database[ppout->getRegionType()][TGCIdBase::SI];
    ndatabaseP = 2;
  }
  
  int id=-1, block=-1, channel=-1;
  bool found = false;
  
  for(int idatabaseP=0; idatabaseP<ndatabaseP; idatabaseP++) {
    // EI/FI
    //  wire(TGCIdBase::WI) and strip(TGCIdBase::SI) of a chamber
    //  use the same SLB chip
    //  The SLB chip is treated as TGCIdBase::WI in TGCCableSLBToSSW.cxx
    if(idatabaseP==1) {
      moduleType = TGCIdBase::SI;
    }

    int indexIn[TGCDatabaseInPP::NIndexIn] = 
      {ppout->getId(), ppout->getBlock(), ppout->getChannel()};
    int i = databaseP[idatabaseP]->getIndexDBIn(indexIn);
    if(i<0) continue;
    
    if(orChannel==false){
      // first channel
      id = databaseP[idatabaseP]->getEntry(i,3);
      block = databaseP[idatabaseP]->getEntry(i,4);
      channel = databaseP[idatabaseP]->getEntry(i,5);
      if(id==-1 && block==-1 && channel==-1) continue;
      found = true; 
      break;
    } else {
      // ored channel
      if(databaseP[idatabaseP]->getEntrySize(i)==9) {
	id = databaseP[idatabaseP]->getEntry(i,6);
	block = databaseP[idatabaseP]->getEntry(i,7);
	channel = databaseP[idatabaseP]->getEntry(i,8);
	found = true; 
      } 
    }
  } 
  
  if(!found) return 0;
  
  TGCChannelPPIn* ppin = 
    new TGCChannelPPIn(ppout->getSideType(),
		       moduleType,
		       ppout->getRegionType(),
		       ppout->getSector(),
		       id,
		       block,
		       channel);
  
  return ppin;
}

TGCChannelId* TGCCableInPP::getChannelOut(const TGCChannelId* ppin,
					  bool orChannel) const {
  if(ppin->isValid()==false) return 0;

  const int ppinChannel = ppin->getChannel();
  const int ppinBlock = ppin->getBlock();
  const int ppinId = ppin->getId();

  TGCDatabase* databaseP =
    m_database[ppin->getRegionType()][ppin->getModuleType()];

  TGCChannelPPOut* ppout = 0;
  const int MaxEntry = databaseP->getMaxEntry();
  for(int i=0; i<MaxEntry; i++){
    bool cond1 = (databaseP->getEntry(i,5)==ppinChannel)&& 
      (databaseP->getEntry(i,4)==ppinBlock)&&
      (databaseP->getEntry(i,3)==ppinId);
      
    bool cond2 = (databaseP->getEntrySize(i)==9)&&
      (databaseP->getEntry(i,8)==ppinChannel)&&
      (databaseP->getEntry(i,7)==ppinBlock)&&
      (databaseP->getEntry(i,6)==ppinId);
                 
    if(cond1 || cond2) {
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
       
      if((moduleType == TGCIdBase::SD)  &&
	 (ppin->getRegionType() == TGCIdBase::Endcap)){
	// Strips of Middle doublets are ORed to the adjacent chamber  
	adjacent = cond2;
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
