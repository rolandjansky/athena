/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_OverlapDescriptor.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// InDet
#include "InDetTrackingGeometry/TRT_OverlapDescriptor.h"
#include "InDetReadoutGeometry/TRT_BaseElement.h"
// Trk
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/SurfaceBounds.h"


/** get the compatible surfaces */
bool InDet::TRT_OverlapDescriptor::reachableSurfaces(std::vector<Trk::SurfaceIntersection>& cSurfaces, 
                                                       const Trk::Surface& tsf,
                                                       const Amg::Vector3D& position,
                                                       const Amg::Vector3D& momentum) const
                                                            
{
   const Amg::Vector3D dir = momentum.unit();        
   // first add the target surface - if it fits
   int nextInt = checkAndFill(cSurfaces, tsf, position, dir);
   if (!nextInt) return true;
            
   const InDetDD::TRT_BaseElement* trtBaseElement = dynamic_cast<const InDetDD::TRT_BaseElement*>(tsf.associatedDetectorElement());
   // we have a trt base element 
   
   if (trtBaseElement){
     
     Identifier strawID = tsf.associatedDetectorElementIdentifier();
     
     // straw Max 
     int strawMax = trtBaseElement->nStraws()-1;
     int strawInt = m_trtIdHelper->straw(strawID);
     
     if (strawInt != 0 && strawInt < strawMax && strawMax != -999){
        // get the next straw
        Identifier          nextID       =   m_trtIdHelper->straw_id(trtBaseElement->identify(), strawInt+nextInt);
        const Trk::Surface* nextStraw    = (strawInt > 1  && strawInt < strawMax) ? (&(trtBaseElement->surface(nextID))) : 0;
        if (nextStraw)
            nextInt = checkAndFill(cSurfaces, *nextStraw, position, dir);
    } 
  }
  
  return true;
}  

int InDet::TRT_OverlapDescriptor::checkAndFill(std::vector<Trk::SurfaceIntersection>& cSurfaces, 
                                               const Trk::Surface& sf,
                                               const Amg::Vector3D& pos,
                                               const Amg::Vector3D& dir) const
{
    Trk::Intersection sfIntersection = sf.straightLineIntersection(pos,dir,false,false);
    // we have a valid intersection - is it within tolerance ?
    if (sfIntersection.valid){
        // this is the boundary cylinder of the straw surface
        double rSurface2 = sf.bounds().r()*sf.bounds().r();
        // what's the distance - hess' normal form
        const Amg::Vector3D& sCenter    =  sf.center();
        const Amg::Vector3D  sDirection =  sf.transform().rotation().col(2);
        // d^2 = | (dir) x 
        double d2 = (sDirection.cross((sfIntersection.position-sCenter))).squaredNorm();
        // check it 
        bool acceptSurface = d2 < (m_outsideTolerance*m_outsideTolerance*rSurface2);
        if (acceptSurface) cSurfaces.push_back( Trk::SurfaceIntersection(sfIntersection, &sf) );
        // give the break signal if it's really inside
        return ( d2 < (m_breakTolerance*m_breakTolerance*rSurface2) ? 0 : (sfIntersection.position.phi() > sf.center().phi() ? 1 : -1) );
    }
    // base the decision on the position phi value
    return (pos.phi() > sf.center().phi() ? 1 : -1);                                            
}
