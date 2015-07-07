/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MeasRecalibSteeringTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKMEASRECALIBSTEERINGTOOL_H
#define TRKMEASRECALIBSTEERINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkFitterInterfaces/IMeasurementRecalibrator.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk 
{

  class MeasurementBase;
  class RIO_OnTrack;
  class MeasurementTypeID;
  class IRIO_OnTrackCreator;

  /** @class MeasRecalibSteeringTool
      @brief tool to repeat the calibration PrepRawData to RIO_OnTrack on an
             existing RIO_OnTrack specifying if the calibration should be
             refined or reset.

      Predecessor tool to a more comprehensive approach to error tuning.
      
      @author  Wolfgang Liebig <Wolfgang.Liebig -at- cern.ch>
  */  

  class MeasRecalibSteeringTool : virtual public IMeasurementRecalibrator, public AthAlgTool
    {
    public:
      MeasRecalibSteeringTool(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~MeasRecalibSteeringTool ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();

      virtual const RIO_OnTrack*
        makePreciseMeasurement (const MeasurementBase& meas, const TrackParameters& trk,
                                const TrackState::MeasurementType) const;
      
      virtual TrackState::CalibrationType
        calibrationStatus (const MeasurementBase& meas,
                           const TrackState::MeasurementType) const;

      virtual const RIO_OnTrack*
        makeBroadMeasurement(const MeasurementBase&, const TrackParameters&,
                             const TrackState::MeasurementType ) const;
    private:
      
      // tool handles for a whole zoo of ROT creators
      ToolHandle< IRIO_OnTrackCreator >   m_rotCreator;
      ToolHandle< IRIO_OnTrackCreator >   m_broadPixelClusterCreator;
      ToolHandle< IRIO_OnTrackCreator >   m_broadSctClusterCreator;
      ToolHandle< IRIO_OnTrackCreator >   m_trtDriftCircleCreator;
      ToolHandle< IRIO_OnTrackCreator >   m_trtTubeHitCreator;
      bool                                m_haveInDetTools;
      //bool                                m_haveMuonTools;

      //! Helper to detect type of sub-detector
      const AtlasDetectorID*            m_idHelper;
      MeasurementTypeID*                m_mbHelper;

    }; 
} // end of namespace

#endif 
