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

#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"

#ifndef ISF_DETDESCRMODEL_DISCDETADDNEXTPHIETA
#define ISF_DETDESCRMODEL_DISCDETADDNEXTPHIETA
#define addSurfaceDPO(cur,surfaces) if (cur) surfaces.push_back(Trk::SurfaceIntersection(Trk::Intersection(Amg::Vector3D(0.,0.,0.),0.,true),&(cur->surface(cur->identify()))))
#define addOtherSideDPO(cur, surfaces) if (cur && cur->otherSide()) surfaces.push_back(Trk::SurfaceIntersection(Trk::Intersection(Amg::Vector3D(0.,0.,0.),0.,true),&(cur->otherSide()->surface(cur->otherSide()->identify()))))
#define addNextInPhiDPO(cur, surfaces) addSurfaceDPO(cur->nextInPhi(), surfaces); addOtherSideDPO(cur->nextInPhi(),surfaces)
#define addPrevInPhiDPO(cur, surfaces) addSurfaceDPO(cur->prevInPhi(), surfaces); addOtherSideDPO(cur->prevInPhi(),surfaces)
#define addNextInEtaDPO(cur, surfaces) addSurfaceDPO(cur->nextInEta(), surfaces); addOtherSideDPO(cur->nextInEta(),surfaces)
#define addPrevInEtaDPO(cur, surfaces) addSurfaceDPO(cur->prevInEta(), surfaces); addOtherSideDPO(cur->prevInEta(),surfaces)
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
			  const std::vector<Trk::BinUtility*>* singleBinUtils = 0, 
			  const bool debug = false);
         
    /** Destructor */
    virtual ~DiscOverlapDescriptor() {}
         
    /**Pseudo-Constructor*/
    virtual DiscOverlapDescriptor* clone() const override;
     
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
    void dumpSurfaces(const PixelID* pixId, const SCT_ID* sctId, std::vector<Trk::SurfaceIntersection>& surfaces) const;
    
    const Trk::BinnedArray<Trk::Surface>*           m_bin_array;
    const std::vector<Trk::BinUtility*>*            m_singleBinUtils;

    const bool                                      m_debug;
 
    
  };
  
  
  inline DiscOverlapDescriptor* DiscOverlapDescriptor::clone() const { return new DiscOverlapDescriptor(); }

  inline void DiscOverlapDescriptor::dumpSurfaces(const PixelID* pixIdHelper, const SCT_ID* sctIdHelper, std::vector<Trk::SurfaceIntersection>& surfaces) const {
    std::cout << "Dumping Surfaces for "<< (pixIdHelper ? "Pixel " : "SCT ") << "with size = " << surfaces.size() << std::endl;
    for (unsigned int surf = 0; surf < surfaces.size(); surf++) {
      Identifier hitId = ((surfaces.at(surf)).object)->associatedDetectorElementIdentifier(); 
      if (pixIdHelper)
	std::cout <<  "barrel_ec " << pixIdHelper->barrel_ec(hitId) << ", layer_disk " << pixIdHelper->layer_disk(hitId) << ", phi_module " << pixIdHelper->phi_module(hitId) << ", eta_module " << pixIdHelper->eta_module(hitId) << std::endl;
      else if (sctIdHelper)
	std::cout <<  "barrel_ec " << sctIdHelper->barrel_ec(hitId) << ", layer_disk " << sctIdHelper->layer_disk(hitId) << ", phi_module " << sctIdHelper->phi_module(hitId) << ", eta_module " << sctIdHelper->eta_module(hitId) << ", side " << sctIdHelper->side(hitId) << std::endl;
    }
  }
  
}

#endif // ISF_DETDESCRMODEL_DISCOVERLAPDESCRIPTOR_H
