/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "NswStationFitter.h"
#include "xAODTrigMuon/TrigMuonDefs.h"

#include "RecMuonRoIUtils.h"
#include <cmath>
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include <vector>
#include <array>

TrigL2MuonSA::NswStationFitter::NswStationFitter(const std::string& type,
             const std::string& name,
             const IInterface*  parent):
  AthAlgTool(type,name,parent)
{
}

StatusCode TrigL2MuonSA::NswStationFitter::superPointFitter(const TrigRoiDescriptor* p_roids,
                                                            TrigL2MuonSA::TrackPattern& trackPattern) const
{

  ATH_MSG_DEBUG("NswStationFitter::findSuperPoints() was called.");

  // selection for sTGC hits, RoI matching based
  ATH_CHECK( selectStgcHits(p_roids,trackPattern.stgcSegment) );
  ATH_CHECK( selectMmHits(p_roids,trackPattern.mmSegment) );

  ATH_CHECK( findStgcHitsInSegment(trackPattern.stgcSegment) );
  ATH_MSG_DEBUG("@@STGC@@ number of STGC hits for SPs " << trackPattern.stgcSegment.size());
  ATH_CHECK( findMmHitsInSegment(trackPattern.mmSegment) );
  ATH_MSG_DEBUG("@@MM@@ number of MM hits for SPs " << trackPattern.mmSegment.size());

  ATH_MSG_DEBUG("Number of sTGC and MM hits for SPs " << trackPattern.stgcSegment.size() << " " << trackPattern.mmSegment.size());  
  if (trackPattern.stgcSegment.size() < 9 && trackPattern.mmSegment.size() < 6) {
    ATH_MSG_WARNING("Number of sTGC and MM hits for SPs is small " << trackPattern.stgcSegment.size() + trackPattern.mmSegment.size());  
  }
  else {
    ATH_CHECK( calcMergedHit(trackPattern) );
  }

  return StatusCode::SUCCESS;

}

StatusCode TrigL2MuonSA::NswStationFitter::selectStgcHits(const TrigRoiDescriptor* p_roids,
                                                          TrigL2MuonSA::StgcHits& stgcHits) const
{

  TrigL2MuonSA::StgcHits selectedStgcHits;
  selectedStgcHits.clear();

  // define region where RoI is near
  double etaMin = p_roids->eta() - 1.;
  double etaMax = p_roids->eta() + 1.;
  double phiMin = p_roids->phi() - 1.;
  double phiMax = p_roids->phi() + 1.;
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
      ATH_MSG_DEBUG("sTGC hits eta = "<<hit.eta<<", phi = "<<hit.phi<<", r = "<<hit.r<<", z = "<<hit.z<<", stationEta = "<<hit.stationEta<<", stationPhi = "<<hit.stationPhi<<", channelType = "<<hit.channelType<<", stationName = "<<hit.stationName<<",layerNumber ="<<hit.layerNumber<<", matched RoI? = "<<inRoi);
      // pushback if the hit is near RoI
      if (!inRoi){
         continue;
      }
      selectedStgcHits.push_back(hit);
    }
  }

  stgcHits.clear();
  stgcHits = selectedStgcHits;

  return StatusCode::SUCCESS;

}

StatusCode TrigL2MuonSA::NswStationFitter::selectMmHits(const TrigRoiDescriptor* p_roids,
                                                        TrigL2MuonSA::MmHits& mmHits) const
{

  TrigL2MuonSA::MmHits selectedMmHits;
  selectedMmHits.clear();

  // define region where RoI is near
  double etaMin = p_roids->eta() - 1.;
  double etaMax = p_roids->eta() + 1.;
  double phiMin = p_roids->phi() - 1.;
  double phiMax = p_roids->phi() + 1.;
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
      if (!inRoi){ 
        continue;
      }
      selectedMmHits.push_back(hit);
    }
  }

  mmHits.clear();
  mmHits = selectedMmHits;

  return StatusCode::SUCCESS;

}

StatusCode TrigL2MuonSA::NswStationFitter::calcWeightedSumHit(TrigL2MuonSA::TrackPattern& trackPattern) const
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
      if (hit.stationName == 57){
        localPhiCenter += M_PI/8.; // small sTGC sectors
        if (hit.stationPhi == 5) localPhiCenter -= 2 * M_PI;
      }
      
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
      
      if (hit.stationName == 55){
        localPhiCenter += M_PI/8.; // small MM sectors
        if (hit.stationPhi == 5) localPhiCenter -= 2 * M_PI;
      }

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

StatusCode TrigL2MuonSA::NswStationFitter::findStgcHitsInSegment(TrigL2MuonSA::StgcHits& stgcHits) const
{
  if(stgcHits.size() < 9) {
    ATH_MSG_INFO("Number of STGC hits is too small, at least 9 hits required : "<<stgcHits.size()<<" hits");
    return StatusCode::SUCCESS;
    } else if(stgcHits.size() > 100) {
    ATH_MSG_WARNING("Number of STGC hits is too large, at most (2^16 - 1) hits allowed : "<<stgcHits.size()<<" hits");
    return StatusCode::SUCCESS;
  }

  // cassifyDataEachLayer
  std::array<std::vector<int>, 8> strHitIdByLayer;
  std::array<std::vector<int>, 8> wireHitIdByLayer;
  for(unsigned int iHit = 0; iHit < stgcHits.size(); ++iHit){
    int layerNumber = stgcHits.at(iHit).layerNumber;
    if (layerNumber > 7) {
      ATH_MSG_WARNING("STGC hit layer number > 7");
      continue;
    }
    if(stgcHits.at(iHit).channelType == 1){
      strHitIdByLayer[layerNumber].push_back(iHit);
    }else if(stgcHits.at(iHit).channelType == 2){
      wireHitIdByLayer[layerNumber].push_back(iHit);
    }
  }
  ATH_MSG_DEBUG("@@STGC@@ strip Nhits " << strHitIdByLayer[0].size()
                         << " " << strHitIdByLayer[1].size()
                         << " " << strHitIdByLayer[2].size()
                         << " " << strHitIdByLayer[3].size()
                         << " " << strHitIdByLayer[4].size()
                         << " " << strHitIdByLayer[5].size()
                         << " " << strHitIdByLayer[6].size()
                         << " " << strHitIdByLayer[7].size());
  ATH_MSG_DEBUG("@@STGC@@ wire Nhits " << wireHitIdByLayer[0].size()
                         << " " << wireHitIdByLayer[1].size()
                         << " " << wireHitIdByLayer[2].size()
                         << " " << wireHitIdByLayer[3].size()
                         << " " << wireHitIdByLayer[4].size()
                         << " " << wireHitIdByLayer[5].size()
                         << " " << wireHitIdByLayer[6].size()
                         << " " << wireHitIdByLayer[7].size());

  ATH_MSG_DEBUG("check point 1");
  std::vector< std::array<int, 8> > strHitIds; // Candidates' sets of strip hit ids in 8 layers
  findSetOfStgcHitIds(stgcHits, strHitIdByLayer, strHitIds);
  ATH_MSG_DEBUG("check point 2");
  std::vector< std::array<int, 8> > wireHitIds; // Candidates' sets of wire hit ids in 8 layers
  findSetOfStgcHitIds(stgcHits, wireHitIdByLayer, wireHitIds);
  ATH_MSG_DEBUG("check point 3");
  ATH_MSG_DEBUG("@@STGC@@ strip wire " << strHitIds.size() << " " << wireHitIds.size());
  TrigL2MuonSA::StgcHits selectedStgcHits;
  selectedStgcHits.clear();
  
  bool isLargeStrip = false;
  bool isSmallStrip = false;
  for (unsigned int iHit = 0; iHit < strHitIds.size(); ++iHit) {
    std::array<int, 8> hitIds = strHitIds.at(iHit);
    for (unsigned int iLayer = 0; iLayer < 8; ++iLayer) {
      if (hitIds[iLayer] != -1) {
        selectedStgcHits.push_back(stgcHits.at(hitIds[iLayer]));
        if(stgcHits.at(hitIds[iLayer]).stationName == 58) isLargeStrip = true;
        else if(stgcHits.at(hitIds[iLayer]).stationName == 57) isSmallStrip = true;
      }
    }
  }
  for (unsigned int iHit = 0; iHit < wireHitIds.size(); ++iHit) {
    std::array<int, 8> hitIds = wireHitIds.at(iHit);
    for (unsigned int iLayer = 0; iLayer < 8; ++iLayer) {
      if (hitIds[iLayer] != -1) {
        if(isLargeStrip){
          if(stgcHits.at(hitIds[iLayer]).stationName != 58) continue;
          selectedStgcHits.push_back(stgcHits.at(hitIds[iLayer]));
        }
        else if(isSmallStrip){
          if(stgcHits.at(hitIds[iLayer]).stationName != 57) continue;
          selectedStgcHits.push_back(stgcHits.at(hitIds[iLayer]));
        }
      }
    }
  }
  stgcHits.clear();
  stgcHits = selectedStgcHits;
  ATH_MSG_DEBUG("check point 4");
  return StatusCode::SUCCESS;

}

