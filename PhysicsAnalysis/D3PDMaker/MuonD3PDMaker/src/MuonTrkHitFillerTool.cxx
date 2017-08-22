/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////
//
// Author : Alaettin Serhan Mete (serhan@iastate.edu)
// Date   : 03 August 2009
//
//////////////////////////////////////////////////////
#include "MuonTrkHitFillerTool.h"
#include "AthenaKernel/errorcheck.h"
#include "TrkTrackSummary/TrackSummary.h"


namespace {

const int ID = 1;
const int MS = 2;

struct Var
{
  const char* name;
  xAOD::SummaryType type;
  int which;
};
const Var  vars[] =
{
  {"nOutliersOnTrack", xAOD::numberOfOutliersOnTrack, ID+MS},

  {"nBLHits",           xAOD::numberOfInnermostPixelLayerHits,               ID},
  {"nPixHits",          xAOD::numberOfPixelHits,                ID},
  {"nSCTHits",          xAOD::numberOfSCTHits,                  ID},
  {"nTRTHits",          xAOD::numberOfTRTHits,                  ID},
  {"nTRTHighTHits",     xAOD::numberOfTRTHighThresholdHits,     ID},
  {"nBLSharedHits",     xAOD::numberOfInnermostPixelLayerSharedHits,         ID},
  {"nPixSharedHits",    xAOD::numberOfPixelSharedHits,          ID},
  {"nPixHoles",         xAOD::numberOfPixelHoles,               ID},
  {"nSCTSharedHits",    xAOD::numberOfSCTSharedHits,            ID},
  {"nSCTHoles",         xAOD::numberOfSCTHoles,                 ID},
  {"nTRTOutliers",      xAOD::numberOfTRTOutliers,              ID},
  {"nTRTHighTOutliers", xAOD::numberOfTRTHighThresholdOutliers, ID},
  {"nGangedPixels",     xAOD::numberOfGangedPixels,             ID},
  {"nPixelDeadSensors", xAOD::numberOfPixelDeadSensors,         ID},
  {"nSCTDeadSensors",   xAOD::numberOfSCTDeadSensors,           ID},
  {"nTRTDeadStraws",    xAOD::numberOfTRTDeadStraws,            ID},
  {"expectInnermostPixelLayerHit",   xAOD::expectInnermostPixelLayerHit,                  ID},

  {"nprecisionLayers",      xAOD::numberOfPrecisionLayers,      MS},
  {"nprecisionHoleLayers",  xAOD::numberOfPrecisionHoleLayers,  MS},
  {"nphiLayers",            xAOD::numberOfPhiLayers,            MS},
  {"nphiHoleLayers",        xAOD::numberOfPhiHoleLayers,        MS},
  {"ntrigEtaLayers",     xAOD::numberOfTriggerEtaLayers,     MS},
  {"ntrigEtaHoleLayers", xAOD::numberOfTriggerEtaHoleLayers, MS},
};


}


namespace D3PD {


MuonTrkHitFillerTool::MuonTrkHitFillerTool (const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent)
  : BlockFillerTool<xAOD::Muon> (type, name, parent)
{
  declareProperty ("SaveIDMuonHitSummary", m_saveIDMuonHitSummary = true);
  declareProperty ("SaveMSMuonHitSummary", m_saveMSMuonHitSummary = true);
}


StatusCode MuonTrkHitFillerTool::book()
{
  int mask = 0;
  if (m_saveIDMuonHitSummary) mask += ID;
  if (m_saveMSMuonHitSummary) mask += MS;

  m_vars.reserve (std::distance (std::begin (vars), std::end (vars)));
  for (const Var& v : vars) {
    if (v.which & mask) {
      m_vars.push_back (std::make_pair (v.type, nullptr));
      CHECK( addVariable (v.name, m_vars.back().second) );
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode MuonTrkHitFillerTool::fill (const xAOD::Muon& p)
{
  for (auto& v : m_vars)
    p.summaryValue (*v.second, v.first);
  return StatusCode::SUCCESS;
}


} // namespace D3PD
