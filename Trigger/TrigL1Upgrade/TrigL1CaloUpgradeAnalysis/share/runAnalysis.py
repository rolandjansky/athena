
import ROOT

# later on we could define this in ROOT
TestFile="myAODTTv3.root"
#OutputFile="test.output.root"
#PoolAODInput=[TestFile]

# set conditions
if not ( "DetDescrVersion" in dir() ) :
   DetDescrVersion="ATLAS-R2-2015-03-01-00"

if not ( "ConditionsTag" in dir() ) :
   ConditionsTag="OFLCOND-RUN12-SDR-31"
# set everything
include ("RecExCond/RecExCommon_DetFlags.py")
DetFlags.detdescr.all_setOff()
DetFlags.detdescr.Calo_setOn()
include ("RecExCond/AllDet_detDescr.py")

# Set the input file
import AthenaPoolCnvSvc.ReadAthenaPool

# commented if running on the grid
#svcMgr.EventSelector.InputCollections = [TestFile]


# import the algorithm in python and make sure it
# is attached to the sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from TrigL1CaloUpgrade.TrigL1CaloUpgradeConf import SimpleSuperCellChecks
conddb.addFolder("LAR_OFL","/LAR/IdentifierOfl/OnOffIdMap_SC<tag>LARIdentifierOflOnOffIdMap_SC-000</tag>")
try:
  from LArTools.LArToolsConf import LArSuperCellCablingTool
except Exception:
  print 'Did not work from LArTools, will try something else'
try:
  from LArCabling.LArCablingConf import LArSuperCellCablingTool
except Exception:
  print 'Did not work from LArCabling, will try something else'
ToolSvc+=LArSuperCellCablingTool()
from TrigL1CaloUpgrade.TrigL1CaloUpgradeConf import SimpleLArDigitsChecks
from TrigL1CaloUpgrade.TrigL1CaloUpgradeConf import TrigT1CaloEFex
from TrigL1CaloUpgrade.TrigL1CaloUpgradeConf import TrigT1CaloTauFex
from TrigL1CaloUpgradeAnalysis.TrigL1CaloUpgradeAnalysisConf import EFexAnalysis
from TrigL1CaloUpgradeAnalysis.TrigL1CaloUpgradeAnalysisConf import EFexEratioAlgo
from TrigL1CaloUpgradeAnalysis.TrigL1CaloUpgradeAnalysisConf import EFexEratioRateAlgo
from TrigL1CaloUpgradeAnalysis.TrigL1CaloUpgradeAnalysisConf import DumpAll
from TrigL1CaloUpgrade.TrigL1CaloUpgradeConf import LArFex
from TrigL1CaloUpgradeAnalysis.TrigL1CaloUpgradeAnalysisConf import LArFexAnalysis
from TrigL1CaloUpgradeAnalysis.TrigL1CaloUpgradeAnalysisConf import CaloBandwidth

from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
theCaloNoiseTool=CaloNoiseToolDefault()
ToolSvc+=theCaloNoiseTool

a=SimpleSuperCellChecks()
a1=SimpleLArDigitsChecks()
d=EFexAnalysis(EnableMonitoring=True)
d1=EFexAnalysis(name="EFexAnalysisCl",EnableMonitoring=True,InputClusterName="SClusterCl")
topSequence+=a
topSequence+=a1
topSequence+=d
topSequence+=d1
topSequence+=LArFexAnalysis()
topSequence+=DumpAll()
topSequence+=DumpAll(name="DumpAllCl")
topSequence+=DumpAll(name="DumpAllCl1Sig",InputLArFexName="LArLayer1Vars1Sig",SaveLayer1Cells=False)
topSequence+=DumpAll(name="DumpAllCl2Sig",InputLArFexName="LArLayer1Vars2Sig",SaveLayer1Cells=False)
topSequence+=DumpAll(name="DumpAllCl3Sig",InputLArFexName="LArLayer1Vars3Sig",SaveLayer1Cells=False)
topSequence+=CaloBandwidth(EtInSigma=2.0,CaloNoiseTool=theCaloNoiseTool)
topSequence+=EFexEratioAlgo(name="EFexEratioAlgo",InputClusterName="SCluster")
topSequence+=EFexEratioAlgo(name="EFexEratioAlgoCl",InputClusterName="SClusterCl")
topSequence+=EFexEratioRateAlgo(name="EFexEratioRateAlgo",InputClusterName="SCluster")
topSequence+=EFexEratioRateAlgo(name="EFexEratioRateAlgoCl",InputClusterName="SClusterCl")

import GaudiSvc.GaudiSvcConf as GaudiSvcConf
svcMgr += GaudiSvcConf.StatusCodeSvc()
svcMgr.StatusCodeSvc.AbortOnError=False

