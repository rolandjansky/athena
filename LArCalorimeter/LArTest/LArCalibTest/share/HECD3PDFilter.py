include('LArIdCnv/LArIdCnv_joboptions.py')
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from LArCalibTest.HECNoiseD3PDFlags import HECNoiseflags
from LArCalibTest.HECNoiseD3PDMaker import HECNoiseD3PDMaker

from LArRecUtils.LArRecUtilsConf import LArFlatConditionSvc
theLArCondSvc=LArFlatConditionSvc(DoSuperCells=False,DoRegularCells=True)
svcMgr+=theLArCondSvc
svcMgr.ProxyProviderSvc.ProviderNames += [ "LArFlatConditionSvc" ]
conddb.addFolder("LAR","/LAR/ElecCalibFlat/Pedestal")
svcMgr.LArFlatConditionSvc.PedestalInput="/LAR/ElecCalibFlat/Pedestal"

filAlg=HECNoiseD3PDMaker("HECNoiseFilter", NtupleFileName = HECNoiseflags.FileName())
topSequence  += filAlg

