/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_OverlapDescriptor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETTRACKINGGEOMETRY_SCT_OVERLAPDESCRIPTOR_H
#define INDETTRACKINGGEOMETRY_SCT_OVERLAPDESCRIPTOR_H

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkGeometry/OverlapDescriptor.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk {
  class Surface;
}


namespace InDetDD {   
     class SiDetectorElement;
     class SiDetectorManager;
}

class SCT_ID;
          
namespace InDet {

    /** @class SCT_OverlapDescriptor
         
        Class to describe overlaps in the SCT detector,
        it extends the Trk::OverlapDescriptor base class.
        
        If a binned Array is given, it does not take ownership
        
        @author Andreas.Salzburger@cern.ch        
        */
        
     class SCT_OverlapDescriptor : public Trk::OverlapDescriptor {
       public:

         /** Constructor */
         SCT_OverlapDescriptor(double loc1range=0.1,
                               double loc2range=0.1,
                               bool checkLC=true,
                               const Trk::BinnedArray<Trk::Surface>* ba = 0,
                               const SCT_ID* sctIdHelper = 0,
                               const InDetDD::SiDetectorManager* sctMgr = 0,
			                   int m_robustOverlapMode=1);
         
         /** Destructor */
         virtual ~SCT_OverlapDescriptor(){}
         /**Pseudo-Constructor*/
         SCT_OverlapDescriptor* clone() const;
     
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
        // the surface check for backside surfaces
        bool checkBackside(const Trk::Surface& sf,
                           const Amg::Vector3D& direction,
                           Trk::PropDirection dir,
                           const Trk::Surface* startSurface = 0,
                           const Trk::Surface* endSurface = 0) const; 
       
        // the surface check for overlap surfaces --- called by checkAndRegisterWithBackside
        void registerWithBackside(const InDetDD::SiDetectorElement& siDetElement,
                                          Trk::PropDirection dir,
                                  		  const Trk::Surface* startSf = 0,
                                          const Trk::Surface* endSf= 0) const;
                            
        // the surfac check for overlap surfaces --- called by checkSurface
        bool checkAndRegisterWithBackside(const InDetDD::SiDetectorElement& siDetElement,
                           				  const Amg::Vector3D& position,
                           				  const Amg::Vector3D& direction,
                                          const Trk::Surface& checkSurface,
                                          double fraction,
                                          double halflength,
                                          double flipDirection,
                                          Trk::PropDirection dir,
                                  		  const Trk::Surface* startSf = 0,
                                          const Trk::Surface* endSf= 0) const;

      
        mutable std::vector<Trk::OverlapCellCondition> m_overlapCells;  //!< return object to be cleared                                          
        bool                                           m_checkLocalCoords; //!< make a check on the local coordinates
        double                                         m_loc1fraction;  //!< define outside fraction in 1st local coordinate
        double                                         m_loc2fraction;  //!< define outside fraction in 2nd local coordinate


        // the binned array, only the pointer is kept (not a deep copy)
        const Trk::BinnedArray<Trk::Surface>*          m_binnedSurfaceArray; //!< never delete this
        const InDetDD::SiDetectorManager*              m_sctMgr;
        const SCT_ID*                                  m_sctIdHelper;                                           
 	    int                                            m_robustOverlapMode; //!< check for overlap modules in a more robust way                                                      
    };

  inline SCT_OverlapDescriptor* SCT_OverlapDescriptor::clone() const { return new SCT_OverlapDescriptor(); }     
          
}

#endif
