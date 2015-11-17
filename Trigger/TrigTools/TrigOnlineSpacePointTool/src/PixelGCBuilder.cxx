/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigOnlineSpacePointTool/PixelGCBuilder.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "TrigInDetEvent/TrigSiSpacePoint.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"

#include <cmath>
//#define GCDEBUG 

PixelGCBuilder::PixelGCBuilder(const InDetDD::SiDetectorManager* &manager, const PixelID* pixelId, 
			       int offsetEndcapPixels) {
  m_manager=manager;
  m_pixelID = pixelId;
  m_OffsetEndcapPixels = offsetEndcapPixels;
}

PixelGCBuilder::~PixelGCBuilder() {
}

void PixelGCBuilder::formSpacePoints (const InDet::PixelClusterCollection& clusterColl, 
				      std::vector<TrigSiSpacePoint*>& space_points) {
  double locT, locL, errLocT, errLocL, x, dx, y, dy;
  double r, dr, phi, dphi, z, dz;


  space_points.reserve(clusterColl.size());

  // Get element id    
  const Identifier& waferId = clusterColl.identify();
  const IdentifierHash& waferIdHash = clusterColl.identifyHash();
  const InDetDD::SiDetectorElement* element=m_manager->getDetectorElement(waferIdHash);

  double pitchPhi = element->phiPitch();
  double pitchRz  = element->etaPitch();
  errLocT= pitchPhi*sqrt(1./12.); 
  errLocL= pitchRz*sqrt(1./12.);
  
  const Amg::Vector3D posn = element->center();  
  const Amg::Vector3D& dir1 =  element->phiAxis();
  const Amg::Vector3D& dir2 =  element->etaAxis();
  dx = fabs(errLocT * dir1.x() + errLocL * dir2.x());
  dy = fabs(errLocT * dir1.y() + errLocL * dir2.y());

  long layerId = m_pixelID->layer_disk(waferId);

  if (m_pixelID->barrel_ec(waferId) !=0) { // endcap
    dz = fabs(errLocT * dir1.z() + errLocL * dir2.z() );   
    layerId += m_OffsetEndcapPixels;
  } else {
    dz = fabs(errLocT * dir1.z() + errLocL * dir2.z());       
  }
  
  // Loop over all clusters in the element
  InDet::PixelClusterCollection::const_iterator iter = clusterColl.begin();
   
  for (; iter != clusterColl.end(); iter++) {

    const InDet::SiCluster* cluster =  *iter;
    const Amg::Vector2D pos = cluster->localPosition();
    locT = pos[0];
    locL = pos[1];

    // Transform to global co-ordinates
    x = posn.x() + locT * dir1.x() + locL * dir2.x();
    y = posn.y() + locT * dir1.y() + locL * dir2.y();
    r = sqrt(x*x + y*y);
    dr = sqrt(std::pow(static_cast<double>(dx*x/r),2) 
		       + std::pow(static_cast<double>(dy*y/r),2));
    phi = atan2(y,x);
    dphi = errLocT/r;

    z = posn.z() + locT * dir1.z() + locL * dir2.z();
  
    // Create the SiSpacePoint object
    TrigSiSpacePoint* sp = new TrigSiSpacePoint(cluster, waferId, layerId, 
                                                r, phi, z, dr, dphi, dz);
    sp->locL(locL);
    sp->locT(locT);

#ifdef GCDEBUG
    std::cout.precision(8);
    std::cout << " posn (X,Y,Z) " << posn.x() 
	      << " ,  " << posn.y() << " ,  " 
	      << posn.z() << std::endl; 
    std::cout << " dir1 (X,Y,Z) " << dir1.x() << " ,  " << dir1.y() << " ,  " << dir1.z() << std::endl; 
    std::cout << " dir2 (X,Y,Z) " << dir2.x() << " ,  " << dir2.y() << " ,  " << dir2.z() << std::endl; 
    std::cout << " locT = " << locT  
	      << " locL = " << locL  
	      << " r = " << r
	      << " dr = " << dr
	      << " phi = " << phi   
	      << " dphi = " << dphi 
	      << " z = " << z 
	      << " dz = " << dz
	      << std::endl;
#endif 
    
    // Put it into the vector
    space_points.push_back(sp);       
  }
}

