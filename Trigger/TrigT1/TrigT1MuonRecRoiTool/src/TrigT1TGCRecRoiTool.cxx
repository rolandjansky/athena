/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGCRecRoiTool.h"
#include "TGCcablingInterface/TGCCablingBase.h"
#include "TGCcablingInterface/ITGCcablingServerSvc.h"
#include "TGCcablingInterface/ITGCcablingSvc.h"
#include "TGCcablingInterface/TGCIdBase.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include <fstream>

namespace LVL1 {

  TrigT1TGCRecRoiTool::TrigT1TGCRecRoiTool( const std::string& type, const std::string& name, const IInterface* parent) : base_class(type,name,parent)
  {
  }
  
  TrigT1TGCRecRoiTool::~TrigT1TGCRecRoiTool() {
  }
  
  StatusCode TrigT1TGCRecRoiTool::initialize() {
    ATH_CHECK(m_DetectorManagerKey.initialize());
    ATH_CHECK( m_idHelperSvc.retrieve() );
    ServiceHandle<ITGCcablingServerSvc> tgcCabGet ("TGCcablingServerSvc", name());
    ATH_CHECK( tgcCabGet.retrieve() );
    ATH_CHECK( tgcCabGet->giveCabling(m_cabling) );
    m_isAtlas = tgcCabGet->isAtlas();
    if(m_useRun3Config){
      ATH_MSG_INFO("update to Run 3 bit mask");
      updateBitMask( Run3 );
    }
    return StatusCode::SUCCESS;
  }
 
  TrigT1MuonRecRoiData TrigT1TGCRecRoiTool::roiData( const unsigned int & roiWord)const
  {
    TrigT1MuonRecRoiData data;
    // get SLB parameters
    TGCIdBase tgcIdBase;
    unsigned int sectorID, roiNumber,r,phi;
    int wireSLBId, block;  
    
    if (! getSLBparameters(roiWord,tgcIdBase,sectorID,roiNumber,r,phi,wireSLBId,block)){
      ATH_MSG_ERROR( "TGCRecRoiSvc::reconstruct  : BAD roiWord. " 
                     << "Can not get SLBparameters "
                     << " roi word = " << std::hex << roiWord  );
      return data;
    }
    if (tgcIdBase.getSideType() == TGCIdBase::NoSideType) {
      ATH_MSG_ERROR( "TGCRecRoiSvc::reconstruct  : "
                     << "This roiWord is not of TGC" );
      return data;
    } 
    
    
    // get ASDout
    int offset = 4;
    std::unique_ptr<TGCIdBase> w_asdout =
      getWireASDOut(tgcIdBase,sectorID,wireSLBId,block,phi,offset);
    if (w_asdout == nullptr) {
      ATH_MSG_WARNING( "TGCRecRoiSvc::reconstruct  : "
		       << "Cannot get ASD out for Wire " );
      return data;
    }
    
    std::unique_ptr<TGCIdBase> s_asdout =
      getStripASDOut(tgcIdBase,sectorID,wireSLBId,block,phi,offset);
    if (s_asdout == nullptr) {
      ATH_MSG_WARNING( "TGCRecRoiSvc::reconstruct  : "
		       << "Cannot get ASD out for Strip " );
      return data;
    }
    
    // wire(eta)
    Identifier wireId;
    double w_eta,w_phi;
    getWireInfo(w_eta,w_phi,wireId,std::move(w_asdout));
    
    // strip(phi)
    Identifier stripId;
    double s_eta,s_phi;
    getStripInfo(s_eta,s_phi,stripId,std::move(s_asdout));
    
    ATH_MSG_DEBUG( "(eta,phi)=" <<" (" << w_eta <<","<< s_phi <<")" );
    ATH_MSG_DEBUG( "channelId_eta=" << wireId << "  channelId_phi=" << stripId );
    
    data.set_eta(w_eta);
    data.set_phi(s_phi);

    double etaMin, etaMax, phiMin, phiMax;
    RoIsize(roiWord,etaMin,etaMax,phiMin,phiMax);
    data.set_etaMin(etaMin);
    data.set_etaMax(etaMax);
    data.set_phiMin(phiMin);
    data.set_phiMax(phiMax);

    return data;
  }
  
