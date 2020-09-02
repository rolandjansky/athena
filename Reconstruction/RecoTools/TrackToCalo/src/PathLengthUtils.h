/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 PathLengthUtils.h  -  Description
 -------------------
 begin   : Summer 2014
 authors : Marius Cornelis van Woerden
 email   : mvanwoer@cern.ch
 changes :
 
 ***************************************************************************/

#ifndef PATHLENGTUTILS_H
#define PATHLENGTUTILS_H

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"

#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloGeoHelpers/CaloPhiRange.h"

#include <cmath>

#include "TrkCaloExtension/CaloExtensionHelpers.h"

  class PathLengthUtils{
  public:
      PathLengthUtils();
      ~PathLengthUtils();


      double pathInsideCell(const CaloCell& cell, const CaloExtensionHelpers::EntryExitLayerMap&  entryExitLayerMap) const;
      double get3DPathLength(const CaloCell& cell, const Amg::Vector3D& entry, const Amg::Vector3D& exit, double drFix, double dzFix ) const;

      double getPathLengthInTile(const CaloCell& cell, const Amg::Vector3D& entry, const Amg::Vector3D& exit ) const;
      //double path(xAOD::TrackParticle& trackParticle, const CaloCell* cell);
      //double path(xAOD::TrackParticle& trackParticle, const CaloCell_ID::CaloSample sample); 
      
  private:
      CaloSampling::CaloSample tileEntrance(CaloSampling::CaloSample sample) const;
      CaloSampling::CaloSample tileExit(CaloSampling::CaloSample sample) const;

      double phiMean(double a, double b) const;
      bool   crossedPhi(const CaloCell& cell, double phi_entrance, double phi_exit) const;
      double getPathLengthInEta(const CaloCell& cell, double eta_entrance, double eta_exit) const;
      double getPathLengthInZ(double zMin, double zMax, double z_entrance, double z_exit) const;
      double getPathLengthInZ(const CaloCell& cell, double z_entrance, double z_exit) const;
      bool   crossingMatrix(const Amg::MatrixX& Matrix ,const Amg::Vector3D& entry, Amg::Vector3D& path) const;
  }; 

inline double PathLengthUtils::phiMean(double a, double b) const { return 0.5*(a+b) + (a*b < 0)*M_PI; }

/** Return true if the cell crossed was crossed by the track in phi **/
inline bool PathLengthUtils::crossedPhi(const CaloCell& cell, double phi_entrance, double phi_exit) const {
  double mean_phi = phiMean(phi_entrance, phi_exit);
  double dphi = fabs( CaloPhiRange::diff(phi_entrance, phi_exit) );
  double phi_min = mean_phi - dphi, phi_max = mean_phi + dphi;
 
  return (CaloPhiRange::diff(cell.phi() + cell.caloDDE()->dphi()/2., phi_min) > 0 &&
          CaloPhiRange::diff(phi_max, cell.phi() - cell.caloDDE()->dphi()/2.) > 0);
}

/** Return the % of path length crossed by the track inside a cell in eta **/
inline double PathLengthUtils::getPathLengthInEta(const CaloCell& cell, double eta_entrance, double eta_exit) const {
  double etaMin = cell.eta() - 0.5*cell.caloDDE()->deta();
  double etaMax = cell.eta() + 0.5*cell.caloDDE()->deta();
  if ( fabs(eta_entrance  - eta_exit) < 1e-6 ) // to avoid FPE
    return eta_entrance > etaMin && eta_entrance < etaMax;
   
  double etaMinTrack = std::min(eta_entrance, eta_exit);
  double etaMaxTrack = std::max(eta_entrance, eta_exit);
  return (std::min(etaMax, etaMaxTrack) - std::max(etaMin, etaMinTrack))/(etaMaxTrack - etaMinTrack);
}

/** Return the % of path length crossed by the track inside a cell in Z **/
inline double PathLengthUtils::getPathLengthInZ(double zMin, double zMax, double z_entrance, double z_exit) const {
  if ( fabs(z_entrance  - z_exit) < 1e-6 ) // to avoid FPE
    return z_entrance > zMin && z_entrance < zMax;
 
  double zMinTrack = std::min(z_entrance, z_exit);
  double zMaxTrack = std::max(z_entrance, z_exit);
  return (std::min(zMax, zMaxTrack) - std::max(zMin, zMinTrack))/(zMaxTrack - zMinTrack);
}

/** Return the % of path length crossed by the track inside a cell in Z **/
inline double PathLengthUtils::getPathLengthInZ(const CaloCell& cell, double z_entrance, double z_exit) const {
  return getPathLengthInZ(cell.z() - 0.5*cell.caloDDE()->dz(), cell.z() + 0.5*cell.caloDDE()->dz(), z_entrance, z_exit);
}


#endif //> !PATHLENGTUTILS_H

