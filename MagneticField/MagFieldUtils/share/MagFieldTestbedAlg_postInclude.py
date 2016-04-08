# (0.) configure new MagFieldSvc
from AthenaCommon.AppMgr import ServiceMgr
from MagFieldServices.MagFieldServicesConf import MagField__AtlasFieldSvc
ServiceMgr += MagField__AtlasFieldSvc("AtlasFieldSvc");
#ServiceMgr.AtlasFieldSvc.FieldMapFile = '/afs/cern.ch/user/m/masahiro/public/BFieldMap/combinedMap.root'

from IOVDbSvc.CondDB import conddb
conddb.addOverride('/GLOBAL/BField/Map','BFieldMap-FullAsym-09-solTil3') 

# (1.) setup the Athena algorithm
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from MagFieldUtils.MagFieldUtilsConf import MagField__MagFieldTestbedAlg
topSequence += MagField__MagFieldTestbedAlg('MagFieldTestbedAlg')
topSequence.MagFieldTestbedAlg.useOldMagFieldSvc = True
topSequence.MagFieldTestbedAlg.numberOfReadings = 1000000
topSequence.MagFieldTestbedAlg.recordReadings = True
topSequence.MagFieldTestbedAlg.coordsAlongBeam = True

topSequence.MagFieldTestbedAlg.explicitCoords = True
topSequence.MagFieldTestbedAlg.explicitX = 4846.35
topSequence.MagFieldTestbedAlg.explicitY = 2037.96
topSequence.MagFieldTestbedAlg.explicitZ = -12625.6


topSequence.MagFieldTestbedAlg.useDerivatives = True
topSequence.MagFieldTestbedAlg.onlyCheckSolenoid = False

topSequence.MagFieldTestbedAlg.ReferenceFile = 'oldMagField.root'


# (2.) configure the THistStream
from GaudiSvc.GaudiSvcConf import THistSvc
if not hasattr(ServiceMgr, 'THistSvc'):
  ServiceMgr += THistSvc("THistSvc")

ServiceMgr.THistSvc.Output = ["MagFieldTestbedAlg DATAFILE='comparedMagFields.root' OPT='RECREATE'"]
