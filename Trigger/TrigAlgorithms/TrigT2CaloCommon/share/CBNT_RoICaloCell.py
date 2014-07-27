
svcMgr.THistSvc.Output = ["AANT DATAFILE='ntuple2.root' TYPE='ROOT' OPT='RECREATE'"] 

NtupleName="ntuple1.root"
from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
aa=AANTupleStream()
aa.ExtraRefNames = [ "StreamESD","StreamRDO" ]
aa.OutputName = NtupleName
aa.ExistDataHeader = False
aa.WriteInputDataHeader = True
topSequence+=aa

from CBNT_Athena.CBNT_AthenaAwareCfg import CBNT_AthenaAware
CBNT_Athena = CBNT_AthenaAware()
topSequence+=CBNT_Athena
include( "CBNT_Athena/CBNT_EventInfo_jobOptions.py" )

from CaloIdentifier import SUBCALO
from CaloRec.CaloRecConf import CBNTAA_CaloCell

theApp.Dlls += [ "CaloRec" ]
#CBNT_Athena.Members  += [ "CBNT_CaloCell/CBNT_RoILArCaloCell" ]     
CBNT_RoILArCaloCell = CBNTAA_CaloCell( "CBNT_RoILArCaloCell" )
CBNT_Athena+=CBNT_RoILArCaloCell
CBNT_RoILArCaloCell.CaloNums=[SUBCALO.LAREM, SUBCALO.LARHEC, SUBCALO.LARFCAL]
#CBNT_RoILArCaloCell.NtupleLocID="/FILE1/CALO/168"; 
CBNT_RoILArCaloCell.CellsName="HLT_TrigT2CaloEgammaCells"
CBNT_RoILArCaloCell.MaxNCells=10000
CBNT_RoILArCaloCell.Suffix="LRoI" 
CBNT_RoILArCaloCell.SaveDetInfo=True
CBNT_RoILArCaloCell.SaveTimeInfo=True
CBNT_RoILArCaloCell.OutputLevel=DEBUG

#CBNT_Athena.Members  += [ "CBNT_CaloCell/CBNT_RoITileCaloCell" ]     
CBNT_RoITileCaloCell = CBNTAA_CaloCell( "CBNT_RoITileCaloCell" )
CBNT_Athena+=CBNT_RoITileCaloCell
CBNT_RoITileCaloCell.CaloNums=[SUBCALO.TILE]
#CBNT_RoITileCaloCell.NtupleLocID="/FILE1/CALO/169"; 
CBNT_RoITileCaloCell.CellsName="HLT_TrigT2CaloEgammaCells"
CBNT_RoITileCaloCell.MaxNCells=2000
CBNT_RoITileCaloCell.Suffix="TRoI" 
CBNT_RoITileCaloCell.SaveDetInfo=True
CBNT_RoITileCaloCell.SaveTimeInfo=True
CBNT_RoITileCaloCell.OutputLevel=VERBOSE

from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_eGamma
T2CaloEgamma_eGamma().StoreCells=True
T2CaloEgamma_eGamma().OutputLevel=0
svcMgr.StoreGateSvc.Dump=True
for item in T2CaloEgamma_eGamma().IAlgToolList:
     item.SaveCellsInContainer=True
     item.ThresholdKeepCells=0.0

