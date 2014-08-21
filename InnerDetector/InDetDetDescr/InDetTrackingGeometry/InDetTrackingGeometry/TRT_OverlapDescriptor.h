/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_OverlapDescriptor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETTRACKINGGEOMETRY_TRT_OVERLAPDESCRIPTOR_H
#define INDETTRACKINGGEOMETRY_TRT_OVERLAPDESCRIPTOR_H

// Trk
#include "TrkGeometry/OverlapDescriptor.h"
#include "TrkParameters/TrackParameters.h"
// ID
#include "InDetIdentifier/TRT_ID.h"

namespace Trk {
  class Surface;
  //class TrackParameters;
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
         TRT_OverlapDescriptor(const TRT_ID* helper) : 
          m_trtIdHelper(helper)
         {}
         /** Copy Constructor */
         TRT_OverlapDescriptor(const TRT_OverlapDescriptor& od) :
          Trk::OverlapDescriptor(),
          m_trtIdHelper(od.m_trtIdHelper)
         {}
         /** Destructor */
         virtual ~TRT_OverlapDescriptor(){}
         /**Pseudo-Constructor*/
         TRT_OverlapDescriptor* clone() const;
     
         /**Single interface method*/
         const Trk::OverlapCell overlapSurface(const Trk::TrackParameters& tp,
                                               const Trk::Surface* sfreq = 0,
                                               Trk::PropDirection dir = Trk::alongMomentum) const;

         /** Interface method for RISC logic */
         const std::vector<Trk::OverlapCellCondition>& overlapCells(const Trk::TrackParameters& tp,
                                                                    Trk::PropDirection dir = Trk::anyDirection,
                                                                    const Trk::Surface* startSf = 0,
                                                                    const Trk::Surface* endSf = 0) const;
                                                                                                    
       private:
         mutable const TRT_ID* m_trtIdHelper;     //!< the Id helper
         static  int           s_overlapCounter;  //!< static overlap counter
         mutable std::vector<Trk::OverlapCellCondition> m_overlapCells;  //!< return object to be cleared                                             
              
     };

inline TRT_OverlapDescriptor* TRT_OverlapDescriptor::clone() const { return new TRT_OverlapDescriptor(*this); }     
     
}

#endif
