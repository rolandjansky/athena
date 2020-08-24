/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenericGeometryBuilderCond.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRTOOLS_GENERICGEOMETRYBUILDERCOND_H
#define TRKDETDESCRTOOLS_GENERICGEOMETRYBUILDERCOND_H

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkDetDescrInterfaces/IGeometryBuilderCond.h"
#include "TrkDetDescrUtils/GeometrySignature.h"

// Gaudi & Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

class IEnvelopeDefSvc;

namespace Trk {

    class TrackingGeometry;
    class TrackingVolume;
    class ITrackingVolumeCreator;   
    class ITrackingVolumeHelper;   

    /** @class GenericGeometryBuilderCond

      A TrackingGeometry builder that takes the EnvelopSvc and creates 
      a very simple generic tracking geoemtry configured with Layers
      
      @author Andreas.Salzburger@cern.ch   
     */

    class GenericGeometryBuilderCond : public AthAlgTool, virtual public IGeometryBuilderCond {

      public:
        /** Constructor */
        GenericGeometryBuilderCond(const std::string&,const std::string&,const IInterface*);
        
        /** Destructor */
        virtual ~GenericGeometryBuilderCond();

        /** AlgTool initialize method */
        StatusCode initialize();
        
        /** AlgTool finalize method */
        StatusCode finalize();
        
        /** TrackingGeometry Interface method - optionally a pointer to Bounds */
        std::pair<EventIDRange, const Trk::TrackingGeometry*> trackingGeometry(const EventContext& ctx, std::pair<EventIDRange, const Trk::TrackingVolume*> tVolPair) const;

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
    
    inline GeometrySignature GenericGeometryBuilderCond::geometrySignature() const { return (GeometrySignature)m_geometrySignature; }

} // end of namespace

#endif // TRKDETDESCRTOOLS_GENERICGEOMETRYBUILDERCOND_H

