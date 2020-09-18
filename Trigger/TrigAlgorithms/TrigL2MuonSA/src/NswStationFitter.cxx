/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "NswStationFitter.h"
#include "xAODTrigMuon/TrigMuonDefs.h"

#include "RecMuonRoIUtils.h"
#include <cmath>
#include "AthenaBaseComps/AthMsgStreamMacros.h"

TrigL2MuonSA::NswStationFitter::NswStationFitter(const std::string& type,
						 const std::string& name,
						 const IInterface*  parent):
  AthAlgTool(type,name,parent)
{
}

StatusCode TrigL2MuonSA::NswStationFitter::superPointFitter(const LVL1::RecMuonRoI* p_roi,
                                                            TrigL2MuonSA::TrackPattern& trackPattern)
{

  ATH_MSG_DEBUG("NswStationFitter::findSuperPoints() was called.");

  // selection for sTGC hits, RoI matching based
  ATH_CHECK( selectStgcHits(p_roi,trackPattern.stgcSegment) );
  ATH_CHECK( selectMmHits(p_roi,trackPattern.mmSegment) );

  // calcurate weighted sum of hits, get superpoint information
  ATH_CHECK( calcWeightedSumHit(trackPattern) );

  return StatusCode::SUCCESS;

}

StatusCode TrigL2MuonSA::NswStationFitter::selectStgcHits(const LVL1::RecMuonRoI* p_roi,
                                                          TrigL2MuonSA::StgcHits& stgcHits)
{

  TrigL2MuonSA::StgcHits selectedStgcHits;
  selectedStgcHits.clear();

  // define region where RoI is near
  double etaMin = p_roi->eta() - 1.;
  double etaMax = p_roi->eta() + 1.;
  double phiMin = p_roi->phi() - 1.;
  double phiMax = p_roi->phi() + 1.;
  if( phiMin > M_PI ) phiMin -= 2*M_PI;
  if( phiMax > M_PI ) phiMax -= 2*M_PI;
  if( phiMin < -1.*M_PI ) phiMin += 2*M_PI;
  if( phiMax < -1.*M_PI ) phiMax += 2*M_PI;
  // boolian to check if sTGC hits are near RoI
  bool inRoiEta, inRoiPhi, inRoi;

  // loop over sTGC digits.
  unsigned int iHit;
  if (stgcHits.size()>0) {
    for (iHit = 0; iHit < stgcHits.size(); iHit++){
      // Get the digit point.
      TrigL2MuonSA::StgcHitData& hit = stgcHits[iHit];

      // check if sTGC hits are near RoI
      inRoiEta = false; inRoiPhi = false; inRoi = false;
      if ( etaMin<=hit.eta && hit.eta<=etaMax ) inRoiEta = true;
      if ( phiMin<=phiMax && phiMin<=hit.phi && hit.phi<=phiMax ) inRoiPhi= true;
      if ( phiMin>phiMax && (phiMin<=hit.phi || hit.phi<=phiMax)) inRoiPhi= true;
      if ( inRoiEta && inRoiPhi ) inRoi = true;
      ATH_MSG_DEBUG("sTGC hits eta = "<<hit.eta<<", phi = "<<hit.phi<<", r = "<<hit.r<<", z = "<<hit.z<<", stationEta = "<<hit.stationEta<<", stationPhi = "<<hit.stationPhi<<", channelType = "<<hit.channelType<<", stationName = "<<hit.stationName<<", matched RoI? = "<<inRoi);
      // pushback if the hit is near RoI
      if (!inRoi) continue;
      selectedStgcHits.push_back(hit);
    }
  }

  stgcHits.clear();
  stgcHits = selectedStgcHits;

  return StatusCode::SUCCESS;

}

