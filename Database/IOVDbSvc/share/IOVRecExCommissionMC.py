# IOVRecExCommission.py - setup IOVDbSvc for RecExCommissionMC
# commissioning with Monte Carlo

log.warning("IOVDbSvc/IOVRecExCommissionMC.py - This file is obselete; conditions DB access should automatically be configured from detector geometry")

from AthenaCommon.GlobalFlags import globalflags
if (globalflags.DetGeo()=='commis' and globalflags.DataSource()!='data'):
    from IOVDbSvc.CondDB import conddb
else:
    raise RuntimeError,'IOVRecExCommissionMC.py included but geometry is not commis / Monte Carlo'
