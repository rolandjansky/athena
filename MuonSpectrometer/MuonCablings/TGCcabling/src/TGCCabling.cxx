/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCcabling/TGCCabling.h"

#include <iostream>

namespace LVL1TGCCabling8 {

// Constructor & Destructor
TGCCabling::TGCCabling (std::string filenameASDToPP,
			std::string filenameInPP,
			std::string filenamePPToSL,
			std::string filenameSLBToROD)
  : TGCCablingBase()
{
  m_cableInASD    = new TGCCableInASD(filenameASDToPP);
  m_cableASDToPP  = new TGCCableASDToPP(filenameASDToPP);
  m_cableInPP     = new TGCCableInPP(filenameInPP);
  m_cablePPToSLB  = new TGCCablePPToSLB(filenamePPToSL);
  m_cableInSLB    = new TGCCableInSLB();
  m_cableSLBToHPB = new TGCCableSLBToHPB(filenamePPToSL);
  m_cableHPBToSL  = new TGCCableHPBToSL(filenamePPToSL);
  m_cableSLBToSSW = new TGCCableSLBToSSW(filenameSLBToROD);
  m_cableSSWToROD = new TGCCableSSWToROD(filenameSLBToROD);
}

TGCCabling::~TGCCabling (void) 
{
  delete m_cableInASD;
  delete m_cableASDToPP;
  delete m_cableInPP;
  delete m_cablePPToSLB;
  delete m_cableInSLB;
  delete m_cableSLBToHPB;
  delete m_cableHPBToSL;
  delete m_cableSLBToSSW;
  delete m_cableSSWToROD;

  std::map<int, TGCModuleId*>::iterator it   = m_slbModuleIdMap.begin(); 
  std::map<int, TGCModuleId*>::iterator it_e = m_slbModuleIdMap.end(); 
  for(; it!=it_e; it++) delete ((*it).second); 
}

// virtual method  of TGCCabligBase   
// slbIn --> AsdOut
TGCIdBase* TGCCabling::getASDOutChannel(const TGCIdBase* in) const
{
  TGCChannelSLBIn slb_in( in->getSideType(),
                          in->getModuleType(),
                          in->getRegionType(),
                          in->getSector(),
                          in->getId(),
                          in->getChannel() );

  return getChannel (&slb_in,
                     TGCIdBase::ASDOut); 
}




// readout ID -> SLB Module
const TGCModuleId* TGCCabling::getSLBFromReadout (TGCIdBase::SideType side,
						  int rodId,
						  int sswId,
						  int slbId) const {
  int indexFromReadoutWithoutChannel  
    = getIndexFromReadoutWithoutChannel(side, rodId, sswId, slbId); 
  std::map<int, TGCModuleId*>::iterator it 
    = m_slbModuleIdMap.find(indexFromReadoutWithoutChannel); 
  if(it!=m_slbModuleIdMap.end()) { 
    // Already seen this ReadoutID without channel.  
    // Stored pointer is returned.  
    return (*it).second; 
  } 

  // ROD Module
  TGCModuleROD rod(side,rodId);

  // SSW Module
  TGCModuleMap* sswMap =  getModule(&rod,TGCModuleId::SSW);
  if(!sswMap) {
    m_slbModuleIdMap.insert(std::pair<int, TGCModuleId*>(indexFromReadoutWithoutChannel, 0));
    return 0;
  }

  TGCModuleId* ssw = 0;
  int size = sswMap->size();
  for(int i=0; i<size; i++){
    if((sswMap->moduleId(i))->getId()==sswId){
      ssw = sswMap->popModuleId(i);
      break;
    }
  }
  delete sswMap;
  if(!ssw) {
    m_slbModuleIdMap.insert(std::pair<int, TGCModuleId*>(indexFromReadoutWithoutChannel, 0));
    return 0;
  }

  // SLB Module
  TGCModuleMap* slbMap =  getModule(ssw,TGCModuleId::SLB);
  delete ssw;
  if(!slbMap) {
    m_slbModuleIdMap.insert(std::pair<int, TGCModuleId*>(indexFromReadoutWithoutChannel, 0));
    return 0;
  }

  TGCModuleId* slb = 0;
  int index = slbMap->find(slbId);
  if (index < slbMap->size()) {
    slb = slbMap->popModuleId(index);
  }
  delete slbMap;

  m_slbModuleIdMap.insert(std::pair<int, TGCModuleId*>(indexFromReadoutWithoutChannel, slb));
  return slb;
}
  

// SLB Module -> readout ID
bool TGCCabling::getReadoutFromSLB (const TGCModuleSLB* slb,
				    TGCIdBase::SideType & side,
				    int & rodId,
				    int & sswId,
				    int & slbId) const {
  // initialize
  side = TGCIdBase::NoSideType;
  rodId = -1;
  sswId = -1;
  slbId = -1;

  if(!slb) return false;
  
  // Fill side
  side = slb->getSideType();

  TGCModuleMap* sswMap = getModule(slb,TGCModuleId::SSW);
  if(!sswMap) return false;
  
  // SSW Module
  slbId = sswMap->connector(0);
  TGCModuleId* ssw = sswMap->popModuleId(0);
  delete sswMap;
  if(!ssw) return false;
    
  // Fill SSW ID
  sswId = ssw->getId();

  TGCModuleMap* rodMap = getModule(ssw,TGCModuleId::ROD);
  delete ssw;
  if(!rodMap) return false;

  // ROD Module
  TGCModuleId* rod = rodMap->popModuleId(0);
  delete rodMap;
  if(!rod) return false;

  // Fill ROD ID
  rodId = rod->getOctant();
  delete rod;

  return true;  
}


// coincidence channel -> readout channel
bool TGCCabling::getReadoutFromHighPtID(TGCIdBase::SideType side,
					 int rodId,
					 int & sswId,
					 int & slbId,
					 int & channel,
					 TGCIdBase::SignalType signal,
					 TGCIdBase::RegionType region,
					 int sectorInOctant,
					 int hpbId,
					 int block,
					 int hitId,
					 int pos,
					 TGCIdBase::ModuleType moduleType,
					 bool orChannel) const 
{
  // initialize
  sswId = -1;
  slbId = -1;
  channel = -1;
  
  int sector = (region==TGCIdBase::Forward)?
    3*rodId+sectorInOctant : 6*rodId+sectorInOctant;
  
  TGCChannelHPBIn hpbin(side,
			signal,
			region,
			sector,
			hpbId,
			block,
			hitId*2+pos);
  if(!hpbin.isValid()) return false;

  TGCChannelId* slbout = m_cableSLBToHPB->getChannelInforHPB(&hpbin,moduleType,false);
  if(!slbout) return 0;
  if(!slbout->isValid()){
    delete slbout;
    return 0;
  }
  TGCChannelId* slbin = m_cableInSLB->getChannel(slbout,orChannel);
  delete slbout;
  
  //TGCChannelId* slbin = getChannel(&hpbin,TGCChannelId::SLBInforHPB,false);
  if(!slbin) return false;
  channel = slbin->getChannel();
  
  TGCModuleSLB* slb = dynamic_cast<TGCModuleSLB*>(slbin->getModule());
  delete slbin;
  if(!slb) return false;
  
  // SLB Module -> readout ID
  TGCIdBase::SideType sideType;
  int rodid;
  bool status = getReadoutFromSLB (slb,
				   sideType,
				   rodid,
				   sswId,
				   slbId);
  
  delete slb;
  
  return status;
}

// readout channel -> coincidence channel
bool TGCCabling::getHighPtIDFromReadout(TGCIdBase::SideType side,
					 int rodId,
					 int sswId,
					 int slbId,
					 int channel,
					 TGCIdBase::SignalType & signal,
					 TGCIdBase::RegionType & region,
					 int & sectorInOctant,
					 int & hpbId,
					 int & block,
					 int & hitId,
					 int & pos) const 
{
  signal = TGCIdBase::NoSignalType;
  region = TGCIdBase::NoRegionType;
  sectorInOctant = -1;
  hpbId  = -1;
  block  = -1;
  hitId  = -1;
  pos    = -1;

  const TGCModuleId* slb = getSLBFromReadout (side,
					      rodId,
					      sswId,
					      slbId);
  if(!slb) return 0;
  
  TGCChannelSLBIn slbin(slb->getSideType(),
                        slb->getModuleType(),
                        slb->getRegionType(),
                        slb->getSector(),
                        slb->getId(),
                        channel);
  
  TGCChannelId* hpbin = getChannel(&slbin,TGCChannelId::HPBIn,false); 
  if(!hpbin) return 0;
  if(!hpbin->isValid()) {
    delete hpbin;
    return 0;
  }
  signal = hpbin->getSignalType();
  region = hpbin->getRegionType();
  sectorInOctant = hpbin->getSectorInOctant();
  hpbId  = hpbin->getId();
  block  = hpbin->getBlock();
  hitId  = hpbin->getChannel()/2;
  pos    = hpbin->getChannel()%2;
  
  delete hpbin;
  return true;
}


// coincidence channel -> readout channel
bool TGCCabling::getReadoutFromLowPtCoincidence(TGCIdBase::SideType side,
						int rodId,
						int sswId,
						int slbId,
						int & channel,
						int block,
						int pos,
						bool middle) const {

  const TGCModuleId* slb = getSLBFromReadout (side,
					      rodId,
					      sswId,
					      slbId);
  if(!slb) return 0;
  
  TGCChannelSLBOut slbout(slb->getSideType(),
			  slb->getModuleType(),
			  slb->getRegionType(),
			  slb->getSector(),
			  slb->getId(),
			  block,
			  pos);
  
  TGCChannelId* slbin = getChannel(&slbout,TGCChannelId::SLBIn,middle); 
  if(!slbin) return false;
  
  channel = slbin->getChannel();
  delete slbin;
  
  return true;
}


// readout channel -> coincidence channel
bool TGCCabling::getLowPtCoincidenceFromReadout(TGCIdBase::SideType side,
						int rodId,
						int sswId,
						int slbId,
						int channel,
						int & block,
						int & pos,
						bool middle) const {
  
  const TGCModuleId* slb = getSLBFromReadout (side,
					      rodId,
					      sswId,
					      slbId);
  if(!slb) return 0;
  
  TGCChannelSLBIn slbin(slb->getSideType(),
			slb->getModuleType(),
			slb->getRegionType(),
			slb->getSector(),
			slb->getId(),
			channel);
  
  TGCChannelId* slbout = getChannel(&slbin,TGCChannelId::SLBOut,middle); 
  if(!slbout) return false;
  
  block = slbout->getBlock();
  pos = slbout->getChannel();
  delete slbout;
  
  return true;
}


// readout channel -> chamber channel
TGCChannelId* TGCCabling::getASDOutFromReadout (TGCIdBase::SideType side,
						int rodId,
						int sswId,
						int slbId,
						int channel,
						bool orChannel) const {
  const TGCModuleId* slb = getSLBFromReadout (side,
					      rodId,
					      sswId,
					      slbId);
  if(!slb) return 0;
  
  TGCChannelSLBIn slbin(slb->getSideType(),
			slb->getModuleType(),
			slb->getRegionType(),
			slb->getSector(),
			slb->getId(),
			channel);
  
  return getChannel(&slbin,TGCChannelId::ASDOut,orChannel); 
}


// chamber channel -> readout channel
bool TGCCabling::getReadoutFromASDOut (const TGCChannelASDOut* asdout,
				       TGCIdBase::SideType & side,
				       int & rodId,
				       int & sswId,
				       int & slbId,
				       int & channel,
				       bool orChannel) const {
  // initialize
  side = TGCIdBase::NoSideType;
  rodId = -1;
  sswId = -1;
  slbId = -1;
  channel = -1;

  // SLBIn channel
  TGCChannelId* slbin = getChannel(asdout,TGCChannelId::SLBIn,orChannel);
  if(!slbin) return false;
  channel = slbin->getChannel();
  
  TGCModuleSLB* slb = dynamic_cast<TGCModuleSLB*>(slbin->getModule());
  delete slbin;
  if(!slb) return false;
  
  // SLB Module -> readout ID
  bool status = getReadoutFromSLB (slb,
				   side,
				   rodId,
				   sswId,
				   slbId);
  delete slb;
  
  return status;
}

TGCChannelId* TGCCabling::getChannel (const TGCChannelId* channelId,
				      TGCIdBase::ChannelIdType type,
				      bool orChannel) const {
  switch(channelId->getChannelIdType()){
  case TGCChannelId::ASDIn:
    if(type==TGCChannelId::ASDOut)
      return m_cableInASD->getChannel(channelId,orChannel);
    if(type==TGCChannelId::SLBIn){
      TGCChannelId* asdout = m_cableInASD->getChannel(channelId,false);
      if(!asdout) return 0;
      if(!asdout->isValid()){
	delete asdout;
	return 0;
      }
      TGCChannelId* ppin = m_cableASDToPP->getChannel(asdout,false);
      delete asdout;
      if(!ppin) return 0;
      if(!ppin->isValid()){
	delete ppin;
	return 0;
      }
      TGCChannelId* ppout = m_cableInPP->getChannel(ppin,orChannel);
      delete ppin;
      if(!ppout) return 0;
      if(!ppout->isValid()){
	delete ppout;
	return 0;
      }
      TGCChannelId* slbin = m_cablePPToSLB->getChannel(ppout,false);
      delete ppout;
      return slbin;
    } 
    break;
  case TGCChannelId::ASDOut:
    if(type==TGCChannelId::ASDIn)
      return m_cableInASD->getChannel(channelId,orChannel);
    if(type==TGCChannelId::PPIn)
      return m_cableASDToPP->getChannel(channelId,orChannel);
    if(type==TGCChannelId::SLBIn){
      TGCChannelId* ppin = m_cableASDToPP->getChannel(channelId,false);
      if(!ppin) return 0;
      if(!ppin->isValid()){
	delete ppin;
	return 0;
      }
      TGCChannelId* ppout = m_cableInPP->getChannel(ppin,orChannel);
      delete ppin;
      if(!ppout) return 0;
      if(!ppout->isValid()){
	delete ppout;
	return 0;
      }
      TGCChannelId* slbin = m_cablePPToSLB->getChannel(ppout,false);
      delete ppout;
      return slbin;
    } 
    break;
  case TGCChannelId::PPIn:
    if(type==TGCChannelId::ASDOut)
      return m_cableASDToPP->getChannel(channelId,orChannel);
    if(type==TGCChannelId::PPOut)
      return m_cableInPP->getChannel(channelId,orChannel);
    break;
  case TGCChannelId::PPOut:
    if(type==TGCChannelId::PPIn)
      return m_cableInPP->getChannel(channelId,orChannel);
    if(type==TGCChannelId::SLBIn)
      return m_cablePPToSLB->getChannel(channelId,orChannel);
    break;
  case TGCChannelId::SLBIn:
    if(type==TGCChannelId::HPBIn){
      TGCChannelId* slbout = m_cableInSLB->getChannel(channelId,orChannel);
      if(!slbout) return 0; 
      if(!slbout->isValid()) {
	delete slbout;
	return 0;
      } 
      TGCChannelId* hpbin = m_cableSLBToHPB->getChannel(slbout,false);
      delete slbout;
      return hpbin;
    }
    if(type==TGCChannelId::PPOut)
      return m_cablePPToSLB->getChannel(channelId,orChannel);
    if(type==TGCChannelId::SLBOut)
      return m_cableInSLB->getChannel(channelId,orChannel);
    if(type==TGCChannelId::ASDOut){
      TGCChannelId* ppout = m_cablePPToSLB->getChannel(channelId,false);
      if(!ppout) return 0;
      if(!ppout->isValid()){
	delete ppout;
	return 0;
      }
      TGCChannelId* ppin = m_cableInPP->getChannel(ppout,orChannel);
      delete ppout;
      if(!ppin) return 0;
      if(!ppin->isValid()){
	delete ppin;
	return 0;
      }
      TGCChannelId* asdout = m_cableASDToPP->getChannel(ppin,false);
      delete ppin;
      return asdout;
    } 
    if(type==TGCChannelId::ASDIn){
      TGCChannelId* ppout = m_cablePPToSLB->getChannel(channelId,false);
      if(!ppout) return 0;
      if(!ppout->isValid()){
	delete ppout;
	return 0;
      }
      TGCChannelId* ppin = m_cableInPP->getChannel(ppout,orChannel);
      delete ppout;
      if(!ppin) return 0;
      if(!ppin->isValid()){
	delete ppin;
	return 0;
      }
      TGCChannelId* asdout = m_cableASDToPP->getChannel(ppin,false);
      delete ppin;
      if(!asdout) return 0;
      if(!asdout->isValid()){
	delete asdout;
	return 0;
      }
      TGCChannelId* asdin = m_cableInASD->getChannel(asdout,false);
      delete asdout;
      return asdin;
    } 
    break;
  case TGCChannelId::SLBOut:
    if(type==TGCChannelId::SLBIn)
      return m_cableInSLB->getChannel(channelId,orChannel);
    if(type==TGCChannelId::HPBIn)
      return m_cableSLBToHPB->getChannel(channelId,orChannel);
    break;
  case TGCChannelId::HPBIn:
    if(type==TGCChannelId::SLBIn){
      TGCChannelId* slbout = m_cableSLBToHPB->getChannel(channelId,false);
      if(!slbout) return 0;
      if(!slbout->isValid()){
	delete slbout;
	return 0;
      }
      TGCChannelId* slbin = m_cableInSLB->getChannel(slbout,orChannel);
      delete slbout;
      return slbin;
    }
    if(type==TGCChannelId::SLBOut)
      return m_cableSLBToHPB->getChannel(channelId,orChannel);
    break;
  default:
    break;
  }
  return 0;
}

TGCModuleMap* TGCCabling::getModule (const TGCModuleId* moduleId,
				     TGCModuleId::ModuleIdType type) const {
  switch(moduleId->getModuleIdType()){
  case TGCModuleId::PP:
    if(type==TGCModuleId::SLB)
      return m_cablePPToSLB->getModule(moduleId);
    break;
  case TGCModuleId::SLB:
    if(type==TGCModuleId::PP)
      return m_cablePPToSLB->getModule(moduleId);
    if(type==TGCModuleId::HPB)
      return m_cableSLBToHPB->getModule(moduleId);
    if(type==TGCModuleId::SSW)
      return m_cableSLBToSSW->getModule(moduleId);
    break;
  case TGCModuleId::HPB:
    if(type==TGCModuleId::SLB)
      return m_cableSLBToHPB->getModule(moduleId);
    if(type==TGCModuleId::SL)
      return m_cableHPBToSL->getModule(moduleId);
    break;
  case TGCModuleId::SL:
    if(type==TGCModuleId::HPB)
      return m_cableHPBToSL->getModule(moduleId);
    break;
  case TGCModuleId::SSW:
    if(type==TGCModuleId::SLB)
      return m_cableSLBToSSW->getModule(moduleId);
    if(type==TGCModuleId::ROD)
      return m_cableSSWToROD->getModule(moduleId);
    break;
  case TGCModuleId::ROD:
    if(type==TGCModuleId::SSW)
      return m_cableSSWToROD->getModule(moduleId);
    break;
  default:
    break;
  }
  return 0;
} 

int TGCCabling::getIndexFromReadoutWithoutChannel(const TGCIdBase::SideType side, 
						  const int rodId, 
						  const int sswId, 
						  const int sbLoc) const { 
  return ((((side-TGCIdBase::Aside) 
	    *(MAXRODID-MINRODID+1) + rodId-MINRODID) 
	   *(MAXSSWID-MINSSWID+1) + sswId-MINSSWID) 
	  *(MAXSBLOC-MINSBLOC+1) + sbLoc-MINSBLOC); 
} 

} //end of namespace
