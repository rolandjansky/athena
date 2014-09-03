/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage LArConditionsCommon Package

This package provides jobOptions for accessing conditions data 

@author Hong Ma <hma@bnl.gov>
@author RD Schaffer
@author Walter Lampl

@section  Electronic calibration conditions data 

LArConditionsCommon_MC_jobOptions.py: conditions data for G4 MC events

LArConditionsCommon_comm_jobOptions.py: conditions data for Commissioning

LArConditionsCommon_H6_jobOptions.py: conditions data for CTB 2004 (H6)

LArConditionsCommon_H8_jobOptions.py: conditions data for CTB 2004 (H8)

@section Identifier Mapping

LArIdMap_H6_jobOptions.py : Identifier mapping for H6
         
LArIdMap_H8_jobOptions.py : Identifier mapping for H8

LArIdMap_MC_jobOptions.py : Identifier mapping for G4 MC         

LArIdMap_comm_jobOptions.py : : Identifier mapping for commissioning

@section Alignment 
LArAlignable.py : Alignment conditions data 


@section Python classes
LArCondContChannels : helper for selecting conditions container channels
LArCondDataDumper:  helper for dumping conditions data in python.

@section Testing jobOptions based on interactive Athena.

 CheckADC2MeV.py
 CheckLArIdMap_jobOptions.py
 CheckOFC_Shape.py
 DumpLArCondData_Commission.py
 DumpLArCondData_Config.py
 DumpLArCondData_MC.py




*/
