/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// MaterialValidation.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRALGS_MATERIALVALIDATION_H
#define TRKDETDESCRALGS_MATERIALVALIDATION_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ToolHandle.h"
//Eigen
#include "GeoPrimitives/GeoPrimitives.h"

class StoreGateSvc;


namespace Trk {

    class ITrackingGeometrySvc;
    class IMaterialMapper;
    class TrackingGeometry;
    class TrackingVolume;

    typedef std::pair<Amg::Vector3D, const Trk::TrackingVolume*> PositionAtBoundary;

    /** @class MaterialValidation

      @author Andreas.Salzburger@cern.ch      
     */

    class MaterialValidation : public AthAlgorithm {

      public:

        /** Standard Athena-Algorithm Constructor */
        MaterialValidation(const std::string& name, ISvcLocator* pSvcLocator);
        /** Default Destructor */
        ~MaterialValidation();

        /** standard Athena-Algorithm method */
        StatusCode          initialize();
        
        /** standard Athena-Algorithm method */
        StatusCode          execute();
        
        /** standard Athena-Algorithm method */
        StatusCode          finalize();

      private:
        PositionAtBoundary collectMaterialAndExit(const Trk::TrackingVolume& tvol, 
                                                  const Amg::Vector3D& position, 
                                                  const Amg::Vector3D& direction);
          
        /** Retrieve the TrackingGeometry */
        StatusCode retrieveTrackingGeometry();

        /** Tracking Geometry */
        ServiceHandle<Trk::ITrackingGeometrySvc>             m_trackingGeometrySvc;       //!< Name of the TrackingGeometrySvc
        mutable const TrackingGeometry*                      m_trackingGeometry;          //!< The underlying TrackingGeometry
        
        /** Mapper and Inspector */                          
        ToolHandle<IMaterialMapper>                          m_materialMapper;            //!< Pointer to an IMaterialMapper algTool
        int                                                  m_materialMappingEvents;     //!< count the number of validation records to avoid 2G files
        int                                                  m_maxMaterialMappingEvents;  //!< limit the number of validation records to avoid 2G files
        

        Rndm::Numbers*                                       m_flatDist;                 //!< Random generator for flat distribution
        double                                               m_etaMin;
        double                                               m_etaMax;


    };
}

#endif 
