/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//implementation

#ifndef __TRIG_FTK_TRACK_CONVERTER_TOOL_H__
#define __TRIG_FTK_TRACK_CONVERTER_TOOL_H__

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigFTKSim/FTKTrack.h"
#include "TrigFTKToolInterfaces/ITrigFTKTrackConverter.h"
#include "TrigFTKToolInterfaces/ITrigFTKClusterConverterTool.h"

#include "TrkFitterInterfaces/ITrackFitter.h" 
#include "TrkFitterUtils/FitterTypes.h" 

class AtlasDetectorID;
class PixelID;
class SCT_ID;
class Identifier;
class IdentifierHash;

namespace InDetDD {
  class PixelDetectorManager;
}

namespace InDet {
  class PixelCluster;
  class SCT_Cluster;
}

class PRD_MultiTruthCollection;
class McEventCollection;


class TrigFTKTrackConverter : public AthAlgTool, virtual public ITrigFTKTrackConverter
{
 public:
  TrigFTKTrackConverter( const std::string&, const std::string&, const IInterface* );
  virtual ~TrigFTKTrackConverter(){};
  virtual StatusCode initialize();
  virtual StatusCode finalize  ();

  virtual StatusCode convert(const std::vector<FTKTrack*>&, InDet::PixelClusterContainer*, 
			     InDet::SCT_ClusterContainer*, TrackCollection*);
	virtual StatusCode convert(const TrackCollection* offlineTracks, TrigInDetTrackCollection* trigInDetTracks);

  virtual StatusCode addSiHitInfo(const FTKTrack*, TrigInDetTrack*);

 protected:

  InDet::SCT_Cluster*  createSCT_Cluster(const FTKHit&);
  InDet::PixelCluster* createPixelCluster(const FTKHit&, float);
  InDet::PixelClusterCollection* getCollection(InDet::PixelClusterContainer*, IdentifierHash);
  InDet::SCT_ClusterCollection*  getCollection(InDet::SCT_ClusterContainer*, IdentifierHash);
  StatusCode getMcTruthCollections();
  void createSCT_Truth(Identifier,const MultiTruth&);
  void createPixelTruth(Identifier,const MultiTruth&);

private:

  const PixelID* m_pixelId;
  const SCT_ID* m_sctId;
  
  const InDetDD::PixelDetectorManager* m_pixelManager;

  ToolHandle<Trk::ITrackFitter> m_trackFitter;
  ToolHandle<ITrigFTKClusterConverterTool>  m_clusterConverterTool;


  bool m_doFit;
  bool m_doTruth;
  std::string m_ftkPixelTruthName;
  std::string m_ftkSctTruthName;
  std::string m_mcTruthName;

  PRD_MultiTruthCollection* m_ftkPixelTruth;
  PRD_MultiTruthCollection* m_ftkSctTruth;

  const McEventCollection*  m_mcEventCollection;
  const AtlasDetectorID* m_idHelper;
  bool m_collectionsReady;
};

#endif
