/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include "TrigInDetPattRecoEvent/TrigInDetTracklet.h"
#include "TrigInDetPattRecoTools/LayeredSpacePointStorage.h"
#include "TrigInDetPattRecoTools/TripletMerging.h"
#include <algorithm>

void TripletMerger::merge(MERGED_TRIPLETS &p1, MERGED_TRIPLETS &p2) {
  
  if(!p1.m_active || !p2.m_active) return;

  // Insert second track space points in first track
  for(int i=0; i<m_helper.getNumberOfLayers(); i++) {
    // Add first layer SP
    if(p1.m_vSPA[0][i].m_pSP==NULL && p2.m_vSPA[0][i].m_pSP!=NULL) {
      p1.m_vSPA[0][i] = p2.m_vSPA[0][i];
    }
    // Add second layer SP
    if(p1.m_vSPA[0][i].m_pSP!=NULL && p1.m_vSPA[1][i].m_pSP==NULL && p2.m_vSPA[0][i].m_pSP!=NULL) {
      // Barrel
      if(m_helper.isBarrel(i)) {
	float dr = p2.m_vSPA[0][i].m_pSP->r()-p1.m_vSPA[0][i].m_pSP->r();
	if(dr>0.5)  {
	  p1.m_vSPA[1][i] = p2.m_vSPA[0][i];
	}
	if(dr<-0.5) {
	  p1.m_vSPA[1][i] = p1.m_vSPA[0][i];
	  p1.m_vSPA[0][i] = p2.m_vSPA[0][i];
	}
      }
      // Endcap
      else {
	float dz = p2.m_vSPA[0][i].m_pSP->z()-p1.m_vSPA[0][i].m_pSP->z();

	if(p2.m_vSPA[0][i].m_pSP->z()<0) dz=-dz;
	if(dz>0.5)  {
	  p1.m_vSPA[1][i] = p2.m_vSPA[0][i];
	}
	if(dz<-0.5) {
	  p1.m_vSPA[1][i] = p1.m_vSPA[0][i];
	  p1.m_vSPA[0][i] = p2.m_vSPA[0][i];
	}
      }
    }
  }
  p1.m_chi2+=p2.m_chi2;
  p2.m_active=false;
}
