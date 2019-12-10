/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConditionsData/PixelDistortionData.h"
#include "CLHEP/Units/SystemOfUnits.h"

std::vector<float> PixelDistortionData::getDistortionMap(uint32_t hashID) const {
  std::vector<float> map(3,0.0);
  auto itr = m_distortionMap.find(hashID);
  if (itr!=m_distortionMap.end()) { 
    return itr->second; 
  }
  return map;
}

Amg::Vector2D PixelDistortionData::correction(uint32_t hashID, const Amg::Vector2D & locpos, const Amg::Vector3D & direction) const {
  double localphi = locpos[0];
  double localeta = locpos[1];
  const Amg::Vector2D nullCorrection(0.0,0.0);

  // This is needed to avoid rounding errors if the z component of the
  // direction vector is too small.
  if (std::fabs(direction.z())<1.*CLHEP::micrometer) { return nullCorrection; }

  // If a particle has a too shallow trajectory with respect to the 
  // module direction, it is likely to be a secondary particle and no 
  // shift should be applied.
  double invtanphi = direction.x()/direction.z(); 
  double invtaneta = direction.y()/direction.z();
  if (sqrt(invtanphi*invtanphi+invtaneta*invtaneta)>100.0) { return nullCorrection; }

  const double xFE = 22.0*CLHEP::millimeter; //Distance between the 2 Front-End line, where bows have been measured
  const double yFE = 60.8*CLHEP::millimeter;  //Length of the active surface of each module

  std::vector<float> map = getDistortionMap(hashID);
  double   data0 = map.at(0)/CLHEP::meter;            // curvature is in m-1
  double   data1 = map.at(1)/CLHEP::meter;            // curvature is in m-1
  double   data2 = tan(0.5*map.at(2)*CLHEP::degree);  // twist angle in degree

  double twist1 = -data2;
  double twist2 =  data2;
  double b1 = sqrt((1.+twist1*twist1)*(1.+twist1*twist1)*(1.+twist1*twist1));
  double b2 = sqrt((1.+twist2*twist2)*(1.+twist2*twist2)*(1.+twist2*twist2));
  double z1 = localeta*twist1-0.5*b1*localeta*localeta*data1;
  double z2 = localeta*twist2-0.5*b2*localeta*localeta*data0;
  double zoff1 = (b1*yFE*yFE*data1)/24.;
  double zoff2 = (b2*yFE*yFE*data0)/24.;
  z1 = z1+zoff1;
  z2 = z2+zoff2;
  double zbow = z1+((z2-z1)/xFE)*(localphi+xFE/2.);

  double localphiCor = -zbow*invtanphi;
  double localetaCor = -zbow*invtaneta;

  // In earlies code version there was a bug in the sign of the correction. 
  // In MC this was not seen as reco just corrects back what was done in digitization. 
  // In order to maintain backward compatibilty in older MC we need to reproduce this wrong behaviour.
  if (getVersion()==0) { localphiCor = -localphiCor; }
  
  return Amg::Vector2D(localphiCor, localetaCor); 
}

Amg::Vector2D PixelDistortionData::correctReconstruction(uint32_t hashID, const Amg::Vector2D & locpos, const Amg::Vector3D & direction) const {
  Amg::Vector2D newlocpos(locpos);
  newlocpos += correction(hashID, locpos, direction);
  return newlocpos;
}

Amg::Vector2D PixelDistortionData::correctSimulation(uint32_t hashID, const Amg::Vector2D & locpos, const Amg::Vector3D & direction) const {
  Amg::Vector2D newlocpos(locpos);
  newlocpos -= correction(hashID, locpos, direction);
  return newlocpos;
}

void PixelDistortionData::clear() {
  m_distortionMap.clear();
}

