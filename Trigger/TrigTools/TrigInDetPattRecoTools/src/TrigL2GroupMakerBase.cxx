/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetPattRecoTools/TrigL2HoughTransformClass.h"
#include "TrigInDetPattRecoTools/TrigL2HashKeyFunction.h"
#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include "TrigInDetPattRecoEvent/TrigL2TrackCandidate.h"
#include "TrigInDetPattRecoTools/TrigL2GroupMakerBase.h"

#include <cmath>
#include <list>
#include <algorithm>

void TrigL2GroupMakerBase::makeTrackCandidates(std::vector<TrigSiSpacePointBase*>& vsp, 
					       std::vector<TrigL2TrackCandidate*>& vtr, 
					       double zVertex, 
					       double layerThreshold) {//full-scan method
  m_zVertex = zVertex;
  m_meanEta=0;
  m_roiPhiMin=-M_PI;
  m_deltaEta = 3.0;
  m_isBoundary = false;

  executeAlgorithm(vsp,vtr,layerThreshold);
}

void TrigL2GroupMakerBase::makeTrackCandidates(std::vector<TrigSiSpacePointBase*>& spVec, 
					       std::vector<TrigL2TrackCandidate*>& vtr, 
					       double zVertex, 
					       double layerThreshold, 
					       double meanEta,
					       double deltaEta,
					       double meanPhi,
					       double deltaPhi) {
  m_zVertex = zVertex;
  m_meanEta = meanEta;
  m_deltaEta = deltaEta;

  double roiPhiMax;

  // If we trust that all the SPs are properly input, we determine the RoI phi width
  //  using the SPs themselves.
  //  If the RoI phi range is wider than pi, ie. if the RoI is large enough, we keep everything as usual.
  //   if ( 2 * m_useROIphiHalfWidth < M_PI )
  if ( 2 * deltaPhi < M_PI ) {
  
    double roiPhiPosMin( 9.9), roiPhiPosMax(0);
    double roiPhiNegMin(-9.9), roiPhiNegMax(0);  // least negative and most negative
	
    std::vector<TrigSiSpacePointBase* >::const_iterator SpItr( spVec.begin() );
    for(; SpItr!=spVec.end();++SpItr) {

      double spphi = (*SpItr)->phi();
      if ( spphi>0  &&  spphi>roiPhiPosMax )  roiPhiPosMax = spphi;
      if ( spphi>0  &&  spphi<roiPhiPosMin )  roiPhiPosMin = spphi;
	    
      if ( spphi<0  &&  spphi<roiPhiNegMax )  roiPhiNegMax = spphi;
      if ( spphi<0  &&  spphi>roiPhiNegMin )  roiPhiNegMin = spphi;
    }
	
    // if all SPs are in (0, pi):
    if ( roiPhiNegMax > roiPhiNegMin )  { 
      m_roiPhiMin = roiPhiPosMin; 
      roiPhiMax = roiPhiPosMax; 
    }
    // if all SPs are in (-pi, 0):
    else 
      if ( roiPhiPosMax < roiPhiPosMin )  { 
	m_roiPhiMin = roiPhiNegMax; 
	roiPhiMax = roiPhiNegMin; 
      }
    // if we have an RoI that covers phi=0 axis
      else 
	if ( roiPhiPosMin - roiPhiNegMin < M_PI )  { 
	  m_roiPhiMin = roiPhiNegMax; roiPhiMax = roiPhiPosMax; 
	}
    // if we have an RoI that covers phi=pi axis
	else  { 
	  m_roiPhiMin = roiPhiPosMin; roiPhiMax = roiPhiNegMin; 
	}
	
    m_roiPhiMin -= 1e-10;
    roiPhiMax += 1e-10;
  }
  else {
    m_roiPhiMin = meanPhi - deltaPhi;
    roiPhiMax = meanPhi + deltaPhi;
  }
    
  if ( m_roiPhiMin < -M_PI ) m_roiPhiMin += 2*M_PI;
  if ( roiPhiMax >  M_PI ) roiPhiMax -= 2*M_PI;

  m_isBoundary = (m_roiPhiMin>roiPhiMax);
  executeAlgorithm(spVec,vtr,layerThreshold);
}


