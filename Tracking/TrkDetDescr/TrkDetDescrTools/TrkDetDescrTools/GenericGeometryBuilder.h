/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenericGeometryBuilder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRTOOLS_GENERICGEOMETRYBUILDER_H
#define TRKDETDESCRTOOLS_GENERICGEOMETRYBUILDER_H

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkDetDescrInterfaces/IGeometryBuilder.h"
#include "TrkDetDescrUtils/GeometrySignature.h"

// Gaudi & Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "CxxUtils/checker_macros.h"
class IEnvelopeDefSvc;

namespace Trk {

    class TrackingGeometry;
    class TrackingVolume;
    class ITrackingVolumeCreator;   
    class ITrackingVolumeHelper;   

    /** @class GenericGeometryBuilder

      A TrackingGeometry builder that takes the EnvelopSvc and creates 
      a very simple generic Trackigngeoemtry configured with Layers
      
      @author Andreas.Salzburger@cern.ch   
     */
    class ATLAS_NOT_THREAD_SAFE GenericGeometryBuilder
      : public AthAlgTool
      , virtual public IGeometryBuilder
    {

      public:
        /** Constructor */
        GenericGeometryBuilder(const std::string&,const std::string&,const IInterface*);
        
        /** Destructor */
        virtual ~GenericGeometryBuilder();

        /** AlgTool initialize method */
        StatusCode initialize();
        
        /** TrackingGeometry Interface method - optionally a pointer to Bounds */
        TrackingGeometry* trackingGeometry(const TrackingVolume* tvol = 0) const;

        /** The unique signature */
        GeometrySignature geometrySignature() const;

      private:
        ServiceHandle<IEnvelopeDefSvc>          m_enclosingEnvelopeSvc;         //!< The service that determines the outermost boundaries
        int                                     m_enclosingEnvelope;            //!< define which service you wanna have
                                                                                
        ToolHandle<Trk::ITrackingVolumeCreator> m_trackingVolumeCreator;        //!< Helper Tool to create TrackingVolumes
                                                                                
        int                                     m_geometrySignature;            //!< should hopefully correspond soon to the enclosing envelope
        std::string                             m_geometryName;                 //!< name for the volumes
        int                                     m_geometryColorCode;            //!< ->registerColorCode()
        
        double                                  m_barrelFraction;               //!< Barrel configuration                                                                        
        int                                     m_barrelLayers;                 //!< Barrel configuration
        int                                     m_endcapLayers;                 //!< Endcap configuration
        mutable bool                            m_extendedEndcap;               //!< extended Endcap configuration
        int                                     m_extendedEndcapLayers;         //!< extended Endcap configuration

    };
    
    inline GeometrySignature GenericGeometryBuilder::geometrySignature() const { return (GeometrySignature)m_geometrySignature; }

} // end of namespace

#endif // TRKDETDESCRTOOLS_GENERICGEOMETRYBUILDER_H

