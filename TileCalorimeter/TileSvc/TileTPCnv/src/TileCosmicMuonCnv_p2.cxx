///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileCosmicMuonCnv_p2.cxx 
// Implementation file for class TileCosmicMuonCnv_p2
// Author: Jose Maneira <maneira@lip.pt>
// Date:   July 2009
/////////////////////////////////////////////////////////////////// 

#include "TileEvent/TileCosmicMuon.h"
#include "TileTPCnv/TileCosmicMuonCnv_p2.h"

#include <algorithm>

void TileCosmicMuonCnv_p2::transToPers(const TileCosmicMuon* transObj, TileCosmicMuon_p2* persObj, MsgStream &/*log*/) {

  persObj->m_time = transObj->GetTime();
  persObj->m_positionX = transObj->GetPositionX();
  persObj->m_positionY = transObj->GetPositionY();
  persObj->m_positionZ = transObj->GetPositionZ();
  persObj->m_directionPhi = transObj->GetDirectionPhi();
  persObj->m_directionTheta = transObj->GetDirectionTheta();
  persObj->m_fitQuality = transObj->GetFitQuality();
  persObj->m_fitNCells = transObj->GetFitNCells();

  size_t sz = transObj->GetNSamples();
  persObj->m_pathTop.resize(sz);
  persObj->m_pathBottom.resize(sz);
  persObj->m_energyTop.resize(sz);
  persObj->m_energyBottom.resize(sz);
  for (size_t i = 0; i < sz; i++) {
    persObj->m_pathTop[i] = transObj->GetPathTop(i);
    persObj->m_pathBottom[i] = transObj->GetPathBottom(i);
    persObj->m_energyTop[i] = transObj->GetEnergyTop(i);
    persObj->m_energyBottom[i] = transObj->GetEnergyBottom(i);
  }

  size_t ncells = transObj->GetTrackNCells();
  persObj->m_trackCellHash.resize(ncells);
  for (size_t i = 0; i < ncells; i++) {
    persObj->m_trackCellHash[i] = transObj->GetTrackCellHash(i);
  }

  size_t nseg = transObj->GetNSegments();
  persObj->m_segmentPath.resize(nseg);
  persObj->m_segmentPartitionModuleSampling.resize(nseg);
  for (size_t i = 0; i < nseg; i++) {
    persObj->m_segmentPath[i] = transObj->GetSegmentPath(i);
    int part = transObj->GetSegmentPartition(i);
    int mod = transObj->GetSegmentModule(i);
    int samp = transObj->GetSegmentSampling(i);
    persObj->m_segmentPartitionModuleSampling[i] = 
      ((part&0xFF)<<24) | ((mod&0xFF)<<16) | ((samp&0xFF)<<8);
      
  }
}


void TileCosmicMuonCnv_p2::persToTrans(const TileCosmicMuon_p2* persObj, TileCosmicMuon* transObj, MsgStream &/*log*/) {

  transObj->SetTime (persObj->m_time);
  transObj->SetPositionX (persObj->m_positionX);
  transObj->SetPositionY (persObj->m_positionY);
  transObj->SetPositionZ (persObj->m_positionZ);
  transObj->SetDirectionPhi (persObj->m_directionPhi);
  transObj->SetDirectionTheta (persObj->m_directionTheta);
  transObj->SetFitQuality (persObj->m_fitQuality);
  transObj->SetFitNCells (persObj->m_fitNCells);

  transObj->SetPathTop (std::vector<double> (persObj->m_pathTop.begin(),
                                             persObj->m_pathTop.end()));

  transObj->SetPathBottom (std::vector<double> (persObj->m_pathBottom.begin(),
                                                persObj->m_pathBottom.end()));
  
  transObj->SetEnergyTop (std::vector<double> (persObj->m_energyTop.begin(),
                                               persObj->m_energyTop.end()));
  
  transObj->SetEnergyBottom (std::vector<double> (persObj->m_energyBottom.begin(),
                                                  persObj->m_energyBottom.end()));
  

  transObj->SetTrackCellHash (std::vector<IdentifierHash> (persObj->m_trackCellHash.begin(),
                                                           persObj->m_trackCellHash.end()));


  transObj->SetSegmentPath (std::vector<double> (persObj->m_segmentPath.begin(),
                                                 persObj->m_segmentPath.end()));


  unsigned int pms = persObj->m_segmentPartitionModuleSampling.size();
  std::vector<int> segmentPartition(pms);
  std::vector<int> segmentModule(pms);
  std::vector<int> segmentSampling(pms);

  for (size_t i = 0; i < pms; i++) {
    unsigned int x = persObj->m_segmentPartitionModuleSampling[i];
    segmentPartition[i] = (x>>24) & 0xFF;
    segmentModule[i]    = (x>>16) & 0xFF;
    segmentSampling[i]  = (x>>8)  & 0xFF;
  }

  transObj->SetSegmentPartition (std::move (segmentPartition));
  transObj->SetSegmentModule (std::move (segmentModule));
  transObj->SetSegmentSampling (std::move (segmentSampling));
}
