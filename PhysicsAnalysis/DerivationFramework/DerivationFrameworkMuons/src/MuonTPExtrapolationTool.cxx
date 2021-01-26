/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkMuons/MuonTPExtrapolationTool.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TVector2.h"

namespace{
    constexpr float min_warn_pt = 3500;
    constexpr float dummy_result = 5.;
    
    enum ExtStatus{
        Success = 1,
        Failed = 2,
        NotPresent = 0,
    };
}
MuonTPExtrapolationTool::MuonTPExtrapolationTool(const std::string& myname) :
        AsgTool(myname),
        m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
         m_endcapPivotPlaneZ(15525.),
         m_endcapPivotPlaneMinimumRadius(0.),
         m_endcapPivotPlaneMaximumRadius(11977),
         m_barrelPivotPlaneRadius(8000.),
         m_barrelPivotPlaneHalfLength(9700),
         m_is_on_DAOD(false) {
    declareProperty("EndcapPivotPlaneZ", m_endcapPivotPlaneZ); // z position of pivot plane in endcap region
    declareProperty("EndcapPivotPlaneMinimumRadius", m_endcapPivotPlaneMinimumRadius); // minimum radius of pivot plane in endcap region
    declareProperty("EndcapPivotPlaneMaximumRadius", m_endcapPivotPlaneMaximumRadius); // maximum radius of pivot plane in endcap region
    declareProperty("BarrelPivotPlaneRadius", m_barrelPivotPlaneRadius ); // radius of pivot plane in barrel region
    declareProperty("BarrelPivotPlaneHalfLength", m_barrelPivotPlaneHalfLength); // half length of pivot plane in barrel region
    declareProperty("Extrapolator", m_extrapolator);
    declareProperty("IsRunOnDAOD", m_is_on_DAOD);
}



StatusCode MuonTPExtrapolationTool::initialize() {
    if (!m_is_on_DAOD) ATH_CHECK(m_extrapolator.retrieve());
    return StatusCode::SUCCESS;
}
bool MuonTPExtrapolationTool::is_extrapolated(const xAOD::IParticle* probe) const{
    static const SG::AuxElement::ConstAccessor<char> acc_Decorated("DecoratedPivotEtaPhi");
    return  acc_Decorated.isAvailable(*probe) && acc_Decorated(*probe) == ExtStatus::Success;
}
bool MuonTPExtrapolationTool::extrapolateAndDecorateTrackParticle(const xAOD::TrackParticle* particle, float & eta, float & phi) const {

    // decorators used to access or store the information
    static const SG::AuxElement::Decorator<char> dec_Decorated("DecoratedPivotEtaPhi");
    static const SG::AuxElement::Decorator<float> dec_Eta("EtaTriggerPivot");
    static const SG::AuxElement::Decorator<float> dec_Phi("PhiTriggerPivot");


    static const SG::AuxElement::Accessor<float> acc_Eta("EtaTriggerPivot");
    static const SG::AuxElement::Accessor<float> acc_Phi("PhiTriggerPivot");
    
    eta = phi = dummy_result;
    if (!is_extrapolated(particle)) {
        if (!m_is_on_DAOD) {
            // in the athena release, we can run the extrapolation if needed
            std::unique_ptr<const Trk::TrackParameters> pTag = extrapolateToTriggerPivotPlane(*particle);
            if (!pTag) {
                // complain only if the particle has sufficient pt to actually make it to the MS...
                if (particle->pt() > min_warn_pt) ATH_MSG_WARNING("Warning - Pivot plane extrapolation failed for a track particle with IP pt " << particle->pt() << ", eta " << particle->eta() << ", phi " << particle->phi());
                dec_Decorated(*particle) = ExtStatus::Failed;
                return false;
            }
            dec_Eta(*particle) = pTag->position().eta();
            dec_Phi(*particle) = pTag->position().phi();
            dec_Decorated(*particle) = ExtStatus::Success;            
        } else return false;        
    }    
    if (m_is_on_DAOD || is_extrapolated(particle) ) {
        eta = acc_Eta(*particle);
        phi = acc_Phi(*particle);
    } 
    return true;
}



