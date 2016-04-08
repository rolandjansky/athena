from AthenaCommon.DetFlags import DetFlags 
DetFlags.Muon_setOn()
DetFlags.Calo_setOn()
DetFlags.ID_setOn()

include('RecExCond/AllDet_detDescr.py')

if len(globalflags.ConditionsTag()):
  from IOVDbSvc.CondDB import conddb
  conddb.setGlobalTag(globalflags.ConditionsTag())

# (0.) configure new MagFieldSvc
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import theApp   

import MagFieldServices.SetupField


theApp.EvtMax = 1

from IOVDbSvc.CondDB import conddb
conddb.addOverride('/GLOBAL/BField/Map','BFieldMap-FullAsym-09-solTil3')

# (1.) setup the Athena algorithm
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from MagFieldUtils.MagFieldUtilsConf import MagField__MagFieldTestbedAlg
topSequence += MagField__MagFieldTestbedAlg('MagFieldTestbedAlg')
topSequence.MagFieldTestbedAlg.useOldMagFieldSvc = False
topSequence.MagFieldTestbedAlg.numberOfReadings = 1000000
topSequence.MagFieldTestbedAlg.recordReadings = True
topSequence.MagFieldTestbedAlg.coordsAlongBeam = False
topSequence.MagFieldTestbedAlg.useDerivatives = True

#topSequence.MagFieldTestbedAlg.explicitCoords = False
#topSequence.MagFieldTestbedAlg.explicitX = 4846.35
#topSequence.MagFieldTestbedAlg.explicitY = 2037.96
#topSequence.MagFieldTestbedAlg.explicitZ = -12625.6
topSequence.MagFieldTestbedAlg.onlyCheckSolenoid = False

#topSequence.MagFieldTestbedAlg.ReferenceFile = 'NewMagField.root'


# (2.) configure the THistStream
from GaudiSvc.GaudiSvcConf import THistSvc
if not hasattr(ServiceMgr, 'THistSvc'):
  ServiceMgr += THistSvc("THistSvc")

ServiceMgr.THistSvc.Output = ["MagFieldTestbedAlg DATAFILE='NewMagField.root' OPT='RECREATE'"]
