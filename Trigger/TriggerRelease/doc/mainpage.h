/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage TriggerRelease Package
@author John.Baines@cern.ch Simon.George@cern.ch Tomasz.Bold@cern.ch Xin.Wu@cern.ch Brian.Petersen@cern.ch
@section IntroTriggerRelease Introduction

This package contains the top level jobOptions for the standalone (e.g. online) running of the trigger
and for the decoding of trigger results at the tier 0. The main jobOptions are 

   - runHLT_standalone.py : main jobOptions for running standalone trigger. 
                            Usage is documented at the head of the file and at https://twiki.cern.ch/twiki/bin/view/Atlas/HLTAlgorithmOnlineIntegration
   - Trigger_topOptions_writeBS.py: 
                            jobOptions for converting RDO to BS for testing the standalone trigger
   - BStoESD_Tier0_HLTConfig_jobOptions.py: 
                            For setting up the decoding of trigger result in bytestream files recorded at point 1.
                            This is included from RecExCommon    
                              

@section ModifierIntro Modifiers:

The standalone trigger is mainly setup in the file Trigger_topOptions_standalone.py by including 
the relevant detector jobOptions, trigger getter and generating the HLT menu from python. However,
it is frequently necessary to do some minor changes to the settings in order to properly run on
data or with some special settings. To simplify and make it clear what modifications are used,
runHLT_standalone.py allows one to apply "modifiers" before and/or after the main setup in 
Trigger_topOptions_standalone.py. Each modifier is a short snippet of python code in python/Modifiers.py
and is mostly used to modify default values for a few configurables. Default lists of modifers are defined
up for cosmics and physics menus plus data and MC running in runHLT_standalone.py, but can be modified by
the user by using the option "ModifierName=True/False".

*/