void TrigL2MuonSA::NswStationFitter::findSetOfStgcHitIds(TrigL2MuonSA::StgcHits& stgcHits,
                                                         std::array<std::vector<int>,8> hitIdByLayer,
                                                         std::vector<std::array<int, 8>>& hitIdsCandidate) const
{
  double NSWCenterZ = 7526.329; //FIX ME!!!

  bool isStrip = 0;
  for (unsigned int iLayer = 0; iLayer < 8; ++iLayer) {
    if ( hitIdByLayer[iLayer].size() > 0) {
      if ( stgcHits.at(hitIdByLayer[iLayer].at(0)).channelType == 1 ){
        isStrip = 1;
        break;
      }
    }
  }
  ATH_MSG_DEBUG("check point 2.1");
  std::array<std::vector<unsigned long int>,4> hitIdsInTwo;
  std::array<std::vector<double>,4> slopeInTwo;
  std::array<std::vector<double>,4> interceptInTwo;

  // Loop over pairs of the i-th and the (i+4)-th layers
  for(unsigned int iPair = 0; iPair < 4; ++iPair){
    ATH_MSG_DEBUG("check point 2.1.1 " << iPair);
    unsigned int nHitsInInner = hitIdByLayer[iPair].size();
    unsigned int nHitsInOuter = hitIdByLayer[iPair+4].size();
    if ( nHitsInInner > 0xffff-1 || nHitsInOuter > 0xffff-1) {
      ATH_MSG_WARNING("Number of Stgc hits in layers exceeds the limit of (2^16 - 1) :  Number of Stgc hits in "<<iPair<<"th layer = "<< nHitsInInner
                      <<", Number of Stgc hits in "<<iPair+4<<"th layer = "<<nHitsInOuter);
      ATH_MSG_WARNING("Number of Stgc hits is limitted to (2^16 - 1) and hits with id more than (2^16 -1) will be trancated.");
      if (nHitsInInner > 0xffff-1) {nHitsInInner = 0xffff-1;}
      if (nHitsInOuter > 0xffff-1) {nHitsInOuter = 0xffff-1;}
    }

    bool foundCounterparts[256] = {};
    // Loop over hits in the i-th layer
    for(unsigned int iHit = 0; iHit < nHitsInInner; ++iHit){
      ATH_MSG_DEBUG("check point 2.1.1.1 " << iPair << " " << iHit);
      bool foundCounterpart = 0;

      double z[2] = {};
      double r[2] = {};

      int iHitId = hitIdByLayer[iPair].at(iHit);
      if(isStrip){
        r[0] = stgcHits.at(iHitId).r;
        z[0] = stgcHits.at(iHitId).z;
      } else {
        double localPhiCenter;
        if (stgcHits.at(iHitId).stationPhi<=5) {
          localPhiCenter = 0.25 * M_PI * ((double)stgcHits.at(iHitId).stationPhi-1.);
        } else {
          localPhiCenter = 0.25 * M_PI * ((double)stgcHits.at(iHitId).stationPhi-9.);
        }
        
        if (stgcHits.at(iHitId).stationName == 57){
          localPhiCenter += M_PI/8.; // small sTGC sectors
          if (stgcHits.at(iHitId).stationPhi == 5) localPhiCenter -= 2. * M_PI;
        }
      
        double phiProj = stgcHits.at(iHitId).phi - localPhiCenter;
        if (phiProj > M_PI) phiProj -= 2.0*M_PI;
        if (phiProj < -1.*M_PI) phiProj += 2.0*M_PI;
        r[0] = stgcHits.at(iHitId).r;
        z[0] = phiProj;
      }

      // Loop over hits in the (i+4)-th layer
      for(unsigned int jHit = 0; jHit < nHitsInOuter; ++jHit){
        ATH_MSG_DEBUG("check point 2.1.1.1.1 " << iPair << " " << iHit << " " << jHit);
        int jHitId = hitIdByLayer[iPair+4].at(jHit);
        double slope, intercept;
        if(isStrip) {
          r[1] = stgcHits.at(jHitId).r;
          z[1] = stgcHits.at(jHitId).z;
          slope = (r[1] - r[0]) / (z[1] - z[0]);
          intercept = slope*(0. - z[0]) + r[0];
          // select pairs whose slops in limited regions
          if(std::abs(slope) < 0.14 || std::abs(slope) > 0.6 || std::abs(intercept) > 300.) continue;
        } else {
          double localPhiCenter;
          if (stgcHits.at(jHitId).stationPhi<=5) {
            localPhiCenter = 0.25 * M_PI * ((double)stgcHits.at(jHitId).stationPhi-1.);
          } else {
            localPhiCenter = 0.25 * M_PI * ((double)stgcHits.at(jHitId).stationPhi-9.);
          }
          
          if (stgcHits.at(jHitId).stationName == 57){
            localPhiCenter += M_PI/8.; // small sTGC sectors
            if (stgcHits.at(jHitId).stationPhi == 5) localPhiCenter -= 2 * M_PI;
          }
      
          double phiProj = stgcHits.at(jHitId).phi - localPhiCenter;
          if (phiProj > M_PI) phiProj -= 2.0*M_PI;
          if (phiProj < -1.*M_PI) phiProj += 2.0*M_PI;
          r[1] = stgcHits.at(jHitId).r;
          z[1] = phiProj;
          slope = (z[0]+z[1])/2.;
          intercept = (r[0]+r[1])/2.;
          if(std::abs(r[0]*std::sin(z[0]) - r[1]*std::sin(z[1])) > 300.) continue;
        }

        unsigned int encodedIds = (iHitId<<16) + jHitId;
        hitIdsInTwo[iPair].push_back(encodedIds);
        slopeInTwo[iPair].push_back(slope);
        interceptInTwo[iPair].push_back(intercept);

        foundCounterpart = 1;
        foundCounterparts[jHit] = 1;
      }//end of jHit in the (i+4)-th layer
      if(!foundCounterpart){ // in case of no counterpart in the (i+4)-th layer
        unsigned int encodedIds = (iHitId<<16) + 0xffff; // fill all bits with 1 for hit id for the layer with no hit
        hitIdsInTwo[iPair].push_back(encodedIds);
        if(isStrip) {
          slopeInTwo[iPair].push_back(r[0]/z[0]);
          interceptInTwo[iPair].push_back(0.);
        } else {
          slopeInTwo[iPair].push_back(z[0]);
          interceptInTwo[iPair].push_back(r[0]);
        }
      }
    }//end of iHit in the i-th layer
    // Loop over hits in the (i+4)-th layer
    for(unsigned int jHit = 0; jHit < nHitsInOuter; ++jHit){
      ATH_MSG_DEBUG("check point 2.1.1.1.2 " << iPair << " " << jHit);
      if (!foundCounterparts[jHit]) {
        int jHitId = hitIdByLayer[iPair+4].at(jHit);
        unsigned int encodedIds = 0xffff0000 + jHitId; // fill all bits with 1 for hit id for the layer with no hit
        hitIdsInTwo[iPair].push_back(encodedIds);
        if(isStrip) {
          slopeInTwo[iPair].push_back(stgcHits.at(jHitId).r/stgcHits.at(jHitId).z);
          interceptInTwo[iPair].push_back(0.);
        } else {
          double localPhiCenter;
          if (stgcHits.at(jHitId).stationPhi<=5) {
            localPhiCenter = 0.25 * M_PI * ((double)stgcHits.at(jHitId).stationPhi-1.);
          } else {
            localPhiCenter = 0.25 * M_PI * ((double)stgcHits.at(jHitId).stationPhi-9.);
          }
          if (stgcHits.at(jHitId).stationName == 57){
            localPhiCenter += M_PI/8.; // small sTGC sectors
            if (stgcHits.at(jHitId).stationPhi == 5) localPhiCenter -= 2 * M_PI;
          }
      
          double phiProj = stgcHits.at(jHitId).phi - localPhiCenter;
          if (phiProj > M_PI) phiProj -= 2.0*M_PI;
          if (phiProj < -1.*M_PI) phiProj += 2.0*M_PI;
          slopeInTwo[iPair].push_back(phiProj);
          interceptInTwo[iPair].push_back(stgcHits.at(jHitId).r);
        }
      }
    }
  }//end of pair loop

  ATH_MSG_DEBUG("@@STGC@@ isStrip= " << isStrip << " Npairs " << hitIdsInTwo[0].size() << " " << hitIdsInTwo[1].size() << " " << hitIdsInTwo[2].size() << " " << hitIdsInTwo[3].size());
  for (unsigned int iLayer = 0; iLayer < 4; ++ iLayer) {
    for (unsigned int iPair = 0; iPair < slopeInTwo[iLayer].size(); ++iPair) {
      ATH_MSG_DEBUG("@@STGC@@ pair fit isStrip= " << isStrip << " slope= " << slopeInTwo[iLayer].at(iPair) << " intercept= " << interceptInTwo[iLayer].at(iPair));
    }
  }
   
  std::array<std::vector<unsigned long int>,2> hitIdsInFour;
  std::array<std::vector<double>,2> slopeInFour;
  std::array<std::vector<double>,2> interceptInFour;
  ATH_MSG_DEBUG("check point 2.2");
  for(unsigned int iQuad = 0; iQuad < 2; ++iQuad){
    ATH_MSG_DEBUG("check point 2.2.1 " << iQuad);
    unsigned int nPairsInInner = hitIdsInTwo[iQuad].size();
    unsigned int nPairsInOuter = hitIdsInTwo[iQuad+2].size();

    bool foundCounterparts[0xffff] = {};
    for(unsigned int iPair = 0; iPair < nPairsInInner; ++iPair){
      ATH_MSG_DEBUG("check point 2.2.1.1 " << iQuad << " " << iPair);
      bool foundCounterpart = 0;
      double slope[2];
      double intercept[2];
      slope[0] = slopeInTwo[iQuad].at(iPair);
      intercept[0] = interceptInTwo[iQuad].at(iPair);

      for(unsigned int jPair = 0; jPair < nPairsInOuter; ++jPair){
        ATH_MSG_DEBUG("check point 2.2.1.1.1 " << iQuad << " " << iPair << " " << jPair);
        unsigned int ihitIds = hitIdsInTwo[iQuad].at(iPair);
        unsigned int jhitIds = hitIdsInTwo[iQuad+2].at(jPair);
        if ( !(((ihitIds>>16 & 0xffff) != 0xffff || (ihitIds & 0xffff) != 0xffff) &&
               ((jhitIds>>16 & 0xffff) != 0xffff || (jhitIds & 0xffff) != 0xffff ))    ) continue; // require at least 2 hits in 4 layers

        slope[1] = slopeInTwo[iQuad+2].at(jPair);
        intercept[1] = interceptInTwo[iQuad+2].at(jPair);

        if (isStrip) {
          double spR0 = slope[0] * NSWCenterZ + intercept[0];
          double spR1 = slope[1] * NSWCenterZ + intercept[1];
          if(std::abs(spR1 - spR0) > 50.) continue;  //OPTIMIZE ME!!!
        } else {
          if(std::abs(intercept[0]*std::sin(slope[0]) - intercept[1]*std::sin(slope[1])) > 100.) continue;
        }

        foundCounterpart = 1;
        foundCounterparts[jPair] = 1;

        unsigned long int encodedIds = (hitIdsInTwo[iQuad].at(iPair) << 32 ) + hitIdsInTwo[iQuad+2].at(jPair);
        hitIdsInFour[iQuad].push_back(encodedIds);
        slopeInFour[iQuad].push_back((slope[1] + slope[0])/2.);
        interceptInFour[iQuad].push_back((intercept[1] + intercept[0])/2.);
      }// end of iPair of the i-th and (i+4)-th layers, i=0,2

      if(foundCounterpart) continue; // in case of no counterpart of pairs in the inner layers
      if((hitIdsInTwo[iQuad].at(iPair)>>16 & 0xffff) == 0xffff || (hitIdsInTwo[iQuad].at(iPair) & 0xffff) == 0xffff) continue;

      unsigned long int encodedIds = (hitIdsInTwo[iQuad].at(iPair) << 32 ) + 0xffffffff;
      hitIdsInFour[iQuad].push_back(encodedIds);
      slopeInFour[iQuad].push_back(slope[0]);
      interceptInFour[iQuad].push_back(intercept[0]);
    }// end of jPair of the j-th and (j+4)-th layers, j=1,3
    for (unsigned int jPair = 0; jPair < nPairsInOuter; ++jPair) {
      ATH_MSG_DEBUG("check point 2.2.1.1.2 " << iQuad << " " << " " << jPair);
      if(foundCounterparts[jPair]) continue; // in case of no counterpart of pairs in the outner layers
      if((hitIdsInTwo[iQuad+2].at(jPair)>>16 & 0xffff) == 0xffff || (hitIdsInTwo[iQuad+2].at(jPair) & 0xffff) == 0xffff) continue;
//      unsigned long int encodedIds = (0xffffffff << 32) + hitIdsInTwo[iQuad+2].at(jPair);
      unsigned long int encodedIds = (0xffffffff00000000 ) + hitIdsInTwo[iQuad+2].at(jPair);
      hitIdsInFour[iQuad].push_back(encodedIds);
      slopeInFour[iQuad].push_back(slopeInTwo[iQuad+2].at(jPair));
      interceptInFour[iQuad].push_back(interceptInTwo[iQuad+2].at(jPair));
    }
  }// end of quad loop

  ATH_MSG_DEBUG("@@STGC@@ isStrip= " << isStrip << " Nquads " << hitIdsInFour[0].size() << " " << hitIdsInFour[1].size());
  for (unsigned int iLayer = 0; iLayer < 2; ++ iLayer) {
    for (unsigned int iQuad = 0; iQuad < slopeInFour[iLayer].size(); ++iQuad) {
      ATH_MSG_DEBUG("@@STGC@@ quad fit isStrip= " << isStrip << " slope= " << slopeInFour[iLayer].at(iQuad) << " intercept= " << interceptInFour[iLayer].at(iQuad));
    }
  }

  ATH_MSG_DEBUG("check point 2.3");
  std::vector< std::array<int, 8> > hitIdsInEight;
  std::vector<double> mseInEight;

  unsigned int nQuadInInner = hitIdsInFour[0].size();
  unsigned int nQuadInOuter = hitIdsInFour[1].size();

  for(unsigned int iQuad = 0; iQuad < nQuadInInner; ++iQuad){
    ATH_MSG_DEBUG("check point 2.3.1" << iQuad);
    double slope[2];
    double intercept[2];
    slope[0] = slopeInFour[0].at(iQuad);
    intercept[0] = interceptInFour[0].at(iQuad);

    for(unsigned int jQuad = 0; jQuad < nQuadInOuter; ++jQuad){
      ATH_MSG_DEBUG("check point 2.3.1.1" << iQuad << " " << jQuad);
      unsigned long int ihitIds = hitIdsInFour[0].at(iQuad);
      unsigned long int jhitIds = hitIdsInFour[1].at(jQuad);
      int nOfLayersWithNoHit = 0;
      for (unsigned int iLayer = 0; iLayer < 4; ++iLayer) {
        if ( (ihitIds>>(3-iLayer)*16 & 0xffff) == 0xffff ) {++nOfLayersWithNoHit;}
        if ( (jhitIds>>(3-iLayer)*16 & 0xffff) == 0xffff ) {++nOfLayersWithNoHit;}
      }
      if (nOfLayersWithNoHit > 4) continue; // require at least 4 hits in 8 layers

      slope[1] = slopeInFour[1].at(jQuad);
      intercept[1] = interceptInFour[1].at(jQuad);

      if(isStrip) {
        double spR0 = slope[0] * NSWCenterZ + intercept[0];
        double spR1 = slope[1] * NSWCenterZ + intercept[1];
        if(std::abs(spR1 - spR0) > 10. ||
           std::abs(intercept[1] + intercept[0]) / 2 > 100.) continue; // OPTIMIZE ME!!!
      } else {
        if(std::abs(intercept[0]*std::sin(slope[0]) - intercept[1]*std::sin(slope[1])) > 100. ) continue;
      }

      std::array<int,8> setOfHitIds = {-1,-1,-1,-1,-1,-1,-1,-1};
      std::vector<double> r, z;
      for(unsigned int i = 0; i < 8; ++i) {
        unsigned int iHitId, iLayer = 0;
        if (i <= 3) {
          iHitId = (unsigned int) ((ihitIds>>(3-i)*16) & 0xffff);
          iLayer = i+3*(i%2);
        } else {
          iHitId = (unsigned int) ((jhitIds>>(3-i%4)*16) & 0xffff);
          iLayer = (i-4)+3*(i%2)+1;
        }
        ATH_MSG_DEBUG("check point 2.3.1.1.1" << iQuad << " " << jQuad << " " << iLayer);
        if ( iHitId != 0xffff ) {
          if (isStrip) {
            r.push_back(stgcHits.at(iHitId).r);
            z.push_back(stgcHits.at(iHitId).z);
          }
          else {
            double localPhiCenter;
            if (stgcHits.at(iHitId).stationPhi<=5) {
              localPhiCenter = 0.25 * M_PI * ((double)stgcHits.at(iHitId).stationPhi-1.);
            } else {
              localPhiCenter = 0.25 * M_PI * ((double)stgcHits.at(iHitId).stationPhi-9.);
            }
            if (stgcHits.at(iHitId).stationName == 57){
              localPhiCenter += M_PI/8.; // small sTGC sectors
              if (stgcHits.at(iHitId).stationPhi == 5) localPhiCenter -= 2 * M_PI;
            }
           
            double phiProj = stgcHits.at(iHitId).phi - localPhiCenter;
            if (phiProj > M_PI) phiProj -= 2.0*M_PI;
            if (phiProj < -1.*M_PI) phiProj += 2.0*M_PI;
            r.push_back(phiProj);
            z.push_back(stgcHits.at(iHitId).r);
          }
          setOfHitIds[iLayer] = iHitId;
          ATH_MSG_DEBUG("@@STGC@@ strip_pos iHitId " << iLayer << " " << iHitId);
        }
      } 
      double slopefit=0., interceptfit=99999., mse =-1.;
      if (isStrip) {
        LinearFit(z,r,&slopefit,&interceptfit,&mse);
      } else {
        double phiavg = 0.;
        for (unsigned int iHit = 0; iHit < r.size(); ++iHit){
          phiavg += r.at(iHit);
        }
        phiavg /= r.size();
        mse = 0.;
        for (unsigned int iHit = 0; iHit < r.size(); ++iHit){
          mse += std::pow(r.at(iHit) - phiavg,2);
        }
      }
      hitIdsInEight.push_back(setOfHitIds);
      mseInEight.push_back(mse);
    } // end of quad loop in the outer layers
  } // end of quad loop in the inner layers
  ATH_MSG_DEBUG("check point 2.4");
  if(!hitIdsInEight.size()){
    ATH_MSG_WARNING("No candidate segment found in STGC");
    return;
  }

  ATH_MSG_DEBUG("@@STGC@@ isStrip= " << isStrip << " Noctets " << hitIdsInEight.size());
  std::vector<int> nOctetSegments; 
  std::vector<int> patternStationName; 
  
  for (unsigned int iOctet = 0; iOctet < hitIdsInEight.size(); ++iOctet) {

    bool isFirstHit = true;
    int hitStationName = 0;

    int nOctetSegment = 0;
    ATH_MSG_DEBUG("@@STGC@@ octet fit isStrip= " << isStrip << " mse " << mseInEight.at(iOctet));
    std::array<int, 8> tmpOctet = hitIdsInEight.at(iOctet);
    for (unsigned int iLayer = 0; iLayer < 8; ++iLayer) {
      if (tmpOctet[iLayer] != -1) {

        if(isFirstHit){
          hitStationName = stgcHits.at(tmpOctet[iLayer]).stationName;
          isFirstHit = false;

          ATH_MSG_DEBUG("@@STGC@@ octet pos isStrip= " << isStrip << " r= " << stgcHits.at(tmpOctet[iLayer]).r << " phi= " << stgcHits.at(tmpOctet[iLayer]).phi << " z= " << stgcHits.at(tmpOctet[iLayer]).z);
          nOctetSegment++;
        }
        else if(stgcHits.at(tmpOctet[iLayer]).stationName == hitStationName){
          ATH_MSG_DEBUG("@@STGC@@ octet pos isStrip= " << isStrip << " r= " << stgcHits.at(tmpOctet[iLayer]).r << " phi= " << stgcHits.at(tmpOctet[iLayer]).phi << " z= " << stgcHits.at(tmpOctet[iLayer]).z);
          nOctetSegment++;
        }
      }
    }
    nOctetSegments.push_back(nOctetSegment);
    patternStationName.push_back(hitStationName);
  }
  double nOcSegMax = 0;
  for(unsigned int iOctet = 0;  iOctet < hitIdsInEight.size(); ++iOctet){
    if(nOctetSegments.at(iOctet) > nOcSegMax){
      nOcSegMax = nOctetSegments.at(iOctet);
    }
  }

  double msemin = 1000000.;
  double mseminWireL = 1000000.; // Large sector
  double mseminWireS = 1000000.; // Small sector
  
  std::vector<int> octetIds(2,-1);

  if(isStrip){   
    for(unsigned int iOctet = 0; iOctet < hitIdsInEight.size(); ++iOctet){
      ATH_MSG_DEBUG("check point 2.4.1" << iOctet);
      if(nOctetSegments.at(iOctet) != nOcSegMax){
        continue;
      }
      if( mseInEight.at(iOctet) < msemin) {
        msemin = mseInEight.at(iOctet);
      }
      ATH_MSG_DEBUG("check point 2.4.2" << iOctet);
    }// end of Octet loop
    for(unsigned int iOctet = 0; iOctet < hitIdsInEight.size(); ++iOctet){
      if(nOctetSegments.at(iOctet) != nOcSegMax) continue;
      if(mseInEight.at(iOctet) != msemin){
        continue;
      }
      octetIds.push_back(iOctet);
    }
  } else { 
    for(unsigned int iOctet = 0;  iOctet < hitIdsInEight.size(); ++iOctet){
      ATH_MSG_DEBUG("check point 2.4.1" << iOctet);
      if(patternStationName.at(iOctet) == 58){
        if( mseInEight.at(iOctet) < mseminWireL) {
          mseminWireL = mseInEight.at(iOctet);
        }
      } 
      else if(patternStationName.at(iOctet) == 57){
        if( mseInEight.at(iOctet) < mseminWireS) {
          mseminWireS = mseInEight.at(iOctet);
        }
      } 
    }// end of Octet loop
    for(unsigned int iOctet = 0; iOctet < hitIdsInEight.size(); ++iOctet){
      if(patternStationName.at(iOctet) == 58){
        if(mseInEight.at(iOctet) != mseminWireL){
          continue;
        }
      }
      else if(patternStationName.at(iOctet) == 57){
        if(mseInEight.at(iOctet) != mseminWireS){
          continue;
        }
      }
      octetIds.push_back(iOctet);
    }
  }
  for(unsigned int ids = 0; ids < octetIds.size(); ids++){
    if (octetIds.at(ids) != -1) {
      hitIdsCandidate.push_back(hitIdsInEight.at(octetIds.at(ids)));
    }
  }
}