StatusCode TrigL2MuonSA::NswStationFitter::selectMmHits(const LVL1::RecMuonRoI* p_roi,
                                                        TrigL2MuonSA::MmHits& mmHits)
{

  TrigL2MuonSA::MmHits selectedMmHits;
  selectedMmHits.clear();

  // define region where RoI is near
  double etaMin = p_roi->eta() - 1.;
  double etaMax = p_roi->eta() + 1.;
  double phiMin = p_roi->phi() - 1.;
  double phiMax = p_roi->phi() + 1.;
  if( phiMin > M_PI ) phiMin -= 2*M_PI;
  if( phiMax > M_PI ) phiMax -= 2*M_PI;
  if( phiMin < -1.*M_PI ) phiMin += 2*M_PI;
  if( phiMax < -1.*M_PI ) phiMax += 2*M_PI;
  // boolian to check if sTGC hits are near RoI
  bool inRoiEta, inRoiPhi, inRoi;

  // loop over MM digits.
  unsigned int iHit;
  if (mmHits.size()>0) {
    for (iHit = 0; iHit < mmHits.size(); iHit++){

      // Get the digit point.
      TrigL2MuonSA::MmHitData& hit = mmHits[iHit];

      // check if MM hits are near RoI
      inRoiEta = false; inRoiPhi = false; inRoi = false;
      if ( etaMin<=hit.eta && hit.eta<=etaMax ) inRoiEta = true;
      if ( phiMin<=phiMax && phiMin<=hit.phi && hit.phi<=phiMax ) inRoiPhi= true;
      if ( phiMin>phiMax && (phiMin<=hit.phi || hit.phi<=phiMax)) inRoiPhi= true;
      if ( inRoiEta && inRoiPhi ) inRoi = true;
      ATH_MSG_DEBUG("MM hits eta = "<<hit.eta<<", phi = "<<hit.phi<<", r = "<<hit.r<<", z = "<<hit.z<<", stationEta = "<<hit.stationEta<<", stationPhi = "<<hit.stationPhi<<", stationName = "<<hit.stationName<<", matched RoI? = "<<inRoi);

      // pushback if the hit is near RoI
      if (!inRoi) continue;
      selectedMmHits.push_back(hit);
    }
  }

  mmHits.clear();
  mmHits = selectedMmHits;

  return StatusCode::SUCCESS;

}

StatusCode TrigL2MuonSA::NswStationFitter::calcWeightedSumHit(TrigL2MuonSA::TrackPattern& trackPattern)
{

  TrigL2MuonSA::StgcHits stgcHits = trackPattern.stgcSegment;
  TrigL2MuonSA::MmHits mmHits = trackPattern.mmSegment;

  if(stgcHits.size()==0 && mmHits.size()==0)
    return StatusCode::SUCCESS;

  //superpoint information
  float rWeightedCenter=0.,zWeightedCenter=0.,phiWeightedCenter=0.;
  int totNumRWeightedCenter=0.,totNumZWeightedCenter=0.,totNumPhiWeightedCenter=0.;
  float localPhiCenter=0., localPhi;

  // loop over sTGC digits.
  unsigned int iHit;
  if (stgcHits.size()>0) {
    for (iHit = 0; iHit < stgcHits.size(); iHit++){

      // Get the digit point.
      TrigL2MuonSA::StgcHitData& hit = stgcHits[iHit];

      if (iHit==0 && hit.stationPhi<=5) localPhiCenter = 0.25 * M_PI * ((float)hit.stationPhi-1.);
      if (iHit==0 && hit.stationPhi> 5) localPhiCenter = 0.25 * M_PI * ((float)hit.stationPhi-9.);
      localPhi = hit.phi - localPhiCenter;
      if (localPhi > M_PI) localPhi -= 2.0*M_PI;
      if (localPhi < -1.*M_PI) localPhi += 2.0*M_PI;

      // calculate Weighted Center
      if (hit.channelType == 1) { // strip
	rWeightedCenter += hit.r;
	totNumRWeightedCenter += 1;
      }
      if (hit.channelType == 2) { // wire
	phiWeightedCenter += localPhi;
	totNumPhiWeightedCenter += 1;
      }
      zWeightedCenter += hit.z;
      totNumZWeightedCenter += 1;
    }
  }

  // loop over MM digits.
  if (mmHits.size()>0) {
    for (iHit = 0; iHit < mmHits.size(); iHit++){

      // Get the digit point.
      TrigL2MuonSA::MmHitData& hit = mmHits[iHit];

      localPhi = hit.phi - localPhiCenter;
      if (localPhi > M_PI) localPhi -= 2.0*M_PI;
      if (localPhi < -1.*M_PI) localPhi += 2.0*M_PI;
    }
  }

  // calculate Weighted Center
  if (totNumRWeightedCenter!=0) rWeightedCenter /= totNumRWeightedCenter;
  if (totNumPhiWeightedCenter!=0) phiWeightedCenter /= totNumPhiWeightedCenter;
  if (totNumZWeightedCenter!=0) zWeightedCenter /= totNumZWeightedCenter;

  // store superpoint info in TrackData
  xAOD::L2MuonParameters::Chamber inner = xAOD::L2MuonParameters::Chamber::EndcapInner;
  TrigL2MuonSA::SuperPoint* superPoint = &(trackPattern.superPoints[inner]);
  superPoint->R = rWeightedCenter;
  superPoint->Phim = phiWeightedCenter+localPhiCenter;
  superPoint->Z = zWeightedCenter;
  superPoint->Npoint = totNumZWeightedCenter;
  if (zWeightedCenter!=0) superPoint->Alin = rWeightedCenter/zWeightedCenter;
  superPoint->Blin = 0.;

  ATH_MSG_DEBUG("Nsw Super Point r/phi/z/slope = "<<superPoint->R<<"/"<<superPoint->Phim<<"/"<<superPoint->Z<<"/"<<superPoint->Alin);

  return StatusCode::SUCCESS;

}
