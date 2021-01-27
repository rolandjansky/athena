/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// StaticNavigationEngine.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXENGINE_STATICNAVIGATIONENGINE_H
#define TRKEXENGINE_STATICNAVIGATIONENGINE_H
#define LEGACY_TRKGEOM

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

#include "StoreGate/ReadCondHandleKey.h"
#include "TrkGeometry/TrackingGeometry.h"
#ifdef LEGACY_TRKGEOM
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"
#endif

namespace Trk {

  class IPropagationEngine;
  class IMaterialEffectsEngine;
    
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
        using INavigationEngine::resolvePosition;

        /** resolve the boundary situation - for charged particles */
        virtual ExtrapolationCode resolveBoundary(Trk::ExCellCharged& eCell, PropDirection dir=alongMomentum) const;                                                                                         

        /** resolve the boundary situation - for neutral particles */
        virtual ExtrapolationCode resolveBoundary(Trk::ExCellNeutral& eCelll, PropDirection dir=alongMomentum) const;

        /** resolve the boundary situation - for charged particles */
        virtual ExtrapolationCode resolvePosition(Trk::ExCellCharged& eCell, PropDirection dir=alongMomentum, bool noLoop=false) const;                                                                                         

        /** resolve the boundary situation - for neutral particles */
        virtual ExtrapolationCode resolvePosition(Trk::ExCellNeutral& eCelll, PropDirection dir=alongMomentum, bool noLoop=false) const;

        /** acces to tracking geometry */
        virtual const TrackingGeometry& trackingGeometry() const;

     private:
        /** resolve the boundary situation */
        template <class T> ExtrapolationCode resolveBoundaryT(ExtrapolationCell<T>& eCell,
                                                             PropDirection dir=alongMomentum) const;
                                                             
        /** resolve position */
        template <class T> ExtrapolationCode resolvePositionT(ExtrapolationCell<T>& eCell,
							      PropDirection dir=alongMomentum,
                                                              bool noLoop=false) const;
                                                             
        /** deal with the boundary Surface - called by resolveBoundary */
        template <class T> ExtrapolationCode handleBoundaryT(ExtrapolationCell<T>& eCell,
                                                             const BoundarySurface<TrackingVolume>& bSurfaceTV, 
                                                             PropDirection dir=alongMomentum,
                                                             bool stepout=false) const;     
                                             

#ifdef LEGACY_TRKGEOM
        ServiceHandle<ITrackingGeometrySvc> m_trackingGeometrySvc {this, "TrackingGeometrySvc", "",""};
#endif
        void throwFailedToGetTrackingGeomtry() const;
        const TrackingGeometry* retrieveTrackingGeometry(const EventContext& ctx) const {
#ifdef LEGACY_TRKGEOM
           if (m_trackingGeometryReadKey.key().empty()) {
              return m_trackingGeometrySvc->trackingGeometry();
           }
#endif
           SG::ReadCondHandle<TrackingGeometry>  handle(m_trackingGeometryReadKey,ctx);
           if (!handle.isValid()) {
              EX_MSG_FATAL("", "updateGeo", "", "Could not load TrackingGeometry with name '" << m_trackingGeometryReadKey.key() << "'. Aborting." );
              throwFailedToGetTrackingGeomtry();
           }
           return handle.cptr();
        }

        SG::ReadCondHandleKey<TrackingGeometry>   m_trackingGeometryReadKey
           {this, "TrackingGeometryReadKey", "", "Key of the TrackingGeometry conditions data."};

        //!< retrieve TrackingGeometry
        StatusCode  updateTrackingGeometry() const; 
        
        ToolHandle<IPropagationEngine>                       m_propagationEngine;        //!< the used propagation engine
        ToolHandle<IMaterialEffectsEngine>                   m_materialEffectsEngine;    //!< the material effects updated

        mutable const TrackingGeometry*                      m_trackingGeometry;          //!< the tracking geometry owned by the navigator
        std::string                                          m_trackingGeometryName;      //!< Name of the TrackingGeometry as given in Detector Store
            
    };

    inline const Trk::TrackingGeometry& StaticNavigationEngine::trackingGeometry() const {
       const Trk::TrackingGeometry *tracking_geometry = retrieveTrackingGeometry(Gaudi::Hive::currentContext());
       if (!tracking_geometry){
          EX_MSG_FATAL("", "updateGeo", "", "Did not get valid TrackingGeometry. Aborting." );
          throw GaudiException("ExtrapolationEngine", "Problem with TrackingGeometry loading.", StatusCode::FAILURE);
       }
       return *tracking_geometry;
    }

} // end of namespace

//!< define the templated function    
#include "StaticNavigationEngine.icc"  

#endif // TRKEXENGINE_STATICNAVIGATIONENGINE_H

