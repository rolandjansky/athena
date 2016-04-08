/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaUtils/EMConversionUtils.h"
#include "xAODTracking/Vertex.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkSurfaces/PerigeeSurface.h" 

namespace EMConversionUtils {

  /** @brief struct of Particle Masses */
  const Trk::ParticleMasses particleMasses;

  /** @brief Get the position and momentum of the conversion candidate  */
  void getConversionPositionAndPt(const xAOD::Vertex* myCandidate, Amg::Vector3D& pos, double& pt) 
  {
    
    const std::vector< Trk::VxTrackAtVertex > trkAtVx ((myCandidate->vxTrackAtVertex()));
    
    pos = myCandidate->position();
    
    //invariant mass, photon pT
    if(trkAtVx.size() == 2) {
      const Trk::TrackParameters*perigee1 = dynamic_cast<const Trk::Perigee*>(trkAtVx[0].perigeeAtVertex());
      const Trk::TrackParameters*perigee2 = dynamic_cast<const Trk::Perigee*>(trkAtVx[1].perigeeAtVertex());
      if (perigee1 && perigee2) {
	const  Amg::Vector3D sum_mom = perigee1->momentum() + perigee2->momentum();
	pt = sum_mom.perp();
      }
    } else {
      const Trk::TrackParameters*perigee1 = dynamic_cast<const Trk::Perigee*>(trkAtVx[0].perigeeAtVertex());
      if (perigee1) {
	pt = perigee1->momentum().perp();
      }
    }
  }

  /** @brief Get the momentum of the conversion candidate  */
  double getConversionP(const xAOD::Vertex* myCandidate) 
  {
    const std::vector< Trk::VxTrackAtVertex > trkAtVx ((myCandidate->vxTrackAtVertex()));
    
    //invariant mass, photon pT
    if(trkAtVx.size() == 2) {
      const Trk::TrackParameters* perigee1 = dynamic_cast<const Trk::Perigee*>(trkAtVx[0].perigeeAtVertex());
      const Trk::TrackParameters* perigee2 = dynamic_cast<const Trk::Perigee*>(trkAtVx[1].perigeeAtVertex());
      if (perigee1 && perigee2) {
	const Amg::Vector3D sum_mom = perigee1->momentum() + perigee2->momentum();
	return sum_mom.mag();
      } else {
	return -999.;
      }
    } else {
      const Trk::TrackParameters* perigee1 = dynamic_cast<const Trk::Perigee*>(trkAtVx[0].perigeeAtVertex());
      if (perigee1) {
	return perigee1->momentum().mag();
      } else {
	return -999.;
      }
    }
  }

  /** @brief Get the position of the conversion candidate  */
  Amg::Vector3D getConversionPosition(const xAOD::Vertex* myCandidate) 
  {
    
    return  myCandidate->position();
  }

  /** @brief Create neutral track parameters for the reconstructed converted photon.  */
  const Trk::NeutralPerigee* createPhotonParameters(const xAOD::Vertex* myCandidate)
  {
    const std::vector< Trk::VxTrackAtVertex > trkAtVx ((myCandidate->vxTrackAtVertex()));

    if(trkAtVx.size() == 2){
      const Trk::TrackParameters* perigee1 = dynamic_cast<const Trk::Perigee*>(trkAtVx[0].perigeeAtVertex());
      const Trk::TrackParameters* perigee2 = dynamic_cast<const Trk::Perigee*>(trkAtVx[1].perigeeAtVertex());
      if (!perigee1 || !perigee2)
        return 0;
      const Amg::Vector3D sum_mom = perigee1->momentum() + perigee2->momentum();

      //Get the photon inverse momentum
      const double p = sum_mom.mag();
      const double inv_p = (p != 0) ? 1.0/p : 0;

      //const Trk::RecVertex & vertex = myCandidate->recVertex();

      const AmgSymMatrix(5) cov1 = *(perigee1->covariance());
      const AmgSymMatrix(5) cov2 = *(perigee2->covariance());
      AmgSymMatrix(5)* eM =  new AmgSymMatrix(5); 
      (*eM) = cov1+cov2;

      //
      Amg::Vector3D pos(myCandidate->position().x(),myCandidate->position().y(),myCandidate->position().z()); 
      const Trk::PerigeeSurface   surface(pos); 
      //Construct the photon neutral parameters as neutral perigee
      const Trk::NeutralPerigee* neutPar = surface.createParameters<5,Trk::Neutral>(  
						  0.0 ,
						  0.0, 
						  sum_mom.phi(),
						  sum_mom.theta(),
						  inv_p,
						  eM);
      return neutPar;
    } else {
      return 0;
    } 
  }
}
