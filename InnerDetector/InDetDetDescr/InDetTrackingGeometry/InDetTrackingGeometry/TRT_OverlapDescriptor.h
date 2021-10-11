/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_OverlapDescriptor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETTRACKINGGEOMETRY_TRT_OVERLAPDESCRIPTOR_H
#define INDETTRACKINGGEOMETRY_TRT_OVERLAPDESCRIPTOR_H

// Trk
#include "TrkGeometry/OverlapDescriptor.h"
// ID
#include "InDetIdentifier/TRT_ID.h"

namespace Trk {
  class Surface;
}

namespace InDet {
    
    /** @class TRT_OverlapDescriptor
         
        Class to describe overlaps in the TRT detector,
        it extends the Trk::OverlapDescriptor base class.
        
        @author Andreas.Salzburger@cern.ch        
        */
        
     class TRT_OverlapDescriptor : public Trk::OverlapDescriptor {
       public:
         /** Constructor */
         TRT_OverlapDescriptor(const TRT_ID* helper, double bTolerance = 0.9, double oTolerance = 1.1) : 
          m_trtIdHelper(helper),
          m_breakTolerance(bTolerance),
          m_outsideTolerance(oTolerance)
         {}
         /** Copy Constructor */
         TRT_OverlapDescriptor(const TRT_OverlapDescriptor& od) :
          Trk::OverlapDescriptor(),
          m_trtIdHelper(od.m_trtIdHelper),
          m_breakTolerance(od.m_breakTolerance),
          m_outsideTolerance(od.m_outsideTolerance)
         {}
         
         ///Default assignment
         TRT_OverlapDescriptor & operator = (const TRT_OverlapDescriptor &) = default;
             
         /** Destructor */
         virtual ~TRT_OverlapDescriptor(){}
         
         /**Pseudo-Constructor*/
         TRT_OverlapDescriptor* clone() const;

         /** get the compatible surfaces */
         bool reachableSurfaces(std::vector<Trk::SurfaceIntersection>& cSurfaces,
                                const Trk::Surface& sf,
                                const Amg::Vector3D& pos,
                                const Amg::Vector3D& dir) const;
                                  
       private:
         // this checks if the surface intersection is within bounds, if it returns 0 break, otherwise +/- for checking in +/- of index
         int checkAndFill(std::vector<Trk::SurfaceIntersection>& cSurfaces, 
                          const Trk::Surface& sf,
                          const Amg::Vector3D& pos,
                          const Amg::Vector3D& dir) const;
           
         const TRT_ID* m_trtIdHelper;     //!< the Id helper
         double                m_breakTolerance;  //!< if the straight line intersection is within this fraction of the bounds - don't look for neighbours
         double                m_outsideTolerance; //!< accept even if it outside the bounds by this tolerance
              
     };

inline TRT_OverlapDescriptor* TRT_OverlapDescriptor::clone() const { return new TRT_OverlapDescriptor(*this); }     
     
}

#endif
