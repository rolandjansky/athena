/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include<iostream>
#include<algorithm>

#include "TrigInDetPattRecoTools/HoughBinCluster.h"

#include "TrigInDetPattRecoTools/TrigL2GroupCleaner.h"

void TrigL2GroupCleaner::execute(const TrigL2TrackCandidate* pTrack, 
				 std::vector<TrigL2TrackCandidate*>& trackCandVect, 
				 double layerThreshold, double minPhi) {

#ifdef IDSCAN_DEBUG
  std::cout<<"Processing a large group... "<<std::endl;
  std::cout << "IDSCAN_DEBUG: number of SPs = " << pTrack->spacePoints().size() << std::endl;
  std::vector<const TrigSiSpacePointBase *>::const_iterator spIt=pTrack->spacePoints().begin();
  for( ; spIt!=pTrack->spacePoints().end(); spIt++) {
    const TrigSiSpacePointBase* pSP=(*spIt);
    std::cout << "IDSCAN_DEBUG: after ambig. resol. - layer = " << pSP->layer()
	      << " phi= " << pSP->phi()
	      << " rho= " << pSP->r()
	      << " zed= " << pSP->z()
	      << std::endl;
  }
#endif
  
  double invdPhidRBinSize = m_tight.invdPhidRBinSize;
  double dPhidRCut = m_tight.dPhidRCut;
  double invPhi0BinSize = m_tight.invPhi0BinSize;
  double phi0Offset = m_tight.phi0Offset;

  if( pTrack->spacePoints().size() > m_looseTight ) {
  
      m_pTripletMakerTight->execute(pTrack, minPhi);

#ifdef IDSCAN_DEBUG
      std::cout << "IDSCAN_DEBUG: tight set of groupCleaner parameters" << std::endl;
#endif

    }
  else {
    m_pTripletMakerLoose->execute(pTrack, minPhi);
    invdPhidRBinSize = m_loose.invdPhidRBinSize;
    dPhidRCut = m_loose.dPhidRCut;
    invPhi0BinSize = m_loose.invPhi0BinSize;
    phi0Offset = m_loose.phi0Offset;
    
#ifdef IDSCAN_DEBUG
    std::cout << "IDSCAN_DEBUG: loose set of groupCleaner parameters" << std::endl;
#endif
  }
    
  std::list<HoughBinCluster> clusterList;
  clusterList.clear();
  m_pHough->clusterBins( clusterList, layerThreshold);
  
  unsigned long newGroups = clusterList.size();

#ifdef IDSCAN_DEBUG
  std::cout << "IDSCAN_DEBUG: groupCleaner found " << newGroups << " groups in the original group." << std::endl;
#endif
    
  if (newGroups==0) return;

  trackCandVect.clear();

  double eta = pTrack->m_params.m_eta; //the same preliminary eta for all track candidates
  double z0  = pTrack->m_params.m_z0;

  // 1. converting clusters of bins into track candidates

  std::list<HoughBinCluster>::iterator clIt = clusterList.begin();
  while( clIt != clusterList.end() ) {
      
    (*clIt).removeDuplicates();
    if ( (*clIt).NumberOfHits()<=layerThreshold ) {
      clusterList.erase( clIt++ );
      continue;
    }

    double dfdrho = (((*clIt).averageBinY()+0.5)/invdPhidRBinSize)-dPhidRCut;
    double ptinv =  -dfdrho/dPhidRCut/1000.;
    if(fabs(ptinv)<1e-9) ptinv=1e-9;// to prevent possible FPEs
    double phi0 = ((*clIt).averageBinX()+0.5)/invPhi0BinSize - dfdrho*phi0Offset;

    // creating a new track candidate...

    std::list< const TrigSiSpacePointBase* >& spList = (*clIt).spacePoints();

    spList.sort(orderSpacePointsInR());
    
    TrigL2TrackCandidate* pTrack = new TrigL2TrackCandidate(0);//empty track
    std::for_each(spList.begin(),spList.end(),
		  std::bind1st(std::mem_fun(&TrigL2TrackCandidate::addSpacePoint), &(*pTrack)));  
    
    pTrack->m_params.m_phi0 = phi0;
    pTrack->m_params.m_pT = 1.0/ptinv;
    pTrack->m_params.m_eta = eta;
    pTrack->m_params.m_z0 = z0;
    
    trackCandVect.push_back(pTrack);

#ifdef IDSCAN_DEBUG
    std::cout << "IDSCAN_DEBUG: New groupCleaner group with number of hits = " << pTrack->spacePoints().size() 
	      << " 1/pT= " << ptinv 
	      << " phi0= " << phi0
	      << " eta= " << eta 
	      << " z0= " << z0
	      << std::endl;
#endif
    clusterList.erase( clIt++ );
  }
}
