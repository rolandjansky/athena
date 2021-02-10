/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelOverlapDescriptor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETTRACKINGGEOMETRYUTILS_PIXELOVERLAPDESCRIPTOR_H
#define INDETTRACKINGGEOMETRYUTILS_PIXELOVERLAPDESCRIPTOR_H


// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkGeometry/OverlapDescriptor.h"
#include "TrkDetDescrUtils/Intersection.h"

#ifndef TRKDETDESCR_SIDETADDNEXTPHIETA
#define TRKDETDESCR_SIDETADDNEXTPHIETA
#define addSurface(cur,surfaces) if (cur) surfaces.push_back(Trk::SurfaceIntersection(Trk::Intersection(Amg::Vector3D(0.,0.,0.),0.,true),&(cur->surface(cur->identify()))))
#define addOtherSide(cur, surfaces) if (cur && cur->otherSide()) surfaces.push_back(Trk::SurfaceIntersection(Trk::Intersection(Amg::Vector3D(0.,0.,0.),0.,true),&(cur->otherSide()->surface(cur->otherSide()->identify()))))
#define addNextInPhi(cur, surfaces) addSurface(cur->nextInPhi(), surfaces); addOtherSide(cur->nextInPhi(),surfaces)
#define addPrevInPhi(cur, surfaces) addSurface(cur->prevInPhi(), surfaces); addOtherSide(cur->prevInPhi(),surfaces)
#define addNextInEta(cur, surfaces) addSurface(cur->nextInEta(), surfaces); addOtherSide(cur->nextInEta(),surfaces)
#define addPrevInEta(cur, surfaces) addSurface(cur->prevInEta(), surfaces); addOtherSide(cur->prevInEta(),surfaces)
#endif


namespace Trk {
  class Surface;
}

namespace InDetDD {
    class SiDetectorElement;
}

namespace InDet {
    
    /** @class PixelOverlapDescriptor
         
        Class to describe overlaps in the pixel detector,
        it extends the Trk::OverlapDescriptor base class.
        
        There are two interface methods, one provides the most probably overlapcell,
        the second provides a list of overlap cells, based on an restricted area
        
        @author Andreas.Salzburger@cern.ch
        
        */
     class PixelOverlapDescriptor : public Trk::OverlapDescriptor {
       public:
         
         /** Constructor (area restriction, LC check) */
         PixelOverlapDescriptor(bool addMoreSurfaces = true, int eta_slices = 3, int phi_slices = 1, bool addStaves = false);

         /** Destructor */
         virtual ~PixelOverlapDescriptor(){}
         
         /**Pseudo-Constructor*/
         PixelOverlapDescriptor* clone() const;

         /** get the compatible surfaces */
         bool reachableSurfaces(std::vector<Trk::SurfaceIntersection>& cSurfaces, 
                                const Trk::Surface& sf,
                                const Amg::Vector3D& pos,
                                const Amg::Vector3D& dir) const override;
                                
        bool reachableSurfaces(std::vector<Trk::SurfaceIntersection>& cSurfaces, 
                               const Amg::Vector3D& pos,
                               const Amg::Vector3D& dir) const override;
                                  

      private :                                  
         void addPhiNeighbours(std::vector<Trk::SurfaceIntersection>& cSurfaces, 
                               InDetDD::SiDetectorElement& sElement) const;
			       
	 void dumpSurfaces(std::vector<Trk::SurfaceIntersection>& surfaces) const;

	 bool m_robustMode;
	 bool m_addMoreSurfaces;
	 int  m_etaSlices;
	 int  m_phiSlices;
	 bool m_addEntireStave;
     };

     
  inline PixelOverlapDescriptor* PixelOverlapDescriptor::clone() const { return new PixelOverlapDescriptor(); }     

  inline bool PixelOverlapDescriptor::reachableSurfaces(std::vector<Trk::SurfaceIntersection>& /*cSurfaces*/, 
                                                        const Amg::Vector3D& /*pos*/,
                                                        const Amg::Vector3D& /*dir*/) const {
    return false; 
  }

}

#endif // end of INDETTRACKINGGEOMETRYUTILS_PIXELOVERLAPDESCRIPTOR_H
