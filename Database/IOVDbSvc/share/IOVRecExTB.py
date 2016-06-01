# IOVRecExTB.py - setup IOVDbSvc for RecExTB
# combined testbeam analysis with data or Monte Carlo

log.warning("IOVDbSvc/IOVRecExTB.py - This file is obselete; conditions DB access should automatically be configured from detector geometry")

from AthenaCommon.GlobalFlags import globalflags
if (globalflags.DetGeo() in ['ctbh8','ctbh6']):
    from IOVDbSvc.CondDB import conddb
else:
    raise RuntimeError,'IOVRecExTB.py included but geometry is not CTB'
