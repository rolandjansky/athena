# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Job options fragment to set field currents, used in magFieldOnOff.trans

from GaudiPython.Bindings import iService, iAlgorithm
from AthenaCommon.Logging import logging

log = logging.getLogger("setMagFieldCurrents.py")
mapCondAlg = iAlgorithm('AtlasFieldMapCondAlg')

if 'solCur' in dir():
   log.info("Setting solenoid current to %d", solCur)
   setattr(mapCondAlg, 'MapSoleCurrent', solCur)

if 'torCur' in dir():
   log.info("Setting toroid current to %d", torCur)
   setattr(mapCondAlg, 'MapToroCurrent', torCur)

if 'run' in dir():
   log.info("Setting run number to %d", run)
   setattr(iService('HltEventLoopMgr'), 'forceRunNumber', run)
