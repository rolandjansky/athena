/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page MuonCalibEventBase_page MuonCalibEventBase Package
@author Niels.Van.Eldik@cern.ch and Zdenko.Van.Kesteren@cern.ch

@section MuonCalibEventBase_MuonCalibEventBaseIntro Introduction
This package implements the internal Muon calibration EDM,
with no dependencies on ATHENA. This feature allows processing the CalibrationNtuples within the Muon calibration EDM, without ATHENA dependencies.

@section MuonCalibEventBase_MuonCalibEventBaseOverview Class Overview
The MuonCalibEventBase package contains the following classes :
- MuonCalib::MuonCalibEvent             : a container class containing all information needed for calibration.
- MuonCalib::MuonCalibEventInfo         : contains extra information of the CalibEvent (runnumber/eventnumber/timestamp/tag).
- MuonCalib::MuonCalibPattern           : prototrack from patternrecognition. Owns one or more MuonCalib::MuonCalibSegments.
- MuonCalib::MuonCalibSegment           : track segment on station level. Contains XxxCalibHits.
- MuonCalib::MuonCalibTruthCollection   : collection of MuonCalibXxxTruthHits and MuonCalib::MuonCalibTruth s.
- MuonCalib::MuonCalibRawHitCollection  : collection of MuonCalibXxxRawHits. A 'raw hit' is uncalibrated, and has an occupancy of -1 when the hit is not assigned to any segment.

- MuonCalib::MdtCalibHitBase            : MDT hit, can be used for calibration. Is always assigned to a segment. 
- MuonCalib::RpcCalibHitBase            : RPC hit, used for MDT calibration. Is assigned to a segment.
- MuonCalib::TgcCalibHitBase            : TGC hit, used for MDT calibration. Is assigned to a segment.
- MuonCalib::CscCalibHitBase            : CSC hit, assigned to a segment.

- MuonCalib::MuonCalibTruth             : MuonCalib version of Truth track.
- MuonCalib::MuonCalibMdtTruthHit       : MuonCalib version of MDT SDO.
- MuonCalib::MuonCalibRpcTruthHit       : MuonCalib version of RPC SDO.
- MuonCalib::MuonCalibTgcTruthHit       : MuonCalib version of TGC SDO.
- MuonCalib::MuonCalibCscTruthHit       : MuonCalib version of CSC SDO.

- MuonCalib::MuonCalibRawMdtHit         : MuonCalib version of MdtPrepData. Uncalibrated hit.
- MuonCalib::MuonCalibRawRpcHit         : MuonCalib version of RpcPrepData.
- MuonCalib::MuonCalibRawTgcHit         : MuonCalib version of TgcPrepData.
- MuonCalib::MuonCalibRawCscHit         : MuonCalib version of CscPrepData.



*/
