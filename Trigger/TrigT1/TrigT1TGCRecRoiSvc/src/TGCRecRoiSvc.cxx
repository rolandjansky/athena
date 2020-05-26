/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGCRecRoiSvc/TGCRecRoiSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "Identifier/Identifier.h"

#include "MuonIdHelpers/TgcIdHelper.h"

#include "TGCcablingInterface/TGCCablingBase.h"
#include "TGCcablingInterface/ITGCcablingServerSvc.h"
#include "TGCcablingInterface/ITGCcablingSvc.h"

using namespace LVL1TGC;

inline void SWAP(double &a, double &b) 
{
  double temp=a; a=b; b=temp;
}

TGCRecRoiSvc::TGCRecRoiSvc (const std::string& name, ISvcLocator* svc)
  : LVL1::RecMuonRoiSvc( name, svc ), 
    m_phi(0),
    m_eta(0),
    m_cabling(0),
    m_muonMgr(0),
    m_isAtlas(true)
{
  declareProperty("PatchForM5", m_patchForM5 = false);
  declareProperty("PatchForP4", m_patchForP4 = false);
  declareProperty("PatchForP5", m_patchForP5 = false);
  declareProperty("PatchForRoIWord", m_patchForRoIWord = false);
}

StatusCode TGCRecRoiSvc::initialize (void)
{
  ATH_MSG_DEBUG( "initialize" );

  ServiceHandle<StoreGateSvc> detStore ("DetectorStore", name() );
  ATH_CHECK( detStore.retrieve() );
  ATH_CHECK( detStore->retrieve(m_muonMgr) );
  
  ATH_CHECK( m_idHelperSvc.retrieve() );
  
  // try to initialize the TGCcabling
  StatusCode sc = getCabling();
  if(sc.isFailure()) {
    ATH_MSG_INFO( "TGCcablingServer not yet configured; postone TGCcabling initialization at first event. " );
  }
  
  if (m_patchForM5) {
    ATH_MSG_INFO( "PatchForM5 is obsolete. No effects on this service" );
  }
  if (m_patchForP5) {
    ATH_MSG_INFO( "PatchForP5 is obsolete. No effects on this service" );
  }
  if (m_patchForP4) {
    ATH_MSG_INFO( "PatchForP4 is obsolete. No effects on this service" );
  }

  return StatusCode::SUCCESS;
}  

void TGCRecRoiSvc::reconstruct (const unsigned int & roIWord) const 
{
  m_eta = 0;
  m_phi = 0;

  ATH_MSG_DEBUG( "reconstruct" );

  // get SLB parameters
  TGCIdBase::SideType sideType;
  TGCIdBase::RegionType regionType;
  unsigned int sectorID, roiNumber,r,phi;
  int wireSLBId, block;  

  if (! this->getSLBparameters(roIWord,sideType,regionType,sectorID,roiNumber,r,phi,wireSLBId,block)){
    ATH_MSG_ERROR( "TGCRecRoiSvc::reconstruct  : BAD roiWord. " 
                     << "Can not get SLBparameters "
                     << " roi word = " << std::hex << roIWord  );
    return;
  }
  if (sideType == TGCIdBase::NoSideType) {
    ATH_MSG_ERROR( "TGCRecRoiSvc::reconstruct  : "
                     << "This roiWord is not of TGC" );
    return;
  } 
  
  
  // get ASDout
  int offset = 4;
  TGCIdBase * w_asdout;
  this->getWireASDOut(w_asdout,sideType,regionType,sectorID,wireSLBId,block,phi,offset);
  if (w_asdout ==0) {
    ATH_MSG_WARNING( "TGCRecRoiSvc::reconstruct  : "
                     << "Cannot get ASD out for Wire " );
    return;
   }
  
  TGCIdBase * s_asdout;
  this->getStripASDOut(s_asdout,sideType,regionType,sectorID,wireSLBId,block,phi,offset);
  if (s_asdout ==0) {
    ATH_MSG_WARNING( "TGCRecRoiSvc::reconstruct  : "
                     << "Cannot get ASD out for Strip " );
    return;
  }

  // wire(eta)
  Identifier wireId;
  Amg::Vector3D w_pos;
  this->getWireInfo(w_pos,wireId,w_asdout);

  // strip(phi)
  Identifier stripId;
  Amg::Vector3D s_pos;
  this->getStripInfo(s_pos,stripId,s_asdout);

  m_eta = w_pos.eta();
  m_phi = s_pos.phi();
  m_channelId_eta = wireId;
  m_channelId_phi = stripId;
  
  ATH_MSG_DEBUG( "(eta,phi)=" <<" (" << m_eta <<","<< m_phi <<")" );
  ATH_MSG_DEBUG( "channelId_eta=" << m_channelId_eta << "  channelId_phi=" << m_channelId_phi );
   
  if (w_asdout) delete w_asdout;
  if (s_asdout) delete s_asdout;
  return;
}

