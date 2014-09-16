/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/MdtDataPreparator.h"

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "MuonRDO/MdtCsmContainer.h"

#include "CLHEP/Units/PhysicalConstants.h"

#include "Identifier/IdentifierHash.h"
#include "MuonContainerManager/MuonRdoContainerAccess.h"

#include "TrigL2MuonSA/MdtRegionDefiner.h"

#include "xAODTrigMuon/TrigMuonDefs.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/PhiHelper.h"


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

static const InterfaceID IID_MdtDataPreparator("IID_MdtDataPreparator", 1, 0);

const InterfaceID& TrigL2MuonSA::MdtDataPreparator::interfaceID() { return IID_MdtDataPreparator; }

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::MdtDataPreparator::MdtDataPreparator(const std::string& type, 
						   const std::string& name,
						   const IInterface*  parent): 
   AlgTool(type,name,parent),
   m_msg(0),
   m_storeGateSvc( "StoreGateSvc", name ),
   m_mdtRawDataProvider("Muon__MDT_RawDataProviderTool"),
   m_mdtCablingSvc("MDTcablingSvc",""),
   m_regionSelector(0), m_robDataProvider(0), m_recMuonRoIUtils(),
   m_mdtRegionDefiner(0)
{
   declareInterface<TrigL2MuonSA::MdtDataPreparator>(this);

   declareProperty("MDT_RawDataProvider", m_mdtRawDataProvider);
   declareProperty("MDT_CablingSvc",      m_mdtCablingSvc);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::MdtDataPreparator::~MdtDataPreparator() 
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MdtDataPreparator::initialize()
{
   // Get a message stream instance
   m_msg = new MsgStream( msgSvc(), name() );
   msg() << MSG::DEBUG << "Initializing MdtDataPreparator - package version " << PACKAGE_VERSION << endreq ;
   
   StatusCode sc;
   sc = AlgTool::initialize();
   if (!sc.isSuccess()) {
      msg() << MSG::ERROR << "Could not initialize the AlgTool base class." << endreq;
      return sc;
   }
   
   // Locate the StoreGateSvc
   sc =  m_storeGateSvc.retrieve();
   if (!sc.isSuccess()) {
      msg() << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
      return sc;
   }

   // Locate MDT RawDataProvider
   sc = m_mdtRawDataProvider.retrieve();
   if ( sc.isFailure() ) {
      msg() << MSG::ERROR << "Could not retrieve " << m_mdtRawDataProvider << endreq;
      return sc;
   }
   msg() << MSG::DEBUG << "Retrieved tool " << m_mdtRawDataProvider << endreq;

   // Locate MDT CablingSvc
   sc = m_mdtCablingSvc.retrieve();
   if ( sc.isFailure() ) {
      msg() << MSG::ERROR << "Could not retrieve " << m_mdtCablingSvc << endreq;
      return sc;
   }
   msg() << MSG::DEBUG << "Retrieved service " << m_mdtCablingSvc << endreq;

   //
   std::string serviceName;

   // Locate RegionSelector
   serviceName = "RegionSelector";
   sc = service("RegSelSvc", m_regionSelector);
   if(sc.isFailure()) {
      msg() << MSG::ERROR << "Could not retrieve " << serviceName << endreq;
      return sc;
   }
   msg() << MSG::DEBUG << "Retrieved service " << serviceName << endreq;

   // Locate ROBDataProvider
   serviceName = "ROBDataProvider";
   IService* svc = 0;
   sc = service("ROBDataProviderSvc", svc);
   if(sc.isFailure()) {
      msg() << MSG::ERROR << "Could not retrieve " << serviceName << endreq;
      return sc;
   }
   m_robDataProvider = dynamic_cast<ROBDataProviderSvc*> (svc);
   if( m_robDataProvider == 0 ) {
      msg() << MSG::ERROR << "Could not cast to ROBDataProviderSvc " << endreq;
      return StatusCode::FAILURE;
   }
   msg() << MSG::DEBUG << "Retrieved service " << serviceName << endreq;

   //
   m_mdtRegionDefiner = new TrigL2MuonSA::MdtRegionDefiner(m_msg);

   // 
   return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MdtDataPreparator::prepareData(const LVL1::RecMuonRoI*  p_roi,
							const TrigL2MuonSA::RpcFitResult& rpcFitResult,
							TrigL2MuonSA::MuonRoad&  muonRoad,
							TrigL2MuonSA::MdtRegion& mdtRegion,
							TrigL2MuonSA::MdtHits&   mdtHits_normal,
							TrigL2MuonSA::MdtHits&   mdtHits_overlap)
{
  StatusCode sc;

  const MDTGeometry* mdtGeometry = m_mdtCablingSvc->mdtGeometry();
  m_mdtRegionDefiner->setMdtGeometry(mdtGeometry);
  
  // define regions
  sc = m_mdtRegionDefiner->getMdtRegions(p_roi, rpcFitResult, muonRoad, mdtRegion);
  if( sc!= StatusCode::SUCCESS ) {
    msg() << MSG::ERROR << "Error in getting MDT region" << endreq;
    return sc;
  }

  sc = getMdtHits(p_roi, mdtRegion, muonRoad, mdtHits_normal, mdtHits_overlap);
  if( sc!= StatusCode::SUCCESS ) {
    msg() << MSG::ERROR << "Error in getting MDT hits" << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;

}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MdtDataPreparator::prepareData(const LVL1::RecMuonRoI*           p_roi,
							const TrigL2MuonSA::TgcFitResult& tgcFitResult,
							TrigL2MuonSA::MuonRoad&           muonRoad,
							TrigL2MuonSA::MdtRegion&          mdtRegion,
							TrigL2MuonSA::MdtHits&            mdtHits_normal,
							TrigL2MuonSA::MdtHits&            mdtHits_overlap)
{
  StatusCode sc;

  const MDTGeometry* mdtGeometry = m_mdtCablingSvc->mdtGeometry();
  m_mdtRegionDefiner->setMdtGeometry(mdtGeometry);
  
  // define regions
  sc = m_mdtRegionDefiner->getMdtRegions(p_roi, tgcFitResult, muonRoad, mdtRegion);
  if( sc!= StatusCode::SUCCESS ) {
    msg() << MSG::ERROR << "Error in getting MDT region" << endreq;
    return sc;
  }

  sc = getMdtHits(p_roi, mdtRegion, muonRoad, mdtHits_normal, mdtHits_overlap);
  if( sc!= StatusCode::SUCCESS ) {
    msg() << MSG::ERROR << "Error in getting MDT hits" << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MdtDataPreparator::getMdtHits(const LVL1::RecMuonRoI* p_roi,
						       const TrigL2MuonSA::MdtRegion& mdtRegion,
						       TrigL2MuonSA::MuonRoad& muonRoad,
						       TrigL2MuonSA::MdtHits& mdtHits_normal,
						       TrigL2MuonSA::MdtHits& mdtHits_overlap)
{
  // preload ROBs
  std::vector<uint32_t> v_robIds;
  double etaMin = p_roi->eta() - 0.2;
  double etaMax = p_roi->eta() + 0.2;
  double phiMin = p_roi->phi() - 0.1;
  double phiMax = p_roi->phi() + 0.1;
  if( phiMin < 0 ) phiMin += 2*CLHEP::pi;
  if( phiMax < 0 ) phiMax += 2*CLHEP::pi;
  v_robIds = m_mdtCablingSvc->mdtGeometry()->getRobId(etaMin,etaMax,phiMin,phiMax);
  m_robDataProvider->addROBData(v_robIds);
  
  // get MdtCsmContainer
  const MdtCsmContainer* pMdtCsmContainer =
    Muon::MuonRdoContainerAccess::retrieveMdtCsm("MDTCSM");
  if( pMdtCsmContainer==0 ) {
    if ( msgLvl() <= MSG::DEBUG ) {
      msg() << MSG::DEBUG << "MDT CSM container not registered by "
	    << "MuonRdoContainerManager; retrieving it "
	    << "from the store! " << endreq;
    }
    StatusCode sc = m_storeGateSvc->retrieve(pMdtCsmContainer, "MDTCSM");
    if (sc.isFailure()) {
      msg() << MSG::ERROR << "Retrieval of MdtCsmContainer failed" << endreq;
      return sc;
    }
  }
  
  mdtHits_normal.clear();
  mdtHits_overlap.clear();
  
  // get IdHashes
  std::vector<IdentifierHash> v_idHash_normal;
  std::vector<IdentifierHash> v_idHash_overlap;
  
   // get hashIdlist by using region selector
   if (muonRoad.isEndcap) getMdtIdHashesEndcap(mdtRegion, v_idHash_normal,v_idHash_overlap);
   else getMdtIdHashesBarrel(mdtRegion, v_idHash_normal,v_idHash_overlap);

   msg() << MSG::DEBUG << "List of MDT Chambers (normal)..." << v_idHash_normal.size() << endreq;
   for (unsigned int i=0;i<v_idHash_normal.size();i++) {
      msg() << MSG::DEBUG << "MDT chamber n.  " << i << ": hash id " << v_idHash_normal[i] << endreq;
   }
   msg() << MSG::DEBUG << "List of MDT Chambers (overlap)..." << v_idHash_overlap.size() << endreq;
   for (unsigned int i=0;i<v_idHash_overlap.size();i++) {
      msg() << MSG::DEBUG << "MDT chamber n.  " << i << ": hash id " << v_idHash_overlap[i] << endreq;
   }

   // get MdtCsm
   std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> v_robFragments;
   m_robDataProvider->getROBData(v_robIds,v_robFragments);
   std::vector<const MdtCsm*> v_mdtCsms_normal;
   std::vector<const MdtCsm*> v_mdtCsms_overlap;
   msg() << MSG::DEBUG << "getting MdtCsm (normal)..." << endreq;
   StatusCode sc = getMdtCsm(pMdtCsmContainer, v_robFragments, v_idHash_normal, v_mdtCsms_normal);
   if( sc!= StatusCode::SUCCESS ) {
      msg() << MSG::ERROR << "Error in getting MdtCsm (normal)" << endreq;
      return sc;
   }
   msg() << MSG::DEBUG << "getting MdtCsm (overlap)..." << endreq;
   sc = getMdtCsm(pMdtCsmContainer, v_robFragments, v_idHash_overlap,v_mdtCsms_overlap);
   if( sc!= StatusCode::SUCCESS ) {
      msg() << MSG::ERROR << "Error in getting MdtCsm (overlap)" << endreq;
      return sc;
   }

   // decode 
   msg() << MSG::DEBUG << "decoding MdtCsm (normal)..." << endreq;
   for(unsigned int i=0; i<v_mdtCsms_normal.size(); i++) {
     if (!decodeMdtCsm(v_mdtCsms_normal[i], mdtHits_normal, muonRoad)) return StatusCode::FAILURE;
   }
   msg() << MSG::DEBUG << "decoding MdtCsm (overlap)..." << endreq;
   for(unsigned int i=0; i<v_mdtCsms_overlap.size(); i++) {
     if (!decodeMdtCsm(v_mdtCsms_overlap[i],mdtHits_overlap, muonRoad)) return StatusCode::FAILURE;
   }

   //
   return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MdtDataPreparator::getMdtCsm(const MdtCsmContainer* pMdtCsmContainer,
						      const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& v_robFragments,
						      const std::vector<IdentifierHash>& v_idHash,
						      std::vector<const MdtCsm*>& v_mdtCsms)
{
   if( m_mdtRawDataProvider->convert(v_robFragments, v_idHash).isFailure() ) {
      msg() << MSG::ERROR << "Failed to convert MDT CSM hash Ids: " << endreq;
      for(unsigned int i=0; i < v_idHash.size(); i++) {
	 msg() << MSG::ERROR << v_idHash[i] << endreq;
      }
      return StatusCode::FAILURE;
   }

   for(unsigned int i=0; i < v_idHash.size(); i++) {
      msg() << MSG::DEBUG << "i=" << i << endreq;
      MdtCsmContainer::const_iterator pCsmIt = pMdtCsmContainer->indexFind(v_idHash[i]);
      if( pCsmIt!=pMdtCsmContainer->end() ) {
	 v_mdtCsms.push_back(*pCsmIt);
	 msg() << MSG::DEBUG << "MDT Collection hash " << v_idHash[i]
	       << " associated to:  SubDet 0x" << MSG::hex
	       << (*pCsmIt)->SubDetId() << " MRod 0x"
	       << (*pCsmIt)->MrodId() << " Link 0x"
	       << (*pCsmIt)->CsmId() << MSG::dec << endreq;
	 msg() << MSG::DEBUG << "Number of digit in  MDT Collection "
	       << v_idHash[i] << ": " << (*pCsmIt)->size() << endreq;
      }
   }
   return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

bool TrigL2MuonSA::MdtDataPreparator::decodeMdtCsm(const MdtCsm* csm,
						   TrigL2MuonSA::MdtHits& mdtHits,
						   const TrigL2MuonSA::MuonRoad& muonRoad)
{    
   if( csm->empty() ) return true;

   float scale = (muonRoad.isEndcap)? 10.: 1.;

   unsigned short int SubsystemId = csm->SubDetId();
   unsigned short int MrodId      = csm->MrodId();
   unsigned short int LinkId      = csm->CsmId();
   CSMid csmId = m_mdtCablingSvc->mdtGeometry()->getCsm(SubsystemId,MrodId,LinkId);

   const Chamber* cha = csmId.get_chamber();
   if( cha==0 ) {
      msg() << MSG::ERROR << "CSM for Subsystem " << SubsystemId << ", MrodId " << MrodId << ", LinkId " << LinkId << " not found!" << endreq;
      return false;
   }

   float cYmid = cha->Ymid*scale;   // chamber Y-Middle position 
   float cXmid = cha->Xmid*scale;   // chamber X-Middle position
   float cAmid = cha->Amid;       // chamber middle inclination
   float cPhip = cha->Phipos;     // chamber Phi-Middle position 
   float cInCo = 1./cos(fabsf(atan(cha->OrtoRadialPos/cha->Rmin)));
   float cPhi0 = cPhip - atan(cha->OrtoRadialPos/cha->Rmin);
   if(cPhi0 > CLHEP::pi) cPhip -= 2*CLHEP::pi;
   if(cPhip<0. && (fabs(CLHEP::pi+cPhip) < 0.05) ) cPhip = acos(0.)*2.;
   
   int StationPhi  = csmId.stationPhi();
   int StationName = csmId.stationName();

   char st = cha->Type[1];

   int chamber = 0;
   if (cha->Type[0]=='E') {
     /// Endcap
     if (st=='I') chamber = xAOD::L2MuonParameters::Chamber::EndcapInner;
     if (st=='M') chamber = xAOD::L2MuonParameters::Chamber::EndcapMiddle;
     if (st=='O') chamber = xAOD::L2MuonParameters::Chamber::EndcapOuter;
     if (st=='E') chamber = xAOD::L2MuonParameters::Chamber::EndcapExtra;
   } else {
     /// Barrel
     if (st=='I') chamber = xAOD::L2MuonParameters::Chamber::BarrelInner;
     if (st=='M') chamber = xAOD::L2MuonParameters::Chamber::BarrelMiddle;
     if (st=='O') chamber = xAOD::L2MuonParameters::Chamber::BarrelOuter;
   }

   MdtCsm::const_iterator amt = csm->begin();
   while(amt != csm->end())
   {
      if( ! (*amt)->leading() ) continue;

      unsigned short int TdcId     = (*amt)->tdcId();
      unsigned short int ChannelId = (*amt)->channelId();
      unsigned short int drift     = (*amt)->fine() | ( (*amt)->coarse() << 5);
      int StationEta = csmId.stationEta(TdcId);
		   
      uint32_t OnlineId = ChannelId | (TdcId << 5) |
	 (LinkId << 10) | (get_system_id(SubsystemId) << 13) | (MrodId <<16);
      uint16_t adc        = (*amt)->width();
      uint16_t coarseTime = (*amt)->coarse();
      uint16_t fineTime   = (*amt)->fine();
      
      const TDCgeo* tdc = csmId.get_tdc(TdcId);
      if( !tdc ) {
	 msg() << MSG::ERROR << ": TDC # " << TdcId << " not found into the chamber structure!" << endreq;
	 return false;
      }   
      
     double dphi  = 0;
     double cphi  = muonRoad.phi[chamber][0];
     if( cPhip*cphi>0 ) dphi = fabsf(cPhip - cphi);
     else {
       if(fabs(cphi) > CLHEP::pi/2.) {
	 double phi1 = (cPhip>0.)? cPhip-CLHEP::pi : cPhip+CLHEP::pi;
	 double phi2 = (cphi >0.)? cphi -CLHEP::pi : cphi +CLHEP::pi;
	 dphi = fabsf(phi1) + fabsf(phi2); 
       }
       else {
	 dphi = fabsf(cPhip) + fabsf(cphi);
       }
     }

     double Z  = tdc->RZ[ChannelId][1] * scale;
     double R  = tdc->RZ[ChannelId][0] * scale;
     if (muonRoad.isEndcap) R = sqrt(R*R+R*R*tan(dphi)*tan(dphi));
     int Tube  = tdc->Tub[ChannelId];
     int Layer = tdc->Lay[ChannelId];

     msg() << MSG::DEBUG << " ...MDT hit chamber/ChannelId/Z/R/Tube/Layer="
	   << chamber << "/" << ChannelId << "/" << Z << "/" << R << "/" << Tube << "/" << Layer << endreq;
     
     // no residual check for the moment
     // (residual check at pattern finder)
     if(Layer!=0 && Tube !=0)
       {
	 int TubeLayers = (csmId.get_chamber(TdcId))->multi[0]->TubeLayers;
	 int MultiLayer = 1;
	 
	 int TubeLayer  = Layer;
	 if (TubeLayer > TubeLayers) {
	   TubeLayer -= TubeLayers;
	   MultiLayer = 2;
	 }
	 int station = 0;
         if (chamber == 0 || chamber == 3 ) station = 0;
         if (chamber == 1 || chamber == 4 ) station = 1;
         if (chamber == 2 || chamber == 5 ) station = 2;
         if (chamber == 6 ) station = 3;
	 // create the new digit
	 TrigL2MuonSA::MdtHitData tmp;
	 tmp.name       = StationName;
	 tmp.isEndcap   = (cha->Type[0]=='E')? 1: 0;
         tmp.Chamber    = chamber;
	 tmp.Station    = station;
	 tmp.StationEta = StationEta;
	 tmp.StationPhi = StationPhi;
	 tmp.Multilayer = MultiLayer;
	 tmp.Layer      = Layer - 1;
	 tmp.TubeLayer  = TubeLayer;
	 tmp.Tube       = Tube;
	 tmp.cYmid      = cYmid;
	 tmp.cXmid      = cXmid;
	 tmp.cAmid      = cAmid;
	 tmp.cPhip      = cPhip;
	 tmp.cInCo      = cInCo;
	 tmp.cPhi0      = cPhi0;
	 for(unsigned int i=0; i<4; i++) { tmp.cType[i] = cha->Type[i]; }
	 tmp.Z          = Z;
	 tmp.R          = R;
	 tmp.DriftTime  = drift;
	 tmp.DriftSpace = 0.;
	 tmp.DriftSigma = 0;
	 tmp.Adc        = adc;
	 tmp.OnlineId   = OnlineId;
	 tmp.LeadingCoarseTime  = coarseTime;
	 tmp.LeadingFineTime    = fineTime;
	 tmp.TrailingCoarseTime = 0;
	 tmp.TrailingFineTime   = 0;
	 tmp.Residual  = 0;
	 tmp.isOutlier = 0;

	 // 
	 mdtHits.push_back(tmp);
      }

      //
      ++amt;
   }
   

   //
   return true;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

uint32_t TrigL2MuonSA::MdtDataPreparator::get_system_id (unsigned short int SubsystemId) const
{
   if(SubsystemId==0x61) return 0x0;
   if(SubsystemId==0x62) return 0x1;
   if(SubsystemId==0x63) return 0x2;
   if(SubsystemId==0x64) return 0x3;
    
   return 0x4;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::MdtDataPreparator::getMdtIdHashesBarrel(const TrigL2MuonSA::MdtRegion& mdtRegion,
						   std::vector<IdentifierHash>& mdtIdHashes_normal,
						   std::vector<IdentifierHash>& mdtIdHashes_overlap)
{
   std::vector<IdentifierHash> idList;

   // get hashIdlist by using region selector
   for(int i_station=0; i_station<3; i_station++) {
     for(int i_sector=0; i_sector<2; i_sector++) {
       for(int i_type=0; i_type<2; i_type++) {
         if (i_station==3) break;
	 idList.clear();
	 msg() << MSG::DEBUG << "i_station/i_sector=" << i_station << "/" << i_sector << endreq;
	 msg() << MSG::DEBUG << "...etaMin/etaMax/phiMin/phiMax="
	       << mdtRegion.etaMin[i_station][i_sector] << "/"
	       << mdtRegion.etaMax[i_station][i_sector] << "/"
	       << mdtRegion.phiMin[i_station][i_sector] << "/"
	       << mdtRegion.phiMax[i_station][i_sector] << endreq;

	 TrigRoiDescriptor _roi( 0.5*(mdtRegion.etaMin[i_station][i_sector]+mdtRegion.etaMin[i_station][i_sector]),
				 mdtRegion.etaMin[i_station][i_sector],
				 mdtRegion.etaMax[i_station][i_sector],
				 HLT::phiMean(mdtRegion.phiMin[i_station][i_sector],mdtRegion.phiMax[i_station][i_sector]),
				 mdtRegion.phiMin[i_station][i_sector],
				 mdtRegion.phiMax[i_station][i_sector]);
	 
	 m_regionSelector->DetHashIDList(MDT,static_cast<TYPEID>(mdtRegion.chamberType[i_station][i_sector][i_type]),
					 _roi, idList);
	 msg() << MSG::DEBUG << "...chamberType=" << mdtRegion.chamberType[i_station][i_sector][i_type] << endreq;
	 msg() << MSG::DEBUG << "...size IDlist=" << idList.size() << endreq;
	 std::vector<IdentifierHash>::const_iterator it = idList.begin();
	 while(it != idList.end()) {
	   if (i_sector==0) {
	     mdtIdHashes_normal.push_back(*it++);
	   } else {
	     mdtIdHashes_overlap.push_back(*it++);
	   }
	 }
       }
     }
   }
}

void TrigL2MuonSA::MdtDataPreparator::getMdtIdHashesEndcap(const TrigL2MuonSA::MdtRegion& mdtRegion,
						   std::vector<IdentifierHash>& mdtIdHashes_normal,
						   std::vector<IdentifierHash>& mdtIdHashes_overlap)
{
   std::vector<IdentifierHash> idList;

   // get hashIdlist by using region selector
   for(int i_station=0; i_station<4; i_station++) {
     int chamber = 0;
     if (i_station==0) chamber = 3;//endcap inner 
     if (i_station==1) chamber = 4;//endcap middle
     if (i_station==2) chamber = 5; //endcap outer
     if (i_station==3) chamber = 0; //barrel inner
     for(int i_sector=0; i_sector<2; i_sector++) {
       for(int i_type=0; i_type<2; i_type++) {
	 idList.clear();
	 msg() << MSG::DEBUG << "chamber/i_sector=" << chamber << "/" << i_sector << endreq;
	 msg() << MSG::DEBUG << "...etaMin/etaMax/phiMin/phiMax="
	       << mdtRegion.etaMin[chamber][i_sector] << "/"
	       << mdtRegion.etaMax[chamber][i_sector] << "/"
	       << mdtRegion.phiMin[chamber][i_sector] << "/"
	       << mdtRegion.phiMax[chamber][i_sector] << endreq;

	 TrigRoiDescriptor _roi( 0.5*(mdtRegion.etaMin[chamber][i_sector]+mdtRegion.etaMin[chamber][i_sector]),
				 mdtRegion.etaMin[chamber][i_sector],
				 mdtRegion.etaMax[chamber][i_sector],
				 HLT::phiMean(mdtRegion.phiMin[chamber][i_sector],mdtRegion.phiMax[chamber][i_sector]),
				 mdtRegion.phiMin[chamber][i_sector],
				 mdtRegion.phiMax[chamber][i_sector]);
	 
	 m_regionSelector->DetHashIDList(MDT,static_cast<TYPEID>(mdtRegion.chamberType[chamber][i_sector][i_type]),
					 _roi, idList);
	 msg() << MSG::DEBUG << "...chamberType=" << mdtRegion.chamberType[chamber][i_sector][i_type] << endreq;
	 msg() << MSG::DEBUG << "...size IDlist=" << idList.size() << endreq;
	 std::vector<IdentifierHash>::const_iterator it = idList.begin();
	 while(it != idList.end()) {
	   if (i_sector==0) {
	     mdtIdHashes_normal.push_back(*it++);
	   } else {
	     mdtIdHashes_overlap.push_back(*it++);
	   }
	 }
       }
     }
   }
}
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MdtDataPreparator::finalize()
{
   msg() << MSG::DEBUG << "Finalizing MdtDataPreparator - package version " << PACKAGE_VERSION << endreq;
   
   // delete message stream
   if ( m_msg ) delete m_msg;
   if ( m_mdtRegionDefiner ) delete m_mdtRegionDefiner;
   
   StatusCode sc = AlgTool::finalize(); 
   return sc;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
