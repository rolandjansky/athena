///////////////////////// -*- C++ -*- ////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// JetCustomVertexTaggerTool.cxx
// Implementation file for class JetCustomVertexTaggerTool
// Author: James Frost <james.frost@cern.ch>
/////////////////////////////////////////////////////////////////// 

#include "JetMomentTools/JetCustomVertexTaggerTool.h"
#include "PathResolver/PathResolver.h"

#include "xAODTracking/VertexContainer.h"

using std::string;
using xAOD::JetFourMom_t;

//**********************************************************************

JetCustomVertexTaggerTool::JetCustomVertexTaggerTool(const std::string& name)
: JetVertexTaggerTool(name)
{
    declareProperty("customSumPtTrkName", m_customSumPtTrkName="customSumPtTrkPt500", "Name of output decoration, used for RpT");
    declareProperty("AssociatedTracks", m_assocTracksName = "GhostTrack",             "Name of associated tracks");
    declareProperty("z0cutHighEta",m_z0cutHighEta = 3., "z0sinTheta threshold for tracks with |eta|<z0etaDiv");
    declareProperty("z0cutLowEta", m_z0cutLowEta = 3.,  "z0sinTheta threshold for tracks with |eta|>=z0etaDiv");
    declareProperty("z0etaDiv",    m_z0etaDiv = 1.5,    "|eta| threshold above (below) which z0cutHighEta (z0cutLowEta) is used");
}

//**********************************************************************

int JetCustomVertexTaggerTool::modify(xAOD::JetContainer& jetCont) const {

  const xAOD::Vertex* HSvertex = findHSVertex();
  if(!HSvertex) return 1;

  for(xAOD::Jet * jet : jetCont) 
    {      
      float customsumpttrkpt500 = getSumPtTrkPt500(jet,HSvertex);
      jet->setAttribute(m_customSumPtTrkName,customsumpttrkpt500);
    } 

  return 0;
}

//**********************************************************************

float JetCustomVertexTaggerTool::updateJvt(const xAOD::Jet& jet) const {
  string sjvfcorr = m_jvfCorrName;
  float jvfcorr = jet.getAttribute<float>(sjvfcorr);
  float customsumpttrkpt500 = jet.getAttribute<float>(m_customSumPtTrkName);
  const float rptnew = customsumpttrkpt500/jet.pt();
  return evaluateJvt(rptnew, jvfcorr);
}

float JetCustomVertexTaggerTool::getSumPtTrkPt500(const xAOD::Jet* jet, const xAOD::Vertex* pv) const {
  std::vector<const xAOD::TrackParticle*> tracks;
  if ( ! jet->getAssociatedObjects(m_assocTracksName, tracks) ) {
    ATH_MSG_WARNING("Associated tracks not found.");
  }
  float tracksum = 0;
  for (size_t iTrack = 0; iTrack < tracks.size(); ++iTrack) {
    const xAOD::TrackParticle* track = tracks.at(iTrack);
    if (!m_htsel->keep(*track)) continue;
    if (track->vertex() && track->vertex()!=pv) continue;
    if (!track->vertex() && fabs((track->z0()+track->vz()-pv->z())*sin(track->theta()))>(fabs(track->eta())<m_z0etaDiv?m_z0cutLowEta:m_z0cutHighEta)) continue;
    tracksum += track->pt();
  }
  return tracksum;
}

//**********************************************************************
