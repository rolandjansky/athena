/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TRT_ConditionsServices_page The TRT_ConditionsServices package

@author Denver.Whittington@cern.ch

@section TRT_ConditionsServices_CondSvcIntro Introduction

TRT_ConditionsServices contains tools for accessing and managing TRT conditions data

@section TRT_ConditionsServices_CondSvcOverview Class Overview

 - TRT_ConditionsSummarySvc : provides summary of status from all tools
 - TRT_ConditionsTestSvc : returns InDet::TRT_COND_GOOD for testing
 - TRT_DCS_ConditionsSvc : interface to DCS data in COOL
 - TRT_DAQ_ConditionsSvc : interface to DAQ configuration data
 - TRT_ByteStream_ConditionsSvc : interface to ByteStream errors
 - TRT_CalDbSvc : manages  calibration and alignment constants.
 - TRT_AlignDbSvc  : manages  module alignment constants.
 - TRT_StrawAlignDbSvc  : manages  straw alignment constants.
 - TRTStrawStatusSummarySvc : manages dead/noisy straw lists
 - TRTStrawNeighbourSvc : mapping of straws to electronic pads 
 - TRT_HWMappingSvc: mapping of straws to COOL DCS HV channel names/numbers

@section TRT_ConditionsServices_TRT_ConditionsSummarySvc TRT_ConditionsSummarySvc
 This Service provides an overall assessment of an identifier, determined
 by considering the TRT Conditions status flags returned by a list of services
 of type ITRT_ConditionsSvc.

@section TRT_ConditionsServices_TRT_ConditionsTestSvc TRT_ConditionsTestSvc
 This Service provides a simple example of a service which inherets from
 ITRT_ConditionsSvc.  It returns a TRT Conditions status flag (GOOD)
 for an identifier.

@section TRT_ConditionsServices_TRT_DCS_ConditionsSvc TRT_DCS_ConditionsSvc
 This Service provides access to DCS conditions information read from the
 COOL offline database.  It inherets from ITRT_ConditionsSvc and
 returns a TRT Conditions status flag for an identifier indicating whether
 its straw voltage was within tolerance range.

@section TRT_ConditionsServices_TRT_DAQ_ConditionsSvc TRT_DAQ_ConditionsSvc
 This Service provides access to DAQ conditions information from the DAQ
 configuration database. It inherits from ITRT_ConditionsSvc and
 returns a TRT Conditions status flag for an identifier indicating
 whether the ROD it is read out from participated in the run.

@section TRT_ConditionsServices_TRT_ByteStream_ConditionsSvc TRT_ByteStream_ConditionsSvc
 This Service provides access and management of TRT ByteStream errors,
 stored in StoreGate containers.

@section TRT_ConditionsServices_TRT_CalDbSvc TRT_CalDbSvc

 This Service manages TRT calibration data stored in
 TRTCond::NestedContainer classes.
 The following methods are provided:<br>
 Always called: <br>
   - initialize       (register callbacks and cache pointers if requested,
                       read constants from text file if requested)

 TDS Accessors: <br>
   - getT0 (for a given identifier and level - default STRAW)
   - getRtRelation (for a given identifier and level)
   - driftRadius  (for a given drifttime and id. Returns also t0)
   - trtcondid (get TRTCond::ExpandedIdentifier from Identifier)
   - getRtContainer (used in calibration)
   - getT0Container (used in calibration)

 TDS Modifiers:<br>
   - setT0 (for a given TRTCond::ExpandedIdentifier)
   - setRtRelation  (for a given TRTCond::ExpandedIdentifier)

 Persistifiers: <br>
   - writeTextFile  (export TDS contents to a text file)
   - readTextFile   (fill TDS contents to a text file)
   - streamOutCalibObjects  (stream objects to POOL output)
   - registerCalibObjects   (register objects in IOVDb)
   - printCondObjects   (printout details of the data in the TDS) 

 - IOVDbSvc.Folders+=["/TRT/Calib <tag>sometag</tag>"]
 - IOVDbSvc.Folders+=["/TRT/Align <tag>sometag</tag>"]

  For reading in constants from text file (see code and
  share/CalibrationConstants for format) supply file name and
  set readFromTextFile. Also comment out the Folders+=.

  This will work in normal reconstruction. If the purpose
  is to write the constants to database, call the tool
  from the TRT_AlignAlgs package.

@section TRT_ConditionsServices_TRT_AlignDbSvc TRT_AlignDbSvc

 This Service manages TRT alignment data stored in
 AlignableTransform classes.
 The following methods are provided:<br>
 Always called: <br>
   - initialize       (register callbacks and cache pointers if requested,
                       read constants from text file if requested)

 TDS Accessors: <br>
   - getTrans (get level 1 transform for a given identifier)
   - getGlobalTrans (get level 2 transform for a given identifier)
   - getLvl1Align (get level 1 AlignableTransform for a given identifier)
   - getLvl2Align (get level 2 AlignableTransform for a given identifier)
   - getTransPtr (get pointer to AlignableTransform for given key)
   - cgetTransPtr (get const pointer to AlignableTransform for given key)

 TDS Modifiers:<br>
  - createAlignObjects (create empty set of folders according to GeoModel version)
  - setLvl1Align (set level 1 AlignableTransform for a given identifier)
  - setLvl2Align (set level 2 AlignableTransform for a given identifier)
  - tweakTrans (tweak level 1 transform for a given identifier)
  - tweakTrans2 (tweak level 2 transform for a given identifier)

 Persistifiers: <br>
   - writeAlignTextFile  (write TDS contents to a text file)
   - readAlignTextFile   (fill TDS from a text file)
   - streamOutAlignObjects  (stream objects to POOL output)
   - registerAlignObjects   (register objects in IOVDb)
   - printCondObjects   (printout details of the alignment data in the TDS) 

 - IOVDbSvc.Folders+=["/TRT/Calib <tag>sometag</tag>"]
 - IOVDbSvc.Folders+=["/TRT/Align <tag>sometag</tag>"]

  For reading in constants from text file (see code and
  share/CalibrationConstants for format) supply file name and
  set readFromTextFile. Also comment out the Folders+=.

  This will work in normal reconstruction. If the purpose
  is to write the constants to database call the tool from
  the TRT_CalibAlgs package. 

@section TRT_ConditionsServices_TRT_StrawAlignDbSvc TRT_StrawAlignDbSvc
Like TRT_CalDbSvc, but the stored data are here two independent
Rphi rotations of each end of a straw.

@section TRT_ConditionsServices_TRTStrawStatusSummarySvc TRTStrawStatusSummarySvc
 This Service provides access to the dead/noisy straw lists.

@section TRT_ConditionsServices_TRT_HWMappingSvc TRT_HWMappingSvc
 This Service provides a mapping of straws to COOL DCS HV channel
 names/numbers.  The barrel HV-line map can be created by accessing the
 HW Mapping Database by Serguei Oleshko, or by a hard-coded mapping scheme
 defined in the tool's source code.  The endcap map is currently only available
 through the hard-coded mapping scheme.

@section TRT_ConditionsServices_jobOptions jobOptions

*/