void TGCRecRoiSvc::RoIsize(const unsigned int & roIWord,
			   double & etaMin, double & etaMax, 
			   double & phiMin, double & phiMax) const 
{
  // init
  etaMin=etaMax=phiMin=phiMax=0;
  // 
  TGCIdBase::SideType sideType;
  TGCIdBase::RegionType regionType;
  unsigned int sectorID, roiNumber,r,phi;
  int wireSLBId, block;    
  Amg::Vector3D w_pos, s_pos;
  Identifier wireId, stripId;
  // get wire/strip positions of both edges
  for(int i=0; i<2; i++) {
    // get edge, the range is from 1 to 8. One subsector consists of 8 (or less) wire channels and 8 strip channels
    int offset=1+i*7;
    if (! this->getSLBparameters(roIWord,sideType,regionType,sectorID,roiNumber,r,phi,wireSLBId,block)) {
      ATH_MSG_ERROR( "TGCRecRoiSvc::RoIsize  : "
                     << "BAD roiWord " );
      return;
    }

    if (sideType == TGCIdBase::NoSideType) {
      ATH_MSG_ERROR( "TGCRecRoiSvc::RoIsize  : "
                     << "This roiWord is not of TGC" );
      return;
    }

    int wireOffset = offset;
    if(regionType==TGCIdBase::Endcap) {
      if(roiNumber<4) { // Outermost ROI (|eta|~1.04)
	if(wireOffset==1) wireOffset = 3; // The largest R channel
      } else if(roiNumber>=144) { // Innermost ROI (|eta|~1.91)
	if(wireOffset==8) wireOffset = 4; // The smallest R channel
      }
    } else if(regionType==TGCIdBase::Forward) {
      if(roiNumber<4) { // Outermost ROI (|eta|~1.93)
	if(wireOffset==1) wireOffset = 4; // The largest R channel
      } else if(roiNumber>=60) { // Innermost ROI (|eta|~2.40)
	if(wireOffset==8) wireOffset = 5; // The smallest R channel
      }
    }

    TGCIdBase * w_asdout;
    this->getWireASDOut(w_asdout,sideType,regionType,sectorID,wireSLBId,block,phi,wireOffset);

    TGCIdBase * s_asdout;
    this->getStripASDOut(s_asdout,sideType,regionType,sectorID,wireSLBId,block,phi,wireOffset,offset);  

    if (w_asdout) {
      this->getWireInfo(w_pos,wireId,w_asdout, (i==0 ? UpperREdge : LowerREdge));
      if (i==0) etaMin=w_pos.eta();
      else      etaMax=w_pos.eta();
      delete w_asdout;
    }

    if (s_asdout) {
      EdgeType stripEdge = NonEdge;
      // Logic to know the relation between phi and strip channel directions
      if(!m_isAtlas) { // 8-fold
	stripEdge = ((i==0) ^ (sectorID%2==0)) ? LowerPhiEdge : UpperPhiEdge;
      } else { // 12-fold
	stripEdge = ((i==0) ^ ((s_asdout->isAside()) ^ (!(s_asdout->isBackward())))) ? LowerPhiEdge : UpperPhiEdge;
      }

      this->getStripInfo(s_pos,stripId,s_asdout,stripEdge);
      if (i==0) phiMin=s_pos.phi();
      else      phiMax=s_pos.phi();
      delete s_asdout;
    }
  }
  
  if (etaMin>etaMax) SWAP(etaMin, etaMax);
  if (phiMin>phiMax) SWAP(phiMin, phiMax);

  if(phiMin<-3. && phiMax>3.) {
    phiMin += 2.*M_PI;
    SWAP(phiMin, phiMax);
  }
}


