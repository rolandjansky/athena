/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////
// Markus Elsing
/////////////////////////////////

#ifndef INDETNNSCORINGTOOL_H
#define INDETNNSCORINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "InDetTrackScoringTools/ROIInfoVec.h"
#include "TrkEventPrimitives/TrackScore.h"
#include "TrkToolInterfaces/ITrackScoringTool.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "lwtnn/lightweight_network_config.hh"
#include "lwtnn/LightweightNeuralNetwork.hh"
#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/parse_json.hh"
#include "PathResolver/PathResolver.h"
#include "TrkParameters/TrackParameters.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/IOVSvcDefs.h"
// MagField cache
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldElements/AtlasFieldCache.h"
#include <fstream>
#include <vector>
#include <string>
#include "BeamSpotConditionsData/BeamSpotData.h"
#include "ROIInfoVec.h"

namespace Trk {
  class IExtrapolator;
  class Track;
  class TrackSummary;

}

namespace InDet {
class ITrtDriftCircleCutTool;

/**Concrete implementation of the ITrackScoringTool pABC*/
class InDetNNScoringTool : virtual public Trk::ITrackScoringTool,  
                             public AthAlgTool
{

 public:
  InDetNNScoringTool(const std::string&,const std::string&,const IInterface*);
  virtual ~InDetNNScoringTool ();
  virtual StatusCode initialize() override;
  virtual StatusCode finalize  () override;
  /** create a score based on how good the passed track is*/
  virtual Trk::TrackScore score( const Trk::Track& track, const bool suppressHoleSearch ) const override;
  
  /** create a score based on how good the passed TrackSummary is*/
  virtual Trk::TrackScore simpleScore( const Trk::Track& track, const Trk::TrackSummary& trackSum ) const override;
  Trk::TrackScore  ambigScore( const Trk::Track& track, const Trk::TrackSummary& trackSum ) const;
  Trk::TrackScore calcNnScore( const Trk::Track &track, const Trk::TrackSummary& trackSum , const Trk::Perigee *extrapolatedPerigee) const;
  

 private:
  void setupScoreModifiers();
  
    // DNN tools
    std::unique_ptr<lwt::LightweightGraph> m_graph;
    std::map<std::string, double> m_DNN_inputValues;
  
    /** Check if the cluster is compatible with a EM cluster*/
  bool isEmCaloCompatible(const Trk::Track& track) const;
  
  
  //these are used for ScoreModifiers 
  int m_maxDblHoles, m_maxPixHoles, m_maxSCT_Holes,  m_maxHits, m_maxSigmaChi2, m_maxTrtRatio, m_maxTrtFittedRatio,
    m_maxB_LayerHits, m_maxPixelHits, m_maxPixLay,  m_maxGangedFakes;
  std::vector<double> m_factorDblHoles, m_factorPixHoles, m_factorSCT_Holes,  m_factorHits,
    m_factorSigmaChi2, m_factorB_LayerHits, m_factorPixelHits, m_factorPixLay, m_factorHoles, m_factorGangedFakes;
  std::vector<double> m_boundsSigmaChi2, m_boundsHits,
    m_boundsTrtRatio, m_factorTrtRatio, m_boundsTrtFittedRatio, m_factorTrtFittedRatio;
  
  /**\todo make this const, once createSummary method is const*/
  ToolHandle<Trk::ITrackSummaryTool>         m_trkSummaryTool;

  /** Returns minimum number of expected TRT drift circles depending on eta. */
  ToolHandle<ITrtDriftCircleCutTool>          m_selectortool;
  
  /**holds the scores assigned to each Trk::SummaryType from the track's Trk::TrackSummary*/
  std::vector<Trk::TrackScore>           m_summaryTypeScore;
  
  SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
  
  ToolHandle<Trk::IExtrapolator>         m_extrapolator;

  // Read handle for conditions object to get the field cache
  SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};

  
  /** use the scoring tuned to Ambiguity processing or not */
  bool m_useAmbigFcn;
  bool m_useTRT_AmbigFcn;
  bool m_useSigmaChi2;
  std::string m_nnCutConfig;
  double m_nnCutThreshold;
  
  bool m_usePixel;
  bool m_useSCT;

  /** cuts for selecting good tracks*/
  double m_minPt;         //!< minimal Pt cut
  double m_maxEta;        //!< maximal Eta cut
  double m_maxRPhiImp;    //!< maximal RPhi impact parameter cut
  double m_maxZImp;       //!< maximal z impact parameter cut
  int    m_minSiClusters; //!< minimal number of Si clusters
  int    m_maxDoubleHoles;//|< maximum number of SCT double holes
  int    m_maxSiHoles;    //!< maximal number of holes (Pixel+SCT)
  int    m_maxPixelHoles; //!< maximal number of Pixel holes
  int    m_maxSctHoles;   //!< maximal number of SCT holes
  int    m_minTRTonTrk;   //!< minimum number of TRT hits
  double m_minTRTprecision;   //!< minimum fraction of TRT precision hits
  int    m_minPixel;      //!< minimum number of pixel clusters
  double m_maxRPhiImpEM;    //!< maximal RPhi impact parameter cut track that match EM clusters

  bool  m_useEmClusSeed;
  float m_phiWidthEm;
  float m_etaWidthEm;

  SG::ReadHandleKey<InDet::ROIInfoVec> m_caloROIInfoKey
     {this,"CaloROIInfoName", "ROIInfoVec","Name of the calo cluster ROI vector."};
};


} // namespace InDet


#endif 
