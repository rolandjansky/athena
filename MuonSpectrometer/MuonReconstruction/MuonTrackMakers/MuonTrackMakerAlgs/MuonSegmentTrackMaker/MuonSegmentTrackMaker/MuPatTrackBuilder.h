/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUPATTRACKBUILDER_H
#define MUPATTRACKBUILDER_H

#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

namespace Muon {
  class IMuonTrackFinder;
  class MuonEDMHelperTool;
}
//class IMuonboyToParticleTool;
//namespace Muon {
//  class IMuonBackTracker;
//}

class MuPatTrackBuilder : public AthAlgorithm
{
 public:
  MuPatTrackBuilder(const std::string& name, ISvcLocator* pSvcLocator);

 public:
  virtual ~MuPatTrackBuilder();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:

  std::string m_segmentLocation;       //!< Location of input MuonSegmentCombination collection
  std::string m_spectroTrackLocation;  //!< Track output location for tracks strictly in MS
  std::string m_spectroPartiLocation;  //!< Track output location for track particles (strictly in MS)
  std::string m_extrapTrackLocation;  //!< Output location for back-extrapolated tracks
  std::string m_extrapPartiLocation;  //!< Output location for back-extrapolated particles

  ToolHandle<Muon::IMuonTrackFinder> m_trackMaker;  //!< Actual tool to do the track finding
//  ToolHandle<IMuonboyToParticleTool> m_convTool;    //!< Tool for converting from tracks to track particles (acts as a flag)
//  ToolHandle<Muon::IMuonBackTracker> p_IMuonBackTracker;    //!< Tool for extrapolating tracks
  ToolHandle<Muon::MuonEDMHelperTool> m_helper;    //!< helper Tool 
};

#endif 