void TGCRecRoiSvc::print(const unsigned int & roIWord) 
{
  TGCIdBase::SideType sideType;
  TGCIdBase::RegionType regionType;

  unsigned int sectorID, roiNumber,r,phi;
  int wireSLBId, block;  

  if( !this->getSLBparameters(roIWord,sideType,regionType,sectorID,roiNumber,r,phi,wireSLBId,block)){
    ATH_MSG_ERROR( " TGCRecRoiSvc::print() "
	   << " can not get SLBparameters "
	   << " roi word = " << std::hex << roIWord  );
       return;
  }


  if (sideType == TGCIdBase::NoSideType) {
    ATH_MSG_ERROR( " TGCRecRoiSvc::print() "
	<< "This roiWord is not of TGC" 
 	<< " roi word = " << std::hex << roIWord  );
 } else {
    ATH_MSG_DEBUG( " SideType  : " << (sideType==TGCIdBase::Aside ? "A-side" : "C-side") 
                   << " RegionType: " << (regionType==TGCIdBase::Endcap ? "Endcap" : "Foward") 
                   << " Sector id : " << sectorID 
                   << " RoI number: " << roiNumber 
                   << " Chamber R : " << r 
                   << " Phi       : " << phi );
  }
}

bool TGCRecRoiSvc::getSLBparameters(const unsigned int & roIWord,
				    TGCIdBase::SideType & sideType,
				    TGCIdBase::RegionType & regionType,
				    unsigned int & sectorID,
				    unsigned int & roiNumber,
				    unsigned int & r, 
				    unsigned int & phi,
				    int & wireSLBId, 
				    int & block ) const 
{
  // init
  sideType =  TGCIdBase::NoSideType;
  regionType= TGCIdBase::NoRegionType;
  sectorID = roiNumber = r = phi = 9999;
  wireSLBId = block = 0;
  
  // sectorAddress, sectorRoiOvl
  unsigned int sectorAddress = (roIWord & 0x003FC000) >> 14;
  unsigned int sectorRoIOvl  = (roIWord & 0x000007FC) >> 2;
  
  // sysID, subSysID
  unsigned int sysID    =  (sectorAddress & 0x000000C0) >> 6;
  unsigned int subSysID =  sectorAddress & 0x00000001;
  
  // trigger sector ID in the roIWord
  unsigned int t_sectorID;

  //
  bool isOK = true;

  if (sysID==1) { // Forward
    regionType= TGCIdBase::Forward;
    // trigger sector ID
    t_sectorID  = (sectorAddress & 0x0000003E) >> 1;
    // roi number
    roiNumber = sectorRoIOvl & 0x0000003F;
    
   // convert trigger sector ID to sector ID 
    if (m_patchForRoIWord) { 
      isOK = (t_sectorID<32) && (roiNumber<64); 
    } else {
      isOK = (t_sectorID<24) && (roiNumber<64); 
    } 

    sideType= (subSysID==1) ? TGCIdBase::Aside : TGCIdBase::Cside;
    r         = (roiNumber & 0x0000003C) >> 2;
    phi       = roiNumber & 0x00000003;
    wireSLBId = r/4;
    block     = r%4;
    // convert trigger sector ID to sector ID 
    if (m_patchForRoIWord) { 
      int sec = (t_sectorID + 2 )%32;
      int oct = sec/4;
      sectorID = 3*oct + sec%3;
    } else {
      sectorID = (t_sectorID )%24;
    } 
    
  } else if (sysID>1) {
    // Endcap
    regionType= TGCIdBase::Endcap;
    // trigger sector ID
    t_sectorID  = (sectorAddress & 0x0000007E) >> 1;
    // roi number
    roiNumber = sectorRoIOvl & 0x000000FF;

    isOK = (t_sectorID<48) && (roiNumber<148); 

    sideType= (subSysID==1) ? TGCIdBase::Aside : TGCIdBase::Cside;
    r         = (roiNumber & 0x000000FC) >> 2;
    phi       = roiNumber & 0x00000003;
    wireSLBId = (r+3)/4;
    block     = (r+3)%4;
    // convert trigger sector ID to sector ID 
    if (m_patchForRoIWord) { 
      sectorID = (t_sectorID + 2 )%48; 
    } else {
      sectorID = (t_sectorID )%48; 
    }

  } else {
    ATH_MSG_INFO( "Bad roiWord  " 
                  << " roi word = " << std::hex << roIWord 
                  << " sysID = " << std::dec << sysID );
    return false;
  }

  if (!isOK) {
    ATH_MSG_INFO( "Bad roiWord  " 
                  << " roi word = " << std::hex << roIWord 
                  << " sysID = " << std::dec << sysID
                  << " trigger sectorID = " << std::dec << t_sectorID
                  << " sectorID = " << std::dec << sectorID
                  << " roiNumber = " << std::dec << roiNumber );
    return false;
  } else {
    ATH_MSG_DEBUG( " roi word = " << std::hex << roIWord 
                   << " sysID = " << std::dec << sysID
                   << " trigger sectorID = " << std::dec << t_sectorID
                   << " sectorID = " << std::dec << sectorID
                   << " roiNumber = " << std::dec << roiNumber );
  }
  return true;

}

