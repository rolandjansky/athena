/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeometryBuilder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRTOOLS_GEOMETRYBUILDER_H
#define TRKDETDESCRTOOLS_GEOMETRYBUILDER_H

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkDetDescrInterfaces/IGeometryBuilder.h"
#include "TrkDetDescrUtils/GeometrySignature.h"
#include "TrkGeometry/TrackingVolumeManipulator.h"
#include "TrkGeometry/Material.h"
// Gaudi & Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#ifdef TRKDETDESCR_MEMUSAGE   
#include "TrkDetDescrUtils/MemoryLogger.h"
#endif  

namespace Trk {

    class TrackingGeometry;
    class TrackingVolume;
    class ITrackingVolumeBuilder;
    class ITrackingVolumeHelper;
    class ITrackingVolumeArrayCreator;

    /** @class GeometryBuilder

      The Trk::TrackingGeometry Builder for ATLAS Geometry

      It retrieves Trk::TrackingGeometry builders for the subdetectors and joins them together 
      to a single Trk::TrackingGeometry.

      @author Andreas.Salzburger@cern.ch   
     */

    class GeometryBuilder : public AthAlgTool,
                            public TrackingVolumeManipulator,
                    virtual public IGeometryBuilder {

      public:
        /** Constructor */
        GeometryBuilder(const std::string&,const std::string&,const IInterface*);
        
        /** Destructor */
        virtual ~GeometryBuilder();

        /** AlgTool initialize method */
        StatusCode initialize();
        
        /** AlgTool finalize method */
        StatusCode finalize();
        
        /** TrackingGeometry Interface method - optionally a pointer to Bounds */
        const TrackingGeometry* trackingGeometry(const TrackingVolume* tvol = 0) const;

        /** The unique signature */
        GeometrySignature geometrySignature() const { return Trk::Global; }

      private:

        /** TrackingGeometry for ATLAS setup */
        const TrackingGeometry* atlasTrackingGeometry() const;

#ifdef TRKDETDESCR_MEMUSAGE         
        MemoryLogger                        m_memoryLogger;                //!< in case the memory is logged
#endif      

        bool                                m_createWorld;                 //!< Boolean Switch to create World manually
        int                                 m_navigationLevel;             //!< NavigationLevel

        std::vector< double >               m_worldDimension;              //!< The dimensions of the manually created world
        std::vector< double >               m_worldMaterialProperties;     //!< The material properties of the created world
        Material                            m_worldMaterial;               //!< the world material

        // -------------------------- Tools for geometry building ------------------------------------------------------ //

        ToolHandle<ITrackingVolumeArrayCreator>   m_trackingVolumeArrayCreator;       //!< Helper Tool to create TrackingVolume Arrays

        ToolHandle<ITrackingVolumeHelper>         m_trackingVolumeHelper;             //!< Helper Tool to create TrackingVolumes

        //bool                                      m_inDetGeometry;                     //!< switch on TrackingGeometry for the InnerDetector
        ToolHandle<IGeometryBuilder>              m_inDetGeometryBuilder;              //!< GeometryBuilder for the InnerDetector
        
        bool                                      m_hgtdGeometry;                     //! switch on TrackingGeometry for HGTD 
        ToolHandle<IGeometryBuilder>              m_hgtdGeometryBuilder;              //!< GeometryBuilder for the HGTD

        bool                                      m_caloGeometry;                     //!< switch on TrackingGeometry for the Calorimeters
        ToolHandle<IGeometryBuilder>              m_caloGeometryBuilder;              //!< GeometryBuilder for the Calorimeters

        bool                                      m_muonGeometry;                     //!< GeometryBuilder for the Muon System
        ToolHandle<IGeometryBuilder>              m_muonGeometryBuilder;              //!< GeometryBuilder for the Muon System
        
        bool                                      m_compactify;                       //!< optimize event memory usage: register all surfaces with TG
        bool                                      m_synchronizeLayers;                //!< synchronize contained layer dimensions to volumes

    };

} // end of namespace

#endif // TRKDETDESCRTOOLS_GEOMETRYBUILDER_H

