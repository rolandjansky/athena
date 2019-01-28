/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_TRACKSELECTORTOOL_H
#define INDETPHYSVALMONITORING_TRACKSELECTORTOOL_H 1

#include "AthenaBaseComps/AthAlgTool.h"
#include "PATCore/IAsgSelectionTool.h"
#include "xAODTracking/TrackParticle.h"
#include "AsgTools/AsgTool.h"


class TrackSelectionTool:
  public virtual ::IAsgSelectionTool,
  public asg::AsgTool  {
  ASG_TOOL_CLASS1(TrackSelectionTool, IAsgSelectionTool);
public:
  TrackSelectionTool(const std::string& name);
  virtual
  ~TrackSelectionTool();

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual const asg::AcceptInfo& getAcceptInfo( ) const override;
  virtual asg::AcceptData accept(const xAOD::IParticle* p) const override;
  virtual asg::AcceptData accept(const xAOD::TrackParticle* p) const;
private:
  asg::AcceptInfo m_accept;
  std::vector<std::pair<std::string, std::string> > m_cuts;
  mutable ULong64_t m_numProcessed; // !< a counter of the number of tracks proccessed
  mutable ULong64_t m_numPassed; // !< a counter of the number of tracks that passed all cuts
  mutable std::vector<ULong64_t> m_numPassedCuts; // !< tracks the number of tracks that passed each cut family

  // Cut vales;
  float m_maxPt;
  float m_minPt;
  float m_maxEta;
  float m_minEta;
  float m_maxPrimaryImpact;
  float m_maxZImpact;
  float m_minPrimaryImpact;
  float m_minZImpact;
  float m_maxSecondaryImpact;
  float m_minSecondaryPt;
  int m_minClusters;
  int m_minSiNotShared;
  int m_maxShared;
  int m_minPixelHits;
  int m_maxHoles;
  int m_maxPixelHoles;
  int m_maxSctHoles;
  int m_maxDoubleHoles;
  float m_radMax;
  int m_nHolesMax;
  int m_nHolesGapMax;
  int m_seedFilterLevel;
  int m_maxTRTHighThresholdHits;
  int m_minTRTHighThresholdHits;
  int m_maxTRTHighThresholdOutliers;
  int m_maxSCTHits;
  int m_minSCTHits;
  int m_maxTRTOutliers;
  int m_maxBLayerSplitHits;
  int m_maxPixelOutliers;

  /* From TrackTruthSelectionTool
     float m_maxEta;
     float m_maxPt;
     float m_minPt;
     float m_maxBarcode;
     bool m_requireCharged;
     bool m_requireDecayBeforePixel;
     bool m_requireStatus1;
     int m_pdgId;
   */
};

#endif // > !INDETPHYSVALMONITORING_TRACKSELECTORTOOL_H
