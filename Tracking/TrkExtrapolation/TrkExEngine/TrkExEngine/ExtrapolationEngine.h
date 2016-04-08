/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ExtrapolationEngine.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXINTERFACES_EXTRAPOLATIONENGINE_H
#define TRKEXINTERFACES_EXTRAPOLATIONENGINE_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
// Trk
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"
#include "TrkExInterfaces/IExtrapolationEngine.h"
#include "TrkExInterfaces/ExtrapolationMacros.h"
#include "TrkExUtils/ExtrapolationCell.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkNeutralParameters/NeutralParameters.h"
// throw GaudiExceptions where necessary
#include "GaudiKernel/GaudiException.h"

namespace Trk {
  
  class TrackingGeometry;
  class IPropagationEngine;    

  /** @class ExtrapolationEngine 
      
      Master extrapolation engine for extrapolation through the TrackingGeometry,
      it delegates the extrapolation to optimised engines, handing over the ExtrapolationCell
      as internal cache.
  
      There are identical interfaces for charged and neutral track parameters.
      Providing a destination surface is optional, if no destination surface is given the extrapolation 
      process can be stopped by other directives, e.g. stopping at a certain path limit, material limit
      or with a change of detector signature.
  
      @author Andreas.Salzburger -at- cern.ch 
  */
  class ExtrapolationEngine : public AthAlgTool, virtual public IExtrapolationEngine {
      
      friend class NavigationInitTest;
      
      public:
        /** Constructor */
        ExtrapolationEngine(const std::string&,const std::string&,const IInterface*);
        
        /** Destructor */
        ~ExtrapolationEngine();

        /** AlgTool initialize method */
        StatusCode initialize();
        
        /** AlgTool finalize method */
        StatusCode finalize();
        
        using IExtrapolationEngine::extrapolate;
        
        /** charged extrapolation - public interface */
        virtual ExtrapolationCode extrapolate(ExCellCharged& ecCharged,
                                              const Surface* sf = 0,
                                              BoundaryCheck bcheck = true) const;

        /** neutral extrapolation - public interface */
        virtual ExtrapolationCode extrapolate(ExCellNeutral& ecNeutral,
                                              const Surface* sf = 0,
                                              BoundaryCheck bcheck = true) const;
                         
                         
        /** define for which GeometrySignature this extrapolator is valid - this is GLOBAL */
        GeometryType geometryType() const;                           
                         
     private:
        /** main loop extrapolation method */
        template <class T> ExtrapolationCode extrapolateT(ExtrapolationCell<T>& eCell,
                                                          const Surface* sf = 0,
                                                          PropDirection dir=alongMomentum,
                                                          BoundaryCheck bcheck = true) const;
            
        /** initialization method */                                      
        template <class T>  ExtrapolationCode initNavigation(ExtrapolationCell<T>& eCell,
                                                             const Surface* sf = 0,
                                                             PropDirection dir=alongMomentum) const throw (GaudiException);
                
                
        //!< retrieve TrackingGeometry
        StatusCode  updateTrackingGeometry() const; 

        //!< return and retrieve
        const TrackingGeometry& trackingGeometry() const throw (GaudiException);

        mutable const TrackingGeometry*                     m_trackingGeometry;          //!< the tracking geometry owned by the navigator
        ServiceHandle<ITrackingGeometrySvc>                 m_trackingGeometrySvc;       //!< ToolHandle to the TrackingGeometrySvc
        std::string                                         m_trackingGeometryName;      //!< Name of the TrackingGeometry as given in Detector Store
        
        //!< the tool handle array for static / dense / detached
        ToolHandleArray<IExtrapolationEngine>               m_extrapolationEngines;      //!< the extrapolation engines for retrieval
        ToolHandle<IPropagationEngine>                      m_propagationEngine;         //!< the used propagation engine for navigation initialization
        std::vector<const IExtrapolationEngine*>            m_eeAccessor;                //!< the extrapolation engines for 

        //!< forces a global search for the initialization, allows to switch TrackingGeometries in one job
        bool                                                m_forceSearchInit; 
    
    };

  inline GeometryType  ExtrapolationEngine::geometryType() const 
      { return Trk::Master; }


  inline const Trk::TrackingGeometry& ExtrapolationEngine::trackingGeometry() const throw (GaudiException) {
      if (!m_trackingGeometry && updateTrackingGeometry().isFailure()){
          EX_MSG_FATAL("", "updateGeo", "", "Could not load TrackingGeometry with name '" << m_trackingGeometryName << "'. Aborting." );
          throw GaudiException("ExtrapolationEngine", "Problem with TrackingGeometry loading.", StatusCode::FAILURE);
      }
      return (*m_trackingGeometry);
  }
   
   

} // end of namespace

//!< define the templated function    
#include "ExtrapolationEngine.icc"  

#endif // TRKEXINTERFACES_IEXTRAPOLATIONENGINE_H