  bool TrigT1TGCRecRoiTool::getSLBparameters(const unsigned int & roiWord,
					     TGCIdBase & tgcIdBase,
					     unsigned int & sectorID,
					     unsigned int & roiNumber,
					     unsigned int & r, 
					     unsigned int & phi,
					     int & wireSLBId, 
					     int & block ) const 
  {
    // init
    tgcIdBase.setSideType( TGCIdBase::NoSideType );
    tgcIdBase.setRegionType( TGCIdBase::NoRegionType );
    sectorID = roiNumber = r = phi = 9999;
    wireSLBId = block = 0;
    
    // sysID, subSysID
    unsigned int sysID    =  getBitMaskValue(&roiWord,SysIDMask());
    unsigned int subSysID =  getBitMaskValue(&roiWord,SubSysIDMask());
    
    // trigger sector ID in the roiWord
    unsigned int t_sectorID;
    
    //
    bool isOK = true;
    
    if (sysID==1) { // Forward
      tgcIdBase.setRegionType( TGCIdBase::Forward );
      // trigger sector ID
      t_sectorID  = getBitMaskValue(&roiWord,ForwardSectorIDMask());
      // roi number
      roiNumber = getBitMaskValue(&roiWord,ForwardRoIMask());
      
      // convert trigger sector ID to sector ID 
      if (m_patchForRoIWord) { 
	isOK = (t_sectorID<32) && (roiNumber<64); 
      } else {
	isOK = (t_sectorID<24) && (roiNumber<64); 
      } 
      
      tgcIdBase.setSideType( (subSysID==1) ? TGCIdBase::Aside : TGCIdBase::Cside );
      r         = getBitMaskValue(&roiWord,ForwardRMask());
      phi       = getBitMaskValue(&roiWord,ForwardPhiMask());
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
      tgcIdBase.setRegionType( TGCIdBase::Endcap );
      // trigger sector ID
      t_sectorID  = getBitMaskValue(&roiWord,EndcapSectorIDMask());
      // roi number
      roiNumber = getBitMaskValue(&roiWord,EndcapRoIMask());
      
      isOK = (t_sectorID<48) && (roiNumber<148); 
      
      tgcIdBase.setSideType( (subSysID==1) ? TGCIdBase::Aside : TGCIdBase::Cside );
      r         = getBitMaskValue(&roiWord,EndcapRMask());
      phi       = getBitMaskValue(&roiWord,EndcapPhiMask());
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
		    << " roi word = " << std::hex << roiWord 
		    << " sysID = " << std::dec << sysID );
      return false;
    }
    
