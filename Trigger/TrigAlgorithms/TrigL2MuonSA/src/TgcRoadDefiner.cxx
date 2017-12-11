/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/TgcRoadDefiner.h"
#include "xAODTrigMuon/L2StandAloneMuonAuxContainer.h"
#include "xAODTrigMuon/TrigMuonDefs.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "TrigL2MuonSA/MdtRegion.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/PhiHelper.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

static const InterfaceID IID_TgcRoadDefiner("IID_TgcRoadDefiner", 1, 0);

const InterfaceID& TrigL2MuonSA::TgcRoadDefiner::interfaceID() { return IID_TgcRoadDefiner; }

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::TgcRoadDefiner::TgcRoadDefiner(const std::string& type,
					     const std::string& name,
					     const IInterface*  parent):
     AthAlgTool(type, name, parent), 
     m_backExtrapolatorTool(0),
     m_ptEndcapLUT(0),
     m_tgcFit("TrigL2MuonSA::TgcFit"),
     m_rWidth_TGC_Failed(0),
     m_regionSelector(0),
     m_mdtIdHelper(0)
{
  declareInterface<TrigL2MuonSA::TgcRoadDefiner>(this);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::TgcRoadDefiner::~TgcRoadDefiner(void)
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::TgcRoadDefiner::initialize()
{
  ATH_MSG_DEBUG("Initializing TgcRoadDefiner - package version " << PACKAGE_VERSION) ;
   
  StatusCode sc;
  sc = AthAlgTool::initialize();
  if (!sc.isSuccess()) {
    ATH_MSG_ERROR("Could not initialize the AthAlgTool base class.");
    return sc;
  }

  sc =m_tgcFit.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve " << m_tgcFit);
    return sc;
  }
  ATH_MSG_DEBUG("Retrieved service " << m_tgcFit);

  // 
  return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::TgcRoadDefiner::setMdtGeometry(IRegSelSvc* regionSelector, const MdtIdHelper* mdtIdHelper)
{
  m_regionSelector = regionSelector;
  m_mdtIdHelper = mdtIdHelper;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::TgcRoadDefiner::setRoadWidthForFailure(double rWidth_TGC_Failed)
{
  m_rWidth_TGC_Failed = rWidth_TGC_Failed;
  return;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::TgcRoadDefiner::setExtrapolatorTool(ToolHandle<ITrigMuonBackExtrapolator>* backExtrapolator)
{
  m_backExtrapolatorTool = backExtrapolator;
  return;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::TgcRoadDefiner::setPtLUT(const TrigL2MuonSA::PtEndcapLUTSvc* ptEndcapLUTSvc)
{
  m_ptEndcapLUT = ptEndcapLUTSvc->ptEndcapLUT();
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::TgcRoadDefiner::defineRoad(const LVL1::RecMuonRoI*      p_roi,
                                                    const TrigL2MuonSA::TgcHits& tgcHits,
                                                    TrigL2MuonSA::MuonRoad&      muonRoad,
                                                    TrigL2MuonSA::TgcFitResult&  tgcFitResult)
{
  const int N_STATION = 10;

  bool isMiddleFailure = true;

  // Define road by using TGC fit result
  const double R_WIDTH_DEFAULT       = 100;
  const double R_WIDTH_INNER_NO_HIT  = 200;
  
  const double ZERO_LIMIT = 1e-5;

  muonRoad.isEndcap = true;

  int side;
  double roiEta;
  double theta;
  double aw;
  int endcap_inner = xAOD::L2MuonParameters::Chamber::EndcapInner; 
  int endcap_middle = xAOD::L2MuonParameters::Chamber::EndcapMiddle; 
  int endcap_outer = xAOD::L2MuonParameters::Chamber::EndcapOuter;
  int endcap_extra = xAOD::L2MuonParameters::Chamber::EndcapExtra;
  int barrel_inner = xAOD::L2MuonParameters::Chamber::BarrelInner;
  int csc = xAOD::L2MuonParameters::Chamber::CSC;
  int bee = xAOD::L2MuonParameters::Chamber::BEE;
 
  if (tgcHits.size()>0) {
    // TGC data is properly read
 
    // Split digits to Strip/Wire points.
    if( ! prepareTgcPoints(tgcHits) ) {
      ATH_MSG_ERROR("Preparation of Tgc points failed");
      return StatusCode::FAILURE;
    }
    

    // Fit lines to TGC middle station
    isMiddleFailure = false;
    TgcFit::Status status = m_tgcFit->runTgcMiddle(m_tgcStripMidPoints, m_tgcWireMidPoints, tgcFitResult);
    if (status == TgcFit::FIT_NONE) {
      ATH_MSG_WARNING("Fit to TGC middle station points failed");
      isMiddleFailure = true;
    }
    else if (status == TgcFit::FIT_POINT) {
      ATH_MSG_DEBUG("Fit to TGC middle station returns only a point");
    }
    
    // Fit lines to TGC inner station
    status = m_tgcFit->runTgcInner(m_tgcStripInnPoints, m_tgcWireInnPoints, tgcFitResult);
    if (status == TgcFit::FIT_NONE) {
      ATH_MSG_DEBUG("Fit to TGC inner station points failed");
    } 
    else if (status == TgcFit::FIT_POINT) {
      ATH_MSG_DEBUG("Fit to TGC inner station returns only a point");
    }
    
    ATH_MSG_DEBUG("tgcFitResult.tgcInn[0/1/2/3]=" << tgcFitResult.tgcInn[0] << "/" << tgcFitResult.tgcInn[1]
		  << "/" << tgcFitResult.tgcInn[2] << "/" << tgcFitResult.tgcInn[3]);
    ATH_MSG_DEBUG("tgcFitResult.tgcMid1[0/1/2/3]=" << tgcFitResult.tgcMid1[0] << "/" << tgcFitResult.tgcMid1[1]
		  << "/" << tgcFitResult.tgcMid1[2] << "/" << tgcFitResult.tgcMid1[3]);
    ATH_MSG_DEBUG("tgcFitResult.tgcMid2[0/1/2/3]=" << tgcFitResult.tgcMid2[0] << "/" << tgcFitResult.tgcMid2[1]
		  << "/" << tgcFitResult.tgcMid2[2] << "/" << tgcFitResult.tgcMid2[3]);
  }
   
  if (tgcHits.size()>0 && !isMiddleFailure){

    tgcFitResult.isSuccess = true;
    
    // PT calculation by using TGC fit result
    const double PHI_RANGE = 12./(CLHEP::pi/8.);
    side = (tgcFitResult.tgcMid2[3]<=0) ? 0 : 1;
    double alpha = (*m_ptEndcapLUT)->alpha(tgcFitResult.tgcMid1[3], tgcFitResult.tgcMid1[2],
                                        tgcFitResult.tgcMid2[3], tgcFitResult.tgcMid2[2]);
    
    int Octant = (int)(tgcFitResult.tgcMid1[1] / (CLHEP::pi/4.));
    double PhiInOctant = fabs(tgcFitResult.tgcMid1[1] - Octant * (CLHEP::pi/4.));
    if (PhiInOctant > (CLHEP::pi/8.)) PhiInOctant = (CLHEP::pi/4.) - PhiInOctant;
    
    int phiBin = static_cast<int>(PhiInOctant * PHI_RANGE);
    int etaBin = static_cast<int>((fabs(tgcFitResult.tgcMid1[0]) - 1.)/0.05);
    
    int charge = (tgcFitResult.intercept * tgcFitResult.tgcMid2[3]) < 0.0 ? 0: 1;
    
    tgcFitResult.tgcPT = (*m_ptEndcapLUT)->lookup(side, charge, PtEndcapLUT::TGCALPHAPOL2, etaBin, phiBin, alpha) / 1000.;
    if (charge==0) tgcFitResult.tgcPT = -1.*tgcFitResult.tgcPT;
    
    // Determine phi direction
    if (fabs(tgcFitResult.tgcMid1[3])<=ZERO_LIMIT || fabs(tgcFitResult.tgcMid2[3])<=ZERO_LIMIT) {

      tgcFitResult.isPhiDir = false;
      if (fabs(tgcFitResult.tgcMid1[3])>=0.) tgcFitResult.phi = tgcFitResult.tgcMid1[1];
      if (fabs(tgcFitResult.tgcMid2[3])>=0.) tgcFitResult.phi = tgcFitResult.tgcMid2[1];

    } else {

      tgcFitResult.isPhiDir = true;

      if( tgcFitResult.tgcMid1[1]*tgcFitResult.tgcMid2[1] < 0
          && fabsf(tgcFitResult.tgcMid1[1])>CLHEP::pi/2. ) {

        double tmp1 = (tgcFitResult.tgcMid1[1]>0)?
          tgcFitResult.tgcMid1[1] - CLHEP::pi : tgcFitResult.tgcMid1[1] + CLHEP::pi;

        double tmp2 = (tgcFitResult.tgcMid2[1]>0)?
          tgcFitResult.tgcMid2[1] - CLHEP::pi : tgcFitResult.tgcMid2[1] + CLHEP::pi;

        double tmp  = (tmp1+tmp2)/2.;

        tgcFitResult.dPhidZ = (fabs(tgcFitResult.tgcMid2[3]-tgcFitResult.tgcMid1[3]) > ZERO_LIMIT)?
          (tmp2-tmp1)/fabs(tgcFitResult.tgcMid2[3]-tgcFitResult.tgcMid1[3]): 0;

        tgcFitResult.phi = (tmp>0.)? tmp - CLHEP::pi : tmp + CLHEP::pi;

      } else {

        tgcFitResult.dPhidZ = (fabs(tgcFitResult.tgcMid2[3]-tgcFitResult.tgcMid1[3]) > ZERO_LIMIT)?
	  (tgcFitResult.tgcMid2[1]-tgcFitResult.tgcMid1[1])/fabs(tgcFitResult.tgcMid2[3]-tgcFitResult.tgcMid1[3]): 0;
        tgcFitResult.phi = (tgcFitResult.tgcMid2[1]+tgcFitResult.tgcMid1[1])/2.;

      }
    }
    float X1 = tgcFitResult.tgcMid1[3] * cos(tgcFitResult.tgcMid1[1]);
    float Y1 = tgcFitResult.tgcMid1[3] * sin(tgcFitResult.tgcMid1[1]);
    float X2 = tgcFitResult.tgcMid2[3] * cos(tgcFitResult.tgcMid2[1]);
    float Y2 = tgcFitResult.tgcMid2[3] * sin(tgcFitResult.tgcMid2[1]);
    if (X1>ZERO_LIMIT && X2>ZERO_LIMIT) tgcFitResult.phiDir = (Y1/X1 + Y2/X2)/2.;

    muonRoad.aw[endcap_middle][0]     = tgcFitResult.slope;
    muonRoad.bw[endcap_middle][0]     = tgcFitResult.intercept;
    muonRoad.aw[endcap_outer][0]     = tgcFitResult.slope;
    muonRoad.bw[endcap_outer][0]     = tgcFitResult.intercept;
    muonRoad.aw[endcap_extra][0]     = tgcFitResult.slope;
    muonRoad.bw[endcap_extra][0]     = tgcFitResult.intercept;
    muonRoad.aw[bee][0]     = tgcFitResult.slope;
    muonRoad.bw[bee][0]     = tgcFitResult.intercept;
    for (int i_layer=0; i_layer<8; i_layer++) {
      muonRoad.rWidth[endcap_middle][i_layer] = R_WIDTH_DEFAULT;
      muonRoad.rWidth[endcap_outer][i_layer] = R_WIDTH_DEFAULT;
      muonRoad.rWidth[endcap_extra][i_layer] = R_WIDTH_DEFAULT;
      muonRoad.rWidth[bee][i_layer] = R_WIDTH_DEFAULT;
    }
    
    if( fabs(tgcFitResult.tgcInn[3]) > ZERO_LIMIT ) {
      muonRoad.aw[endcap_inner][0]  = tgcFitResult.tgcInn[2]/tgcFitResult.tgcInn[3];
      muonRoad.aw[barrel_inner][0]  = tgcFitResult.tgcInn[2]/tgcFitResult.tgcInn[3];
      muonRoad.aw[csc][0]           = tgcFitResult.tgcInn[2]/tgcFitResult.tgcInn[3];
      for (int i_layer=0; i_layer<8; i_layer++) muonRoad.rWidth[endcap_inner][i_layer] = R_WIDTH_DEFAULT;
      for (int i_layer=0; i_layer<8; i_layer++) muonRoad.rWidth[barrel_inner][i_layer] = R_WIDTH_DEFAULT;
      for (int i_layer=0; i_layer<8; i_layer++) muonRoad.rWidth[csc][i_layer]          = R_WIDTH_DEFAULT;
    } else {
      // use the back extrapolator to retrieve the Etain the Innermost
      
      double etaMiddle = (tgcFitResult.tgcMid1[3])? tgcFitResult.tgcMid1[0] : tgcFitResult.tgcMid2[0];
      double phiMiddle = (tgcFitResult.tgcMid1[3])? tgcFitResult.tgcMid1[1] : tgcFitResult.tgcMid2[1];
      double eta;
      double sigma_eta;
      double extrInnerEta = 0;
      
      xAOD::L2StandAloneMuon* muonSA = new xAOD::L2StandAloneMuon();
      muonSA->makePrivateStore();
      muonSA->setSAddress(-1);
      muonSA->setPt(tgcFitResult.tgcPT);
      muonSA->setEtaMS(etaMiddle);
      muonSA->setPhiMS(phiMiddle);
      muonSA->setRMS(0.);
      muonSA->setZMS(0.);

      double phi;
      double sigma_phi;

      if (m_backExtrapolatorTool && muonSA) {
        StatusCode sc
          = (*m_backExtrapolatorTool)->give_eta_phi_at_vertex(muonSA, eta,sigma_eta,phi,sigma_phi,0.);
        if (sc.isSuccess() ){
          extrInnerEta = eta;
        } else {
          extrInnerEta = etaMiddle;
        }
      } else {
        ATH_MSG_ERROR("Null pointer to ITrigMuonBackExtrapolator");
        return StatusCode::FAILURE;
      }

      if (muonSA) delete muonSA;
      
      double theta = 0.;
      if (extrInnerEta != 0.) {
        theta = atan(exp(-fabs(extrInnerEta)))*2.;
        muonRoad.aw[endcap_inner][0] = tan(theta)*(fabs(extrInnerEta)/extrInnerEta);
        muonRoad.aw[barrel_inner][0] = tan(theta)*(fabs(extrInnerEta)/extrInnerEta);
        muonRoad.aw[csc][0]          = tan(theta)*(fabs(extrInnerEta)/extrInnerEta);
      } else {
        muonRoad.aw[endcap_inner][0] = 0;
        muonRoad.aw[barrel_inner][0] = 0;
        muonRoad.aw[csc][0]          = 0;
      }
      
      for (int i_layer=0; i_layer<8; i_layer++) muonRoad.rWidth[endcap_inner][i_layer] = R_WIDTH_INNER_NO_HIT;
      for (int i_layer=0; i_layer<8; i_layer++) muonRoad.rWidth[barrel_inner][i_layer] = R_WIDTH_INNER_NO_HIT;
      for (int i_layer=0; i_layer<8; i_layer++) muonRoad.rWidth[csc][i_layer]          = R_WIDTH_INNER_NO_HIT;
      
    }
    
    muonRoad.bw[endcap_inner][0] = 0.;
    muonRoad.bw[barrel_inner][0] = 0.;
    muonRoad.bw[csc][0]          = 0.;
    
  } else {
    // If no TGC hit are available, estimate the road from RoI

    roiEta = p_roi->eta();
    theta  = atan(exp(-fabs(roiEta)))*2.;
    aw     = (fabs(roiEta) > ZERO_LIMIT)? tan(theta)*(fabs(roiEta)/roiEta): 0.;
    
    muonRoad.aw[endcap_inner][0]     = aw;
    muonRoad.bw[endcap_inner][0]     = 0;
    muonRoad.aw[endcap_middle][0]     = aw;
    muonRoad.bw[endcap_middle][0]     = 0;
    muonRoad.aw[endcap_outer][0]     = aw;
    muonRoad.bw[endcap_outer][0]     = 0;
    muonRoad.aw[endcap_extra][0]     = aw;
    muonRoad.bw[endcap_extra][0]     = 0;
    muonRoad.aw[barrel_inner][0]     = aw;
    muonRoad.bw[barrel_inner][0]     = 0;
    muonRoad.aw[bee][0]     = aw;
    muonRoad.bw[bee][0]     = 0;
    muonRoad.aw[csc][0]     = aw;
    muonRoad.bw[csc][0]     = 0;
    for (int i_layer=0; i_layer<8; i_layer++) {
      muonRoad.rWidth[endcap_inner][i_layer] = m_rWidth_TGC_Failed;
      muonRoad.rWidth[endcap_middle][i_layer] = m_rWidth_TGC_Failed;
      muonRoad.rWidth[endcap_outer][i_layer] = m_rWidth_TGC_Failed;
      muonRoad.rWidth[endcap_extra][i_layer] = m_rWidth_TGC_Failed;
      muonRoad.rWidth[barrel_inner][i_layer] = m_rWidth_TGC_Failed;
      muonRoad.rWidth[bee][i_layer] = m_rWidth_TGC_Failed;
      muonRoad.rWidth[csc][i_layer] = m_rWidth_TGC_Failed;
    }
  }
 
  for(int i=0; i<N_STATION; i++) {
    muonRoad.aw[i][1] = muonRoad.aw[i][0];
    muonRoad.bw[i][1] = muonRoad.bw[i][0];
  }
  
  //
  side      = 0;
  float phiMiddle = 0;
  if( ! isMiddleFailure ) {
    side      = (tgcFitResult.tgcMid1[3]<0.)? 0 : 1;
    phiMiddle =  tgcFitResult.tgcMid1[1];
  } else {
    side      = (p_roi->eta()<0.)? 0 : 1;
    phiMiddle =  p_roi->phi();
  }
  muonRoad.side      = side;
  muonRoad.phiMiddle = phiMiddle;
  muonRoad.phiRoI    = p_roi->phi();
  
  int sector_trigger = 99;
  int sector_overlap = 99;
  int temp_sector=99;
  float deltaPhi=99;
  float tempDeltaPhi=99;
  std::vector<Identifier> stationList;
  std::vector<IdentifierHash> mdtHashList;
  
  // get sector_trigger and sector_overlap by using the region selector
  IdContext context = m_mdtIdHelper->module_context();
  
  double etaMin =  p_roi->eta()-.02;
  double etaMax =  p_roi->eta()+.02;
  double phiMin = muonRoad.phiMiddle-.01;
  double phiMax = muonRoad.phiMiddle+.01;
  if(phiMax > CLHEP::pi) phiMax -= CLHEP::pi*2.;
  if(phiMin < CLHEP::pi*-1) phiMin += CLHEP::pi*2.;
  TrigRoiDescriptor* roi = new TrigRoiDescriptor( p_roi->eta(), etaMin, etaMax, p_roi->phi(), phiMin, phiMax ); 
  const IRoiDescriptor* iroi = (IRoiDescriptor*) roi;
  if (iroi) m_regionSelector->DetHashIDList(MDT, *iroi, mdtHashList);
  else m_regionSelector->DetHashIDList(MDT, mdtHashList);
  if(roi) delete roi;
  
  for(int i_hash=0; i_hash<(int)mdtHashList.size(); i_hash++){
    Identifier id;
    int convert = m_mdtIdHelper->get_id(mdtHashList[i_hash], id, &context);

    if(convert!=0) ATH_MSG_ERROR("problem converting hash list to id");

    muonRoad.stationList.push_back(id);
    std::string name = m_mdtIdHelper->stationNameString(m_mdtIdHelper->stationName(id));
    if ( name.substr(0, 1) == 'B' ) continue;
    if ( name.substr(1, 1) != 'M' ) continue;
    int stationPhi = m_mdtIdHelper->stationPhi(id);
    float floatPhi = (stationPhi-1)*CLHEP::pi/4;
    if (name[2]=='S' || name[2]=='E') floatPhi = floatPhi + CLHEP::pi/8;
    tempDeltaPhi = fabs(floatPhi-muonRoad.phiMiddle);
    if (phiMiddle<0) tempDeltaPhi = fabs(floatPhi-muonRoad.phiMiddle-2*CLHEP::pi);
    if(tempDeltaPhi > CLHEP::pi) tempDeltaPhi = fabs(tempDeltaPhi - 2*CLHEP::pi);
    
    int LargeSmall = 0;
    if(name[2]=='S' || name[2]=='E') LargeSmall = 1;
    int sector = (stationPhi-1)*2 + LargeSmall;
    if(sector_trigger == 99)
      sector_trigger = sector;
    else if(sector_trigger != sector)
      sector_overlap = sector;
    
    if(tempDeltaPhi < deltaPhi){
      deltaPhi = tempDeltaPhi;
      temp_sector = sector;
      muonRoad.LargeSmall=LargeSmall;
    }
    
  }
  if(temp_sector != sector_trigger){
    sector_overlap = sector_trigger;
    sector_trigger = temp_sector;  
  }
  
  muonRoad.MDT_sector_trigger = sector_trigger;
  muonRoad.MDT_sector_overlap = sector_overlap;
  
  //
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

bool TrigL2MuonSA::TgcRoadDefiner::prepareTgcPoints(const TrigL2MuonSA::TgcHits& tgcHits)
{
   const double PHI_BOUNDARY = 0.2;

   ATH_MSG_DEBUG(", m_tgcStripMidPoints.size()=" << m_tgcStripMidPoints.size() <<
		 ", m_tgcStripInnPoints.size()=" << m_tgcStripInnPoints.size() <<
		 ", m_tgcWireMidPoints.size()=" << m_tgcWireMidPoints.size() <<
		 ", m_tgcWireInnPoints.size()=" << m_tgcWireInnPoints.size());
   
   m_tgcStripMidPoints.clear();
   m_tgcStripInnPoints.clear();
   m_tgcWireMidPoints.clear();
   m_tgcWireInnPoints.clear();

   // loop over TGC digits.
   unsigned int iHit;
   for (iHit = 0; iHit < tgcHits.size(); iHit++)
   {
      // Get the digit point.
      const TrigL2MuonSA::TgcHitData& hit = tgcHits[iHit];

      // reject width=0 hits
      const double ZERO_LIMIT = 1e-5;
      if( fabs(hit.width) < ZERO_LIMIT ) continue;
      
      double w = 12.0 / hit.width / hit.width;
      if (hit.isStrip)
      {
         w *= hit.r * hit.r;
         double phi = hit.phi;
         if( phi < 0 && ( (CLHEP::pi+phi)<PHI_BOUNDARY) ) phi += CLHEP::pi*2;
         if      ( hit.sta < 3 ) { m_tgcStripMidPoints.push_back(TgcFit::Point(iHit + 1, hit.sta, hit.z, phi, w)); }
         else if ( hit.sta ==3 ) { m_tgcStripInnPoints.push_back(TgcFit::Point(iHit + 1, hit.sta, hit.z, phi, w)); }
      }
      else
      {
         if      ( hit.sta < 3 ) { m_tgcWireMidPoints.push_back(TgcFit::Point(iHit + 1, hit.sta, hit.z, hit.r, w)); }
         else if ( hit.sta ==3 ) { m_tgcWireInnPoints.push_back(TgcFit::Point(iHit + 1, hit.sta, hit.z, hit.r, w)); }
      }
   }

   return true;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::TgcRoadDefiner::finalize()
{
  ATH_MSG_DEBUG("Finalizing TgcRoadDefiner - package version " << PACKAGE_VERSION);
   
  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
