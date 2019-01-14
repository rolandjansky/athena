/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCCabling.h"

#include "GaudiKernel/StatusCode.h"

#include "MuonTGC_Cabling/TGCCableASDToPP.h"
#include "MuonTGC_Cabling/TGCCableHPBToSL.h"
#include "MuonTGC_Cabling/TGCCableInASD.h"
#include "MuonTGC_Cabling/TGCCableInPP.h"
#include "MuonTGC_Cabling/TGCCableInSLB.h"
#include "MuonTGC_Cabling/TGCCablePPToSLB.h"
#include "MuonTGC_Cabling/TGCCableSLBToHPB.h"
#include "MuonTGC_Cabling/TGCCableSLBToSSW.h"
#include "MuonTGC_Cabling/TGCCableSSWToROD.h"
#include "MuonTGC_Cabling/TGCChannelASDOut.h"
#include "MuonTGC_Cabling/TGCChannelHPBIn.h"
#include "MuonTGC_Cabling/TGCChannelSLBIn.h"
#include "MuonTGC_Cabling/TGCChannelSLBOut.h"
#include "MuonTGC_Cabling/TGCModuleROD.h"
#include "MuonTGC_Cabling/TGCModuleSLB.h"

namespace MuonTGC_Cabling {

// Constructor & Destructor
TGCCabling::TGCCabling (void)
  : TGCCablingBase(),
    m_cableInASD(0),
    m_cableASDToPP(0),
    m_cableInPP(0),
    m_cablePPToSLB(0),
    m_cableInSLB(0),
    m_cableSLBToHPB(0),
    m_cableHPBToSL(0),
    m_cableSLBToSSW(0),
    m_cableSSWToROD(0)
{
  // do nothing
}

TGCCabling::TGCCabling (const TGCCabling&)
  : TGCCablingBase(),
    m_cableInASD(0),
    m_cableASDToPP(0),
    m_cableInPP(0),
    m_cablePPToSLB(0),
    m_cableInSLB(0),
    m_cableSLBToHPB(0),
    m_cableHPBToSL(0),
    m_cableSLBToSSW(0),
    m_cableSSWToROD(0)
{
  // do nothing
}
 
TGCCabling& TGCCabling::operator= (const TGCCabling&)
{
  // do nothing
  return *this;
}


TGCCabling::TGCCabling(std::string filenameASDToPP,
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

TGCCabling::~TGCCabling(void) 
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


StatusCode TGCCabling::updateCableASDToPP()
{
  return m_cableASDToPP->updateDatabase(); 
} 

// virtual method  of TGCCabligBase   
// slbIn --> AsdOut
TGCIdBase* TGCCabling::getASDOutChannel(const TGCIdBase* in) const
{
  TGCChannelSLBIn slb_in(in->getSideType(),
			 in->getModuleType(),
			 in->getRegionType(),
			 in->getSector(),
			 in->getId(),
			 in->getChannel());
  return getChannel(&slb_in,
		    TGCIdBase::ASDOut); 
}



// readout ID -> SLB Module
const TGCModuleId* TGCCabling::getSLBFromReadout(TGCIdBase::SideType side,
						 int rodId,
						 int sswId,
						 int sbLoc) const 
{
  int indexFromReadoutWithoutChannel  
    = getIndexFromReadoutWithoutChannel(side, rodId, sswId, sbLoc); 
  std::map<int, TGCModuleId*>::iterator it 
    = m_slbModuleIdMap.find(indexFromReadoutWithoutChannel); 
  if(it!=m_slbModuleIdMap.end()) { 
    // Already seen this ReadoutID without channel.  
    // Stored pointer is returned.  
    return (*it).second; 
  } 
  
  // ROD Module
  int readoutSector = rodId -1 ; //rodID = 1..12
  TGCModuleROD rod(side,readoutSector);

  // SSW Module
  TGCModuleMap* sswMap =  getModule(&rod,TGCModuleId::SSW);
  if(!sswMap) {
    m_slbModuleIdMap.insert(std::pair<int, TGCModuleId*>(indexFromReadoutWithoutChannel, 0));
    return 0;
  }

  TGCModuleId* ssw = 0;
  bool found = false;
  const int sswMapsize = sswMap->size();
  for(int i=0; i<sswMapsize; i++){
    if((sswMap->moduleId(i))->getId()==sswId){
      ssw = sswMap->popModuleId(i);
      found = true;
      break;
    }
  }
  delete sswMap;
  if(!found || !ssw) {
    m_slbModuleIdMap.insert(std::pair<int, TGCModuleId*>(indexFromReadoutWithoutChannel, 0));
    return 0; // Do not need to delete ssw here.  
              // We can delete ssw but nothing will be done.  
  }

  // SLB Module
  TGCModuleMap* slbMap =  getModule(ssw,TGCModuleId::SLB);
  delete ssw;
  if(!slbMap) {
    m_slbModuleIdMap.insert(std::pair<int, TGCModuleId*>(indexFromReadoutWithoutChannel, 0));
    return 0;
  }

  TGCModuleSLB* slb=0; 
  found = false;
  const int slbMapsize = slbMap->size();
  for(int i=0; i<slbMapsize; i++){
    slb = dynamic_cast<TGCModuleSLB*>(slbMap->moduleId(i));
    if(slb && slb->getSBLoc()== sbLoc){
      found = true;
      slb = dynamic_cast<TGCModuleSLB*>(slbMap->popModuleId(i));
      break;
    }
  }
  delete slbMap;

  if(!found || !slb) {
    m_slbModuleIdMap.insert(std::pair<int, TGCModuleId*>(indexFromReadoutWithoutChannel, 0));
    return 0; // Do not delete slb here.  
  }
  
  m_slbModuleIdMap.insert(std::pair<int, TGCModuleId*>(indexFromReadoutWithoutChannel, slb));
  return slb;
}

// readout ID -> RxID
int TGCCabling::getRxIdFromReadout(TGCIdBase::SideType side,
				   int                 rodId,
				   int                 sswId,
				   int                 sbLoc) const
{
  int rxId = -1;

  // ROD Module
  int readoutSector = rodId -1 ; //rodID = 1..12
  TGCModuleROD rod(side,readoutSector);

  // SSW Module
  TGCModuleMap* sswMap = getModule(&rod,TGCModuleId::SSW);
  if(!sswMap) return rxId;

  TGCModuleId* ssw = 0;
  bool found = false;
  const int sswMapsize = sswMap->size();
  for(int i=0; i<sswMapsize; i++){
    if((sswMap->moduleId(i))->getId()==sswId){
      ssw = sswMap->popModuleId(i);
      found = true;
      break;
    }
  }
  delete sswMap;
  if(!found || !ssw) return rxId; // Do not need to delete ssw here.  
                                  // We can delete ssw but nothing will be done.  

  // SLB Module
  TGCModuleMap* slbMap =  getModule(ssw,TGCModuleId::SLB);
  delete ssw;
  if(!slbMap) return rxId;

  TGCModuleSLB*   slb=0; 
  found = false;
  const int slbMapsize = slbMap->size();
  for(int i=0; i<slbMapsize; i++){
    slb = dynamic_cast<TGCModuleSLB*>(slbMap->moduleId(i));
    if(slb && slb->getSBLoc()== sbLoc){
      rxId = slbMap->connector(i);
      break;
    }
  }
  delete slbMap;

  return rxId;
}

// SSW ID/Rx ID -> SLB Module
TGCModuleId* TGCCabling::getSLBFromRxId(TGCIdBase::SideType side,
					int rodId,
					int sswId,
					int rxId) const 
{
  bool found;

  // ROD Module
  int readoutSector = rodId -1 ; //rodID = 1..12
  TGCModuleROD rod(side,readoutSector);

  // SSW Module
  TGCModuleMap* sswMap =  getModule(&rod,TGCModuleId::SSW);
  if(!sswMap) return 0;

  TGCModuleId* ssw = 0;
  found = false;
  const int size = sswMap->size();
  for(int i=0; i<size; i++){
    if((sswMap->moduleId(i))->getId()==sswId){
      ssw = sswMap->popModuleId(i);
      found = true;
      break;
    }
  }
  delete sswMap;
  if(!found || !ssw) return 0; // Do not need to delete ssw here.
                               // We can delete ssw but nothing will be done.

  // SLB Module
  TGCModuleMap* slbMap =  getModule(ssw,TGCModuleId::SLB);
  delete ssw;
  if(!slbMap) return 0;

  TGCModuleSLB*   slb=0;  
  int ip = slbMap->find(rxId);
  if(ip <0 || ip >= slbMap->size()){
    delete slbMap; slbMap = 0;
    return 0;
  }
  slb =  dynamic_cast<TGCModuleSLB*>(slbMap->popModuleId(ip));
  delete slbMap;

  if(!slb) return 0; 
  
  return slb;
}
  

// SLB Module -> readout ID
bool TGCCabling::getReadoutFromSLB(const TGCModuleSLB* slb,
				   TGCIdBase::SideType & side,
				   int & rodId,
				   int & sswId,
				   int & sbLoc) const 
{
  // initialize
  side = TGCIdBase::NoSideType;
  rodId = -1;
  sswId = -1;
  sbLoc = -1;

  if(!slb) return false;
  
  // Fill side
  side = slb->getSideType();

  TGCModuleMap* sswMap = getModule(slb,TGCModuleId::SSW);

  if(!sswMap) return false;
  
  // SSW Module
  TGCModuleId* ssw = sswMap->popModuleId(0);
  delete sswMap;
  if(!ssw) return false;
 
    
  // Fill SSW ID
  sswId = ssw->getId();


  // Fill SBLoc
  sbLoc = slb->getSBLoc();


  if(sbLoc < 0) {
    TGCModuleMap* slbMap =  getModule(ssw,TGCModuleId::SLB);
    if(!slbMap){
      delete ssw; ssw = 0;
      return false;
    }
    
    TGCModuleSLB*   pSlb=0;
    //bool found = false;
    const int size = slbMap->size();
    for(int i=0; i<size; i++){
      pSlb = dynamic_cast<TGCModuleSLB*>(slbMap->moduleId(i));

      if(pSlb &&
	 slb->getRegionType() == pSlb->getRegionType() &&
	 slb->getSector() == pSlb->getSector()  &&
	 slb->getId() == pSlb->getId()){
	if(slb->getModuleType() == pSlb->getModuleType()) {
	  sbLoc = pSlb->getSBLoc();	
	  //found = true;
	  break;
	}
	// SI is connected to the SLB corrsponding WI 
	if((slb->getModuleType()==TGCIdBase::SI) && (pSlb->getModuleType()==TGCIdBase::WI)) {  
	  sbLoc = pSlb->getSBLoc();	
	  //found = true;
	  break;
	}
      }
    }
    delete slbMap;
    if(sbLoc < 0) {
      delete ssw;
      return false;
    }
  }  
    

  TGCModuleMap* rodMap = getModule(ssw,TGCModuleId::ROD);
  delete ssw;
  if(!rodMap) return false;

  // ROD Module
  TGCModuleId* rod = rodMap->popModuleId(0);
  delete rodMap;
  if(!rod) return false;

  // Fill ROD ID
  rodId = rod->getId();
  delete rod;

  return true;  
}


// coincidence channel -> readout channel
bool TGCCabling::getReadoutFromHighPtID(TGCIdBase::SideType side,
					 int rodId,
					 int & sswId,
					 int & sbLoc,
					 int & channel,
					 TGCIdBase::SignalType signal,
					 TGCIdBase::RegionType region,
					 int sectorInReadout,
					 int hpbId,
					 int block,
					 int hitId,
					 int pos,
					 TGCIdBase::ModuleType moduleType,
					 bool orChannel) const 
{
  // initialize
  sswId = -1;
  sbLoc = -1;
  channel = -1;

  // get sector number
  int readoutSector = (rodId -1); //rod ID = 1..12
  int sector = sectorInReadout;
  if(region==TGCIdBase::Forward) {
    sector += readoutSector*(TGCId::NumberOfForwardSector/TGCId::NumberOfReadoutSector);
  } else {
    sector += readoutSector*(TGCId::NumberOfEndcapSector/TGCId::NumberOfReadoutSector);
  }
    
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
  
  if(!slbin) return false;
  channel = slbin->getChannel();
  
  TGCModuleSLB* slb = dynamic_cast<TGCModuleSLB*>(slbin->getModule());
  delete slbin;
  if(!slb) return false;
  
  // SLB Module -> readout ID
  TGCIdBase::SideType sideType;
  int rodid;
  bool status = getReadoutFromSLB(slb,
				  sideType,
				  rodid,
				  sswId,
				  sbLoc);
  
  delete slb;
  
  return status;
}

// readout channel -> coincidence channel
bool TGCCabling::getHighPtIDFromReadout(TGCIdBase::SideType side,
					 int rodId,
					 int sswId,
					 int sbLoc,
					 int channel,
					 TGCIdBase::SignalType & signal,
					 TGCIdBase::RegionType & region,
					 int & sectorInReadout,
					 int & hpbId,
					 int & block,
					 int & hitId,
					 int & pos) const 
{
  signal = TGCIdBase::NoSignalType;
  region = TGCIdBase::NoRegionType;
  sectorInReadout = -1;
  hpbId  = -1;
  block  = -1;
  hitId  = -1;
  pos    = -1;

  const TGCModuleId* slb = getSLBFromReadout(side,
					     rodId,
					     sswId,
					     sbLoc);
  if(!slb) return 0;
  
  TGCChannelSLBIn slbin(slb->getSideType(),
                        slb->getModuleType(),
                        slb->getRegionType(),
                        slb->getSector(),
                        slb->getId(),
                        channel);
  
  TGCChannelId* hpbin = getChannel(&slbin,TGCIdBase::HPBIn,false); 
  if(!hpbin) return 0;
  if(!hpbin->isValid()) {
    delete hpbin;
    return 0;
  }
  signal = hpbin->getSignalType();
  region = hpbin->getRegionType();
  sectorInReadout = hpbin->getSectorInReadout();
  hpbId  = hpbin->getId();
  block  = hpbin->getBlock();
  pos    = hpbin->getChannel()%2;
  hitId  = (hpbin->getChannel() - pos)/2;
  
  delete hpbin;
  return true;
}


// coincidence channel -> readout channel
bool TGCCabling::getReadoutFromLowPtCoincidence(TGCIdBase::SideType side,
						int rodId,
						int sswId,
						int sbLoc,
						int & channel,
						int block,
						int pos,
						bool flag) const 
{
  bool orChannel = flag;    

  const TGCModuleId* slb = getSLBFromReadout(side,
					     rodId,
					     sswId,
					     sbLoc);
  if(!slb) return 0;

  TGCChannelSLBOut slbout(slb->getSideType(),
			  slb->getModuleType(),
			  slb->getRegionType(),
			  slb->getSector(),
			  slb->getId(),
			  block,
			  pos);

  TGCChannelId* slbin = getChannel(&slbout,TGCIdBase::SLBIn, orChannel); 
  
  if(!slbin) return false;
  
  channel = slbin->getChannel();
  
  delete slbin;
  
  return true;
}


// readout channel -> coincidence channel
bool TGCCabling::getLowPtCoincidenceFromReadout(TGCIdBase::SideType side,
						int rodId,
						int sswId,
						int sbLoc,
						int channel,
						int & block,
						int & pos,
						bool middle) const {
  
  const TGCModuleId* slb = getSLBFromReadout(side,
					     rodId,
					     sswId,
					     sbLoc);
  if(!slb) return 0;
  
  TGCChannelSLBIn slbin(slb->getSideType(),
			slb->getModuleType(),
			slb->getRegionType(),
			slb->getSector(),
			slb->getId(),
			channel);
  if(!slbin.isValid()) return false;
  
  TGCChannelId* slbout = getChannel(&slbin,TGCIdBase::SLBOut,middle); 
  if(!slbout) return false;
  
  block = slbout->getBlock();
  pos = slbout->getChannel();
  delete slbout;
  
  return true;
}


// readout channel -> chamber channel
TGCChannelId* TGCCabling::getASDOutFromReadout(TGCIdBase::SideType side,
					       int rodId,
					       int sswId,
					       int sbLoc,
					       int channel,
					       bool orChannel) const {
  const TGCModuleId* slb = getSLBFromReadout(side,
					     rodId,
					     sswId,
					     sbLoc);
  if(!slb) return 0;
  
  TGCChannelSLBIn slbin(slb->getSideType(),
			slb->getModuleType(),
			slb->getRegionType(),
			slb->getSector(),
			slb->getId(),
			channel);
  if(!slbin.isValid()) return 0;  
 
  return getChannel(&slbin,TGCIdBase::ASDOut,orChannel); 
}


// chamber channel -> readout channel
bool TGCCabling::getReadoutFromASDOut(const TGCChannelASDOut* asdout,
				      TGCIdBase::SideType & side,
				      int & rodId,
				      int & sswId,
				      int & sbLoc,
				      int & channel,
				      bool orChannel) const {
  // initialize
  side = TGCIdBase::NoSideType;
  rodId = -1;
  sswId = -1;
  sbLoc = -1;
  channel = -1;


  // SLBIn channel
  TGCChannelId* slbin = getChannel(asdout,TGCIdBase::SLBIn,orChannel);
  
  if(!slbin) return false;
  channel = slbin->getChannel();
  
  TGCModuleSLB* slb = dynamic_cast<TGCModuleSLB*>(slbin->getModule());
  delete slbin;
  if(!slb) return false;
  
  // SLB Module -> readout ID
  bool status = getReadoutFromSLB(slb,
				  side,
				  rodId,
				  sswId,
				  sbLoc);
  delete slb;
  
  return status;
}

TGCChannelId* TGCCabling::getChannel(const TGCChannelId* channelId,
				     TGCIdBase::ChannelIdType type,
				     bool orChannel) const {
  switch(channelId->getChannelIdType()){
  case TGCIdBase::ASDIn:
    if(type==TGCIdBase::ASDOut)
      return m_cableInASD->getChannel(channelId,orChannel);
    if(type==TGCIdBase::SLBIn){
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
  case TGCIdBase::ASDOut:
    if(type==TGCIdBase::ASDIn)
      return m_cableInASD->getChannel(channelId,orChannel);
    if(type==TGCIdBase::PPIn)
      return m_cableASDToPP->getChannel(channelId,orChannel);
    if(type==TGCIdBase::SLBIn){
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
  case TGCIdBase::PPIn:
    if(type==TGCIdBase::ASDOut)
      return m_cableASDToPP->getChannel(channelId,orChannel);
    if(type==TGCIdBase::PPOut)
      return m_cableInPP->getChannel(channelId,orChannel);
    break;
  case TGCIdBase::PPOut:
    if(type==TGCIdBase::PPIn)
      return m_cableInPP->getChannel(channelId,orChannel);
    if(type==TGCIdBase::SLBIn)
      return m_cablePPToSLB->getChannel(channelId,orChannel);
    break;
  case TGCIdBase::SLBIn:
    if(type==TGCIdBase::SLBOut)
      return m_cableInSLB->getChannel(channelId,orChannel);
    if(type==TGCIdBase::HPBIn){
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
    if(type==TGCIdBase::PPOut)
      return m_cablePPToSLB->getChannel(channelId,orChannel);
    if(type==TGCIdBase::ASDOut){
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
    if(type==TGCIdBase::ASDIn){
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
  case TGCIdBase::SLBOut:
    if(type==TGCIdBase::SLBIn)
      return m_cableInSLB->getChannel(channelId,orChannel);
    if(type==TGCIdBase::HPBIn){
      return m_cableSLBToHPB->getChannel(channelId,orChannel);
    }
    break;
  case TGCIdBase::HPBIn:
    if(type==TGCIdBase::SLBIn){
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
    if(type==TGCIdBase::SLBOut)
      return m_cableSLBToHPB->getChannel(channelId,orChannel);
    break;
  default:
    break;
  }
  return 0;
}

TGCModuleMap* TGCCabling::getModule(const TGCModuleId* moduleId,
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
