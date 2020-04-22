# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Job options fragment to set field currents, used in magFieldOnOff.trans

from GaudiPython.Bindings import iService, iAlgorithm
from AthenaCommon.Logging import logging

log = logging.getLogger("setMagFieldCurrents.py")
propSvc = iService('AtlasFieldSvc')   # deprecated soon: ATLASRECTS-4687
propAlg = iAlgorithm('AtlasFieldMapCondAlg')

if 'solCur' in dir():
   log.info("Setting solenoid current to %d", solCur)
   setattr(propSvc, 'UseSoleCurrent', solCur)
   setattr(propAlg, 'MapSoleCurrent', solCur)
   
if 'torCur' in dir():
   log.info("Setting toroid current to %d", torCur)
   setattr(propSvc, 'UseToroCurrent', torCur)
   setattr(propAlg, 'MapToroCurrent', torCur)
