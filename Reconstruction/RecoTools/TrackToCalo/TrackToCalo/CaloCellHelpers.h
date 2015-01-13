/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REC_CALOCELLHELPERS_H
#define REC_CALOCELLHELPERS_H

#include <cmath>
#include "CaloEvent/CaloCell.h"
#include "CaloGeoHelpers/CaloPhiRange.h"

inline double phiMean(double a, double b) { return 0.5*(a+b) + (a*b < 0)*M_PI; }

/** Return true if the cell crossed was crossed by the track in phi **/
inline bool crossedPhi(const CaloCell& cell, double phi_entrance, double phi_exit) {
  double mean_phi = phiMean(phi_entrance, phi_exit);
  double dphi = fabs( CaloPhiRange::diff(phi_entrance, phi_exit) );
  double phi_min = mean_phi - dphi, phi_max = mean_phi + dphi;
 
  return (CaloPhiRange::diff(cell.phi() + cell.caloDDE()->dphi()/2., phi_min) > 0 &&
          CaloPhiRange::diff(phi_max, cell.phi() - cell.caloDDE()->dphi()/2.) > 0);
}

/** Return the % of path length crossed by the track inside a cell in eta **/
inline double getPathLengthInEta(const CaloCell& cell, double eta_entrance, double eta_exit) {
  double etaMin = cell.eta() - 0.5*cell.caloDDE()->deta();
  double etaMax = cell.eta() + 0.5*cell.caloDDE()->deta();
  if ( fabs(eta_entrance  - eta_exit) < 1e-6 ) // to avoid FPE
    return eta_entrance > etaMin && eta_entrance < etaMax;
   
  double etaMinTrack = std::min(eta_entrance, eta_exit);
  double etaMaxTrack = std::max(eta_entrance, eta_exit);
  return (std::min(etaMax, etaMaxTrack) - std::max(etaMin, etaMinTrack))/(etaMaxTrack - etaMinTrack);
}

/** Return the % of path length crossed by the track inside a cell in Z **/
inline double getPathLengthInZ(double zMin, double zMax, double z_entrance, double z_exit) {
  if ( fabs(z_entrance  - z_exit) < 1e-6 ) // to avoid FPE
    return z_entrance > zMin && z_entrance < zMax;
 
  double zMinTrack = std::min(z_entrance, z_exit);
  double zMaxTrack = std::max(z_entrance, z_exit);
  return (std::min(zMax, zMaxTrack) - std::max(zMin, zMinTrack))/(zMaxTrack - zMinTrack);
}

/** Return the % of path length crossed by the track inside a cell in Z **/
inline double getPathLengthInZ(const CaloCell& cell, double z_entrance, double z_exit) {
  return getPathLengthInZ(cell.z() - 0.5*cell.caloDDE()->dz(), cell.z() + 0.5*cell.caloDDE()->dz(), z_entrance, z_exit);
}

// /** Return the % of path length crossed by the track inside a cell in Z for a ladder shaped cell **/
// inline double getPathLengthInZTileBar1(const CaloCell& cell, double z_entrance, double z_exit) {
//   // Divide the problem in 2 rectangles: the cell contains 6 tile rows, 3 in the bottom part, 3 in the top
//   // Calculate the point where the track crossed the boundary between the bottom and top parts
//   // and determine the path lenght inside each part
//   TileCellDim *cell_dim = m_tileDDM->get_cell_dim(cell.caloDDE()->identify());
//   if (!cell_dim || cell_dim->getNRows() != 6) // should always be the case for this sampling
//     return 0;

//   // Get the percentage in depth covered by the bottom part of the cell
//   double R = (cell_dim->getRMax(2) - cell_dim->getRMin(0))/(cell_dim->getRMax(5) - cell_dim->getRMin(0));

//   // The point where the track crossed the boundary
//   double z_middle = z_entrance + R*(z_exit - z_entrance);

//   // Get the boundaries of the 2 half-cells
//   double zBottom_min = cell_dim->getZMin(0), zBottom_max = cell_dim->getZMax(0);
//   double zTop_min = cell_dim->getZMin(3), zTop_max = cell_dim->getZMax(3);
//   double pathBottom = getPathLengthInZ(zBottom_min, zBottom_max, z_entrance, z_middle);

//   // Calculate the path traversed in the bottom and top parts
//   if (zTop_min == zTop_max) { // top part of B9 cell has 0 width. Don't take the mean in this case 
//     if ( fabs(z_middle) > fabs(zBottom_max) && pathBottom > 0)
//       pathBottom = 1;
//     return pathBottom;
//   }
//   double pathTop = getPathLengthInZ(zTop_min, zTop_max, z_middle, z_exit);
//   // Take the weighted mean using the depth ratio since the method gives the %
//   return R*pathBottom + (1.-R)*pathTop;
// }

/** Return the % of the path crossed inside the cell, given the parameters for the extrapolation at entrance and exit of the layer **/
inline double pathInsideCell(const CaloCell& cell, const Amg::Vector3D& entry, const Amg::Vector3D& exit ) {
  if (!crossedPhi(cell, entry.phi(), exit.phi()))
    return 0;
  double pathCrossed = 0;
  if (cell.caloDDE()->getSubCalo() != CaloCell_ID::TILE)
    pathCrossed = getPathLengthInEta(cell, entry.eta(), exit.eta());
  else if (cell.caloDDE()->getSampling() == CaloCell_ID::TileBar1) // ladder shape cells, divide the problem in 2
    pathCrossed = 0; //getPathLengthInZTileBar1(cell, entry.z(), exit.z());
  else
    pathCrossed = getPathLengthInZ(cell, entry.z(), exit.z());
  if (pathCrossed <= 0)
    return 0;
  return pathCrossed;
}


#endif