void TrigL2MuonSA::NswStationFitter::LinearFit(std::vector<double>& x,std::vector<double>& y,
                                               double* slope, double* intercept, double* mse) const
{
  double sumX=0, sumY=0, sumXY=0, sumX2=0;
  int nHits = x.size();
  for (unsigned int iHit = 0; iHit < x.size(); ++iHit){
    sumX += x.at(iHit);
    sumY += y.at(iHit);
    sumXY += x.at(iHit)*y.at(iHit);
    sumX2 += x.at(iHit)*x.at(iHit);
  }
  *slope = (nHits * sumXY - sumX * sumY) / (nHits * sumX2 - (sumX * sumX));
  *intercept = (sumX2 * sumY - sumXY * sumX) / (nHits * sumX2 - sumX * sumX);
  *mse = 0.;  
  for(unsigned int iHit = 0; iHit< x.size(); ++iHit){
    *mse += std::pow(y.at(iHit) - (*slope * x.at(iHit) + *intercept), 2.0);
  }
  *mse = *mse / (nHits - 2);
}

void TrigL2MuonSA::NswStationFitter::LinearFitWeight(std::vector<double>& x,std::vector<double>& y,
                   std::vector<bool>& isStgc, double* slope, double* intercept, double* mse) const
{
  double sigma_stgc = 1; // resolution
  double sigma_mm   = 1;
  double w_stgc = 1/std::pow(sigma_stgc,2);
  double w_mm = 1/std::pow(sigma_mm,2);
  double sumX=0, sumY=0, sumXY=0, sumX2=0, sumW=0;
  int nHits = x.size();
  for (unsigned int iHit = 0; iHit < x.size(); ++iHit){
    if(isStgc.at(iHit)){
      sumX  += w_stgc * x.at(iHit);
      sumY  += w_stgc * y.at(iHit);
      sumXY += w_stgc * x.at(iHit) * y.at(iHit);
      sumX2 += w_stgc * x.at(iHit) * x.at(iHit);
      sumW  += w_stgc;
    } else {
      sumX  += w_mm * x.at(iHit);
      sumY  += w_mm * y.at(iHit);
      sumXY += w_mm * x.at(iHit) * y.at(iHit);
      sumX2 += w_mm * x.at(iHit) * x.at(iHit);
      sumW  += w_mm;
    }
  }
  *slope = (sumW * sumXY - sumX * sumY) / (sumW * sumX2 - (sumX * sumX));
  *intercept = (sumX2 * sumY - sumXY * sumX) / (sumW * sumX2 - sumX * sumX);
  *mse = 0.;  
  for(unsigned int iHit = 0; iHit< x.size(); ++iHit){
    *mse += std::pow((y.at(iHit) - (*slope * x.at(iHit) + *intercept)), 2.0);
  }
  *mse = *mse / (nHits - 2);
}