const xAOD::TrackParticle* MuonTPExtrapolationTool::getPreferredTrackParticle(const xAOD::IParticle* probe) const {
    const xAOD::TrackParticle* probeTrack = nullptr;
    if (probe->type() == xAOD::Type::ObjectType::TruthParticle){
        ATH_MSG_WARNING("Pivot plane extrapolation not supported for Truth probes!");
    } else if (probe->type() == xAOD::Type::ObjectType::TrackParticle){
        probeTrack = dynamic_cast<const xAOD::TrackParticle*>(probe);
    } else if (probe->type() == xAOD::Type::ObjectType::Muon) {
        const xAOD::Muon* probeMuon = dynamic_cast<const xAOD::Muon*>(probe);
        probeTrack = probeMuon->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);
        if (!probeTrack) {
            probeTrack = probeMuon->primaryTrackParticle();
            if (!probeTrack) {
                probeTrack = probeMuon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
            }
        }
    }
    if (!probeTrack) {
        ATH_MSG_WARNING("no valid track found for extrapolating the probe to the pivot plane!");
    }
    return probeTrack;

}
// **********************************************************************
StatusCode MuonTPExtrapolationTool::decoratePivotPlaneCoords(const xAOD::IParticle* particle) const{

    const xAOD::TrackParticle* track = getPreferredTrackParticle(particle);
    float eta, phi = 0;
    if (!extrapolateAndDecorateTrackParticle(track, eta, phi)) {
        return StatusCode::FAILURE;
    }        
    return StatusCode::SUCCESS;
}

// **********************************************************************

double MuonTPExtrapolationTool::dROnTriggerPivotPlane(const xAOD::Muon& tag, const xAOD::IParticle* probe) const {
// should never happen, but better be safe than sorry
    if (!probe) {
        ATH_MSG_WARNING("No valid probe provided to MuonTPExtrapolationTool::dROnTriggerPivotPlane! ");
        return 0;
    }
    // starting values: Track direction @ IP
    float tag_eta = tag.eta();
    float tag_phi = tag.phi();
    float probe_eta = probe->eta();
    float probe_phi = probe->phi();

    // Try to replace the starting values by the extrapolated pivot plane coordinates
    //
    // First, we need to pick the TrackParticle to extrapolate.
    const xAOD::TrackParticle* tagTrack = getPreferredTrackParticle(&tag);
    if (tagTrack) {
        // If we have the track particle, we load the existing decorations for the
        // pivot plane eta/phi, or (in full athena) to run the extrapolation and decorate.
        // If the method returns true, we know that eta/phi were successfully updated
        // If this does not work, the method returns a false.
        // In that case, eta/phi are not changed and we are stuck with the IP direction.
        // Print a warning just to let the user know.
        if (!extrapolateAndDecorateTrackParticle(tagTrack, tag_eta, tag_phi)) {
           
        }
    }

    // Now we repeat exactly the same exercise as above for the probe

    // Special flag in case an ID track doesn't reach the MS, causing the extrap to fail.
    // In this case, we fill an unphysical dummy value of -1 for the delta R, and -10 each eta / phi / dphi / deta.
    // In the selection code, we accept these probes, since they can not cause a trigger-related bias
    bool IDProbeMissesMS = false;
    const xAOD::TrackParticle* probeTrack = getPreferredTrackParticle(probe);
    
    if (probeTrack) {
        if (!extrapolateAndDecorateTrackParticle(probeTrack, probe_eta, probe_phi)) {
            // if an ID probe does not make it into the MS, we kick it out by returning an unphysical delta R of -100.
            if (probe->type() == xAOD::Type::ObjectType::TrackParticle) {
                ATH_MSG_DEBUG("ID probe does not reach MS, returning unphysical dummy value for the exTP branches!");
                IDProbeMissesMS = true;
            } else {
                const xAOD::Muon* mu = dynamic_cast<const xAOD::Muon*>(probe);
                ATH_MSG_DEBUG("Could not get the muon probe eta/phi @ pivot plane, muon type and author are " << mu->muonType() << " and " << mu->author());
            }
        }
    }

    float deta = tag_eta - probe_eta;
    float dphi = TVector2::Phi_mpi_pi(tag_phi - probe_phi);
    float dr = std::sqrt(deta * deta + dphi * dphi);

    // decorators for the delta eta and delta phi and dR
    static const SG::AuxElement::Decorator<float> DecDeta("probe_dEtaTriggerPivot");
    static const SG::AuxElement::Decorator<float> DecDphi("probe_dPhiTriggerPivot");
    static const SG::AuxElement::Decorator<float> DecDR("probe_dRTriggerPivot");

    // here, we copy the eta and phi decorations from the TrackParticles to the
    // tag and probe IParticles - to make them easier to access during the
    // further processing
    // In the case of an unsuccessful extrap, this additionally ensures that the
    // IP direction for eta/phi is written to the output tree
    // instead of a garbage value that would result from a missing decoration
    static const SG::AuxElement::Decorator<float> DecEta("probe_EtaTriggerPivot");
    static const SG::AuxElement::Decorator<float> DecPhi("probe_PhiTriggerPivot");

    // special unphysical values to signify an ID probe not reaching the pivot plane
    if (IDProbeMissesMS) {
        deta = dphi = probe_phi = probe_eta = -10;
        dr = -1;
    }

    // note that here we only decorate the tag muon with the properties of the TP pair.
    // This has the background that, for ID probes, the probe may be a non-const TrackParticle
    // with a locked AuxStore.
    // If we attempted to overwrite an existing (e.g from DAOD) decoration, we would not be able to
    // and get an exception. The tag, being a shallow copy of a muon, is fine however and suited for our purpose
    DecDeta(tag) = deta;
    DecDphi(tag) = dphi;
    DecDR(tag) = dr;

    // here, unlike the above, we can get away with simply ignoring the exception if the store is locked
    // since the eta and the phi in the original decoration continue to be valid.
    try {
        DecEta(tag) = tag_eta;
        DecPhi(tag) = tag_phi;
        DecEta(*probe) = probe_eta;
        DecPhi(*probe) = probe_phi;
    } catch (const SG::ExcStoreLocked &) {
        // Maintain a nice, passive agressive silence here.
    }
    // the final variable we care about is the delta R.
    return dr;
}



