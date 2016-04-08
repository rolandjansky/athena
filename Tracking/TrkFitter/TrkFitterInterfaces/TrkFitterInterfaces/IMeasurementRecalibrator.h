/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMeasurementRecalibrator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ITRKMEASUREMENTRECALIBRATOR_H
#define ITRKMEASUREMENTRECALIBRATOR_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h" // type-def
#include "TrkEventPrimitives/TrackStateDefs.h"
#include "TrkFitterUtils/TrackCalibDefs.h"


class AtlasDetectorID;

namespace Trk 
{
  class RIO_OnTrack;
  class MeasurementBase;

  static const InterfaceID IID_IMeasurementRecalibrator("Trk::IMeasurementRecalibrator", 1, 0);

  /* @class IMeasurementRecalibrator
     @brief interface to inquire and re-make RIO_OnTracks with different error calibration
     @author Wolfgang Liebig
  */

  class IMeasurementRecalibrator : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;
    
    /** @brief abstract base method for re-creating a RIO_OnTrack
        from an existing one (via PrepRawData and std ROT-creator).

        MeasurementBase at input, so return 0 if not a ROT.
        Returns also 0 if it a more precise (than the existing ROT) calibration
        can not be achieved.
        So far for KF internal use, possibly export to full tools later. */
    virtual const RIO_OnTrack*
      makePreciseMeasurement(const MeasurementBase& meas, const TrackParameters& trk,
                             const TrackState::MeasurementType = TrackState::unidentified )
      const =0;

    /** @brief if known, retrieve level of calibration  */
    virtual TrackState::CalibrationType
      calibrationStatus (const MeasurementBase& meas,
                         const TrackState::MeasurementType = TrackState::unidentified )
      const =0;

    /** @brief method to make tube or broad-cluster ROTs. */
    
    virtual const RIO_OnTrack*
      makeBroadMeasurement(const MeasurementBase&, const TrackParameters&,
                           const TrackState::MeasurementType = TrackState::unidentified )
      const =0;
    //      makeUncalibratedMeasurement (const ProtoTrackStateOnSurface&
  };

  inline const InterfaceID& Trk::IMeasurementRecalibrator::interfaceID()
    { 
      return IID_IMeasurementRecalibrator; 
    }

} // end of namespace

#endif 
