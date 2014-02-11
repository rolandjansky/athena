/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// MaterialMapping.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRALGS_MATERIALMAPPING_H
#define TRKDETDESCRALGS_MATERIALMAPPING_H

// Athena & Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TString.h"
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include "GeoPrimitives/GeoPrimitives.h"


class TTree;

namespace Trk {

    class Layer;
    class TrackingVolume;
    class TrackingGeometry;
    class SurfaceMaterialRecord;
    class LayerMaterialRecord;
    class LayerMaterialMap;
    class MaterialProperties;
    class BinnedLayerMaterial;
    class CompressedLayerMaterial;
    class IMaterialMapper;    
    class ITrackingGeometrySvc;
    
    struct IndexedMaterial {
        unsigned short int firstBin;
        unsigned short int secondBin;
        const Trk::MaterialProperties* materialProperties;
    };


    /** @class MaterialMapping

       A simple algorithm that throws random points through detector and associates them with
       the given/found layer.

       @author Andreas.Salzburger@cern.ch

     */

    class MaterialMapping : public AthAlgorithm {

      public:

    	/** Standard Athena-Algorithm Constructor */
        MaterialMapping(const std::string& name, ISvcLocator* pSvcLocator);
        /** Default Destructor */
        ~MaterialMapping();

        /** standard Athena-Algorithm method */
        StatusCode          initialize();
        
        /** standard Athena-Algorithm method */
        StatusCode          execute();
        
        /** standard Athena-Algorithm method */
        StatusCode          finalize();

      private:

        /** Associate the Step to the Layer */
        bool associateHit(const Trk::TrackingVolume& tvol,
                          const Amg::Vector3D& pos,
                          double stepl,
                          double x0,
                          double l0,
                          double a,
                          double z,
                          double rho);

        /** Output information with Level */
        void registerVolume(const Trk::TrackingVolume& tvol, int lvl);

        //!< private void Method to map the layer material
        void assignLayerMaterialProperties(const Trk::TrackingVolume& tvol, Trk::LayerMaterialMap* propSet);

        //!< private method to compress material
        Trk::CompressedLayerMaterial* compressMaterial(const Trk::BinnedLayerMaterial& binnedLayerMaterial);

        /** Retrieve the TrackingGeometry */
        StatusCode retrieveTrackingGeometry();

        /** Tracking Geometry */
        ServiceHandle<Trk::ITrackingGeometrySvc>             m_trackingGeometrySvc;       //!< Name of the TrackingGeometrySvc
        mutable const TrackingGeometry*                      m_trackingGeometry;          //!< The underlying TrackingGeometry
        
        std::string                                          m_mappingVolumeName;
        const Trk::TrackingVolume*                           m_mappingVolume;
                                                             
        /** output / input steering */                       
        std::string                                          m_inputMaterialStepCollection;
        std::string                                          m_outputLayerMaterialSetName;
                                                             
        /** general steering */                              
        double                                               m_etaCutOff;
        int                                                  m_associationType;
                                                             
        /** Mapper and Inspector */                          
        bool                                                 m_mapMaterial;
        ToolHandle<IMaterialMapper>                          m_materialMapper;            //!< Pointer to an IMaterialMapper algTool
        int                                                  m_materialMappingEvents;     //!< count the number of validation records to avoid 2G files
        int                                                  m_maxMaterialMappingEvents;  //!< limit the number of validation records to avoid 2G files

        // the material maps ordered with layer keys
        std::map<const Layer*,  LayerMaterialRecord >        m_layerRecords;

        // compression                      
        bool                                                 m_compressMaterial;
        double                                               m_compressedMaterialThickness;
        size_t                                               m_compressedMaterialX0Bins;
        size_t                                               m_compressedMaterialZARhoBins;
        
        // statistics
        size_t                                               m_mapped;
        size_t                                               m_unmapped;
        size_t                                               m_skippedOutside;
        
        // break for validation
        size_t                                               m_mappedEvents;
        size_t                                               m_maxMappedEvents;
        
        int                                                  m_layerMaterialScreenOutput;
        

    };
}

#endif
