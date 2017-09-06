/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Various flags - put here to avoid the need to include     //
//  other header files just for the flags                     //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef TRACKCOMMONFLAGS_H
#define TRACKCOMMONFLAGS_H

#include <QFlags>
#include <QString>

class TrackCommonFlags {
public:

  enum TrackPartsFlag {
    NoParts          = 0x0000,
    ActualPath       = 0x0001,
    InDetProjections = 0x0002,
    MuonProjections  = 0x0004,
    VertexProjections  = 0x0008
  };
  Q_DECLARE_FLAGS(TrackPartsFlags, TrackPartsFlag)

  enum SELECTIONMODE { SINGLEOBJECT, MULTITRACK, TRACKFIT };
  static QString toString(const SELECTIONMODE&);

  enum FITTERMODE { FROMPRDS, REFITSINGLETRACK, EXTENDTRACKWITHPRDS,COMBINETWOTRACKS };
  static QString toString(const FITTERMODE&);

  enum DETAILLEVEL { SIMPLE, DETAILED, AUTO };
  static QString toString(const DETAILLEVEL& );

   enum TSOSPartsFlag {
     TSOS_NoObjects = 0x0000, 
     TSOS_TrackPars = 0x0001, //!< bit 0
     TSOS_TrackParsErrorsNotPerigee = 0x0002, //!< bit 1
     TSOS_TrackParsErrorsPerigee = 0x0004, //!< bit 2
     TSOS_MeasError = 0x008, //!< bit 3
     TSOS_MeasRioOnTrackNotOutlier = 0x0010, //!< bit 4
     TSOS_MeasCompetingRioOnTrackNotOutlier = 0x0020, //!< bit 5
     TSOS_MeasRioOnTrackOutlier = 0x0040,//!< bit 6
     TSOS_MeasCompetingRioOnTrackOutlier = 0x0080,//!< bit 7
     TSOS_SurfacesDetElem = 0x0100, //!< bit 9
     TSOS_SurfacesCustom = 0x0200, //!< bit 10
     TSOS_TubeSurfaces = 0x0400, //!< bit 11
     TSOS_MaterialEffectsWithNoDeltaE = 0x1000, //!< bit 12
     TSOS_MaterialEffects = 0x2000,//!< bit 13
     TSOS_Hole = 0x4000,//!< bit 14
     //Convenience
     TSOS_AnyMaterialEffects = 0x3000,
     TSOS_AnySurface = 0x0700,
     TSOS_AnyMeasurement = 0x00F0,
     TSOS_AnyMeasurementNotOutlier = 0x0030,
     TSOS_AnyMeasurementOutlier = 0x00C0,
     TSOS_AnyMeasRioOnTrack = 0x0050,
     TSOS_AnyMeasCompetingRioOnTrack = 0x00A0,
     TSOS_AnyParsErrors = 0x0006,
     TSOS_AnyErrors = 0x10FE
   };
   Q_DECLARE_FLAGS(TSOSPartsFlags, TSOSPartsFlag)

private:
  TrackCommonFlags();
  ~TrackCommonFlags();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(TrackCommonFlags::TrackPartsFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(TrackCommonFlags::TSOSPartsFlags)

#endif
