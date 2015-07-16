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

#ifdef TRKDETDESCR_MEMUSAGE   
#include "TrkDetDescrUtils/MemoryLogger.h"
#endif  

#ifndef UCHARCONV
#define UCHARCONV
#define ucharbin 0.00392157 // 1./double(1.*UCHAR_MAX)
// int to unsigned char and vv
#define uchar2uint(uchar) static_cast<unsigned int>(uchar)
#define uint2uchar(unint) static_cast<unsigned char>(unint) 
// double to unsigned char and vv
#define uchar2dfrac(uchar) double(uchar*ucharbin)
#define dfrac2uchar(dfrac) lrint(dfrac*UCHAR_MAX)
#endif

class TTree;

namespace Trk {

    class Layer;
    class TrackingVolume;
    class TrackingGeometry;
    class SurfaceMaterialRecord;
    class LayerMaterialRecord;
    class LayerMaterialMap;
    class Material;
    class MaterialProperties;
    class BinnedLayerMaterial;
    class CompressedLayerMaterial;
    class ElementTable;
    class IExtrapolationEngine;
    class IMaterialMapper;
    class ILayerMaterialAnalyser;
    class ILayerMaterialCreator;
    class ITrackingGeometrySvc;

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
        bool associateHit(const Trk::Layer& tvol,
                          const Amg::Vector3D& pos,
                          const Amg::Vector3D& layerHitPosition,
                          double stepl,
                          const Trk::Material& mat);

        /** Output information with Level */
        void registerVolume(const Trk::TrackingVolume& tvol, int lvl);

        //!< private void Method to map the layer material
        void assignLayerMaterialProperties(const Trk::TrackingVolume& tvol, Trk::LayerMaterialMap* propSet);

        //!< create the LayerMaterialRecord */
        void insertLayerMaterialRecord(const Trk::Layer& lay);

        /** Retrieve the TrackingGeometry */
        StatusCode retrieveTrackingGeometry();

        /** Tracking Geometry */
        ServiceHandle<Trk::ITrackingGeometrySvc>             m_trackingGeometrySvc;       //!< Name of the TrackingGeometrySvc
        mutable const TrackingGeometry*                      m_trackingGeometry;          //!< The underlying TrackingGeometry

        bool                                                 m_checkForEmptyHits;         //!< use extrapoaltion engine to check for empty hits
        ToolHandle<IExtrapolationEngine>                     m_extrapolationEngine;       //!< cross-check for empty hit scaling
        
        std::string                                          m_mappingVolumeName;
        const Trk::TrackingVolume*                           m_mappingVolume;
                                                             
        /** output / input steering */                       
        std::string                                          m_inputMaterialStepCollection;
        std::string                                          m_outputLayerMaterialSetName;
                                                             
        /** general steering */                              
        double                                               m_etaCutOff;
        int                                                  m_etaSide;                       //!< needed for debugging: -1 negative | 0 all | 1 positive
        bool                                                 m_useLayerThickness;             //!< use the actual layer thickness
        int                                                  m_associationType;

        ToolHandle<ILayerMaterialAnalyser>                   m_layerMaterialRecordAnalyser;   //!< the layer material analyser for the layer material record
        ToolHandleArray<ILayerMaterialAnalyser>              m_layerMaterialAnalysers;        //!< the layer material analysers per creator (if wanted)
        ToolHandleArray<ILayerMaterialCreator>               m_layerMaterialCreators;         //!< the layer material creators
                                                             
        /** Mapper and Inspector */                          
        bool                                                 m_mapMaterial;
        ToolHandle<IMaterialMapper>                          m_materialMapper;                //!< Pointer to an IMaterialMapper algTool
        bool                                                 m_mapComposition;                //!< map the composition of the material
        double                                               m_minCompositionFraction;        //!< minimal fraction to be accounted for the composition recording
        
        
        Trk::ElementTable*                                   m_elementTable;                  //!< the accumulated element table
        std::string                                          m_inputEventElementTable;        //!< input event table
        
        // the material maps ordered with layer keys
        std::map<const Layer*,  LayerMaterialRecord >        m_layerRecords;                  //!< this is the general record for the search
        std::map<const Layer*, bool >                        m_layersRecordedPerEvent;        //!< these are the layers hit per event - for empty hit scaling
        double                                               m_accumulatedMaterialXX0;        //!< the accumulated material information 
        double                                               m_accumulatedRhoS;
        
        // statistics for steps
        size_t                                               m_mapped;
        size_t                                               m_unmapped;
        size_t                                               m_skippedOutside;
        
        int                                                  m_layerMaterialScreenOutput;
        
#ifdef TRKDETDESCR_MEMUSAGE      
        MemoryLogger                                         m_memoryLogger;                //!< in case the memory is logged        
#endif 
        

    };
}

#endif
