/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_CLUSTERIZATION_H
#define SCT_CLUSTERIZATION_H

#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "TrkPrepRawData/PrepRawDataCLASS_DEF.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "Identifier/Identifier.h"
#include <vector>

class FastSCT_Clusterization{
public:

  FastSCT_Clusterization () {}

  void setSctID(const SCT_ID* sctID){ m_sctID = sctID; }  

  void addHit( const Identifier elementId, const IdentifierHash
	       hashId, const unsigned int strip );
  void finishHits();

  void initializeGeometry(const InDetDD::SCT_DetectorManager* manager);

  std::vector<const InDet::SCT_ClusterCollection*>&  getClusterCollections();

  //  void prnt_ClusterCollections();
  
  void initialize() {
    m_firstWord = true;
    m_clusterId = 0;
    if ( !m_clusterCollections.empty() ) m_clusterCollections.clear(); 
  }

private:
  void setupNewElement(const Identifier elementId, const IdentifierHash
                       hashId, const unsigned int strip );

  void addCluster();
  
private:

  bool m_firstWord;

  unsigned int m_clusterId;
  InDetDD::SCT_DetectorManager* m_man;
  InDetDD::SiDetectorElement* m_detEl;

  Identifier m_element;
  
  //InDet::SCT_Cluster* m_currentCluster;

  InDet::SCT_ClusterCollection* m_currentClusterColl;

  std::vector<const InDet::SCT_ClusterCollection *> m_clusterCollections;


  unsigned int m_current_width;
  unsigned int m_first_strip;
  unsigned int m_last_strip;
  bool m_strips_ascending;
  const SCT_ID* m_sctID;

  /** Keep cache of the detector information needed for the clusterisation */
  double m_barrel_pitch;
  // The following numbers are from:
  //     InnerDetector/InDetDetDescr/InDetIdentifier/InDetIdentifier/SCT_ID.h
  static const int s_maxstrip=767;
  static const int s_ndisk=9; /// <number of different forward disks
  static const int s_neta=3;  /// <number of different forward module types (inner, mid, outer)
  double m_forward_pitch[s_ndisk][s_neta];   ///<average pitch of these forward elements

  double m_current_pitch;
  double m_localXcorrection;
  double m_deltaXlorentz;
  double m_corrTable[2];
};

#endif 
