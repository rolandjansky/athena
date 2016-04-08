/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// StaticNavigationEngine.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXENGINE_STATICNAVIGATIONENGINE_H
#define TRKEXENGINE_STATICNAVIGATIONENGINE_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
// Trk
#include "TrkExInterfaces/INavigationEngine.h"
#include "TrkExInterfaces/ExtrapolationMacros.h"
#include "TrkExUtils/ExtrapolationCell.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkVolumes/BoundarySurface.h"
// throw GaudiExceptions where necessary
#include "GaudiKernel/GaudiException.h"


namespace Trk {

  class IPropagationEngine;
  class IMaterialEffectsEngine;
  class TrackingGeometry;
    
  /** @class StaticNavigationEntine
    
      The static navigation engine for finding the next volume,
      propagate to the boundary, can be shared with other engines that have a static frame.
    
      @author Andreas.Salzburger -at- cern.ch
    */    
  class StaticNavigationEngine : public AthAlgTool, virtual public INavigationEngine {

      public:
        /** Constructor */
        StaticNavigationEngine(const std::string&,const std::string&,const IInterface*);
        
        /** Destructor */
        ~StaticNavigationEngine();

        /** AlgTool initialize method */
        StatusCode initialize();
        
        /** AlgTool finalize method */
        StatusCode finalize();

        /** avoid method shaddowing */
        using INavigationEngine::resolveBoundary;

        /** resolve the boundary situation - for charged particles */
        virtual ExtrapolationCode resolveBoundary(Trk::ExCellCharged& eCell, PropDirection dir=alongMomentum) const;                                                                                         

        /** resolve the boundary situation - for neutral particles */
        virtual ExtrapolationCode resolveBoundary(Trk::ExCellNeutral& eCelll, PropDirection dir=alongMomentum) const;
        
     private:
        /** resolve the boundary situation */
        template <class T> ExtrapolationCode resolveBoundaryT(ExtrapolationCell<T>& eCell,
                                                             PropDirection dir=alongMomentum) const;
                                                             
        /** deal with the boundary Surface - called by resolveBoundary */
        template <class T> ExtrapolationCode handleBoundaryT(ExtrapolationCell<T>& eCell,
                                                             const BoundarySurface<TrackingVolume>& bSurfaceTV, 
                                                             PropDirection dir=alongMomentum,
                                                             bool stepout=false) const;     
                                             

        //!< retrieve TrackingGeometry
        StatusCode  updateTrackingGeometry() const; 
        
        //!< return and retrieve
        const TrackingGeometry& trackingGeometry() const throw (GaudiException);

        ToolHandle<IPropagationEngine>                       m_propagationEngine;        //!< the used propagation engine
        ToolHandle<IMaterialEffectsEngine>                   m_materialEffectsEngine;    //!< the material effects updated

        mutable const TrackingGeometry*                      m_trackingGeometry;          //!< the tracking geometry owned by the navigator
        std::string                                          m_trackingGeometryName;      //!< Name of the TrackingGeometry as given in Detector Store
            
    };

inline const Trk::TrackingGeometry& StaticNavigationEngine::trackingGeometry() const throw (GaudiException) {
    if (!m_trackingGeometry && updateTrackingGeometry().isFailure()){
        EX_MSG_FATAL("", "updateGeo", "", "Could not load TrackingGeometry with name '" << m_trackingGeometryName << "'. Aborting." );
        throw GaudiException("StaticNavigationEngine", "Problem with TrackingGeometry loading.", StatusCode::FAILURE);
    }
    return (*m_trackingGeometry);
}

} // end of namespace

//!< define the templated function    
#include "StaticNavigationEngine.icc"  

#endif // TRKEXENGINE_STATICNAVIGATIONENGINE_H

