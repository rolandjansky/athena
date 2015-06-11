/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// VertexMapper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRTOOLS_VERTEXMAPPER_H
#define TRKDETDESCRTOOLS_VERTEXMAPPER_H

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Gaudi & Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentListener.h"
// Trk
#include "TrkDetDescrInterfaces/IVertexMapper.h"
// ROOT
#include "TTree.h"
#include "TString.h"

class TTree;

namespace Trk {

    class TrackingGeometry;

    /** @class VertexMapper
    
      The vertex mapper transforms vertices from global coordinates into the local coordinate frame
      of the closest sensitive surface
    
      @TODO add option to also map onto non-sensitive layers

      @author Andreas.Salzburger@cern.ch
     */

    class VertexMapper : public AthAlgTool, virtual public IVertexMapper {

      public:

        /** AlgTool like constructor */
        VertexMapper(const std::string&,
                     const std::string&,
                     const IInterface*);

        /**Virtual destructor*/
        virtual ~VertexMapper();

        /** AlgTool initialize method */
        StatusCode initialize();
        
        /** AlgTool finalize method */
        StatusCode finalize();

        /** Record the vertex into the local frame of the closest module  */
        MappedVertex mapToLocal(const Amg::Vector3D& vertex) const;

        /** Handle the incident from the incident service */
        void handle( const Incident& inc );

      private:
          
        //!< retrieve TrackingGeometry
        StatusCode  updateTrackingGeometry() const;                                       //!< update the tracking geometry
        
        //!< return and retrieve
        const TrackingGeometry& trackingGeometry() const throw (GaudiException);          //!< retrieve the tracking geometry

        mutable const TrackingGeometry*                      m_trackingGeometry;          //!< the tracking geometry owned by the navigator
        std::string                                          m_trackingGeometryName;      //!< Name of the TrackingGeometry as given in Detector Store

    };
    
    inline const Trk::TrackingGeometry& VertexMapper::trackingGeometry() const throw (GaudiException) {
        if (!m_trackingGeometry && updateTrackingGeometry().isFailure()){
            ATH_MSG_FATAL("Could not load TrackingGeometry with name '" << m_trackingGeometryName << "'. Aborting." );
            throw GaudiException("VertexMapper", "Problem with TrackingGeometry loading.", StatusCode::FAILURE);
        }
        return (*m_trackingGeometry);
    }

} // end of namespace

#endif // TRKDETDESCRTOOLS_VERTEXMAPPER_H

