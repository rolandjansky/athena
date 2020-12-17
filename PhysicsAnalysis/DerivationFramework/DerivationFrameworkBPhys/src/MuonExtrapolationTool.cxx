/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
// MuonExtrapolationTool.cxx
#include "DerivationFrameworkBPhys/MuonExtrapolationTool.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TVector2.h"
#include "xAODMuon/MuonContainer.h"
//**********************************************************************

namespace DerivationFramework {

MuonExtrapolationTool::MuonExtrapolationTool(const std::string &t, const std::string& n, const IInterface* p)
  : 
    AthAlgTool(t, n, p),
    m_extrapolator("Trk::Extrapolator/AtlasExtrapolator")
{    
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("EndcapPivotPlaneZ",             m_endcapPivotPlaneZ = 15525.);// z position of pivot plane in endcap region
  declareProperty("EndcapPivotPlaneMinimumRadius", m_endcapPivotPlaneMinimumRadius = 0.);// minimum radius of pivot plane in endcap region
  declareProperty("EndcapPivotPlaneMaximumRadius", m_endcapPivotPlaneMaximumRadius = 11977.); // maximum radius of pivot plane in endcap region
  declareProperty("BarrelPivotPlaneRadius",        m_barrelPivotPlaneRadius = 8000.);// radius of pivot plane in barrel region
  declareProperty("BarrelPivotPlaneHalfLength",    m_barrelPivotPlaneHalfLength = 9700.);// half length of pivot plane in barrel region
  declareProperty("Extrapolator", m_extrapolator);
  declareProperty("MuonCollection", m_muonContainerName = "Muons");
}

//**********************************************************************


StatusCode MuonExtrapolationTool::initialize()
{
  ATH_CHECK(m_extrapolator.retrieve());
  return StatusCode::SUCCESS;
}


//**********************************************************************

bool MuonExtrapolationTool::extrapolateAndDecorateTrackParticle(const xAOD::TrackParticle* particle, float & eta, float & phi) const
{

  // decorators used to access or store the information 
  static SG::AuxElement::Decorator< char > Decorated ("DecoratedPivotEtaPhi");
  static SG::AuxElement::Decorator< float > Eta ("EtaTriggerPivot");
  static SG::AuxElement::Decorator< float > Phi ("PhiTriggerPivot");

  if (! Decorated.isAvailable(*particle) || !Decorated(*particle)){
    // in the athena release, we can run the extrapolation if needed
      const Trk::TrackParameters* pTag = extrapolateToTriggerPivotPlane(*particle);
      if(!pTag) {
        Decorated(*particle) = false;
        return false;
      }
      Eta(*particle) = pTag->position().eta();
      Phi(*particle) = pTag->position().phi();
      Decorated(*particle) = true;
      delete pTag;
    }
    // if we get here, the decoration was either already present or just added by us
    // so we can finally read the values
    eta = Eta(*particle);
    phi = Phi(*particle);
    return true;
}

//**********************************************************************

const xAOD::TrackParticle* MuonExtrapolationTool::getPreferredTrackParticle (const xAOD::IParticle* muon) const
{
  if (dynamic_cast<const xAOD::TruthParticle*>(muon)){
    ATH_MSG_WARNING("Pivot plane extrapolation not supported for Truth muons!");
    return 0;
  }
  const xAOD::TrackParticle* muonTrack = dynamic_cast<const xAOD::TrackParticle*>(muon);
  if(!muonTrack && dynamic_cast<const xAOD::Muon*>(muon)) {
    const xAOD::Muon* theMuon = dynamic_cast<const xAOD::Muon*>(muon);
    muonTrack = theMuon->trackParticle( xAOD::Muon::MuonSpectrometerTrackParticle );
    if(!muonTrack) {
      muonTrack = theMuon->primaryTrackParticle();
      if(!muonTrack) {
       muonTrack = theMuon->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
      }
    }
  }
  if(!muonTrack){
    ATH_MSG_WARNING("no valid track found for extrapolating the muon to the pivot plane!");
  }
  return muonTrack;

}

StatusCode MuonExtrapolationTool::addBranches() const
{
    const xAOD::MuonContainer* muons = NULL;
    CHECK(evtStore()->retrieve(muons, m_muonContainerName));
    for(auto muon : *muons){
       const xAOD::TrackParticle* track = getPreferredTrackParticle(muon);
       float eta, phi = 0;
       if( !extrapolateAndDecorateTrackParticle( track, eta, phi )){
           if( muon->pt() > 3500.){
             //only complain if the muon has sufficient pT to actually reach the pivot plane
             //extrapolation will often fail for muons with pT < 3500 MeV
             ATH_MSG_WARNING("Failed to extrapolate+decorate muon with pivot plane coords - Muon params: pt "<<muon->pt()<<", eta "<< muon->eta()<<", phi "<< muon->phi());
           }
        }   
    }
    return StatusCode::SUCCESS;
}

const Trk::TrackParameters* MuonExtrapolationTool::extrapolateToTriggerPivotPlane(const xAOD::TrackParticle& track) const
{
  // BARREL
  const Trk::Perigee& perigee = track.perigeeParameters();
  
  // create the barrel as a cylinder surface centered at 0,0,0
  Amg::Vector3D barrelCentre(0., 0., 0.);
  Amg::Transform3D* matrix = new Amg::Transform3D(Amg::RotationMatrix3D::Identity(), barrelCentre);
  
  Trk::CylinderSurface* cylinder = 
    new Trk::CylinderSurface(matrix,
			     m_barrelPivotPlaneRadius,
			     m_barrelPivotPlaneHalfLength);
  if (!cylinder) {
    ATH_MSG_WARNING("extrapolateToTriggerPivotPlane :: new Trk::CylinderSurface failed.");
    delete matrix;
    matrix = 0;
    return 0;
  }
  // and then attempt to extrapolate our track to this surface, checking for the boundaries of the barrel
  bool boundaryCheck = true;
  const Trk::Surface* surface = cylinder;
  const Trk::TrackParameters* p = m_extrapolator->extrapolate(perigee,
							      *surface,
							      Trk::alongMomentum,
							      boundaryCheck,
							      Trk::muon);
  delete cylinder;
  // if the extrapolation worked out (so we are in the barrel) we are done and can return the 
  // track parameters at this surface. 
  if (p) return p;

  // if we get here, the muon did not cross the barrel surface
  // so we assume it is going into the endcap. 
  // ENDCAP

  // After 2 years of using this code, we realised that ATLAS actually has endcaps on both sides ;-)
  // So better make sure we place our endcap at the correct side of the detector!  
  // Hopefully no-one will ever read this comment... 
  float SignOfEta = track.eta() > 0 ? 1. : -1.;

  Amg::Vector3D endcapCentre(0., 0., m_endcapPivotPlaneZ);
  // much better!
  matrix = new Amg::Transform3D(Amg::RotationMatrix3D::Identity(), SignOfEta * endcapCentre);
  
  Trk::DiscSurface* disc = 
    new Trk::DiscSurface(matrix,
			 m_endcapPivotPlaneMinimumRadius,
			 m_endcapPivotPlaneMaximumRadius);
  if (!disc) {
    ATH_MSG_WARNING("extrapolateToTriggerPivotPlane :: new Trk::DiscSurface failed."); 
    delete matrix; 
    matrix = 0; 
    return 0;
  }
  
  // for the endcap, we turn off the boundary check, extending the EC infinitely to catch stuff heading for the transition region
  boundaryCheck = false;
  surface = disc;
  p = m_extrapolator->extrapolate(perigee,
				  *surface,
				  Trk::alongMomentum,
				  boundaryCheck,
				  Trk::muon);
  delete disc; 
  return p;
}
}
