# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
## @file  InDetTrigConfConditionsAccess.py
## @brief Configure conditions access for the ID 
## @author Jiri.Masik@cern.ch
##

from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags

if InDetTrigFlags.useConditionsClasses():
  _useDCS = False
  _onlineMode = True
  _prefix = 'InDetTrig'
  #_prefix = ''

  from InDetTrigRecExample.InDetTrigConfigConditions import PixelConditionsSetup
  PixelConditionsSetup.config(useDCS=_useDCS,onlineMode=_onlineMode,prefix=_prefix)
  PixelConditionsSetup.lock()
  PixelConditionsSetup.createSvc()

  
  from InDetTrigRecExample.InDetTrigConfigConditions import SCT_ConditionsSetup
  SCT_ConditionsSetup.config(useDCS=_useDCS, onlineMode=_onlineMode, prefix=_prefix)
  SCT_ConditionsSetup.lock()
  SCT_ConditionsSetup.createSvc()
  
  from InDetTrigRecExample.InDetTrigConfigConditions import TRT_ConditionsSetup
  TRT_ConditionsSetup.config(useDCS=_useDCS, onlineMode=_onlineMode,prefix=_prefix)
  TRT_ConditionsSetup.lock()
  TRT_ConditionsSetup.createSvc()
  
  del _useDCS,_onlineMode,_prefix
else:
  from InDetTrigRecExample.InDetTrigConfigConditions import dummyConditionsSetup
  PixelConditionsSetup = dummyConditionsSetup()
  SCT_ConditionsSetup =  dummyConditionsSetup()
  TRT_ConditionsSetup =  dummyConditionsSetup() 



