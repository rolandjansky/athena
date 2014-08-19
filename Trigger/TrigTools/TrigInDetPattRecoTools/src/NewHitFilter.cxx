/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// NewHitFilter
// ( see header-file for details )
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////

//#include "TrigInDetEvent/TrigSiSpacePointBase.h"
//#include "TrigInDetPattRecoEvent/TrigL2TrackCandidate.h"
#include "TrigInDetPattRecoEvent/TrigSiSpacePointUtils.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigInDetPattRecoTools/NewHitFilter.h"

#include <iostream>
#include <algorithm>

#include <cmath>
#include <cstring>

//#define IDSCAN_DEBUG 1

#include <map>
#include <algorithm>

void NewHitFilter::initialise() 
{

  //in some cases I got 360/3.6 = 99, so divide with something a bit less than m_phiBinSize
  m_maxPhiBins = long(360./(m_phiBinSize-1e-6));

  // avoid obliging the user to use phi bin size that is an integer multiple of 360.
  double tmp_phiBinSize = 360./double(m_maxPhiBins);

  m_invPhiBinSize = 180./(M_PI*tmp_phiBinSize);
  m_invEtaBinSize = 1./m_etaBinSize;

  m_pHoughForHF=new TrigL2HoughTransformClass(m_maxPhiBins,m_fullScan,HashKeyCoeff);

  m_pHoughForGC=new TrigL2HoughTransformClass(HashKeyCoeff,false,HashKeyCoeff);
  m_pKeyGenForHF = new TrigL2HashKeyFunction( m_invEtaBinSize, m_invPhiBinSize, HashKeyCoeff);

  m_pGroupMaker = new TrigL2GroupMaker(m_pHoughForHF,m_pKeyGenForHF,maxEta,
				       m_maxBeamSpotShift,m_enhanceLayer0);

  m_tight = new TrigL2GroupCleanerCutsTight(m_tight_deltadzdrCut, 
					    m_tight_etaCutInner, 
					    m_tight_etaCutOuter,
					    m_tight_d0Cut,
					    m_tight_dPhidRBinSize,
					    m_tight_phi0Bins,
					    m_dPhidRCut);

#ifdef IDSCAN_DEBUG
  std::cout << "tight params: ddzdr,etainner,etaouter,d0,dphidrbinsize,dpdrcut: " << m_tight_deltadzdrCut
	    << " / " <<   m_tight_etaCutInner 
	    << " / " <<	  m_tight_etaCutOuter
	    << " / " <<	  m_tight_d0Cut
	    << " / " <<	  m_tight_dPhidRBinSize
	    << " / " <<	  m_tight_phi0Bins
	    << " / " <<    m_dPhidRCut << std::endl;
#endif


  m_loose = new TrigL2GroupCleanerCutsLoose(m_loose_deltadzdrCut,
					    m_loose_etaCutInner,
					    m_loose_etaCutOuter,
					    m_loose_d0Cut,
					    m_loose_dPhidRBinSize,
					    m_loose_phi0Bins,
					    m_dPhidRCut);

#ifdef IDSCAN_DEBUG
  std::cout << "loose params: ddzdr,etainner,etaouter,d0,dphidrbinsize,dpdrcut: " << m_loose_deltadzdrCut
	    << " / " <<   m_loose_etaCutInner 
	    << " / " <<	  m_loose_etaCutOuter
	    << " / " <<	  m_loose_d0Cut
	    << " / " <<	  m_loose_dPhidRBinSize
	    << " / " <<	  m_loose_phi0Bins
	    << " / " <<    m_dPhidRCut << std::endl;
#endif

  m_pGroupCleaner = new TrigL2GroupCleaner(m_looseTight, (*m_tight), (*m_loose), m_pHoughForGC, HashKeyCoeff);
  
  m_pPrefitter = new TrigL2Prefitter(m_fieldOn,m_removeOutliers);
  m_pCloneRemoval = new TrigL2CloneRemoval(m_usePtSign,m_cloneRemovalCut);

}

