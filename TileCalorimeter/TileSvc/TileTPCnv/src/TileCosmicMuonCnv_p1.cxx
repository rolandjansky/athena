///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileCosmicMuonCnv_p1.cxx 
// Implementation file for class TileCosmicMuonCnv_p1
// Author: Aranzazu Ruiz <aranzazu.ruiz.martinez@cern.ch>
// Date:   July 2008
/////////////////////////////////////////////////////////////////// 

// TileEvent includes
#define private public
#define protected public
#include "TileEvent/TileCosmicMuon.h"
#undef private
#undef protected

// TileTPCnv includes
#include "TileTPCnv/TileCosmicMuonCnv_p1.h"


void TileCosmicMuonCnv_p1::transToPers(const TileCosmicMuon* transObj, TileCosmicMuon_p1* persObj, MsgStream &/*log*/) {

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
    persObj->m_trackCellHash.push_back( *it );
  }
}


void TileCosmicMuonCnv_p1::persToTrans(const TileCosmicMuon_p1* persObj, TileCosmicMuon* transObj, MsgStream &/*log*/) {

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
  
  transObj->m_segmentPath.reserve(0);
  transObj->m_segmentPartition.reserve(0);
  transObj->m_segmentModule.reserve(0);
  transObj->m_segmentSampling.reserve(0);
}
