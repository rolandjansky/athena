/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTPTools/MuonTPVertexHelper.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODMuon/Muon.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODBPhys/BPhysHypoHelper.h"


MuonTPVertexHelper::MuonTPVertexHelper(std::string myname): 
        AsgTool(myname),
        m_helper(0)
        {
    declareProperty("VertexContainer",      m_vertex_container_name="JpsiCandidates");
}


StatusCode MuonTPVertexHelper::initialize  (){
    ATH_MSG_DEBUG("in initialize");
    return StatusCode::SUCCESS;
}
bool MuonTPVertexHelper::checkVertexMatch (Probe& probe){

    ATH_MSG_DEBUG("in assignVertexMatch");
    ATH_MSG_DEBUG("retrieving the vertex container");
    const xAOD::VertexContainer* vertices =0;
    if(evtStore()->retrieve(vertices, m_vertex_container_name).isFailure()) {
        ATH_MSG_FATAL( "Unable to retrieve " << m_vertex_container_name );
        return false;
    }

    ATH_MSG_DEBUG("retrieving the track particles of tag and probe");
    const xAOD::Muon* tag = dynamic_cast<const xAOD::Muon*>(&(probe.tagTrack()));
    const xAOD::Muon* muprobe = dynamic_cast<const xAOD::Muon*>(&(probe.probeTrack()));
    const xAOD::TrackParticle* probeTrack = dynamic_cast<const xAOD::TrackParticle*>(&(probe.probeTrack()));

    if (!tag){
        if (dynamic_cast<const xAOD::TruthParticle*>(&(probe.probeTrack()))){
            ATH_MSG_DEBUG("Skipping truth probe");
            return false;
        }
        else {
            ATH_MSG_ERROR("Unable to retrieve tag muon in a TP pair!");
            return false;
        }
    }
    const xAOD::TrackParticle* tagTrack = tag->trackParticle( xAOD::Muon::InnerDetectorTrackParticle);
    if (muprobe && ! probeTrack){
        probeTrack  = muprobe->trackParticle( xAOD::Muon::InnerDetectorTrackParticle);
    }
    if (!tagTrack){
      if (tag->author() != xAOD::Muon::MuidSA) {
        ATH_MSG_ERROR("Unable to retrieve tag muon ID track!");
	ATH_MSG_ERROR("This tag muon has the following properties:");
	ATH_MSG_ERROR("\t Author: " << tag->author());
	ATH_MSG_ERROR("\t MuonType:" << tag->muonType());
	ATH_MSG_ERROR("\t pT: " << tag->pt());
	ATH_MSG_ERROR("\t eta: " << tag->eta());
	ATH_MSG_ERROR("\t charge: " << tag->charge());
        return false;
      }
      else {
        ATH_MSG_DEBUG("Unable to retrieve tag muon ID track, but is SA Muon!");
      }
    }
    if (!probeTrack){
        ATH_MSG_DEBUG("Probe has no ID track - not considered for a vertex match");
        return false;
    }
    ATH_MSG_DEBUG("attempting to find a matching vertex");
    for (const xAOD::Vertex* vx : *vertices){
        bool match_tag = false;
        bool match_probe = false;
        // check this vertex
        size_t nTracks = vx->nTrackParticles();
        ATH_MSG_DEBUG("  ...checking a vertex with "<<nTracks<<" Tracks...");
        for (size_t i = 0; i < nTracks;++i){
            const xAOD::TrackParticle* tp = vx->trackParticle(i);
            if (tp == tagTrack){
                ATH_MSG_DEBUG("     ...Track "<<i<< " is the tag...");
                match_tag = true;
            }
            else if (tp == probeTrack){
                ATH_MSG_DEBUG("     ...Track "<<i<< " is the probe...");
                match_probe = true;
            }
        }
        if (match_tag && match_probe){
            ATH_MSG_DEBUG("-> Found a matching vertex!");
            probe.setVertex(vx);
            // can safely point our helper towards this vertex
            switchHelper(vx);
            return true;
        }
    }
    ATH_MSG_DEBUG("Did NOT find a matching vertex!");

    return false;
}

float MuonTPVertexHelper::getVertexDisplacement (Probe & probe, const xAOD::BPhysHelper::pv_type pv_t) {

    if (!probe.getVertex()){
        ATH_MSG_DEBUG("no vertex for this TP pair --> not able to compute a displacement!");
        return -1e-9;
    }   
    // call this to make sure our helper points to the correct vertex
    switchHelper(probe.getVertex());
    float lxy = m_helper->lxy(pv_t);
    ATH_MSG_DEBUG("lxy = "<<lxy<<" mm");
    return lxy;
}

float MuonTPVertexHelper::getVertexTau (Probe & probe, const xAOD::BPhysHelper::pv_type pv_t) {

    if (!probe.getVertex()){
        ATH_MSG_DEBUG("no vertex for this TP pair --> not able to compute a displacement!");
        return -1e-9;
    }   
    // call this to make sure our helper points to the correct vertex
    switchHelper(probe.getVertex());
    float tau = m_helper->tau(pv_t);
    ATH_MSG_DEBUG("Obtained a tau of "<<tau<<" ps");
    return tau;
}
void MuonTPVertexHelper::switchHelper(const xAOD::Vertex* vx){
    if (vx == 0){
        ATH_MSG_WARNING("Caught attempt to instantiate a BPhysHypoHelper  with a NULL vertex");
        return;
    }
    ATH_MSG_DEBUG("switching our helper instance to point at vertex "<<vx);
    if (!m_helper){
        ATH_MSG_DEBUG("This is the first time we use the helper, instantiate it");
        m_helper = new xAOD::BPhysHypoHelper ("Jpsi",vx);
    }
    else {
        if (m_helper->vtx() == vx){
            ATH_MSG_DEBUG("Helper already points at the correct vertex");
        }
        else {
            ATH_MSG_DEBUG("Replacing the helper by a new one");
            delete m_helper;
            m_helper = 0;
            m_helper = new xAOD::BPhysHypoHelper ("Jpsi",vx);
        }
    }
}

float MuonTPVertexHelper::getChi2(Probe& probe)
{
    if (!probe.getVertex()){
        ATH_MSG_DEBUG("no vertex for this TP pair --> not able to compute Chi2!");
        return -1;
    }
    // call this to make sure our helper points to the correct vertex
    switchHelper(probe.getVertex());
    float chi2 = m_helper->vtx()->chiSquared();
    ATH_MSG_DEBUG("Obtained a chi2 of vertex fit of "<<chi2);
    return chi2;
}

int MuonTPVertexHelper::getDof(Probe& probe)
{
    if (!probe.getVertex()){
        ATH_MSG_DEBUG("no vertex for this TP pair --> not able to compute Dof!");
        return -1;
    }
    // call this to make sure our helper points to the correct vertex
    switchHelper(probe.getVertex());
    int dof = m_helper->vtx()->numberDoF();
    ATH_MSG_DEBUG("Obtained dof of vertex fit of "<< dof);
    return dof;
}

float MuonTPVertexHelper::getInvMass(Probe& probe)
{
    if (!probe.getVertex()){
        ATH_MSG_DEBUG("no vertex for this TP pair --> not able to compute invariant mass of refitted tracks!");
        return -1;
    }
    // call this to make sure our helper points to the correct vertex
    switchHelper(probe.getVertex());
    float mass = m_helper->mass();
    ATH_MSG_DEBUG("Obtained invariant mass of refitted tracks of "<< mass);
    return mass;
}
