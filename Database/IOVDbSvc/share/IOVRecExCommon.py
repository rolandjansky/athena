# IOVRecExCommon.py - setup IOVDbSvc for RecExCommon
# Full ATLAS Monte Carlo setup

log.warning("IOVDbSvc/IOVRecExCommon.py - This file is obselete; conditions DB access should automatically be configured from detector geometry")

from AthenaCommon.GlobalFlags import globalflags
if (globalflags.DetGeo()=='atlas'):
    from IOVDbSvc.CondDB import conddb
else:
    raise RuntimeError,'IOVRecExCommon.py included but geometry is not atlas'