NewHitFilter::~NewHitFilter()
 { 
   if(m_pHoughForHF) delete m_pHoughForHF;
   if(m_pHoughForGC) delete m_pHoughForGC;
   if(m_pKeyGenForHF) delete m_pKeyGenForHF;
   if(m_pGroupMaker) delete m_pGroupMaker;
   if(m_pGroupCleaner) delete m_pGroupCleaner;
   if(m_pPrefitter) delete m_pPrefitter;
   if(m_pCloneRemoval) delete m_pCloneRemoval;

   if (m_loose)  delete m_loose;
   if (m_tight)  delete m_tight;
 }

void NewHitFilter::execute(std::vector<TrigSiSpacePointBase*>& inputSpVec,
			 const TrigRoiDescriptor& RoI, double zVertex, 
			 std::vector<TrigL2TrackCandidate*>& outputTracks) {

  std::vector<TrigL2TrackCandidate*> trCandVector;
  double minPhi = -M_PI;

  if(m_fullScan) {
    m_pGroupMaker->makeTrackCandidates(inputSpVec,trCandVector,zVertex,m_layerThresholdTmp);
  }
  else {
    m_pGroupMaker->makeTrackCandidates(inputSpVec,trCandVector,zVertex,m_layerThresholdTmp,RoI);
    minPhi = m_pGroupMaker->minPhi();    
  }
#ifdef IDSCAN_DEBUG
  std::cout<<"Large groups to be processed: "<<trCandVector.size()<<std::endl;
#endif
  for(std::vector<TrigL2TrackCandidate*>::iterator trIt = trCandVector.begin(); trIt!=trCandVector.end();++trIt) {

    std::vector<TrigL2TrackCandidate*> resolvedTracks;
    resolvedTracks.clear();

    m_pGroupCleaner->execute((*trIt),resolvedTracks,m_layerThresholdTmp,minPhi);

    delete (*trIt);

    for(std::vector<TrigL2TrackCandidate*>::iterator candIt = resolvedTracks.begin();candIt!=resolvedTracks.end();++candIt) {
      m_pPrefitter->run((*candIt));
    }

    if((resolvedTracks.size()>1)&&m_cloneRemoval) {

      m_pCloneRemoval->removeClones(resolvedTracks);

#ifdef IDSCAN_DEBUG
      int nTracksLeft=0;

      for(std::vector<TrigL2TrackCandidate*>::iterator trIt=resolvedTracks.begin();trIt!=resolvedTracks.end();++trIt) {
	if(!(*trIt)->deleted) nTracksLeft++;
      }

      std::cout << "IDSCAN_DEBUG: after clone removal remaining tracks = " << nTracksLeft << std::endl;

      for(std::vector<TrigL2TrackCandidate*>::iterator trIt=resolvedTracks.begin();trIt!=resolvedTracks.begin();++trIt) {
	std::cout << "IDSCAN_DEBUG: Final track cand. after clone rem. with number of hits = " << (*trIt)->spacePoints().size() 
		  << " 1/pT= " << 1/(*trIt)->m_params.m_pT
		  << " phi0= " << (*trIt)->m_params.m_phi0
		  << " eta= " << (*trIt)->m_params.m_eta
		  << " z0= " << (*trIt)->m_params.m_z0
		  << std::endl;
      }
#endif
    }

    // at the end all good tracks (i.e. passing m_layerThresholdTmp cut) from resolvedTracks transferred into 
    // output track vector and bad tracks are deleted

    for(std::vector<TrigL2TrackCandidate*>::iterator candIt = resolvedTracks.begin();candIt!=resolvedTracks.end();++candIt) {
      if( (*candIt)->deleted || ((*candIt)->spacePoints().size()<=m_layerThresholdTmp)) {
	delete (*candIt); continue;
      }
      outputTracks.push_back(*candIt);
    }
  }
}
