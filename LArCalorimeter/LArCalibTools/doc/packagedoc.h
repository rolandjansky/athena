/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page LArCalibTools_page LArCalibTools
@author Marco.Delmastro@cern.ch

@section LArCalibTools_LArCalibToolsIntro Introduction

This package containes the algorithms to dump the LAr calibration constants
into ROOT TTrees to be used for debugging, displaying, monitoring and analysis
purposes.

@todo
   
   - Uniform TTree variables names/meaning 
   - Support for HEC and FCAL is to be added to   
         - LArAverages2Ntuple
         - LArMphysOverMcalFromTuple
         - LArParams2Ntuple
         - LArPhysWaveFromTuple 
         - LArWFParams2Ntuple
   - LArShape2PhysWave is to be renamed and possibly moved to LArCalibUtils.
   - Cleanup/update of the exaple jobOptions is needed.
   - All the algorithms need to be migrated to configurable.
   - Doxygen description of classes and methods is missing.

@section LArCalibTools_LArCalibToolsOverview Class Overview
  
  The LArCalibTools package contains of following classes:

   - LArAutoCorr2Ntuple
   - LArAverages2Ntuple
   - LArMphysOverMcalFromTuple
   - LArOFC2Ntuple
   - LArParams2Ntuple
   - LArParamsProperties
   - LArPedestals2Ntuple
   - LArPhysWaveFromTuple
   - LArRamps2Ntuple
   - LArReadParamsFromFile
   - LArShape2PhysWave
   - LAruA2MeV2Ntuple
   - LArWaves2Ntuple
   - LArWFParams2Ntuple



*/
