///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileCosmicMuonCnv_p2.cxx 
// Implementation file for class TileCosmicMuonCnv_p2
// Author: Jose Maneira <maneira@lip.pt>
// Date:   July 2009
/////////////////////////////////////////////////////////////////// 

// TileEvent includes
#define private public
#define protected public
#include "TileEvent/TileCosmicMuon.h"
#undef private
#undef protected

// TileTPCnv includes
#include "TileTPCnv/TileCosmicMuonCnv_p2.h"

#include <algorithm>

void TileCosmicMuonCnv_p2::transToPers(const TileCosmicMuon* transObj, TileCosmicMuon_p2* persObj, MsgStream &/*log*/) {

  persObj->m_time = transObj->m_time;
  persObj->m_positionX = transObj->m_positionX;
  persObj->m_positionY = transObj->m_positionY;
  persObj->m_positionZ = transObj->m_positionZ;
  persObj->m_directionPhi = transObj->m_directionPhi;
  persObj->m_directionTheta = transObj->m_directionTheta;
  persObj->m_fitQuality = transObj->m_fitQuality;
  persObj->m_fitNCells = transObj->m_fitNCells;

  for (std::vector<double>::const_iterator it = transObj->m_pathTop.begin(); it != transObj->m_pathTop.end(); ++it) {
    persObj->m_pathTop.push_back((float) *it );
  }

  for (std::vector<double>::const_iterator it = transObj->m_pathBottom.begin(); it != transObj->m_pathBottom.end(); ++it) {
    persObj->m_pathBottom.push_back((float) *it );
  }

  for (std::vector<double>::const_iterator it = transObj->m_energyTop.begin(); it != transObj->m_energyTop.end(); ++it) {
    persObj->m_energyTop.push_back((float) *it );
  }

  for (std::vector<double>::const_iterator it = transObj->m_energyBottom.begin(); it != transObj->m_energyBottom.end(); ++it) {
    persObj->m_energyBottom.push_back((float) *it );
  }

  for (std::vector<IdentifierHash>::const_iterator it = transObj->m_trackCellHash.begin(); it != transObj->m_trackCellHash.end(); ++it) {
    persObj->m_trackCellHash.push_back((unsigned int)*it );
  }

  for (std::vector<double>::const_iterator it = transObj->m_segmentPath.begin(); it != transObj->m_segmentPath.end(); ++it) {
    persObj->m_segmentPath.push_back((float) *it );
  }

  size_t idx = std::max(std::max(transObj->m_segmentPartition.size(),transObj->m_segmentModule.size()),transObj->m_segmentSampling.size ());
  persObj->m_segmentPartitionModuleSampling.resize(idx,0);
  
  idx = 0;
  for (std::vector<int>::const_iterator it = transObj->m_segmentPartition.begin(); it != transObj->m_segmentPartition.end(); ++it) {
    persObj->m_segmentPartitionModuleSampling[idx++] |= ((*it)&0xFF)<<24;
  }

  idx = 0;
  for (std::vector<int>::const_iterator it = transObj->m_segmentModule.begin(); it != transObj->m_segmentModule.end(); ++it) {
    persObj->m_segmentPartitionModuleSampling[idx++] |= ((*it)&0xFF)<<16;
  }

  idx = 0;
  for (std::vector<int>::const_iterator it = transObj->m_segmentSampling.begin(); it != transObj->m_segmentSampling.end(); ++it) {
    persObj->m_segmentPartitionModuleSampling[idx++] |= ((*it)&0xFF)<<8;
  }
}


void TileCosmicMuonCnv_p2::persToTrans(const TileCosmicMuon_p2* persObj, TileCosmicMuon* transObj, MsgStream &/*log*/) {

  transObj->m_time = persObj->m_time;
  transObj->m_positionX = persObj->m_positionX;
  transObj->m_positionY = persObj->m_positionY;
  transObj->m_positionZ = persObj->m_positionZ;
  transObj->m_directionPhi = persObj->m_directionPhi;
  transObj->m_directionTheta = persObj->m_directionTheta;
  transObj->m_fitQuality = persObj->m_fitQuality;
  transObj->m_fitNCells = persObj->m_fitNCells;

  transObj->m_pathTop.reserve(persObj->m_pathTop.size());
  for (std::vector<float>::const_iterator it = persObj->m_pathTop.begin(); it != persObj->m_pathTop.end(); ++it) {
    transObj->m_pathTop.push_back((double) *it );
  }

  transObj->m_pathBottom.reserve(persObj->m_pathBottom.size());
  for (std::vector<float>::const_iterator it = persObj->m_pathBottom.begin(); it != persObj->m_pathBottom.end(); ++it) {
    transObj->m_pathBottom.push_back((double) *it );
  }

  transObj->m_energyTop.reserve(persObj->m_energyTop.size());
  for (std::vector<float>::const_iterator it = persObj->m_energyTop.begin(); it != persObj->m_energyTop.end(); ++it) {
    transObj->m_energyTop.push_back((double) *it );
  }

  transObj->m_energyBottom.reserve(persObj->m_energyBottom.size());
  for (std::vector<float>::const_iterator it = persObj->m_energyBottom.begin(); it != persObj->m_energyBottom.end(); ++it) {
    transObj->m_energyBottom.push_back((double) *it );
  }

  transObj->m_trackCellHash.reserve(persObj->m_trackCellHash.size());
  for (std::vector<unsigned int>::const_iterator it = persObj->m_trackCellHash.begin(); it != persObj->m_trackCellHash.end(); ++it) {
    transObj->m_trackCellHash.push_back( *it );
  }

  transObj->m_segmentPath.reserve(persObj->m_segmentPath.size());
  for (std::vector<float>::const_iterator it = persObj->m_segmentPath.begin(); it != persObj->m_segmentPath.end(); ++it) {
    transObj->m_segmentPath.push_back((double) *it );
  }

  unsigned int pms = persObj->m_segmentPartitionModuleSampling.size();
  transObj->m_segmentPartition.reserve(pms);
  transObj->m_segmentModule.reserve(pms);
  transObj->m_segmentSampling.reserve(pms);

  for (std::vector<unsigned int>::const_iterator it = persObj->m_segmentPartitionModuleSampling.begin(); it != persObj->m_segmentPartitionModuleSampling.end(); ++it) {
    pms = (*it);
    transObj->m_segmentPartition.push_back( (pms>>24) & 0xFF );
    transObj->m_segmentModule.push_back   ( (pms>>16) & 0xFF );
    transObj->m_segmentSampling.push_back ( (pms>>8)  & 0xFF );
  }
}