StatusCode TrigL2MuonSA::NswStationFitter::calcMergedHit(TrigL2MuonSA::TrackPattern& trackPattern) const
{
  TrigL2MuonSA::StgcHits stgcHits = trackPattern.stgcSegment;
  TrigL2MuonSA::MmHits mmHits = trackPattern.mmSegment;

  bool isLargeStgc = false;
  bool isSmallStgc = false;
  for(unsigned int iHit = 0; iHit < stgcHits.size(); ++iHit) {
    if(stgcHits.at(iHit).stationName == 58) isLargeStgc = true; 
    else if(stgcHits.at(iHit).stationName == 57) isSmallStgc = true; 
    if(isLargeStgc && isSmallStgc) continue;
  }
  std::vector<double> r, z;
  std::vector<bool> isStgc;
  for(unsigned int iHit = 0; iHit < stgcHits.size(); ++iHit) {
    if (stgcHits.at(iHit).channelType == 1) {
      r.push_back(stgcHits.at(iHit).r);
      z.push_back(stgcHits.at(iHit).z);
      isStgc.push_back(true);
    }
  }
  for(unsigned int iHit = 0; iHit < mmHits.size(); ++iHit) {
    if(isLargeStgc){
      if(mmHits.at(iHit).stationName == 55) continue;
    }
    else if(isSmallStgc){
      if(mmHits.at(iHit).stationName == 56) continue;
    }
    if (mmHits.at(iHit).layerNumber < 2 || mmHits.at(iHit).layerNumber > 5) {
      r.push_back(mmHits.at(iHit).r);
      z.push_back(mmHits.at(iHit).z);
      isStgc.push_back(false);
    }
  }
  double slopefit=0., interceptfit=99999., mse=-1.;
  LinearFit(z,r,&slopefit,&interceptfit,&mse);
  ATH_MSG_DEBUG("@@Merge@@ stgc_mmX_fit slope= " << slopefit);
  ATH_MSG_DEBUG("@@Merge@@ stgc_mmX_fit intercept= " << interceptfit);
  ATH_MSG_DEBUG("@@Merge@@ stgc_mmX_fit mse= " << mse);

  std::vector<double> phiLocal;
  double localPhiCenter = 3.*M_PI;
  for (unsigned int iHit = 0; iHit < stgcHits.size(); ++iHit) {
    if (stgcHits.at(iHit).channelType != 2) {continue;}
      if (stgcHits.at(iHit).stationPhi<=5) {
        localPhiCenter = 0.25 * M_PI * ((double)stgcHits.at(iHit).stationPhi-1.);
      } else {
        localPhiCenter = 0.25 * M_PI * ((double)stgcHits.at(iHit).stationPhi-9.);
      }
      
      if (stgcHits.at(iHit).stationName == 57){
        localPhiCenter += M_PI/8.; // small sTGC sectors
        if (stgcHits.at(iHit).stationPhi == 5) localPhiCenter -= 2. * M_PI;
      }
    
    double phiProj = stgcHits.at(iHit).phi - localPhiCenter;
    if (phiProj >  M_PI) phiProj -= 2.0*M_PI;
    if (phiProj < -1.*M_PI) phiProj += 2.0*M_PI;
    double rInterpolate = slopefit * stgcHits.at(iHit).z + interceptfit;
    double rProj = stgcHits.at(iHit).r;
    phiLocal.push_back( std::atan(rProj/rInterpolate*std::tan(phiProj)) );
    ATH_MSG_DEBUG("@@Merge@@ philocalwire " << stgcHits.at(iHit).stationPhi << " " << stgcHits.at(iHit).stationName << " "
     << localPhiCenter << " " << stgcHits.at(iHit).phi << " " << stgcHits.at(iHit).r << " " << stgcHits.at(iHit).z);
    ATH_MSG_DEBUG("@@Merge@@ philocalwire " << rProj << " " << rInterpolate << " " << std::tan(phiProj) );
    ATH_MSG_DEBUG("@@Merge@@ philocalwire " << std::atan(rProj/rInterpolate*std::tan(phiProj)) );
  }
  double tanTiltAngleU = std::tan( 1.5/360.*2.*M_PI),
         tanTiltAngleV = std::tan(-1.5/360.*2.*M_PI); 
  for (unsigned int iHit = 0; iHit < mmHits.size(); ++iHit) {
    if(isLargeStgc){
      if(mmHits.at(iHit).stationName == 55) continue;
    }
    else if(isSmallStgc){
      if(mmHits.at(iHit).stationName == 56) continue;
    }
   
    if (localPhiCenter > 2.*M_PI) {
      if (mmHits.at(iHit).stationPhi<=5) {
        localPhiCenter = 0.25 * M_PI * ((double)mmHits.at(iHit).stationPhi-1.);
      } else {
        localPhiCenter = 0.25 * M_PI * ((double)mmHits.at(iHit).stationPhi-9.);
      }
      
      if (mmHits.at(iHit).stationName == 55){
        localPhiCenter += M_PI/8.; // small MM sectors
        if (mmHits.at(iHit).stationPhi == 5) localPhiCenter -= 2 * M_PI;
      }
    
    }
    if (mmHits.at(iHit).layerNumber >1 && mmHits.at(iHit).layerNumber < 6){
      double rInterpolate = slopefit * mmHits.at(iHit).z + interceptfit;
      double rProj = mmHits.at(iHit).r;
      if ((mmHits.at(iHit).layerNumber)%2 == 0) { // layer U
        phiLocal.push_back( std::atan((rProj-rInterpolate)/tanTiltAngleU/rInterpolate));
        ATH_MSG_DEBUG("@@Merge@@ philocalmmU " << std::atan((rProj-rInterpolate)/tanTiltAngleU/rInterpolate));
      } else {  // layer V      
        phiLocal.push_back( std::atan((rProj-rInterpolate)/tanTiltAngleV/rInterpolate));
        ATH_MSG_DEBUG("@@Merge@@ philocalmmV " << std::atan((rProj-rInterpolate)/tanTiltAngleV/rInterpolate));
      }
    }
  }
  double sumPhiLocal = 0;
  for (unsigned int iHit = 0; iHit < phiLocal.size(); ++iHit ) {
    sumPhiLocal += phiLocal.at(iHit);
  }
  double phiLocalAvg = sumPhiLocal/phiLocal.size();
  ATH_MSG_DEBUG("@@Merge@@ philocalAvg " << phiLocalAvg);

  r.clear();
  z.clear();
  isStgc.clear();
  std::vector<double> r_stgc, z_stgc, r_mm, z_mm;
  std::vector<bool> isStgc_stgc, isStgc_mm;
  for(unsigned int iHit = 0; iHit < stgcHits.size(); ++iHit) {
    if (stgcHits.at(iHit).stationPhi<=5) localPhiCenter = 0.25 * M_PI * ((double)stgcHits.at(iHit).stationPhi-1.);
    if (stgcHits.at(iHit).stationPhi> 5) localPhiCenter = 0.25 * M_PI * ((double)stgcHits.at(iHit).stationPhi-9.);
    if (stgcHits.at(iHit).channelType == 1) {
      r_stgc.push_back(stgcHits.at(iHit).r/std::cos(phiLocalAvg));
      z_stgc.push_back(stgcHits.at(iHit).z);
      isStgc_stgc.push_back(true);
      
      ATH_MSG_DEBUG("@@Merge@@ stgc strip_r " << phiLocalAvg << " " << stgcHits.at(iHit).z << " " << stgcHits.at(iHit).r/std::cos(phiLocalAvg));
    }

  }
  double slopefit_stgc=0., interceptfit_stgc=99999., mse_stgc=1.e20;
  if(r_stgc.size() == 0) {
    ATH_MSG_WARNING("No STGC hit to calculate superoint");
  } else {
    LinearFit(z_stgc,r_stgc,&slopefit_stgc,&interceptfit_stgc,&mse_stgc);
  }

  for(unsigned int iHit = 0; iHit < mmHits.size(); ++iHit) {
    if(isLargeStgc){
      if(mmHits.at(iHit).stationName == 55) continue;
    }
    else if(isSmallStgc){
      if(mmHits.at(iHit).stationName == 56) continue;
    }
    if (mmHits.at(iHit).layerNumber < 2 || mmHits.at(iHit).layerNumber > 5) {
      r_mm.push_back(mmHits.at(iHit).r/std::cos(phiLocalAvg));
      z_mm.push_back(mmHits.at(iHit).z);
      isStgc_mm.push_back(false);
    } else {
      z_mm.push_back(mmHits.at(iHit).z);
      isStgc_mm.push_back(false);
      
      double rProj = mmHits.at(iHit).r;
      if ((mmHits.at(iHit).layerNumber)%2 == 0) { // layer U
        double rPrime = (rProj * std::cos(1.5/360.*2.*M_PI))/(std::cos(phiLocalAvg)*std::cos(1.5/360.*2.*M_PI) + std::sin(phiLocalAvg)*std::sin(1.5/360.*2.*M_PI));
        r_mm.push_back(rPrime);
      } else { //layer V
        double rPrime = (rProj * std::cos(-1.5/360.*2.*M_PI))/(std::cos(phiLocalAvg)*std::cos(-1.5/360.*2.*M_PI) + std::sin(phiLocalAvg)*std::sin(-1.5/360.*2.*M_PI));
        r_mm.push_back(rPrime);
      }
    }
  }
  double slopefit_mm=0., interceptfit_mm=99999., mse_mm=1.e20;
  if(r_mm.size() == 0) {
    ATH_MSG_WARNING("No MM hit to calculate superoint");
  } else {
    LinearFit(z_mm,r_mm,&slopefit_mm,&interceptfit_mm,&mse_mm);
  }

  unsigned int fmerge = 0;
  slopefit=0., interceptfit=99999., mse=1.e20;
  if (mse_stgc < 1.e7 && mse_mm < 1.e7) {
    r = r_stgc;
    copy(r_mm.begin(), r_mm.end(), back_inserter(r));
    z = z_stgc;
    copy(z_mm.begin(), z_mm.end(), back_inserter(z));
    isStgc = isStgc_stgc;
    copy(isStgc_mm.begin(), isStgc_mm.end(), back_inserter(isStgc));
    LinearFitWeight(z,r,isStgc,&slopefit,&interceptfit,&mse);
    fmerge = 1;
  }
  if (mse > 1.e7) {
    if (mse_stgc < mse_mm) {
      slopefit = slopefit_stgc;
      interceptfit = interceptfit_stgc;
      mse = mse_stgc;
      fmerge = 2;
    } else {
      slopefit = slopefit_mm;
      interceptfit = interceptfit_mm;
      mse = mse_mm;
      fmerge = 3;
    }
  }
  if (mse > 1.e19) {
    ATH_MSG_WARNING("No sTGC and MM hit to calculate superoint");
    return StatusCode::SUCCESS;
  }

  // store superpoint info in TrackData
  xAOD::L2MuonParameters::Chamber inner = xAOD::L2MuonParameters::Chamber::EndcapInner;
  TrigL2MuonSA::SuperPoint* superPoint = &(trackPattern.superPoints[inner]);
  double NSWCenterZ = 7526.329; 
  superPoint->R = slopefit * NSWCenterZ + interceptfit;
  superPoint->Phim = phiLocalAvg+localPhiCenter;
  superPoint->Z = NSWCenterZ; 
  superPoint->Npoint = z.size();
  
  if (NSWCenterZ != 0) superPoint->Alin = slopefit;
  superPoint->Blin = interceptfit;

  ATH_MSG_DEBUG("Nsw Super Point r/phi/z/slope = "<<superPoint->R<<"/"<<superPoint->Phim<<"/"<<superPoint->Z<<"/"<<superPoint->Alin);

  ATH_MSG_DEBUG("@@Merge@@ Nsw Super Point r/phi/z/slope = "<<superPoint->R<<"/"<<superPoint->Phim<<"/"<<superPoint->Z<<"/"<<superPoint->Alin);
  ATH_MSG_DEBUG("@@Merge@@ fit slope= " << slopefit << " " << slopefit_stgc << " " << slopefit_mm);
  ATH_MSG_DEBUG("@@Merge@@ fit intercept= " << interceptfit << " " << interceptfit_stgc << " " << interceptfit_mm);
  ATH_MSG_DEBUG("@@Merge@@ fit mse= " << mse << " " << mse_stgc << " " << mse_mm);
  ATH_MSG_DEBUG("@@Merge@@ fit tech= " << fmerge);

  return StatusCode::SUCCESS;

}

