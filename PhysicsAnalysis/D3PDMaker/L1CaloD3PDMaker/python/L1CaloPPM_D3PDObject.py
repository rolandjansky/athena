# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $ID$
#
# @file L1CaloD3PDMaker/python/L1CaloPPM_D3PDObject.py
# @author John Morris <john.morris@cern.ch>
# @date March 2010
# @brief Fills L1Calo PPM information
#

import D3PDMakerCoreComps
import L1CaloD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject

from RecExConfig.RecFlags import rec
isMC = rec.doTruth.get_Value()


if not isMC:
  from IOVDbSvc.CondDB import conddb
  conddb.addFolder('TRIGGER','/TRIGGER/L1Calo/V1/Conditions/DisabledTowers')
  conddb.addFolder('TRIGGER','/TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels')

# Maker
def makeL1CaloPPM_D3PDObject(name, prefix, object_name, getter = None, sgkey = ''):
    return D3PDMakerCoreComps.VoidObjFillerTool(name, Prefix = prefix,
                                                ObjectName = object_name)

# create the object type
L1CaloPPM_D3PDObject = D3PDObject(makeL1CaloPPM_D3PDObject,
                                  default_object_name = 'L1CaloPPM_D3PDObject',
                                  default_name = 'L1CaloPPMFiller')

# define blocks
L1CaloPPM_D3PDObject.defineBlock(lod = 0,
                                 name = 'L1CaloPPM',
                                 func = L1CaloD3PDMaker.L1CaloPPMFillerTool,
                                 prefix = 'L1CaloPPM_',
                                 DoCalibration = False,
                                 MinADC = 36,
                                 MinCaloCellEnergy = 0.8,
                                 UseRandom = True,
                                 MinRandom = 0.01,
                                 IsMC = isMC)
