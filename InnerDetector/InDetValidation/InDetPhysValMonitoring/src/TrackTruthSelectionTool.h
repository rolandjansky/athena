/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_TRACKTRUTHSELECTORTOOL_H
#define INDETPHYSVALMONITORING_TRACKTRUTHSELECTORTOOL_H 1

// Framework include(s):
#include "AthenaBaseComps/AthAlgTool.h"
#include "PATCore/IAsgSelectionTool.h"
#include "xAODTruth/TruthParticle.h"
#include "AsgTools/AsgTool.h"

// EDM include(s):
#include "xAODTruth/TruthParticle.h"

// Local include(s):
#include "InDetPhysValMonitoring/ITrackTruthSelectionTool.h"

class TrackTruthSelectionTool:
  public virtual ::IAsgSelectionTool,
  public virtual ::ITrackTruthSelectionTool,
  public asg::AsgTool  {
  ASG_TOOL_CLASS2(TrackTruthSelectionTool, IAsgSelectionTool, ITrackTruthSelectionTool);
public:
  TrackTruthSelectionTool(const std::string &name);
  // TrackTruthSelectionTool(const std::string& type,const std::string& name,const IInterface* parent);
  virtual
  ~TrackTruthSelectionTool();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  // IAsgSelectionTool interface's selection
  virtual const Root::TAccept &getTAccept( ) const;  
  virtual const Root::TAccept &accept(const xAOD::IParticle *p) const;
  // ITrackTruthSelectionTool selection + setter
  virtual const Root::TAccept &accept(const xAOD::TruthParticle *p) const;
  virtual void setGeoTrackTruthSelection( double p_maxEta, double p_pixRad );
private:
  mutable Root::TAccept m_accept;
  std::vector<std::pair<std::string, std::string> > m_cuts;
  mutable ULong64_t m_numTruthProcessed; // !< a counter of the number of tracks proccessed
  mutable ULong64_t m_numTruthPassed; // !< a counter of the number of tracks that passed all cuts
  mutable std::vector<ULong64_t> m_numTruthPassedCuts; // !< tracks the number of tracks that passed each cut family

  // Cut values;
  // inferred from the detector geometry and not steerable from jO:
  //  * eta should correspond to detector's geometric acceptance
  float m_maxEta;
  //  * secondary particles should come from decays before pixel radius[mm]
  float m_decayBeforePixRadius;
 
  // properties steerable from jO:
  float m_maxPt;
  float m_minPt;
  float m_maxBarcode;
  bool m_requireCharged;
  bool m_requireStatus1;
  bool m_requireDecayBeforePixel;
  int m_pdgId;
};

#endif // > !INDETPHYSVALMONITORING_TRACKTRUTHSELECTORTOOL_H
