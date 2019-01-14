/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCcabling/TGCCablePPToSLB.h"

namespace LVL1TGCCabling8 {

// Constructor & Destructor
TGCCablePPToSLB::TGCCablePPToSLB (std::string filename)
  : TGCCable(TGCCable::PPToSLB)
{
  m_database[TGCIdBase::Endcap][TGCIdBase::WT] = 
    new TGCDatabasePPToSL(filename,"PP EWT");
  m_database[TGCIdBase::Endcap][TGCIdBase::WD] =
    new TGCDatabasePPToSL(filename,"PP EWD");
  m_database[TGCIdBase::Endcap][TGCIdBase::ST] = 
    new TGCDatabasePPToSL(filename,"PP EST");
  m_database[TGCIdBase::Endcap][TGCIdBase::SD] = 
    new TGCDatabasePPToSL(filename,"PP ESD");
  m_database[TGCIdBase::Endcap][TGCIdBase::WI] = 
    new TGCDatabasePPToSL(filename,"PP EWI");
  m_database[TGCIdBase::Endcap][TGCIdBase::SI] = 
    new TGCDatabasePPToSL(filename,"PP ESI");
  m_database[TGCIdBase::Forward][TGCIdBase::WT] =
    new TGCDatabasePPToSL(filename,"PP FWT");
  m_database[TGCIdBase::Forward][TGCIdBase::WD] = 
    new TGCDatabasePPToSL(filename,"PP FWD");
  m_database[TGCIdBase::Forward][TGCIdBase::ST] = 
    new TGCDatabasePPToSL(filename,"PP FST");
  m_database[TGCIdBase::Forward][TGCIdBase::SD] = 
    new TGCDatabasePPToSL(filename,"PP FSD");
  m_database[TGCIdBase::Forward][TGCIdBase::WI] = 
    new TGCDatabasePPToSL(filename,"PP FWI");
  m_database[TGCIdBase::Forward][TGCIdBase::SI] = 
    new TGCDatabasePPToSL(filename,"PP FSI");
}

TGCCablePPToSLB::~TGCCablePPToSLB (void)
  {
    delete m_database[TGCIdBase::Endcap][TGCIdBase::WT];
    delete m_database[TGCIdBase::Endcap][TGCIdBase::WD];
    delete m_database[TGCIdBase::Endcap][TGCIdBase::ST];
    delete m_database[TGCIdBase::Endcap][TGCIdBase::SD];
    delete m_database[TGCIdBase::Endcap][TGCIdBase::WI];
    delete m_database[TGCIdBase::Endcap][TGCIdBase::SI];
    delete m_database[TGCIdBase::Forward][TGCIdBase::WT];
    delete m_database[TGCIdBase::Forward][TGCIdBase::WD];
    delete m_database[TGCIdBase::Forward][TGCIdBase::ST];
    delete m_database[TGCIdBase::Forward][TGCIdBase::SD];
    delete m_database[TGCIdBase::Forward][TGCIdBase::WI];
    delete m_database[TGCIdBase::Forward][TGCIdBase::SI];
  }
  

TGCChannelId* TGCCablePPToSLB::getChannel (const TGCChannelId* channelId,
					   bool orChannel) const {
  if(channelId){
    if(channelId->getChannelIdType()==TGCChannelId::PPOut)
      return getChannelOut(channelId,orChannel);
    if(channelId->getChannelIdType()==TGCChannelId::SLBIn)
      return getChannelIn(channelId,orChannel);
  }
  return 0;
}

TGCModuleMap* TGCCablePPToSLB::getModule (const TGCModuleId* moduleId) const {
  if(moduleId){
    if(moduleId->getModuleIdType()==TGCModuleId::PP)
      return getModuleOut(moduleId);
    if(moduleId->getModuleIdType()==TGCModuleId::SLB)
      return getModuleIn(moduleId);
  }
  return 0;
}

TGCChannelId* TGCCablePPToSLB::getChannelIn (const TGCChannelId* slbin,
					     bool orChannel) const {
  if(orChannel) return 0;
  if(slbin->isValid()==false) return 0;
  TGCChannelPPOut* ppout = 0;

  // SLB channel
  const TGCChannelSLBIn* slbIn = 0;
  if(slbin->getChannelIdType()==TGCChannelId::SLBIn){
    slbIn = dynamic_cast<const TGCChannelSLBIn*>(slbin);
  } 
  if(!slbIn) {
    return 0;
  }

  // SLB module
  TGCModuleId* slb = slbIn->getModule();
  if(!slb) return 0;

  // SLB -> PP module connection
  TGCModuleMap* mapId = getModule(slb);
  delete slb;
  if(!mapId) return 0;

  // PP module
  int port = mapId->connector(0);
  TGCModuleId* pp = mapId->popModuleId(0);
  delete mapId;
  if(!pp) return 0;

  // SLB ->PP channel connection
  int block = -1;
  int channel = -1;
  TGCChannelSLBIn::CellType cellType = slbIn->getCellType();
  if(cellType==TGCChannelSLBIn::NoCellType){
    delete pp; 
    return 0;
  }

  if(cellType==TGCChannelSLBIn::CellA) {
    block = 1+2*port;
    int lengthOfSLB = TGCChannelSLBIn::getLengthOfSLB(slbIn->getModuleType(),
						      TGCChannelSLBIn::CellB);
    channel = lengthOfSLB+slbIn->getChannelInSLB();
  }
  if(cellType==TGCChannelSLBIn::CellB) {
    block = 1+2*port;
    channel = slbIn->getChannelInSLB();
  }
  if(cellType==TGCChannelSLBIn::CellC) {
    block = 0+2*port;
    int lengthOfSLB = TGCChannelSLBIn::getLengthOfSLB(slbIn->getModuleType(),
						      TGCChannelSLBIn::CellD);
    channel = lengthOfSLB+slbIn->getChannelInSLB();
  }
  if(cellType==TGCChannelSLBIn::CellD) {
    block = 0+2*port;
    channel = slbIn->getChannelInSLB();
  }
  ppout = new TGCChannelPPOut(pp->getSideType(),
			      pp->getModuleType(),
			      pp->getRegionType(),
			      pp->getSector(),
			      pp->getId(),
			      block,
			      channel);
  
  delete pp;
  return ppout;
}


TGCChannelId* TGCCablePPToSLB::getChannelOut (const TGCChannelId* ppout,
					      bool orChannel) const {
  if(orChannel) return 0;
  if(ppout->isValid()==false) return 0;
  TGCChannelSLBIn* slbin = 0;
  
  // PP module
  TGCModuleId* pp = ppout->getModule();
  if(!pp) return 0;

  // PP -> SLB module connection
  TGCModuleMap* mapId = getModule(pp);
  delete pp;
  if(!mapId) return 0;
  
  // SLB module
  TGCModuleId* slb = 0;
  int size = mapId->size();
  for(int i=0;i<size;i++){
    if(mapId->connector(i)==ppout->getBlock()/2){
      slb = mapId->popModuleId(i);
      break;
    }
  }
  delete mapId;
  if(!slb) return 0;

  // PP ->SLB channel connection
  TGCChannelSLBIn::CellType cellType = TGCChannelSLBIn::NoCellType;
  int channelInSLB = -1;
  if(ppout->getBlock()%2==0){//D,C
    int lengthOfSLB = TGCChannelSLBIn::getLengthOfSLB(slb->getModuleType(),
						      TGCChannelSLBIn::CellD);
    if(ppout->getChannel()<lengthOfSLB){
      cellType = TGCChannelSLBIn::CellD;
      channelInSLB = ppout->getChannel();
    } else {
      cellType = TGCChannelSLBIn::CellC;
      channelInSLB = ppout->getChannel()-lengthOfSLB;
    }
  } else {//B,A
    int lengthOfSLB = TGCChannelSLBIn::getLengthOfSLB(slb->getModuleType(),
						      TGCChannelSLBIn::CellB);
    if(ppout->getChannel()<lengthOfSLB){
      cellType = TGCChannelSLBIn::CellB;
      channelInSLB = ppout->getChannel();
    } else {
      cellType = TGCChannelSLBIn::CellA;
      channelInSLB = ppout->getChannel()-lengthOfSLB;
    }
  }
  
  int channel = TGCChannelSLBIn::convertChannel(slb->getModuleType(),
						cellType,channelInSLB);
  slbin = new TGCChannelSLBIn(slb->getSideType(),
			      slb->getModuleType(),
			      slb->getRegionType(),
			      slb->getSector(),
			      slb->getId(),
			      channel);
  delete slb;

  return slbin;
}
  
TGCModuleMap* TGCCablePPToSLB::getModuleIn (const TGCModuleId* slb) const {
  if(slb->isValid()==false) return 0;

  TGCDatabase* databaseP =m_database[slb->getRegionType()][slb->getModuleType()];
  TGCModuleMap* mapId = 0;
  int MaxEntry = databaseP->getMaxEntry();
  for(int i=0; i<MaxEntry; i++){
    if(databaseP->getEntry(i,1)==slb->getId())
      {
	int id = databaseP->getEntry(i,0);
	TGCModulePP* pp = new TGCModulePP(slb->getSideType(),
					  slb->getModuleType(),
					  slb->getRegionType(),
					  slb->getSector(),
					  id);

	mapId = new TGCModuleMap();
	mapId->insert(0,pp);
	break;
      } 
    if(databaseP->getEntry(i,2)==slb->getId())
      {
	int id = databaseP->getEntry(i,0);
	TGCModulePP* pp = new TGCModulePP(slb->getSideType(),
					  slb->getModuleType(),
					  slb->getRegionType(),
					  slb->getSector(),
					  id);

	mapId = new TGCModuleMap();
	mapId->insert(1,pp);
	break;
      } 
  }
  
  return mapId;
}
  
TGCModuleMap* TGCCablePPToSLB::getModuleOut (const TGCModuleId* pp) const {
  if(pp->isValid()==false) return 0;

  TGCDatabase* databaseP = m_database[pp->getRegionType()][pp->getModuleType()];
  
  TGCModuleMap* mapId = 0;
  int MaxEntry = databaseP->getMaxEntry();
  for(int i=0; i<MaxEntry; i++){
    if(databaseP->getEntry(i,0)==pp->getId())
      {
	int id=-1;
	TGCModuleSLB* slb=0;
	mapId = new TGCModuleMap();

	id = databaseP->getEntry(i,1);
	if(id!=-1){
	  slb = new TGCModuleSLB(pp->getSideType(),
				 pp->getModuleType(),
				 pp->getRegionType(),
				 pp->getSector(),
				 id);
	  
	  mapId->insert(0,slb);
	}

	id = databaseP->getEntry(i,2);
	if(id!=-1){
	  slb = new TGCModuleSLB(pp->getSideType(),
				 pp->getModuleType(),
				 pp->getRegionType(),
				 pp->getSector(),
				 id);
	  
	  mapId->insert(1,slb);
	}
	break;
      } 
  }
  
  return mapId;
}

} //end of namespace
