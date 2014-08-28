# IOVRecExCommission.py - setup IOVDbSvc for RecExCommission
# commissioning with real data

log.warning("IOVDbSvc/IOVRecExCommission.py - This file is obselete; conditions DB access should automatically be configured from detector geometry")

from AthenaCommon.GlobalFlags import globalflags
if (globalflags.DetGeo()=='commis' and globalflags.DataSource()=='data'):
    from IOVDbSvc.CondDB import conddb
else:
    raise RuntimeError,'IOVRecExCommission.py included but geometry is not commis / real data'