std::unique_ptr<const Trk::TrackParameters> MuonTPExtrapolationTool::extrapolateToTriggerPivotPlane(const xAOD::TrackParticle& track) const {
// BARREL
    const Trk::Perigee& perigee = track.perigeeParameters();

// create the barrel as a cylinder surface centered at 0,0,0
    Amg::Vector3D barrelCentre(0., 0., 0.);
    Amg::Transform3D* matrix = new Amg::Transform3D(Amg::RotationMatrix3D::Identity(), barrelCentre);

    std::unique_ptr<Trk::CylinderSurface> cylinder = std::make_unique< Trk::CylinderSurface>(matrix, m_barrelPivotPlaneRadius, m_barrelPivotPlaneHalfLength);
    
    // and then attempt to extrapolate our track to this surface, checking for the boundaries of the barrel
    bool boundaryCheck = true;
       
    std::unique_ptr<const Trk::TrackParameters> p ( m_extrapolator->extrapolate(perigee, *cylinder, Trk::alongMomentum, boundaryCheck, Trk::muon));
    
    // if the extrapolation worked out (so we are in the barrel) we are done and can return the
    // track parameters at this surface.
    if (p) return p;

    // if we get here, the muon did not cross the barrel surface
    // so we assume it is going into the endcap.
    // ENDCAP

    // After 2 years of using this code, we realised that ATLAS actually has endcaps on both sides ;-)
    // So better make sure we place our endcap at the correct side of the detector!
    // Hopefully no-one will ever read this comment...
    const int SignOfEta = track.eta() > 0 ? 1. : -1.;

    Amg::Vector3D endcapCentre(0., 0., m_endcapPivotPlaneZ);
    // much better!
    matrix = new Amg::Transform3D(Amg::RotationMatrix3D::Identity(), SignOfEta * endcapCentre);

    std::unique_ptr<Trk::DiscSurface> disc = std::make_unique< Trk::DiscSurface>(matrix, m_endcapPivotPlaneMinimumRadius, m_endcapPivotPlaneMaximumRadius);
   

    boundaryCheck = false;
    return std::unique_ptr<const Trk::TrackParameters>(m_extrapolator->extrapolate(perigee, *disc, Trk::alongMomentum, boundaryCheck, Trk::muon)); 
}
