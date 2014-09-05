from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

from GaudiPython.Bindings import iService
from AthenaCommon.Logging import logging

log = logging.getLogger("setMagFieldCurrents.py")
prop = iService('AtlasFieldSvc')
if prop.getInterface()==None:
   log.warning("%s does not provide IProperty interface. Will use JobOptionsCatalogue." % prop.name())

if 'solCur' in dir():
   log.info("Setting solenoid current to %d" % solCur)
   setattr(prop, 'UseSoleCurrent', solCur)
   
if 'torCur' in dir():
   log.info("Setting toroid current to %d" % torCur)
   setattr(prop, 'UseToroCurrent', torCur)

