/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////
// Markus Elsing
/////////////////////////////////

#ifndef INDETAMBISCORINGTOOL_H
#define INDETAMBISCORINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrkCaloClusterROI/CaloClusterROI_Collection.h"
#include "TrkEventPrimitives/TrackScore.h"
#include "TrkToolInterfaces/ITrackScoringTool.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkParameters/TrackParameters.h"
#include "SGTools/CLASS_DEF.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include <vector>
#include <string>

class IBeamCondSvc;

namespace Trk {
  class IExtrapolator;
  class Track;
  class TrackSummary;
  class IBeamCondSvc;

}

namespace MagField {
  class IMagFieldSvc;
}

namespace InDet {
class ITrtDriftCircleCutTool;

/**Concrete implementation of the ITrackScoringTool pABC*/
class InDetAmbiScoringTool : virtual public Trk::ITrackScoringTool,  
                             public AthAlgTool
{

 public:
  InDetAmbiScoringTool(const std::string&,const std::string&,const IInterface*);
  virtual ~InDetAmbiScoringTool ();
  virtual StatusCode initialize() override;
  virtual StatusCode finalize  () override;
  /** create a score based on how good the passed track is*/
  virtual Trk::TrackScore score( const Trk::Track& track, const bool suppressHoleSearch ) const override;
  
  /** create a score based on how good the passed TrackSummary is*/
  virtual Trk::TrackScore simpleScore( const Trk::Track& track, const Trk::TrackSummary& trackSum ) const override;
  Trk::TrackScore  ambigScore( const Trk::Track& track, const Trk::TrackSummary& trackSum ) const;
  
  struct ROIInfo {
    ROIInfo (double the_emF,
             //double the_emE,
             double the_emR,
             double the_emZ)
    : emF(the_emF),
      //emE(the_emE),
      emR(the_emR),
      emZ(the_emZ) {}
    double emF;
    //double emE;
    double emR;
    double emZ;
  };
  typedef std::vector<ROIInfo> ROIInfoVec;

 private:
  const ROIInfoVec* getInfo() const;
  
  void setupScoreModifiers();
  
  
    /** Check if the cluster is compatible with a EM cluster*/
  bool isEmCaloCompatible(const Trk::Track& track,
                          const ROIInfoVec* info) const;
  
  
  //these are used for ScoreModifiers 
  int m_maxDblHoles, m_maxPixHoles, m_maxSCT_Holes,  m_maxHits, m_maxSigmaChi2, m_maxTrtRatio, m_maxTrtFittedRatio,
    m_maxB_LayerHits, m_maxPixelHits, m_maxPixLay, m_maxLogProb, m_maxGangedFakes;
  std::vector<double> m_factorDblHoles, m_factorPixHoles, m_factorSCT_Holes,  m_factorHits,
    m_factorSigmaChi2, m_factorB_LayerHits, m_factorPixelHits, m_factorPixLay, m_factorLogProb, m_factorHoles, m_factorGangedFakes;
  std::vector<double> m_boundsSigmaChi2, m_boundsLogProb, m_boundsHits,
    m_boundsTrtRatio, m_factorTrtRatio, m_boundsTrtFittedRatio, m_factorTrtFittedRatio;
  
  /**\todo make this const, once createSummary method is const*/
  ToolHandle<Trk::ITrackSummaryTool>         m_trkSummaryTool;

  /** Returns minimum number of expected TRT drift circles depending on eta. */
  ToolHandle<ITrtDriftCircleCutTool>          m_selectortool;
  
  /**holds the scores assigned to each Trk::SummaryType from the track's Trk::TrackSummary*/
  std::vector<Trk::TrackScore>           m_summaryTypeScore;
  
  ServiceHandle<IBeamCondSvc>            m_iBeamCondSvc; //!< pointer to the beam condition service
  
  ToolHandle<Trk::IExtrapolator>         m_extrapolator;
  ServiceHandle<MagField::IMagFieldSvc>  m_magFieldSvc;

  
  /** use the scoring tuned to Ambiguity processing or not */
  bool m_useAmbigFcn;
  bool m_useTRT_AmbigFcn;
  bool m_useLogProbBins;
  bool m_useSigmaChi2;
  
  bool m_usePixel;
  bool m_useSCT;

  /** cuts for selecting good tracks*/
  int    m_minNDF;        //!< minimal number of degrees of freedom cut
  //bool   m_fieldOn;       //!< do we have field on ?
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
  float m_minPtEm;
  float m_phiWidthEm;
  float m_etaWidthEm;

  SG::ReadHandleKey<CaloClusterROI_Collection> m_inputEmClusterContainerName;
};


} // namespace InDet


CLASS_DEF (InDet::InDetAmbiScoringTool::ROIInfoVec, 169195041, 0)
#endif 
