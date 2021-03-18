
import ROOT

# later on we could define this in ROOT
TestFile="myAODTTv3.root"

# set conditions
if not ( "DetDescrVersion" in dir() ) :
   DetDescrVersion="ATLAS-R2-2015-03-01-00"

if not ( "ConditionsTag" in dir() ) :
   ConditionsTag="OFLCOND-RUN12-SDR-31"
# set everything
include ("RecExCond/RecExCommon_DetFlags.py")
DetFlags.Print()
DetFlags.detdescr.all_setOff()
DetFlags.detdescr.Calo_setOn()
DetFlags.detdescr.BField_setOn()
DetFlags.detdescr.bpipe_setOn()
DetFlags.detdescr.Muon_setOn()
DetFlags.detdescr.SCT_setOn()
DetFlags.detdescr.pixel_setOn()
DetFlags.geometry.pixel_setOn()
if False : 
   DetFlags.detdescr.all_setOn()
   DetFlags.geometry.all_setOn()
   DetFlags.detdescr.ID_setOff()
   DetFlags.geometry.ID_setOff()
include ("RecExCond/AllDet_detDescr.py")

from egammaRec.Factories import Factory, ToolFactory
from IsolationCorrections.IsolationCorrectionsConf import CP__IsolationCorrectionTool as ICT
IsoCorrectionToolTrig = ICT( name = "NewLeakageCorrToolTrig",
                                     IsMC = True)

ToolSvc+=IsoCorrectionToolTrig
from ParticlesInConeTools.ParticlesInConeToolsConf import xAOD__CaloClustersInConeTool
TrigCaloClustersInConeTool = xAOD__CaloClustersInConeTool(CaloClusterLocation = "CaloCalTopoCluster")
ToolSvc+=TrigCaloClustersInConeTool
from IsolationTool.IsolationToolConf import xAOD__CaloIsolationTool
from CaloClusterCorrection import CaloClusterCorrectionConf as Cccc
TrigCaloFillRectangularCluster = Cccc.CaloFillRectangularCluster(
        name = "trigegamma_CaloFillRectangularCluster",
        eta_size = 5,
        phi_size = 7,
        cells_name = "")
ToolSvc+=TrigCaloFillRectangularCluster

def configureEDCorrection(tool):
  from EventShapeTools.EventDensityConfig import configEventDensityTool, EventDensityAlg
  from JetRec.JetRecStandard import jtm
  from AthenaCommon.AppMgr import ToolSvc
  tc=configEventDensityTool("EDtpIsoCentralTool", jtm.emget,
                   radius          = 0.5,
                   AbsRapidityMin  = 0.0,
                   AbsRapidityMax  = 1.5,
                   OutputContainer = "DensitytpIsoCentral",
                   #OutputLevel     = OutputLevel
                   )
  ToolSvc+=tc
  tf=configEventDensityTool("EDtpIsoForwardTool", jtm.emget,
                          radius          = 0.5,
                          AbsRapidityMin  = 1.5,
                          AbsRapidityMax  = 3.0,
                          OutputContainer = "DensitytpIsoForward",
                          #OutputLevel     = OutputLevel
                          )
  ToolSvc += tf
  from AthenaCommon.AlgSequence import AlgSequence
  topSequence = AlgSequence()
  if not hasattr(topSequence,'EDtpIsoCentralAlg'):
    topSequence += EventDensityAlg("EDtpIsoCentralAlg", EventDensityTool = tc)
  if not hasattr(topSequence,'EDtpIsoForwardAlg'):
    topSequence += EventDensityAlg("EDtpIsoForwardAlg", EventDensityTool = tf)

from IsolationTool.IsolationToolConf import xAOD__CaloIsolationTool
from CaloIdentifier import SUBCALO 
TrigCaloTopoIsolationTool = xAOD__CaloIsolationTool(name="CaloIsolationTool",
                                #postInit = [configureEDCorrection],
                                CaloFillRectangularClusterTool  = TrigCaloFillRectangularCluster,
                                IsoLeakCorrectionTool           = IsoCorrectionToolTrig,
                                ClustersInConeTool              = TrigCaloClustersInConeTool,
                                EMCaloNums  = [SUBCALO.LAREM],
                                HadCaloNums = [SUBCALO.LARHEC, SUBCALO.TILE],
                                UseEMScale = True)