StatusCode TrigL2MuonSA::NswStationFitter::findMmHitsInSegment(TrigL2MuonSA::MmHits& mmHits) const
{
  if(mmHits.size() < 6) {
    ATH_MSG_INFO("Number of MM hits is too small, at least 6 hits required : "<<mmHits.size()<<" hits");
    return StatusCode::SUCCESS;
    } else if(mmHits.size() > 100) {
    ATH_MSG_WARNING("Number of MM hits is too large, at most (2^16 - 1) hits allowed : "<<mmHits.size()<<" hits");
    return StatusCode::SUCCESS;
  }

  std::array< std::vector<int>, 8 > hitIdByLayer;
  for(unsigned int iHit = 0; iHit < mmHits.size(); ++iHit){
    int layerNumber = mmHits.at(iHit).layerNumber;
    if (layerNumber > 7) {
      ATH_MSG_WARNING("MM hit layer number > 7");
      continue;
    }
    hitIdByLayer[layerNumber].push_back(iHit);
  }
  ATH_MSG_DEBUG("@@MM@@ Nhits " << hitIdByLayer[0].size() 
                  << " " << hitIdByLayer[1].size()
                  << " " << hitIdByLayer[2].size()
                  << " " << hitIdByLayer[3].size()
                  << " " << hitIdByLayer[4].size()
                  << " " << hitIdByLayer[5].size()
                  << " " << hitIdByLayer[6].size()
                  << " " << hitIdByLayer[7].size());

  ATH_MSG_DEBUG("check point 5");
  std::vector< std::array<int, 8> > mmHitIds;
  findSetOfMmHitIds(mmHits, hitIdByLayer, mmHitIds);
  ATH_MSG_DEBUG("check point 6");
  TrigL2MuonSA::MmHits selectedMmHits;
  selectedMmHits.clear();
  for (unsigned int iHit = 0; iHit < mmHitIds.size(); ++iHit) {
    std::array<int, 8> hitIds = mmHitIds.at(iHit);
    for (unsigned int iLayer = 0; iLayer < 8; ++iLayer) {
      if (hitIds[iLayer] != -1) {
        selectedMmHits.push_back(mmHits.at(hitIds[iLayer]));
      }
    }
  }
  mmHits.clear();
  mmHits = selectedMmHits;
  ATH_MSG_DEBUG("check point 7");
  return StatusCode::SUCCESS;
}

