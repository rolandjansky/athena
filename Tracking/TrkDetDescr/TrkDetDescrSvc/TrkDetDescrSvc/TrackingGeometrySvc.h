/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackingGeometrySvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRINTERFACES_TRACKINGGEOMETRYSERVICE_H
#define TRKDETDESCRINTERFACES_TRACKINGGEOMETRYSERVICE_H

#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"
#include "TrkDetDescrUtils/LayerIndex.h"
#include "TrkGeometry/LayerMaterialMap.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaBaseComps/AthService.h"

#ifdef TRKDETDESCR_MEMUSAGE   
#include "TrkDetDescrUtils/MemoryLogger.h"
#endif    


class StoreGateSvc;
class ISvcLocator;
template <class TYPE> class SvcFactory;

namespace Trk {
  class IGeometryBuilder;
  class IGeometryProcessor;
  class IMagneticFieldTool;
  class TrackingGeometry;
  class TrackingVolume;
  class Layer;
  class LayerMaterialProperties;


  /** @class TrackingGeometrySvc
  
      This service builds and registers the TrackingGeometry in the detector store.
      It is called by an ordered callback after GeoModel was created.
  
      @author Andreas.Salzburger@cern.ch */
     
  class TrackingGeometrySvc : public AthService, virtual public ITrackingGeometrySvc {
  
    public:
  
      //!< Retrieve interface ID
      static const InterfaceID& interfaceID() { return IID_ITrackingGeometrySvc; }
  
      StatusCode initialize();
      StatusCode finalize();
  
      /** Query the interfaces.
      /   Input: riid, Requested interface ID
      /          ppvInterface, Pointer to requested interface
      /   Return: StatusCode indicating SUCCESS or FAILURE.
      / N.B. Don't forget to release the interface after use!!! **/
      StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  
      /** Create the geometry */
      StatusCode trackingGeometryInit(IOVSVC_CALLBACK_ARGS);
  
      /** Provide the TrackingGeometry */
      const Trk::TrackingGeometry* trackingGeometry() const;
  
      //!< Returns the name of the TrackingGeometry built with this Svc
      const std::string& trackingGeometryName() const;
  
      friend class SvcFactory<TrackingGeometrySvc>;
  
      //!< Standard Constructor
      TrackingGeometrySvc(const std::string& name, ISvcLocator* svc);
  
      //!< Standard Destructor
      virtual ~TrackingGeometrySvc();
  
    
    private:
      //!< cached pointers:
      ISvcLocator*                                m_pSvcLocator;
      StoreGateSvc*                               m_pDetStore;
  
      //!< the actual building tool
      ToolHandle<Trk::IGeometryBuilder>           m_trackingGeometryBuilder;
      //!< the cached TrackingGeometry
      mutable const Trk::TrackingGeometry*        m_trackingGeometry;
      //!< the name of the TrackingGeometry
      std::string                                 m_trackingGeometryName;
      //!< processors to help 
      ToolHandleArray<Trk::IGeometryProcessor>    m_geometryProcessors;
      
  #ifdef TRKDETDESCR_MEMUSAGE
      Trk::MemoryLogger                           m_memoryLogger;
      float                                       m_changeVsize;
      float                                       m_changeRss;
  #endif
      
      //!< the name of the callback string
      bool                                        m_callbackStringForced;
      std::string                                 m_callbackString;
      bool                                        m_callbackStringCheck;
      bool                                        m_rerunOnCallback;
      //!< enables the callback
      bool                                        m_buildGeometryFromTagInfo;
  
          
  };
}

inline const Trk::TrackingGeometry* Trk::TrackingGeometrySvc::trackingGeometry() const
  { return m_trackingGeometry; }

inline const std::string& Trk::TrackingGeometrySvc::trackingGeometryName() const
  { return m_trackingGeometryName; }

#endif // TRKDETDESCRINTERFACES_TRACKINGGEOMETRYSERVICE_H

