/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/TgcRoadDefiner.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "TrigL2MuonSA/MdtRegion.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::TgcRoadDefiner::TgcRoadDefiner(MsgStream* msg)
   : m_msg(msg), 
     m_tgcFit(msg,10), // chi2 value 10 given by hand for now
     m_rWidth_TGC_Failed(0)
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::TgcRoadDefiner::~TgcRoadDefiner(void)
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::TgcRoadDefiner::setMdtGeometry(const MDTGeometry* mdtGeometry)
{
  m_mdtGeometry = mdtGeometry;
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

bool TrigL2MuonSA::TgcRoadDefiner::defineRoad(const LVL1::RecMuonRoI*    p_roi,
					      const TrigL2MuonSA::TgcHits& tgcHits,
					      TrigL2MuonSA::MuonRoad&      muonRoad,
					      TrigL2MuonSA::TgcFitResult&  tgcFitResult)
{
  const int N_STATION = 8;

  bool isMiddleFailure = true;

  // Define road by using TGC fit result
  const double R_WIDTH_DEFAULT       = 100;
  const double R_WIDTH_MIDDLE_NO_HIT = 150;
  const double R_WIDTH_INNER_NO_HIT  = 200;
  
  const double ZERO_LIMIT = 1e-5;

  muonRoad.isEndcap = true;

  int side;
  double roiEta;
  double theta;
  double aw;
 
  if (tgcHits.size()>0) {
    // TGC data is properly read
 
    // Split digits to Strip/Wire points.
    if( ! prepareTgcPoints(tgcHits) ) {
      msg() << MSG::ERROR << "Preparation of Tgc points failed" << endreq;
      return false;
    }
    
    tgcFitResult.isSuccess = true;

    // Fit lines to TGC middle station
    isMiddleFailure = false;
    TgcFit::Status status = m_tgcFit.runTgcMiddle(m_tgcStripMidPoints, m_tgcWireMidPoints, tgcFitResult);
    if (status == TgcFit::FIT_NONE) {
      msg() << MSG::WARNING << "Fit to TGC middle station points failed" << endreq;
      isMiddleFailure = true;
    }
    else if (status == TgcFit::FIT_POINT) {
      msg() << MSG::DEBUG << "Fit to TGC middle station returns only a point";
    }
    
    // Fit lines to TGC inner station
    status = m_tgcFit.runTgcInner(m_tgcStripInnPoints, m_tgcWireInnPoints, tgcFitResult);
    if (status == TgcFit::FIT_NONE) {
      msg() << MSG::DEBUG << "Fit to TGC inner station points failed" << endreq;
    } 
    else if (status == TgcFit::FIT_POINT) {
      msg() << MSG::DEBUG << "Fit to TGC inner station returns only a point";
    }
    
    msg() << MSG::DEBUG << "tgcFitResult.tgcInn[0/1/2/3]=" << tgcFitResult.tgcInn[0] << "/" << tgcFitResult.tgcInn[1]
	  << "/" << tgcFitResult.tgcInn[2] << "/" << tgcFitResult.tgcInn[3] << endreq;
    msg() << MSG::DEBUG << "tgcFitResult.tgcMid1[0/1/2/3]=" << tgcFitResult.tgcMid1[0] << "/" << tgcFitResult.tgcMid1[1]
	  << "/" << tgcFitResult.tgcMid1[2] << "/" << tgcFitResult.tgcMid1[3] << endreq;
    msg() << MSG::DEBUG << "tgcFitResult.tgcMid2[0/1/2/3]=" << tgcFitResult.tgcMid2[0] << "/" << tgcFitResult.tgcMid2[1]
	  << "/" << tgcFitResult.tgcMid2[2] << "/" << tgcFitResult.tgcMid2[3] << endreq;
    
    // PT calculation by using TGC fit result
    const double PHI_RANGE = 12./(CLHEP::pi/8.);
    side = (tgcFitResult.tgcMid2[3]<=0) ? 0 : 1;
    double alpha = m_ptEndcapLUT->alpha(tgcFitResult.tgcMid1[3], tgcFitResult.tgcMid1[2],
					tgcFitResult.tgcMid2[3], tgcFitResult.tgcMid2[2]);
    
    int Octant = (int)(tgcFitResult.tgcMid1[1] / (CLHEP::pi/4.));
    double PhiInOctant = fabs(tgcFitResult.tgcMid1[1] - Octant * (CLHEP::pi/4.));
    if (PhiInOctant > (CLHEP::pi/8.)) PhiInOctant = (CLHEP::pi/4.) - PhiInOctant;
    
    int phiBin = static_cast<int>(PhiInOctant * PHI_RANGE);
    int etaBin = static_cast<int>((fabs(tgcFitResult.tgcMid1[0]) - 1.)/0.05);
    
    int charge = (tgcFitResult.intercept * tgcFitResult.tgcMid2[3]) < 0.0 ? 0: 1;
    
    tgcFitResult.tgcPT = m_ptEndcapLUT->lookup(side, charge, PtEndcapLUT::ALPHAPOL2, 2, etaBin, phiBin, alpha) / 1000.;
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

	tgcFitResult.dPhidZ = (fabs(tgcFitResult.tgcMid2[3]-tgcFitResult.tgcMid1[3])>0.)?
	  (tmp2-tmp1)/fabs(tgcFitResult.tgcMid2[3]-tgcFitResult.tgcMid1[3]): 0;

	tgcFitResult.phi = (tmp>0.)? tmp - CLHEP::pi : tmp + CLHEP::pi;

      } else {

	tgcFitResult.dPhidZ = (tgcFitResult.tgcMid2[1]-tgcFitResult.tgcMid1[1])/fabs(tgcFitResult.tgcMid2[3]-tgcFitResult.tgcMid1[3]);
	tgcFitResult.phi = (tgcFitResult.tgcMid2[1]+tgcFitResult.tgcMid1[1])/2.;

      }
    }
    float X1 = tgcFitResult.tgcMid1[3] * cos(tgcFitResult.tgcMid1[1]);
    float Y1 = tgcFitResult.tgcMid1[3] * sin(tgcFitResult.tgcMid1[1]);
    float X2 = tgcFitResult.tgcMid2[3] * cos(tgcFitResult.tgcMid2[1]);
    float Y2 = tgcFitResult.tgcMid2[3] * sin(tgcFitResult.tgcMid2[1]);
    tgcFitResult.phiDir = (Y1/X1 + Y2/X2)/2.;
    
    if( ! isMiddleFailure ) {
      muonRoad.aw[4][0]     = tgcFitResult.slope;
      muonRoad.bw[4][0]     = tgcFitResult.intercept;
      muonRoad.aw[5][0]     = tgcFitResult.slope;
      muonRoad.bw[5][0]     = tgcFitResult.intercept;
      muonRoad.aw[6][0]     = tgcFitResult.slope;
      muonRoad.bw[6][0]     = tgcFitResult.intercept;
      for (int i_layer=0; i_layer<8; i_layer++) {
	muonRoad.rWidth[4][i_layer] = R_WIDTH_DEFAULT;
	muonRoad.rWidth[5][i_layer] = R_WIDTH_DEFAULT;
	muonRoad.rWidth[6][i_layer] = R_WIDTH_DEFAULT;
      }
    } else {
      roiEta = p_roi->eta();
      theta  = atan(exp(-fabs(roiEta)))*2.;
      aw     = (fabs(roiEta) > ZERO_LIMIT)? tan(theta)*(fabs(roiEta)/roiEta): 0.;
      muonRoad.aw[4][0]     = aw;
      muonRoad.bw[4][0]     = 0;
      muonRoad.aw[5][0]     = aw;
      muonRoad.bw[5][0]     = 0;
      muonRoad.aw[6][0]     = aw;
      muonRoad.bw[6][0]     = 0;
      for (int i_layer=0; i_layer<8; i_layer++) {
	muonRoad.rWidth[4][i_layer] = R_WIDTH_MIDDLE_NO_HIT;
	muonRoad.rWidth[5][i_layer] = R_WIDTH_MIDDLE_NO_HIT;
	muonRoad.rWidth[6][i_layer] = R_WIDTH_MIDDLE_NO_HIT;
      }
    }
    
    
    if( fabs(tgcFitResult.tgcInn[3]) > ZERO_LIMIT ) {
      muonRoad.aw[3][0]  = tgcFitResult.tgcInn[2]/tgcFitResult.tgcInn[3];
      muonRoad.aw[0][0]  = tgcFitResult.tgcInn[2]/tgcFitResult.tgcInn[3];
      for (int i_layer=0; i_layer<8; i_layer++) muonRoad.rWidth[3][i_layer] = R_WIDTH_DEFAULT;
      for (int i_layer=0; i_layer<8; i_layer++) muonRoad.rWidth[0][i_layer] = R_WIDTH_DEFAULT;
    } else {
      // use the back extrapolator to retrieve the Etain the Innermost
      
      double etaMiddle = (tgcFitResult.tgcMid1[3])? tgcFitResult.tgcMid1[0] : tgcFitResult.tgcMid2[0];
      double phiMiddle = (tgcFitResult.tgcMid1[3])? tgcFitResult.tgcMid1[1] : tgcFitResult.tgcMid2[1];
      double eta;
      double sigma_eta;
      double extrInnerEta = 0;
      
      MuonFeature* feature =  new MuonFeature(-1,tgcFitResult.tgcPT,0.,etaMiddle,phiMiddle,0.,0.,0.,1.0);
      
      double phi;
      double sigma_phi;
      StatusCode sc
	= (*m_backExtrapolatorTool)->give_eta_phi_at_vertex(feature, eta,sigma_eta,phi,sigma_phi,0.);
      if (sc.isSuccess() ){
	extrInnerEta = eta;
      } else {
	extrInnerEta = etaMiddle;
      }

      if (feature) delete feature;
      
      double theta = 0.;
      if (extrInnerEta != 0.) {
	theta = atan(exp(-fabs(extrInnerEta)))*2.;
	muonRoad.aw[3][0] = tan(theta)*(fabs(extrInnerEta)/extrInnerEta);
	muonRoad.aw[0][0] = tan(theta)*(fabs(extrInnerEta)/extrInnerEta);
      } else {
	muonRoad.aw[3][0] = 0;
	muonRoad.aw[0][0] = 0;
      }
      
      for (int i_layer=0; i_layer<8; i_layer++) muonRoad.rWidth[0][i_layer] = R_WIDTH_INNER_NO_HIT;
      for (int i_layer=0; i_layer<8; i_layer++) muonRoad.rWidth[4][i_layer] = R_WIDTH_INNER_NO_HIT;
      
    }
    
    muonRoad.bw[3][0] = 0.;
    muonRoad.bw[0][0] = 0.;
    
  } else {
    // If no TGC hit are available, estimate the road from RoI

    roiEta = p_roi->eta();
    theta  = atan(exp(-fabs(roiEta)))*2.;
    aw     = (fabs(roiEta) > ZERO_LIMIT)? tan(theta)*(fabs(roiEta)/roiEta): 0.;
    
    muonRoad.aw[3][0]     = aw;
    muonRoad.bw[3][0]     = 0;
    muonRoad.aw[4][0]     = aw;
    muonRoad.bw[4][0]     = 0;
    muonRoad.aw[5][0]     = aw;
    muonRoad.bw[5][0]     = 0;
    muonRoad.aw[6][0]     = aw;
    muonRoad.bw[6][0]     = 0;
    muonRoad.aw[0][0]     = aw;
    muonRoad.bw[0][0]     = 0;
    for (int i_layer=0; i_layer<8; i_layer++) {
      muonRoad.rWidth[3][i_layer] = m_rWidth_TGC_Failed;
      muonRoad.rWidth[4][i_layer] = m_rWidth_TGC_Failed;
      muonRoad.rWidth[5][i_layer] = m_rWidth_TGC_Failed;
      muonRoad.rWidth[6][i_layer] = m_rWidth_TGC_Failed;
      muonRoad.rWidth[0][i_layer] = m_rWidth_TGC_Failed;
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
  
  int sector_trigger = 0;
  int sector_overlap = 0;
  m_mdtGeometry->getEsects(1, muonRoad.side, muonRoad.phiMiddle, sector_trigger, sector_overlap);
  
  muonRoad.MDT_sector_trigger = sector_trigger;
  muonRoad.MDT_sector_overlap = sector_overlap;
  
  //
  return true;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

bool TrigL2MuonSA::TgcRoadDefiner::prepareTgcPoints(const TrigL2MuonSA::TgcHits& tgcHits)
{
   const double PHI_BOUNDARY = 0.2;

   msg() << MSG::DEBUG << 
     ", m_tgcStripMidPoints.size()=" << m_tgcStripMidPoints.size() <<
     ", m_tgcStripInnPoints.size()=" << m_tgcStripInnPoints.size() <<
     ", m_tgcWireMidPoints.size()=" << m_tgcWireMidPoints.size() <<
     ", m_tgcWireInnPoints.size()=" << m_tgcWireInnPoints.size() <<
     endreq;

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
