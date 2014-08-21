/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelOverlapDescriptor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETTRACKINGGEOMETRY_PIXELOVERLAPDESCRIPTOR_H
#define INDETTRACKINGGEOMETRY_PIXELOVERLAPDESCRIPTOR_H


// Trk
#include "TrkGeometry/OverlapDescriptor.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk {
  class Surface;
}

namespace InDetDD {   
     class SiDetectorElement;
     class SiDetectorManager;
}

class PixelID;

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
         PixelOverlapDescriptor(double loc1area=0.1, 
                                double loc2area=0.1, 
                                bool checkLC = true, 
                                const Trk::BinnedArray<Trk::Surface>* ba = 0, 
                                const PixelID* pixIdHelper = 0,
                                const InDetDD::SiDetectorManager* pixMgr = 0, 
                                int robustOverlaps=1);
         
         /** Destructor */
         virtual ~PixelOverlapDescriptor(){}
         
         /**Pseudo-Constructor*/
         PixelOverlapDescriptor* clone() const;
     
         /**Single interface method*/
        const Trk::OverlapCell overlapSurface(const Trk::TrackParameters& tp,
                                              const Trk::Surface* sfreq=0,
                                              Trk::PropDirection dir = Trk::alongMomentum) const;
                                                 
        /** Interface method for RISC logic */
        const std::vector<Trk::OverlapCellCondition>& overlapCells(const Trk::TrackParameters& tp,
                                                                   Trk::PropDirection dir = Trk::anyDirection,
                                                                   const Trk::Surface* startSf = 0,
                                                                   const Trk::Surface* endSf = 0) const;
        
      private:

        mutable std::vector<Trk::OverlapCellCondition> m_overlapCells;  //!< return object to be cleared                                          
        bool                                           m_checkLocalCoords; //!< check on local coordinates
        double                                         m_loc1fraction;  //!< define outside fraction in 1st local coordinate
        double                                         m_loc2fraction;  //!< define outside fraction in 2nd local coordinate
   
        const Trk::BinnedArray<Trk::Surface>*          m_binnedSurfaceArray; //!< never delete this
        const InDetDD::SiDetectorManager*              m_pixMgr;
        const PixelID*                                 m_pixIdHelper;  

        int                                            m_robustOverlapMode; //!< check for overlap modules in a more robust way
                                                      
     };

     
inline PixelOverlapDescriptor* PixelOverlapDescriptor::clone() const { return new PixelOverlapDescriptor(); }     

}

#endif