void TrigL2GroupMakerBase::executeAlgorithm(std::vector<TrigSiSpacePointBase*>& spVec, 
					    std::vector<TrigL2TrackCandidate*>& vtr,
					    double layerThreshold) {

  //actual algorithm implementation


  double dEta = m_deltaEta;

  // The 'core' resolution from muFast is ~0.015
  dEta += 0.045;

  // SP etas can change significantly when shifted to account for an offset beamspot.
  // Offset BS affects most the hits on the B-layer ==> 50mm.
  dEta += (fabs(m_meanEta) + log(tan(atan(50./((50-m_maxBeamSpotShift)/tan(2.*atan(exp(-fabs(m_meanEta))))))/2.)));

  // An extra eta range to account for the fact that the RoI eta is computed with for z=0
  // Assume that the eta of the RoI is computed at the radius of the calorimeter and compute
  //   the eta of an RoI which is said to be at eta=0.
  double extraEta = 0.0;

  if ( m_zVertex!=0 )  extraEta -= log(tan(atan(2800./fabs(m_zVertex))/2));

  //1. loop over spacepoints, applying initial cuts, indexing, filling histogram

  int Counter=1;
  long offset = 2*m_pHough->hashKeyCoeff();

  std::vector<TrigSiSpacePointBase*>::iterator siSpItr( spVec.begin() );
  for ( ; siSpItr!=spVec.end(); ++siSpItr ) {

    double zr = ((*siSpItr)->z()-m_zVertex)/(*siSpItr)->r(); 
    double eta = log(zr+sqrt(1.+zr*zr));
    
    if ( fabs(eta)> m_maxEtaIdScan) continue;
    if ( m_zVertex<=0 && ( eta-m_meanEta>dEta+extraEta || m_meanEta-eta>dEta ) ) continue;
    if ( m_zVertex> 0 && ( m_meanEta-eta>dEta+extraEta || eta-m_meanEta>dEta ) ) continue;

    TrigSiSpacePointBase* pSP=(*siSpItr);
    pSP->index(Counter++);

    double rotatedPhi = pSP->phi() - m_roiPhiMin;

    if ( m_isBoundary ){
      if(rotatedPhi<0.0) rotatedPhi+=2*M_PI;
    }

    long sp_key = (*m_pKeyGen)(rotatedPhi,eta+m_maxEtaIdScan);
    (*m_pHough)[sp_key].AddSpacePoint(pSP);

    //   Special treatment for hits at pixel layer 0. Optionally enter them two more times
    //   in the binMap, at +/- 2 bins in eta, as their eta is affected most by mis-measurements
    //   in the ZFinder. Important mostly for low p_T tracks (B physics, b-tagging).
    //

    if (( m_enhanceLayer0 ) && (pSP->layer() == 0)) {      
	(*m_pHough)[sp_key+offset].AddSpacePoint( pSP );
	(*m_pHough)[sp_key-offset].AddSpacePoint( pSP );
      }
  }

  //   2. Loop over bins to form groups
  //

  std::list<HoughBinCluster> clusterList;
  m_pHough->clusterBins( clusterList, layerThreshold);

  //std::cout<<"First step passed: N large groups = "<<clusterList.size()<<std::endl;

  //   3. Loop over groups to delete "small" groups
  //

  std::list<HoughBinCluster>::iterator clusterItr(clusterList.begin()), clusterEnd(clusterList.end());

  while( clusterItr != clusterEnd ) {
    if ( m_enhanceLayer0 ) {    // if 0 layer hits entered 3 times, remove duplicates first
      (*clusterItr).removeDuplicates();
    }
    if ( (*clusterItr).NumberOfHits()>layerThreshold ) {

      std::list< const TrigSiSpacePointBase* >& spList = (*clusterItr).spacePoints();

      TrigL2TrackCandidate* pTrack = new TrigL2TrackCandidate(0);//empty track
      std::for_each(spList.begin(),spList.end(),
		    std::bind1st(std::mem_fun(&TrigL2TrackCandidate::addSpacePoint), &(*pTrack)));  

      //estimating parameters ...

      pTrack->m_params.m_eta = ((*clusterItr).averageBinX()+0.5)/m_pKeyGen->invXbinSize()-m_maxEtaIdScan;
      pTrack->m_params.m_phi0 =((*clusterItr).averageBinY()+0.5)/m_pKeyGen->invYbinSize();
      pTrack->m_params.m_z0 = m_zVertex;

      vtr.push_back(pTrack);

    }
    clusterList.erase( clusterItr++ );
  }

}
