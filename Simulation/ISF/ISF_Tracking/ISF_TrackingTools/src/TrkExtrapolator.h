/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrkExtrapolator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_TRACKINGTOOLS_TRKEXTRAPOLATOR_H
#define ISF_TRACKINGTOOLS_TRKEXTRAPOLATOR_H 1

// class header
#include "ISF_TrackingInterfaces/ITrkExtrapolator.h"

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"

namespace Trk {
  class IExtrapolator;
  class ITrackingGeometrySvc;
  class TrackingGeometry;
  class TrackingVolume;
  class PdgToParticleHypothesis;
}

namespace ISF {

   class ISFParticle;


  /** @class TrkExtrapolator

      An Athena AlgTool wrapper for the Tracking Extrapolator engine.

      @author Elmar.Ritsch@cern.ch (refactored original implementation by Robert Harrington <roberth@cern.ch>)
     */
  class TrkExtrapolator : public AthAlgTool, virtual public ISF::ITrkExtrapolator {

    public:
      /** Constructor with parameters */
      TrkExtrapolator( const std::string& t, const std::string& n, const IInterface* p );

      /** Destructor */
      ~TrkExtrapolator();

      /** Athena AlgTool initialization **/
      StatusCode  initialize();
      /** Athena AlgTool finalization **/
      StatusCode  finalize();

      /** Extrapolate the given ISFParticle */
      ISF::ISFParticle* extrapolate( const ISF::ISFParticle &particle ) const;

    private:
      bool retrieveTrackingGeometry() const;

      /** tracking geometry for geometry signature */
      mutable const Trk::TrackingGeometry*      m_trackingGeometry;     //!< the tracking geometry owned by the navigator      
      ServiceHandle<Trk::ITrackingGeometrySvc>  m_trackingGeometrySvc;  //!< ServiceHandle to the TrackingGeometrySvc
      std::string                               m_trackingGeometryName; //!< default name of the TrackingGeometry  

      /** extrapolation to calo entry */
      ToolHandle<Trk::IExtrapolator>       m_extrapolator;              //!< ToolHandle for track extrapolator

      std::string                               m_trackingVolumeName;   //!< name of the volume within the extrapolation is carried out
      mutable const Trk::TrackingVolume*        m_trackingVolume;       //!< volume within the extrapolation is carried out

      Trk::PdgToParticleHypothesis*             m_pdgToParticleHypothesis; //!< converts PDG ID to hypothesis for TrackParameters


  };

}


#endif //> !ISF_TRACKINGTOOLS_TRKEXTRAPOLATOR_H
