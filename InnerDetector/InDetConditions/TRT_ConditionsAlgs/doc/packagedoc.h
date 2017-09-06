/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TRT_ConditionsAlgs_page The TRT_ConditionsAlgs package

@author phansen@nbi.dk

@section TRT_ConditionsAlgs_CondAlgIntro Introduction

TRT_ConditionsAlgs contains tools and algorithms for storing and managing TRT conditions data
in the TDS and for persistifying them with the IOV service or on flat files.
It also provides the basic RDO-to-RIO conversion as well
as convenient accessor methods.

@section TRT_ConditionsAlgs_CondAlgOverview Class Overview

 - TRT_TestWrite    : Alg to write CTB conditions data to DB 
 - TRT_TestRead     : Alg to read CTB conditions data from DB 
 - TRTCondWrite     : Alg to write TRTCond:: objects to DB 
 - TRTStrawStatusWrite : write bad channel list to DB
 - TRT_ConditionsSummaryExAlg : example of use of TRT_ConditionsSummaryTool


@section TRT_ConditionsAlgs_TRT_ConditionsSummaryExAlg TRT_ConditionsSummaryExAlg
Example algorithm illustrating use of the TRT_ConditionsSummaryTool from the TRT_ConditionsTools package.

@section TRT_ConditionsAlgs_jobOptions jobOptions
  - TRT_CoolTB.py          include this in the InDetTestBeamRecExample
                           to read CTB RT conditions from DB.
  - TRT_CoolCosmic.py      include this in the InDetCosmicRecExample
                           to read Cosmic SR1 TRT conditions from DB.
  - ReadFromPool.py        read test beam const's from a POOL root file 
  - ReadFromText.py        read test beam const's from a text file 
  - TRTBadChannels.py      include this to read StrawStatusTool bad chans
  - TRTBadChannelsWrite.py write StrawStatusTool bad chans to DB
  - WriteAlignCTB.py       fragment to write alignment data to DB 
  - WriteCalibCTB.py       fragment to write calibration data to DB 
  - WriteCosmicSR1DB.py    Write Cosmic SR1 calibration data to DB 
  - WriteTestBeamDB.py     Write CTB calibration data to DB 
  - TRTCondWrite.py        write calibration objects in TRTCond::
  - TRT_ConditionsSummaryExAlg.py   example of use of TRT_ConditionsSummaryTool

*/
