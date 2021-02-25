/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTT_SGTORAWHITSTOOL_H
#define HTT_SGTORAWHITSTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkToolInterfaces/ITruthToTrack.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "HepPDT/ParticleDataTable.hh"
#include "HepPDT/ParticleData.hh"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "TrigHTTSGInput/HTT_IInputTool.h"
#include "TrigHTTObjects/HTTOptionalEventInfo.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "TrigHTTSGInput/TrigHTTInputUtils.h"

// A few constants for truth cuts
const float HTT_PT_TRUTHMIN = 400.;
const float HTT_Z_TRUTHMIN = 2300.;

class HTTEventInputHeader;
class HTTCluster;
class HTTTruthTrack;
class HTTOfflineTrack;

class HTT_SGToRawHitsTool :  public extends<AthAlgTool, HTT_IInputTool>  {

 public:
  HTT_SGToRawHitsTool(const std::string&, const std::string&, const IInterface*);
  virtual ~HTT_SGToRawHitsTool() {;}

  virtual StatusCode initialize() override;
  virtual StatusCode finalize()   override;
  virtual StatusCode readData(HTTEventInputHeader* header)  override;


private:
  // JO configuration
  ToolHandle<Trk::ITruthToTrack> m_truthToTrack; //!< tool to create track parameters from a gen particle
  ToolHandle<Trk::IExtrapolator> m_extrapolator;
  ServiceHandle<IBeamCondSvc>    m_beamSpotSvc;

  StringProperty 	 m_pixelClustersName;
  StringProperty	 m_sctClustersName;
  StringProperty         m_tracksTruthName;
  StringProperty         m_offlineName;// {this, "OfflineName", "InDetTrackParticles", "name of offline tracks collection"};

  BooleanProperty        m_dumpHitsOnTracks;
  BooleanProperty        m_dumpSpacePoints;
  BooleanProperty        m_dumpTruthIntersections;
  //  BooleanProperty        m_useOfflineTrackSelectorTool;
  BooleanProperty        m_readOfflineClusters;
  BooleanProperty        m_readTruthTracks; // if true the truth tracks are read
  BooleanProperty        m_readOfflineTracks;// {this, "GetOffline", false, "flag to enable the offline tracking save"};
  BooleanProperty        m_UseNominalOrigin; // if true truth values are always with respect to (0,0,0)
  DoubleProperty         m_maxEta;
  DoubleProperty         m_minPt;


  //internal pointers
  AtlasDetectorID*   m_idHelper;
  const PixelID*     m_pixelId;
  const SCT_ID*      m_sctId;

  const InDetDD::SiDetectorManager*     m_PIX_mgr;
  const InDetDD::SiDetectorManager*     m_SCT_mgr;
  const InDet::SiClusterContainer*      m_pixelContainer;
  const InDet::SiClusterContainer*      m_sctContainer;
  const HepPDT::ParticleDataTable*      m_particleDataTable;

  typedef std::map<Identifier,int> HitIndexMap;
  StatusCode read_raw_silicon( HitIndexMap& hitIndexMap, HitIndexMap& pixelClusterIndexMap ) ; // dump raw silicon data to text file and populate hitIndexMap for rec. track processing
  StatusCode read_truth_tracks(HTTOptionalEventInfo& optional);
  StatusCode read_offline_tracks(HTTOptionalEventInfo& optional);
  StatusCode read_offline_clusters(HTTOptionalEventInfo& optional);
  StatusCode ReadPixelSimulation( HitIndexMap& hitIndexMap, unsigned int &hitIndex );
  StatusCode ReadStripSimulation( HitIndexMap& hitIndexMap, unsigned int &hitIndex );  
  StatusCode DumpPixelClusters(HitIndexMap& pixelClusterIndexMap );

  // To get truth information from simulation
  void GetTruthInformation(InDetSimDataCollection::const_iterator &iter, TrigHTTInputUtils::ParentBitmask &parentMask, HepMcParticleLink::ExtendedBarCode &bestExtcode, const HepMC::GenParticle* bestParent);

};



#endif // HTT_SGHINPUTTOOL_H
