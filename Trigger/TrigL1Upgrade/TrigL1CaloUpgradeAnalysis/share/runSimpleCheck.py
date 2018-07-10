
import ROOT

# later on we could define this in ROOT
TestFile="myAODTTv3.root"
OutputFile="test.output.root"
PoolAODInput=[TestFile]

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

svcMgr.EventSelector.InputCollections = [TestFile]

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
b=TrigT1CaloEFex(EnableMonitoring=True,OutputClusterName="SCluster1")
b1=TrigT1CaloEFex(name="TrigT1CaloEFexCl",EnableMonitoring=True,CleanCellContainer=True,OutputClusterName="SCluster1Cl")
c=TrigT1CaloTauFex()
d=EFexAnalysis(EnableMonitoring=True)
d1=EFexAnalysis(name="EFexAnalysisCl",EnableMonitoring=True,InputClusterName="SClusterCl")
d2=EFexAnalysis(name="EFexAnalysis1",EnableMonitoring=True,InputClusterName="SCluster1")
d3=EFexAnalysis(name="EFexAnalysis1Cl",EnableMonitoring=True,InputClusterName="SCluster1Cl")
topSequence+=a
topSequence+=a1
topSequence+=b
topSequence+=b1
#topSequence+=c
topSequence+=d
topSequence+=d1
topSequence+=d2
topSequence+=d3
#theLArFex=LArFex(EtInSigma=3.0,CaloNoiseTool=theCaloNoiseTool)
#topSequence+=theLArFex
topSequence+=LArFexAnalysis()
topSequence+=DumpAll(InputClusterName="SCluster1")
topSequence+=DumpAll(name="DumpAllCl",InputClusterName="SCluster1Cl")
#topSequence+=LArFex(name="1Sig",EtInSigma=3.0,CaloNoiseTool=theCaloNoiseTool,EtInSigmaSelect=1.0,OutputClusterName="LArLayer1Vars1Sig")
topSequence+=DumpAll(name="DumpAllCl1Sig",InputClusterName="SCluster1Cl",InputLArFexName="LArLayer1Vars1Sig",SaveLayer1Cells=False)
#topSequence+=LArFex(name="2Sig",EtInSigma=3.0,CaloNoiseTool=theCaloNoiseTool,EtInSigmaSelect=2.0,OutputClusterName="LArLayer1Vars2Sig")
topSequence+=DumpAll(name="DumpAllCl2Sig",InputClusterName="SCluster1Cl",InputLArFexName="LArLayer1Vars2Sig",SaveLayer1Cells=False)
#topSequence+=LArFex(name="3Sig",EtInSigma=3.0,CaloNoiseTool=theCaloNoiseTool,EtInSigmaSelect=3.0,OutputClusterName="LArLayer1Vars3Sig")
topSequence+=DumpAll(name="DumpAllCl3Sig",InputClusterName="SCluster1Cl",InputLArFexName="LArLayer1Vars3Sig",SaveLayer1Cells=False)
topSequence+=CaloBandwidth(EtInSigma=2.0,CaloNoiseTool=theCaloNoiseTool)
topSequence+=EFexEratioAlgo(name="EFexEratioAlgo",InputClusterName="SCluster")
topSequence+=EFexEratioAlgo(name="EFexEratioAlgoCl",InputClusterName="SClusterCl")
topSequence+=EFexEratioAlgo(name="EFexEratioAlgo1",InputClusterName="SCluster1")
topSequence+=EFexEratioAlgo(name="EFexEratioAlgo1Cl",InputClusterName="SCluster1Cl")
topSequence+=EFexEratioRateAlgo(name="EFexEratioRateAlgo",InputClusterName="SCluster")
topSequence+=EFexEratioRateAlgo(name="EFexEratioRateAlgoCl",InputClusterName="SClusterCl")
topSequence+=EFexEratioRateAlgo(name="EFexEratioRateAlgo1",InputClusterName="SCluster1")
topSequence+=EFexEratioRateAlgo(name="EFexEratioRateAlgo1Cl",InputClusterName="SCluster1Cl")

#svcMgr.StoreGateSvc.Dump=True

# initialize athena
theApp.initialize()
#theApp.run()
# main loop
ev=0
for i in range(0,10000):
#for i in range(0,5):
#for i in range(0,199):
 try : 
  a=theApp.nextEvent().isFailure() 
  if ( a ) :
    print 'should stop it in ev ',ev
    break
  print 'Event Number',ev
  ev=ev+1;
 except : 
  print "except in ev", ev
  break
theApp.finalize().ignore();

print 'writing out file after',ev,'events'
theApp.exit()