    if (!isOK) {
      ATH_MSG_INFO( "Bad roiWord  " 
		    << " roi word = " << std::hex << roiWord 
		    << " sysID = " << std::dec << sysID
		    << " trigger sectorID = " << std::dec << t_sectorID
		    << " sectorID = " << std::dec << sectorID
		    << " roiNumber = " << std::dec << roiNumber );
      return false;
    } else {
      ATH_MSG_DEBUG( " roi word = " << std::hex << roiWord 
		     << " sysID = " << std::dec << sysID
		     << " trigger sectorID = " << std::dec << t_sectorID
		     << " sectorID = " << std::dec << sectorID
		     << " roiNumber = " << std::dec << roiNumber );
    }
    return true;
    
  }
  
  std::unique_ptr<TGCIdBase> TrigT1TGCRecRoiTool::getWireASDOut(TGCIdBase tgcIdBase,
								unsigned int sectorID,
								int wireSLBId,
								int block,
								int /* phi */,
								int offset) const
  {
    // wire
    TGCIdBase w_slbin(TGCIdBase::Channel);
    w_slbin.setChannelIdType(TGCIdBase::SLBIn);
    w_slbin.setSideType(tgcIdBase.getSideType());
    w_slbin.setModuleType(TGCIdBase::WD);
    w_slbin.setRegionType(tgcIdBase.getRegionType());
    w_slbin.setSector(sectorID);
    w_slbin.setId(wireSLBId);
    w_slbin.setChannel(41+offset+8*block);
    
    const TGCCablingBase* tgcCabling = m_cabling->getTGCCabling();
    std::unique_ptr<TGCIdBase> w_asdout( tgcCabling->getASDOutChannel (&w_slbin) );
    return w_asdout;
  }
  
  std::unique_ptr<TGCIdBase> TrigT1TGCRecRoiTool::getStripASDOut(TGCIdBase tgcIdBase,
								 unsigned int sectorID,
								 int wireSLBId,
								 int block,
								 int phi,
								 int wireOffset,
								 int stripOffset) const
  {
    
    // If stripOffset is not given, wireOffset is used as stripOffset
    if(stripOffset==DummyOffset) stripOffset = wireOffset;
    
    // wire
    TGCIdBase w_slbin(TGCIdBase::Channel);
    w_slbin.setChannelIdType(TGCIdBase::SLBIn);
    w_slbin.setSideType(tgcIdBase.getSideType());
    w_slbin.setModuleType(TGCIdBase::WD);
    w_slbin.setRegionType(tgcIdBase.getRegionType());
    w_slbin.setSector(sectorID);
    w_slbin.setId(wireSLBId);
    w_slbin.setChannel(41+wireOffset+8*block);
    
    const TGCCablingBase* tgcCabling = m_cabling->getTGCCabling();
    std::unique_ptr<TGCIdBase> w_asdout( tgcCabling->getASDOutChannel(&w_slbin) );

    // strip
    int stripSLBId = w_asdout->getChamber();
    
    TGCIdBase s_slbin(TGCIdBase::Channel);
    s_slbin.setChannelIdType(TGCIdBase::SLBIn);
    s_slbin.setSideType(tgcIdBase.getSideType());
    s_slbin.setModuleType(TGCIdBase::SD);
    s_slbin.setRegionType(tgcIdBase.getRegionType());
    s_slbin.setSector(sectorID);
    s_slbin.setId(stripSLBId);
    s_slbin.setChannel(41+stripOffset+8*phi);
    
    std::unique_ptr<TGCIdBase> s_asdout( tgcCabling->getASDOutChannel (&s_slbin) );
    return s_asdout;
  }
  
  void TrigT1TGCRecRoiTool::getWireInfo(double& w_eta, double& w_phi,
					Identifier & wireId,
					std::unique_ptr<TGCIdBase> w_asdout,
					EdgeType edge) const {
    
    // init
    wireId=Identifier(0);
    auto w_pos=Amg::Vector3D(0,0,0);
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
    
    SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey};
    const MuonGM::MuonDetectorManager* muonMgr = DetectorManagerHandle.cptr(); 
    if(muonMgr==nullptr){
      ATH_MSG_ERROR("Null pointer to the read MuonDetectorManager conditions object.");
      return;
    }
  
  
    const MuonGM::TgcReadoutElement* tgcwire
      = muonMgr->getTgcReadoutElement(m_idHelperSvc->tgcIdHelper().parentID(wireId));
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
    w_eta = w_pos.eta();
    w_phi = w_pos.phi();
  }
  
  void TrigT1TGCRecRoiTool::getStripInfo(double& s_eta,double& s_phi,
					 Identifier & stripId,
					 std::unique_ptr<TGCIdBase> s_asdout,
					 EdgeType edge) const {
    
    
    // init
    auto s_pos = Amg::Vector3D(0,0,0);
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

    SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey};
    const MuonGM::MuonDetectorManager* muonMgr = DetectorManagerHandle.cptr(); 
    if(muonMgr==nullptr){
      ATH_MSG_ERROR("Null pointer to the read MuonDetectorManager conditions object.");
      return;
    }
    
  
    const MuonGM::TgcReadoutElement* tgcstrip
      = muonMgr->getTgcReadoutElement(m_idHelperSvc->tgcIdHelper().parentID(stripId));
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
    s_eta = s_pos.eta();
    s_phi = s_pos.phi();
  }



  void TrigT1TGCRecRoiTool::RoIsize(const unsigned int & roiWord,
				    double & etaMin, double & etaMax, 
				    double & phiMin, double & phiMax) const 
  {
    // init
    etaMin=etaMax=phiMin=phiMax=0;
    // 
    TGCIdBase tgcIdBase;
    unsigned int sectorID, roiNumber,r,phi;
    int wireSLBId, block;    
    double w_eta,w_phi,s_eta,s_phi;
    Identifier wireId, stripId;
    // get wire/strip positions of both edges
    for(int i=0; i<2; i++) {
      // get edge, the range is from 1 to 8. One subsector consists of 8 (or less) wire channels and 8 strip channels
      int offset=1+i*7;
      if (! getSLBparameters(roiWord,tgcIdBase,sectorID,roiNumber,r,phi,wireSLBId,block)) {
	ATH_MSG_ERROR( "TGCRecRoiSvc::RoIsize  : "
		       << "BAD roiWord " );
	return;
      }
      
      if (tgcIdBase.getSideType() == TGCIdBase::NoSideType) {
	ATH_MSG_ERROR( "TGCRecRoiSvc::RoIsize  : "
		       << "This roiWord is not of TGC" );
	return;
      }
      
      int wireOffset = offset;
      if(tgcIdBase.getRegionType()==TGCIdBase::Endcap) {
	if(roiNumber<4) { // Outermost ROI (|eta|~1.04)
	  if(wireOffset==1) wireOffset = 3; // The largest R channel
	} else if(roiNumber>=144) { // Innermost ROI (|eta|~1.91)
	  if(wireOffset==8) wireOffset = 4; // The smallest R channel
	}
      } else if(tgcIdBase.getRegionType()==TGCIdBase::Forward) {
	if(roiNumber<4) { // Outermost ROI (|eta|~1.93)
	  if(wireOffset==1) wireOffset = 4; // The largest R channel
	} else if(roiNumber>=60) { // Innermost ROI (|eta|~2.40)
	  if(wireOffset==8) wireOffset = 5; // The smallest R channel
	}
      }
      
      std::unique_ptr<TGCIdBase> w_asdout =
	getWireASDOut(tgcIdBase,sectorID,wireSLBId,block,phi,wireOffset);
      
      std::unique_ptr<TGCIdBase> s_asdout =
	getStripASDOut(tgcIdBase,sectorID,wireSLBId,block,phi,wireOffset,offset);
      
      if (w_asdout!=nullptr) {
	getWireInfo(w_eta,w_phi,wireId,std::move(w_asdout), (i==0 ? UpperREdge : LowerREdge));
	if (i==0) etaMin=w_eta;
	else      etaMax=w_eta;
      }
      
      if (s_asdout!=nullptr) {
	EdgeType stripEdge = NonEdge;
	// Logic to know the relation between phi and strip channel directions
	if(!m_isAtlas) { // 8-fold
	  stripEdge = ((i==0) ^ (sectorID%2==0)) ? LowerPhiEdge : UpperPhiEdge;
	} else { // 12-fold
	  stripEdge = ((i==0) ^ ((s_asdout->isAside()) ^ (!(s_asdout->isBackward())))) ? LowerPhiEdge : UpperPhiEdge;
	}
	
	getStripInfo(s_eta,s_phi,stripId,std::move(s_asdout),stripEdge);
	if (i==0) phiMin=s_phi;
	else      phiMax=s_phi;
      }
    }
    
    if (etaMin>etaMax){
      // SWAP(etaMin, etaMax);
      double temp=etaMin; etaMin=etaMax; etaMax=temp;
    }
    if (phiMin>phiMax){
      // SWAP(phiMin, phiMax);
      double temp=phiMin; phiMin=phiMax; phiMax=temp;
    }
    
    if(phiMin<-3. && phiMax>3.) {
      phiMin += 2.*M_PI;
      // SWAP(phiMin, phiMax);
      double temp=phiMin; phiMin=phiMax; phiMax=temp;
    }
  }
  
  bool TrigT1TGCRecRoiTool::dumpRoiMap(const std::string& filename) const
  {
    const unsigned int maxSubsystem = 2;
    const unsigned int maxTriggerSectorEndcap = 48;
    const unsigned int maxTriggerSectorForward = 24;

    const unsigned int maxEndcapR = 37;
    const unsigned int maxForwardR = 16;

    const unsigned int maxEndcapPhi = 4;
    const unsigned int maxForwardPhi = 4;
    
    std::ofstream roi_map;
    roi_map.open(filename.c_str(), std::ios::out );
    if(!roi_map){
      ATH_MSG_WARNING( "Unable to open ROI_Mapping file!" );
    } else {
      roi_map <<"Endcap"<< std::endl;
      roi_map <<"# side     sector   roi      etaMin       etaMax       phiMin       phiMax"<< std::endl;
      roi_map <<"# ------------------------------------------------------------------------"<< std::endl;
      for(unsigned int side=0;side < maxSubsystem; side++){
	for(unsigned int sector=0;sector < maxTriggerSectorEndcap; sector++){
	  for (unsigned int r=0; r<maxEndcapR; r++){
	    for (unsigned int phi=0; phi<maxEndcapPhi; phi++){
	      int roi = (r<<2) + phi;
	      int sectorAddress = sector<<1;
	      sectorAddress |= 0x80;
	      sectorAddress |= (side==0)?(0x1):(0x0);
	      unsigned long int roiWord = (m_useRun3Config) ? (roi+(sectorAddress<<21)) : ((roi<<2)+(sectorAddress<<14));
	      auto data = roiData(roiWord);
	      roi_map << std::setw(8)  << side     << " "
		      << std::setw(8)  << sector   << " "
		      << std::setw(8)  << roi      << " "
		      << std::setw(12) << data.etaMin() << " "
		      << std::setw(12) << data.etaMax() << " "
		      << std::setw(12) << data.phiMin() << " "
		      << std::setw(12) << data.phiMax() << std::endl;
	    } 
	  }
	}    
      }
      roi_map <<"Forward"<< std::endl;
      roi_map <<"# side     sector   roi      etaMin       etaMax       phiMin       phiMax"<< std::endl;
      roi_map <<"# ------------------------------------------------------------------------"<< std::endl;
      for(unsigned int side=0;side < maxSubsystem; side++){
	for(unsigned int sector=0;sector < maxTriggerSectorForward; sector++){
	  for (unsigned int r=0; r<maxForwardR; r++){
	    for (unsigned int phi=0; phi<maxForwardPhi; phi++){
	      int roi = (r<<2) + phi;
	      int sectorAddress = sector<<1;
	      sectorAddress |= 0x80;
	      sectorAddress |= (side==0)?(0x1):(0x0);
	      unsigned long int roiWord = (m_useRun3Config) ? (roi+(sectorAddress<<21)) : ((roi<<2)+(sectorAddress<<14));
	      auto data = roiData(roiWord);
	      roi_map << std::setw(8)  << side     << " "
		      << std::setw(8)  << sector   << " "
		      << std::setw(8)  << roi      << " "
		      << std::setw(12) << data.etaMin() << " "
		      << std::setw(12) << data.etaMax() << " "
		      << std::setw(12) << data.phiMin() << " "
		      << std::setw(12) << data.phiMax() << std::endl;
	    }
	  }
	}
      }
      roi_map.close();
    }
    return true;
  }
  
  
}