void TGCRecRoiSvc::getWireASDOut(TGCIdBase* & w_asdout,
				 TGCIdBase::SideType sideType,
				 TGCIdBase::RegionType regionType,
				 unsigned int sectorID,
				 int wireSLBId,
				 int block,
				 int /* phi */,
				 int offset) const 
{
  if(!m_cabling && getCabling().isFailure()) return;
  
  // init
  w_asdout = 0;
  
  // wire
  TGCIdBase w_slbin(TGCIdBase::Channel);
  w_slbin.setChannelIdType(TGCIdBase::SLBIn);
  w_slbin.setSideType(sideType);
  w_slbin.setModuleType(TGCIdBase::WD);
  w_slbin.setRegionType(regionType);
  w_slbin.setSector(sectorID);
  w_slbin.setId(wireSLBId);
  w_slbin.setChannel(41+offset+8*block);
  
  TGCCablingBase* tgcCabling = m_cabling->getTGCCabling();
  w_asdout = tgcCabling->getASDOutChannel (&w_slbin);
  if (w_asdout==0) return;
  if (w_asdout->isValid()==false) {
    delete w_asdout;
    w_asdout = 0;
    return;
  }
}

void TGCRecRoiSvc::getStripASDOut(TGCIdBase* & s_asdout,
				  TGCIdBase::SideType sideType,
				  TGCIdBase::RegionType regionType,
				  unsigned int sectorID,
				  int wireSLBId,
				  int block,
				  int phi,
				  int wireOffset,
				  int stripOffset) const 
{
  if(!m_cabling && getCabling().isFailure()) return;
  
  // If stripOffset is not given, wireOffset is used as stripOffset
  if(stripOffset==DummyOffset) stripOffset = wireOffset;

  // init
  s_asdout = 0;

  // wire
  TGCIdBase * w_asdout = 0;
  TGCIdBase w_slbin(TGCIdBase::Channel);
  w_slbin.setChannelIdType(TGCIdBase::SLBIn);
  w_slbin.setSideType(sideType);
  w_slbin.setModuleType(TGCIdBase::WD);
  w_slbin.setRegionType(regionType);
  w_slbin.setSector(sectorID);
  w_slbin.setId(wireSLBId);
  w_slbin.setChannel(41+wireOffset+8*block);

  TGCCablingBase* tgcCabling = m_cabling->getTGCCabling();
  w_asdout = tgcCabling->getASDOutChannel(&w_slbin);

  if (w_asdout==0) return;
  if (w_asdout->isValid()==false) {
    delete w_asdout;
    w_asdout =0;
    return;
  }
  
  // strip
  int stripSLBId = w_asdout->getChamber();
  delete w_asdout;
  w_asdout =0;

  TGCIdBase s_slbin(TGCIdBase::Channel);
  s_slbin.setChannelIdType(TGCIdBase::SLBIn);
  s_slbin.setSideType(sideType);
  s_slbin.setModuleType(TGCIdBase::SD);
  s_slbin.setRegionType(regionType);
  s_slbin.setSector(sectorID);
  s_slbin.setId(stripSLBId);
  s_slbin.setChannel(41+stripOffset+8*phi);

  s_asdout = tgcCabling->getASDOutChannel (&s_slbin);

  if(s_asdout==0) return;
  if(s_asdout->isValid()==false){
    delete s_asdout;
    s_asdout =0;
    return;  
  }  
}

