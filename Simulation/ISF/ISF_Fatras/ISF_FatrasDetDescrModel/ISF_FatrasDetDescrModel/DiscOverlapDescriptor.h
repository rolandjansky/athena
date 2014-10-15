/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DiscOverlapDescriptor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_DETDESCRMODEL_DISCOVERLAPDESCRIPTOR_H
#define ISF_DETDESCRMODEL_DISCOVERLAPDESCRIPTOR_H


// Trk
#include "TrkGeometry/OverlapDescriptor.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkParameters/TrackParameters.h"
// Trk inlcude
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/BinnedArray1D1D.h"
#include "TrkDetDescrUtils/Intersection.h"

#ifndef ISF_DETDESCRMODEL_DISCDETADDNEXTPHIETA
#define ISF_DETDESCRMODEL_DISCDETADDNEXTPHIETA
#define addSurface(cur,surfaces) if (cur) surfaces.push_back(Trk::SurfaceIntersection(Trk::Intersection(Amg::Vector3D(0.,0.,0.),0.,true),&(cur->surface(cur->identify()))))
#define addOtherSide(cur, surfaces) if (cur && cur->otherSide()) surfaces.push_back(Trk::SurfaceIntersection(Trk::Intersection(Amg::Vector3D(0.,0.,0.),0.,true),&(cur->otherSide()->surface(cur->otherSide()->identify()))))
#define addNextInPhi(cur, surfaces) addSurface(cur->nextInPhi(), surfaces); addOtherSide(cur->nextInPhi(),surfaces)
#define addPrevInPhi(cur, surfaces) addSurface(cur->prevInPhi(), surfaces); addOtherSide(cur->prevInPhi(),surfaces)
#define addNextInEta(cur, surfaces) addSurface(cur->nextInEta(), surfaces); addOtherSide(cur->nextInEta(),surfaces)
#define addPrevInEta(cur, surfaces) addSurface(cur->prevInEta(), surfaces); addOtherSide(cur->prevInEta(),surfaces)
#endif // ISF_DETDESCRMODEL_DISCDETADDNEXTPHIETA

namespace Trk {
  class Surface;
}

namespace iFatras {
    
  class PlanarDetElement;
  
  /** @class DiscOverlapDescriptor
         
      Class to describe overlaps in the custom detector,
      it extends the Trk::OverlapDescriptor base class.
        
      There are two interface methods, one provides the most probably overlapcell,
      the second provides a list of overlap cells, based on an restricted area
        
      @author Noemi.Calace@cern.ch
        
  */
  class DiscOverlapDescriptor : public Trk::OverlapDescriptor {
  public:
         
    /** Constructor */
    DiscOverlapDescriptor(const Trk::BinnedArray<Trk::Surface>* bin_array = 0,
			  const std::vector<Trk::BinUtility*>* singleBinUtils = 0);
         
    /** Destructor */
    virtual ~DiscOverlapDescriptor() {}
         
    /**Pseudo-Constructor*/
    DiscOverlapDescriptor* clone() const;
     
    /** get the compatible surfaces 
        - return vector : surfaces
        - primary bin surface : sf
        - position & direction : pos, dir
    */
    bool reachableSurfaces(std::vector<Trk::SurfaceIntersection>& surfaces, 
                           const Trk::Surface& sf,
                           const Amg::Vector3D& pos,
                           const Amg::Vector3D& dir) const override;
            
  private:
    
    const Trk::BinnedArray<Trk::Surface>*           m_bin_array;
    const std::vector<Trk::BinUtility*>*            m_singleBinUtils;
 
    
  };
  
  
  inline DiscOverlapDescriptor* DiscOverlapDescriptor::clone() const { return new DiscOverlapDescriptor(); }     
  
}

#endif // ISF_DETDESCRMODEL_DISCOVERLAPDESCRIPTOR_H