void TrigL2MuonSA::NswStationFitter::findSetOfMmHitIds(TrigL2MuonSA::MmHits& mmHits,
                                                       std::array<std::vector<int>,8> hitIdByLayer,
                                                       std::vector<std::array<int, 8>>& hitIdsCandidate) const
{
  
  double NSWCenterZ = 7526.329;

  std::array<std::vector<unsigned long int>,4> hitIdsInTwo;
  std::array<std::vector<double>,4> slopeInTwo;
  std::array<std::vector<double>,4> interceptInTwo;
  ATH_MSG_DEBUG("check point 5.1");
  // Loop over pairs of the i-th and the (i+6)-th layers
  for(unsigned int iPair = 0; iPair < 4; ++iPair){

    unsigned int nHitsInInner = hitIdByLayer[iPair].size();
    unsigned int nHitsInOuter;
    if (iPair < 2) { // paris of X layers
      nHitsInOuter = hitIdByLayer[iPair+6].size();
    } else { // pairs of U or V layers
      nHitsInOuter = hitIdByLayer[iPair+2].size();
    }

    if ( nHitsInInner > 0xffff-1 || nHitsInOuter > 0xffff-1) {
      ATH_MSG_WARNING("Number of Mm hits in layers exceeds the limit of (2^16 - 1) :  Number of Mm hits in "<<iPair<<"th layer = "<< nHitsInInner
                      <<", Number of Mm hits in "<<iPair+4<<"th layer = "<<nHitsInOuter);
      ATH_MSG_WARNING("Number of Mm hits is limitted to (2^16 - 1) and hits with id more than (2^16 -1) will be trancated.");
      if (nHitsInInner > 0xffff-1) {nHitsInInner = 0xffff-1;}
      if (nHitsInOuter > 0xffff-1) {nHitsInOuter = 0xffff-1;}
    }

    bool foundCounterparts[0xffff] = {};
    // Loop over hits in the i-th layer
    for(unsigned int iHit = 0; iHit < nHitsInInner; ++iHit){

      bool foundCounterpart = 0;

      double z[2] = {};
      double r[2] = {};

      int iHitId = hitIdByLayer[iPair].at(iHit);
      r[0] = mmHits.at(iHitId).r;
      z[0] = mmHits.at(iHitId).z;

      // Loop over hits in the (i+6)-th layer
      for(unsigned int jHit = 0; jHit < nHitsInOuter; ++jHit){

        int jHitId;
        if (iPair < 2) {
          jHitId = hitIdByLayer[iPair+6].at(jHit);
        } else {
          jHitId = hitIdByLayer[iPair+2].at(jHit);
        }
        r[1] = mmHits.at(jHitId).r;
        z[1] = mmHits.at(jHitId).z;

        double slope = (r[1] - r[0]) / (z[1] - z[0]);
        double intercept = slope*(0. - z[0]) + r[0];
        // select pairs whose slops in limited regions
        if(std::abs(slope) < 0.1 || std::abs(slope) > 0.7 || std::abs(intercept) > 500.) continue;

        int encodedIds = (iHitId<<16) + jHitId;
        hitIdsInTwo[iPair].push_back(encodedIds);
        slopeInTwo[iPair].push_back(slope);
        interceptInTwo[iPair].push_back(intercept);

        foundCounterpart = 1;
        foundCounterparts[jHit] = 1;
      }//end of jHit in the (i+6)-th layer
      if(!foundCounterpart){ // in case of no counterpart in the (i+4)-th layer
        int encodedIds = (iHitId<<16) + 0xffff; // fill all bits with 1 for hit id for the layer with no hit
        hitIdsInTwo[iPair].push_back(encodedIds);
        slopeInTwo[iPair].push_back(r[0]/z[0]);
        interceptInTwo[iPair].push_back(0.);
      }
    }//end of iHit in the i-th layer
    // Loop over hits in the (i+4)-th layer
    for(unsigned int jHit = 0; jHit < nHitsInOuter; ++jHit){
      if (!foundCounterparts[jHit]) {
        int jHitId;
        if (iPair < 2) {
          jHitId = hitIdByLayer[iPair+6].at(jHit);
        } else {
          jHitId = hitIdByLayer[iPair+2].at(jHit);
        }
        int encodedIds = (0xffff<<16) + jHitId; // fill all bits with 1 for hit id for the layer with no hit
        hitIdsInTwo[iPair].push_back(encodedIds);
        slopeInTwo[iPair].push_back(mmHits.at(jHitId).r/mmHits.at(jHitId).z);
        interceptInTwo[iPair].push_back(0.);
      }
    }
  }//end of pair loop
  ATH_MSG_DEBUG("@@MM@@ Npairs " << hitIdsInTwo[0].size() << " " << hitIdsInTwo[1].size() << " " << hitIdsInTwo[2].size() << " " << hitIdsInTwo[3].size());
  for (unsigned int iLayer = 0; iLayer < 4; ++ iLayer) {
    for (unsigned int iPair = 0; iPair < slopeInTwo[iLayer].size(); ++iPair) {
      ATH_MSG_DEBUG("@@MM@@ pair fit slope= " << slopeInTwo[iLayer].at(iPair) << " intercept= " << interceptInTwo[iLayer].at(iPair));
    }
  }

  ATH_MSG_DEBUG("check point 5.2");
  std::vector<std::array<int, 4>> hitIdsInFourX;
  std::vector<double> slopeInFourX;
  std::vector<double> interceptInFourX;
  std::vector<double> mseInFourX;
    
  unsigned int nPairsInInnerX = hitIdsInTwo[0].size();
  unsigned int nPairsInOuterX = hitIdsInTwo[1].size();

  for(unsigned int iPairX = 0; iPairX < nPairsInInnerX; ++iPairX){

    double slope[2];
    double intercept[2];
    double spR[2];

    slope[0] = slopeInTwo[0].at(iPairX);
    intercept[0] = interceptInTwo[0].at(iPairX);
    spR[0] = slope[0] * NSWCenterZ + intercept[0];
    ATH_MSG_DEBUG("check point 5.2.1 " << nPairsInInnerX << " " << iPairX);
    for(unsigned int jPairX = 0; jPairX < nPairsInOuterX; ++jPairX){
      int ihitIds = hitIdsInTwo[0].at(iPairX);
      int jhitIds = hitIdsInTwo[1].at(jPairX);
      if ( ((ihitIds>>16 & 0xffff) == 0xffff || (ihitIds & 0xffff) == 0xffff) &&
           ((jhitIds>>16 & 0xffff) == 0xffff || (jhitIds & 0xffff) == 0xffff )) continue; // require at least 3 hits in 4 layers

      slope[1] = slopeInTwo[1].at(jPairX);
      intercept[1] = interceptInTwo[1].at(jPairX);
      spR[1] = slope[1] * NSWCenterZ + intercept[1];

      if(std::abs(spR[1] - spR[0]) > 50. ||
         std::abs((intercept[1] + intercept[0]) / 2) > 200.) continue;

      std::array<int, 4> setOfHitIds;
      setOfHitIds[0] = (ihitIds>>16 & 0xffff);
      setOfHitIds[1] = (ihitIds & 0xffff);
      setOfHitIds[2] = (jhitIds>>16 & 0xffff);
      setOfHitIds[3] = (jhitIds & 0xffff);
      ATH_MSG_DEBUG("check point 5.2.1.1 " << nPairsInOuterX << " " << jPairX);
      std::vector<double> r;
      std::vector<double> z;
      for(unsigned int iLayer = 0; iLayer < 4; ++iLayer){
        if(setOfHitIds[iLayer] == 0xffff) {
          continue;
        }
        ATH_MSG_DEBUG("check point 5.2.1.1.1 " << iLayer << " " << setOfHitIds[iLayer]);
        double rhit = mmHits.at(setOfHitIds[iLayer]).r;
        double zhit = mmHits.at(setOfHitIds[iLayer]).z;
        r.push_back(rhit);
        z.push_back(zhit);
      }
      double slopefit=0., interceptfit=99999., mse=-1.;
      LinearFit(z,r,&slopefit, &interceptfit, &mse);

      hitIdsInFourX.push_back(setOfHitIds);
      slopeInFourX.push_back(slopefit);
      interceptInFourX.push_back(interceptfit);
      mseInFourX.push_back(mse);

    }// end of iPair of the i-th and (i+4)-th layers, i=0,2
  }// end of jPair of the j-th and (j+4)-th layers, j=1,3
  ATH_MSG_DEBUG("@@MM@@ X Nquads " << hitIdsInFourX.size());
  for (unsigned int iQuad = 0; iQuad < slopeInFourX.size(); ++iQuad) {
    ATH_MSG_DEBUG("@@MM@@ X quad fit slope= " << slopeInFourX.at(iQuad) << " intercept= " << interceptInFourX.at(iQuad) << " mse= " << mseInFourX.at(iQuad));
  }

  ATH_MSG_DEBUG("check point 5.3");
  if(!hitIdsInFourX.size()){
    ATH_MSG_WARNING("No candidate segment found in MM X layers");
    return;
  }

  double tanTiltAngleU = std::tan( 1.5/360*2*M_PI),
         tanTiltAngleV = std::tan(-1.5/360*2*M_PI);

  std::vector< std::array<int, 8> > hitIdsInEight;
  std::vector<double> mseInEight;

  for(unsigned int iQuadX = 0;  iQuadX < hitIdsInFourX.size(); ++iQuadX){
    if(mseInFourX.at(iQuadX) > 10) continue;

    double slopeX = slopeInFourX.at(iQuadX);
    double interceptX = interceptInFourX.at(iQuadX);
    std::array<int,4> hitIdsX;
    hitIdsX = hitIdsInFourX.at(iQuadX);

    for (unsigned int iPairU = 0; iPairU < hitIdsInTwo[2].size(); ++iPairU) {

      int hitIdsU[2];
      hitIdsU[0] = hitIdsInTwo[2].at(iPairU)>>16 & 0xffff;
      hitIdsU[1] = hitIdsInTwo[2].at(iPairU) & 0xffff;
      double phiLocalU[2]={-99999,-99999};
      for(unsigned int iLayer = 0; iLayer < 2; ++iLayer) {
        if (hitIdsU[iLayer] == 0xffff) continue;
        if (hitIdsU[iLayer] < 0) {
          ATH_MSG_DEBUG("@@MM@@ hitIdsU[iLayer] iLayer= " << iLayer << " hitIdsU[iLayer]= " << hitIdsU[iLayer]);
        }
        double rInterpolate = slopeX * mmHits.at(hitIdsU[iLayer]).z + interceptX;
        double rProj = mmHits.at(hitIdsU[iLayer]).r;
        phiLocalU[iLayer] = std::atan((rProj-rInterpolate)/tanTiltAngleU/rInterpolate);
      }

      for(unsigned int iPairV = 0; iPairV < hitIdsInTwo[3].size(); ++iPairV) {

        int hitIdsV[2];
        hitIdsV[0] = hitIdsInTwo[3].at(iPairV)>>16 & 0xffff;
        hitIdsV[1] = hitIdsInTwo[3].at(iPairV) & 0xffff;

        if( (hitIdsU[0] == 0xffff || hitIdsU[1] == 0xffff) &&
            (hitIdsV[0] == 0xffff || hitIdsV[1] == 0xffff) ) continue; // require at least 3 UV layers having hits

        double phiLocalV[2]={-99999,-99999};
        for(unsigned int iLayer = 0; iLayer < 2; ++iLayer) {
          if (hitIdsV[iLayer] == 0xffff) continue;
          if (hitIdsV[iLayer] < 0) {
            ATH_MSG_DEBUG("@@MM@@ hitIdsV[iLayer] iLayer= " << iLayer << " hitIdsV[iLayer]= " << hitIdsV[iLayer]);
          }
          double rInterpolate = slopeX * mmHits.at(hitIdsV[iLayer]).z + interceptX;
          double rProj = mmHits.at(hitIdsV[iLayer]).r;
          phiLocalV[iLayer] = std::atan((rProj-rInterpolate)/tanTiltAngleV/rInterpolate);
        }

        if ( std::abs(phiLocalU[0]-phiLocalV[0]) > 0.05 &&
             std::abs(phiLocalU[1]-phiLocalV[1]) > 0.05) continue;

        // average of phis in 4 UV layers 
        double phiLocalUV = 0;
        int nPhi = 0;
        for(unsigned int iLayer = 0; iLayer < 2; ++iLayer) {
          if(phiLocalU[iLayer] > -99999.) {
            phiLocalUV += phiLocalU[iLayer];
            ++nPhi;
          }
          if(phiLocalV[iLayer] > -99999.) {
            phiLocalUV += phiLocalV[iLayer];
            ++nPhi;
          }
        }
        phiLocalUV /= nPhi;

        std::array<int, 8> setOfHitIds = {-1,-1,-1,-1,-1,-1,-1,-1};
        std::vector<double> r, z;
        for (unsigned int iLayer = 0; iLayer < 4; ++iLayer) {
          if ( hitIdsX[iLayer] != 0xffff) {
            if (hitIdsX[iLayer] < 0) {
              ATH_MSG_DEBUG("@@MM@@ hitIdsX[iLayer] iLayer= " << iLayer << " hitIdsX[iLayer]= " << hitIdsX[iLayer]);
            }
            z.push_back(mmHits.at(hitIdsX[iLayer]).z);
            r.push_back(mmHits.at(hitIdsX[iLayer]).r / std::cos(phiLocalUV));
            setOfHitIds[iLayer] = hitIdsX[iLayer];
          }
        }
        for (unsigned int iLayer = 0; iLayer < 2; ++iLayer) {
          if ( hitIdsU[iLayer] != 0xffff) {
            if (hitIdsU[iLayer] < 0) {
              ATH_MSG_DEBUG("@@MM@@ 2 hitIdsU[iLayer] iLayer= " << iLayer << " hitIdsU[iLayer]= " << hitIdsU[iLayer]);
            }
            z.push_back(mmHits.at(hitIdsU[iLayer]).z);
            r.push_back(mmHits.at(hitIdsU[iLayer]).r*(std::cos(phiLocalUV) + 1/std::cos(phiLocalUV))/2.);
            setOfHitIds[iLayer+4] = hitIdsU[iLayer];
          }
          if ( hitIdsV[iLayer] != 0xffff) {
            if (hitIdsV[iLayer] < 0) {
              ATH_MSG_DEBUG("@@MM@@ 2 hitIdsV[iLayer] iLayer= " << iLayer << " hitIdsV[iLayer]= " << hitIdsV[iLayer]);
            }
            z.push_back(mmHits.at(hitIdsV[iLayer]).z);
            r.push_back(mmHits.at(hitIdsV[iLayer]).r*(std::cos(phiLocalUV) + 1/std::cos(phiLocalUV))/2.);
            setOfHitIds[iLayer+6] = hitIdsV[iLayer];
          }
        }
        double slopefit=0., interceptfit=99999., mse=-1.;
        LinearFit(z,r,&slopefit,&interceptfit,&mse);

        hitIdsInEight.push_back(setOfHitIds);
        mseInEight.push_back(mse);
      } // end of Pair loop of V layers
    } // end of Pair loop of U Layers
  }// end of Quad loop of X layers
  ATH_MSG_DEBUG("@@MM@@ Noctets " << hitIdsInEight.size());
  
  std::vector<int> nOctetSegments; 
  std::vector<int> patternStationName; 
  for (unsigned int iOctet = 0; iOctet < hitIdsInEight.size(); ++iOctet) {
    bool isFirstHit = true;
    int hitStationName = 0;
    int nOctetSegment = 0;
    ATH_MSG_DEBUG("@@MM@@ octet fit mse " << mseInEight.at(iOctet));
    std::array<int, 8> tmpOctet = hitIdsInEight.at(iOctet);
    for (unsigned int iLayer = 0; iLayer < 8; ++iLayer) {
      if (tmpOctet[iLayer] != -1) {

        if(isFirstHit){
          hitStationName = mmHits.at(tmpOctet[iLayer]).stationName;
          isFirstHit = false;

          ATH_MSG_DEBUG("@@MM@@ octet pos r= " << mmHits.at(tmpOctet[iLayer]).r << " phi= " << mmHits.at(tmpOctet[iLayer]).phi << " z= " << mmHits.at(tmpOctet[iLayer]).z);
          nOctetSegment++;
        } 
        else if(mmHits.at(tmpOctet[iLayer]).stationName == hitStationName){
          ATH_MSG_DEBUG("@@MM@@ octet pos r= " << mmHits.at(tmpOctet[iLayer]).r << " phi= " << mmHits.at(tmpOctet[iLayer]).phi << " z= " << mmHits.at(tmpOctet[iLayer]).z);
          nOctetSegment++;
        }

      }
    }
    nOctetSegments.push_back(nOctetSegment);
    patternStationName.push_back(hitStationName);
  }

  ATH_MSG_DEBUG("check point 5.4");
  double mseminL = 100000.;
  double mseminS = 100000.;
  std::vector<int> octetIds(2,-1);
  for(unsigned int iOctet = 0;  iOctet < hitIdsInEight.size(); ++iOctet){
    if(patternStationName.at(iOctet) == 56){ 
      if( mseInEight.at(iOctet) < mseminL) {
        mseminL = mseInEight.at(iOctet); 
      }
    }
    else if(patternStationName.at(iOctet) == 55){
      if( mseInEight.at(iOctet) < mseminS) {
        mseminS = mseInEight.at(iOctet); 
      }
    }
  }// end of Octet loop
  
  for(unsigned int iOctet = 0; iOctet < hitIdsInEight.size(); ++iOctet){
    if(patternStationName.at(iOctet) == 56){ 
      if( mseInEight.at(iOctet) != mseminL) {
        continue;
      }
    }
    else if(patternStationName.at(iOctet) == 55){
      if( mseInEight.at(iOctet) != mseminS) {
        continue;
      }
    }
    octetIds.push_back(iOctet);
  }
  ATH_MSG_DEBUG("check point 5.5");
  
  for(unsigned int ids = 0; ids < octetIds.size(); ids++){
    if (octetIds.at(ids) != -1) {
      hitIdsCandidate.push_back(hitIdsInEight.at(octetIds.at(ids)));
    }
  }
}
