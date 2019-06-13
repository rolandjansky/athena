/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_CLUSTERIZATION_H
#define SCT_CLUSTERIZATION_H

#include "InDetIdentifier/SCT_ID.h"
#include "TrkPrepRawData/PrepRawDataCLASS_DEF.h"
#include "Identifier/Identifier.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"

#include <vector>

namespace InDetDD {
  class SCT_DetectorManager;
  class SiDetectorElement;
}

class FastSCT_Clusterization{
public:

  FastSCT_Clusterization ():
    m_firstWord{true},
    m_clusterId{0},
    m_man{nullptr},
    m_detEl{nullptr},
    m_currentClusterColl{nullptr},
    m_lorentzAngleTool{nullptr},
    m_current_width{0},
    m_first_strip{0},
    m_last_strip{0},
    m_strips_ascending{false},
    m_sctID{nullptr},
    m_barrel_pitch{0.},
    m_current_pitch{0.},
    m_localXcorrection{0.},
    m_deltaXlorentz{0.}
  {}

  void setSctID(const SCT_ID* sctID){ m_sctID = sctID; }  

  void setLorentzAngleTool(const ISiLorentzAngleTool* lorentzAngleTool) { m_lorentzAngleTool = lorentzAngleTool; }

  void addHit( const Identifier elementId, const IdentifierHash
	       hashId, const unsigned int strip,
               const InDetDD::SiDetectorElement* detElement);
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
                       hashId, const unsigned int strip,
                       const InDetDD::SiDetectorElement* detElement);

  void addCluster();
  
private:

  bool m_firstWord;

  unsigned int m_clusterId;
  const InDetDD::SCT_DetectorManager* m_man;
  const InDetDD::SiDetectorElement* m_detEl;

  Identifier m_element;
  
  //InDet::SCT_Cluster* m_currentCluster;

  InDet::SCT_ClusterCollection* m_currentClusterColl;

  std::vector<const InDet::SCT_ClusterCollection *> m_clusterCollections;

  const ISiLorentzAngleTool* m_lorentzAngleTool;

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