ToolSvc+=TrigCaloTopoIsolationTool
configureEDCorrection(TrigCaloTopoIsolationTool)

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
  print ('Did not work from LArTools, will try something else')
try:
  from LArCabling.LArCablingConf import LArSuperCellCablingTool
except Exception:
  print ('Did not work from LArCabling, will try something else')
ToolSvc+=LArSuperCellCablingTool()
from TrigL1CaloUpgrade.TrigL1CaloUpgradeConf import SimpleLArDigitsChecks
from TrigL1CaloUpgrade.TrigL1CaloUpgradeConf import TrigT1CaloEFex
from TrigL1CaloUpgrade.TrigL1CaloUpgradeConf import TrigT1CaloforwEFex
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

topSequence+=TrigT1CaloforwEFex(EnableMonitoring=True,CleanCellContainer=True)
topSequence+=DumpAll(name="DumpAllClFor",InputClusterName="SClusterFor",CaloTopoIsolationTool=TrigCaloTopoIsolationTool);

a=SimpleSuperCellChecks()
a1=SimpleLArDigitsChecks()
d=EFexAnalysis(EnableMonitoring=True)
d1=EFexAnalysis(name="EFexAnalysisCl",EnableMonitoring=True,InputClusterName="SClusterCl")
topSequence+=a
topSequence+=a1
topSequence+=d
topSequence+=d1
topSequence+=LArFexAnalysis()
topSequence+=DumpAll(CaloTopoIsolationTool=TrigCaloTopoIsolationTool)
topSequence+=DumpAll(name="DumpAllCl",InputClusterName="SClusterCl",CaloTopoIsolationTool=TrigCaloTopoIsolationTool)
topSequence+=DumpAll(name="DumpAllCl1Sig",InputClusterName="SClusterCl",InputLArFexName="LArLayer1Vars1Sig",SaveLayer1Cells=False,CaloTopoIsolationTool=TrigCaloTopoIsolationTool)
topSequence+=DumpAll(name="DumpAllCl2Sig",InputClusterName="SClusterCl",InputLArFexName="LArLayer1Vars2Sig",SaveLayer1Cells=False,CaloTopoIsolationTool=TrigCaloTopoIsolationTool)
topSequence+=DumpAll(name="DumpAllCl3Sig",InputClusterName="SClusterCl",InputLArFexName="LArLayer1Vars3Sig",SaveLayer1Cells=False,CaloTopoIsolationTool=TrigCaloTopoIsolationTool)
topSequence+=CaloBandwidth(EtInSigma=2.0,CaloNoiseTool=theCaloNoiseTool)
topSequence+=EFexEratioAlgo(name="EFexEratioAlgo",InputClusterName="SCluster")
topSequence+=EFexEratioAlgo(name="EFexEratioAlgoCl",InputClusterName="SClusterCl")
topSequence+=EFexEratioRateAlgo(name="EFexEratioRateAlgo",InputClusterName="SCluster")
topSequence+=EFexEratioRateAlgo(name="EFexEratioRateAlgoCl",InputClusterName="SClusterCl")

topSequence+=EFexEratioAlgo(name="EFexEratioAlgoCl98",InputClusterName="SClusterCl",rEtaThreshold=0.775,rEratioThreshold=0.48)
topSequence+=EFexEratioRateAlgo(name="EFexEratioRateAlgoCl98",InputClusterName="SClusterCl",rEtaThreshold=0.775,rEratioThreshold=0.48)
topSequence+=EFexEratioAlgo(name="EFexEratioAlgoCl95",InputClusterName="SClusterCl",rEtaThreshold=0.825,rEratioThreshold=0.68)
topSequence+=EFexEratioRateAlgo(name="EFexEratioRateAlgoCl95",InputClusterName="SClusterCl",rEtaThreshold=0.825,rEratioThreshold=0.68)

topSequence+=EFexEratioAlgo(name="EFexEratioAlgoCl95f",InputClusterName="SClusterCl",rEtaThreshold=0.825,f3Threshold=0.025,rEratioThreshold=0.68)
topSequence+=EFexEratioRateAlgo(name="EFexEratioRateAlgoCl95f",InputClusterName="SClusterCl",rEtaThreshold=0.825,f3Threshold=0.025,rEratioThreshold=0.68)

import GaudiSvc.GaudiSvcConf as GaudiSvcConf
svcMgr += GaudiSvcConf.StatusCodeSvc()
svcMgr.StatusCodeSvc.AbortOnError=False