void TGCRecRoiSvc::getWireInfo(Amg::Vector3D & w_pos,
			       Identifier & wireId,
			       TGCIdBase * w_asdout,
			       EdgeType edge) const {

  if(!m_cabling && getCabling().isFailure()) return;
  
  // init
  wireId=Identifier(0);
  w_pos=Amg::Vector3D(0,0,0);
  //
  int subsystemNumber = (w_asdout->isAside())? 1 : -1;
  int wireOrStrip = (w_asdout->isStrip())? 1 : 0;
  bool status = m_cabling->getOfflineIDfromOnlineID (wireId,
						     subsystemNumber,
						     w_asdout->getOctant(),
						     w_asdout->getSectorModule(),
						     w_asdout->getLayer(),
						     w_asdout->getChamber(),
						     wireOrStrip,
						     w_asdout->getChannel());
  if (status==false) return;
  const MuonGM::TgcReadoutElement* tgcwire
    = m_muonMgr->getTgcReadoutElement(m_idHelperSvc->tgcIdHelper().parentID(wireId));
  w_pos = tgcwire -> channelPos(wireId);

  // If edge correction is needed,
  // the half length in the r direction is added (subtracted) for the upper (lower) edge.
  if(edge==UpperREdge || edge==LowerREdge) {
    int gasGap = m_idHelperSvc->tgcIdHelper().gasGap(wireId);
    int channel = m_idHelperSvc->tgcIdHelper().channel(wireId);
    double halfLength = tgcwire->gangLength(gasGap, channel)/2.;
    double r = w_pos.perp();
    double phi = w_pos.phi();
    r += halfLength*(edge==UpperREdge ? +1. : -1.);
    w_pos[Trk::locX] = r*cos(phi);
    w_pos[Trk::locY] = r*sin(phi);
  }
}

void TGCRecRoiSvc::getStripInfo(Amg::Vector3D & s_pos,
				Identifier & stripId,
				TGCIdBase * s_asdout,
				EdgeType edge) const {

  if(!m_cabling && getCabling().isFailure()) return;

  // init
  s_pos = Amg::Vector3D(0,0,0);
  stripId=Identifier(0);
  //
  int subsystemNumber = (s_asdout->isAside())? 1 : -1;
  int wireOrStrip = (s_asdout->isStrip())? 1 : 0;
  bool status = m_cabling->getOfflineIDfromOnlineID (stripId,
						     subsystemNumber,
						     s_asdout->getOctant(),
						     s_asdout->getSectorModule(),
						     s_asdout->getLayer(),
						     s_asdout->getChamber(),
						     wireOrStrip,
						     s_asdout->getChannel());
  if (status==false) return;
  const MuonGM::TgcReadoutElement* tgcstrip
    = m_muonMgr->getTgcReadoutElement(m_idHelperSvc->tgcIdHelper().parentID(stripId));
  s_pos = tgcstrip -> channelPos(stripId); 

  // If edge correction is needed,
  // the half width in the phi direction is added (subtracted) for the upper (lower) edge.
  if(edge==UpperPhiEdge || edge==LowerPhiEdge) {
    int gasGap = m_idHelperSvc->tgcIdHelper().gasGap(stripId);
    int channel = m_idHelperSvc->tgcIdHelper().channel(stripId);
    double shortWidth = tgcstrip->stripShortWidth(gasGap, channel);
    double longWidth = tgcstrip->stripLongWidth(gasGap, channel);
    double halfWidth = (shortWidth + longWidth)/4.;
    double r = s_pos.perp();
    double phi = s_pos.phi();
    double dPhi = atan2(halfWidth, r);
    phi += dPhi*(edge==UpperPhiEdge ? +1. : -1.);
    s_pos[Trk::locX] = r*cos(phi);
    s_pos[Trk::locY] = r*sin(phi);
  }
}

StatusCode TGCRecRoiSvc::getCabling() const {
  ServiceHandle<ITGCcablingServerSvc> tgcCabGet ("TGCcablingServerSvc", name());
  ATH_CHECK( tgcCabGet.retrieve() );
  ATH_CHECK( tgcCabGet->giveCabling(*&m_cabling) );
  m_isAtlas = tgcCabGet->isAtlas();
  return StatusCode::SUCCESS;
}
