from Digitization.DigitizationFlags import jobproperties
#This is necessary as currently the background magnetic field was configured via Oracle
#while the signal magnetic field configured via COOL
jobproperties.Digitization.overrideMetadata=["MagneticField"]
from PoolSvc.PoolSvcConf import PoolSvc
ServiceMgr+=PoolSvc(SortReplicas=True)
ServiceMgr.PoolSvc.AttemptCatalogPatch = True
