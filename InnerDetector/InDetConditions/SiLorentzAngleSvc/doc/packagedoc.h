/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page SiLorentzAngleSvc_page SiLorentzAngleSvc Package

@authors Grant Gorfine

@section SiLorentzAngleSvc_SiLorentzAngleSvcIntro Introduction

Service for providing Lorentz angle (and the corresponding shift of the measurement)
for each detector element. 

@section SiLorentzAngleSvc_SiLorentzAngleSvcUsage Usage Notes

A python module has been provided for easy setup.
To load simply include the following in your job options.

@verbatim
  from SiLorentzAngleSvc.LorentzAngleSvcSetup import lorentzAngleSvc
@endverbatim

This will setup both pixel and SCT. If you only want to setup them up separately you can do

@verbatim
  from SiLorentzAngleSvc.PixelLorentzAngleSvcSetup import pixelLorentzAngleSvcSetup
  from SiLorentzAngleSvc.SCTLorentzAngleSvcSetup import sctLorentzAngleSvcSetup
@endverbatim


By default it will determine whether to use DB or not from GeoModel.
To force usage of DB (that is, use SiliconConditionsServices which should
normally read from DB):

@verbatim
  lorentzAngleSvc.forceUseDB()
@endverbatim

for both pixel and SCT or to set pixel and SCT separately do:

@verbatim
  lorentzAngleSvc.pixelForceUseDB()
  lorentzAngleSvc.sctForceUseDB()
@endverbatim

If you used PixelLorentzAngleSvcSetup or SCTLorentzAngleSvcSetup do:

@verbatim
  pixelLorentzAngleSvcSetup.forceUseDB()
  sctLorentzAngleSvcSetup.forceUseDB()
@endverbatim

For more configuration one can get access to the service configurable via
members pixel and sct: Eg:

@verbatim
  lorentzAngleSvc.pixel.OutputLevel = VERBOSE
  lorentzAngleSvc.sct.OutputLevel = VERBOSE
  print lorentzAngleSvc.pixel
  print lorentzAngleSvc.sct
@endverbatim

or 

@verbatim
  pixelLorentzAngleSvcSetup.PixelLorentzAngleSvc.OutputLevel = VERBOSE
  sctLorentzAngleSvcSetup.SCTLorentzAngleSvc.OutputLevel = VERBOSE
  print pixelLorentzAngleSvcSetup.PixelLorentzAngleSvc
  print sctLorentzAngleSvcSetup.SCTLorentzAngleSvc
@endverbatim

You can also access the pixel and sct SiliconConditionsSvc:

@verbatim
  lorentzAngleSvc.PixelSiliconConditionsSvc.CheckGeoModel = False # same as forceUseDB
  lorentzAngleSvc.SCT_SiliconConditionsSvc.CheckGeoModel = False # same as forceUseDB
  print lorentzAngleSvc.PixelSiliconConditionsSvc
  print lorentzAngleSvc.SCT_SiliconConditionsSvc
@endverbatim

or 
@verbatim
  pixelLorentzAngleSvcSetup.PixelSiliconConditionsSvc.CheckGeoModel = False # same as forceUseDB
  sctLorentzAngleSvcSetup.SCT_SiliconConditionsSvc.CheckGeoModel = False # same as forceUseDB
  print pixelLorentzAngleSvcSetup.PixelSiliconConditionsSvc
  print sctLorentzAngleSvcSetup.SCT_SiliconConditionsSvc
@endverbatim

Alternatively, you can also access the services by:

@verbatim
  from SiLorentzAngleSvc.LorentzAngleSvcSetup import PixelLorentzAngleSvc,SCTLorentzAngleSvc
  PixelLorentzAngleSvc.OutputLevel = VERBOSE
  SCTLorentzAngleSvc.OutputLevel = VERBOSE
@endverbatim

or 

@verbatim
  from SiLorentzAngleSvc.PixelLorentzAngleSvcSetup import PixelLorentzAngleSvc
  PixelLorentzAngleSvc.OutputLevel = VERBOSE

  from SiLorentzAngleSvc.SCTLorentzAngleSvcSetup import SCTLorentzAngleSvc
  SCTLorentzAngleSvc.OutputLevel = VERBOSE
@endverbatim




*/
