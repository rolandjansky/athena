if not 'inputFiles' in dir():
   inputFiles=['myHITS.pool.root']

if not 'outputNtupleFile' in dir():
   outputNtupleFile="calohitD3PD_from_esd.root"

from RecExConfig.RecFlags  import rec
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
from D3PDMakerCoreComps.resolveSGKey import testSGKey

athenaCommonFlags.PoolESDInput.set_Value_and_Lock(inputFiles)
rec.readESD.set_Value_and_Lock(True)
#rec.doESD.set_Value_and_Lock(False)
#rec.readRDO.set_Value_and_Lock(False)
#rec.doWriteESD.set_Value_and_Lock(False)
tuple_name = "calohitD3PD_from_esd.root" 

    

rec.AutoConfiguration=['everything']
athenaCommonFlags.EvtMax.set_Value_and_Lock(-1)

rec.doTrigger.set_Value_and_Lock(False)
rec.doHist.set_Value_and_Lock(False)
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteTAGCOM.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doMonitoring.set_Value_and_Lock(False)
rec.doPerfMon.set_Value_and_Lock(False)
rec.doDetailedPerfMon.set_Value_and_Lock(False)
rec.doSemiDetailedPerfMon.set_Value_and_Lock(False)
rec.readAOD.set_Value_and_Lock(False)
rec.doJiveXML.set_Value_and_Lock(False)
rec.Commissioning.set_Value_and_Lock(False)
rec.doTruth.set_Value_and_Lock(False)
rec.doInDet.set_Value_and_Lock(False)
rec.doMuon.set_Value_and_Lock(False)
rec.doMuonCombined.set_Value_and_Lock(False)
rec.doEgamma.set_Value_and_Lock(False)
rec.doTau.set_Value_and_Lock(False)
rec.doJetMissingETTag.set_Value_and_Lock(False)

rec.doLArg.set_Value_and_Lock(True)
rec.doTile.set_Value_and_Lock(True)
rec.doCalo.set_Value_and_Lock(True)
rec.doDPD.set_Value_and_Lock(True)


# RecExCommon
include ("RecExCommon/RecExCommon_topOptions.py")

# D3PDMaker calo block
from CaloSysD3PDMaker.CaloHitD3PD import CaloHitD3PD
alg = CaloHitD3PD(file = outputNtupleFile, tuplename = 'calohitD3PD')

if "writeCells" not in dir() :
    writeCells = False

if writeCells:
    calocell_sgkey = 'AllCalo'
    if testSGKey ('CaloCellContainer', calocell_sgkey):
        from CaloSysD3PDMaker.CaloInfoD3PDObject import CaloInfoD3PDObject
        alg += CaloInfoD3PDObject (10)
        from CaloD3PDMaker.CaloCellD3PDObject import SelCaloCellD3PDObject
        alg += SelCaloCellD3PDObject (10)
        from CaloSysD3PDMaker.TileDetailsD3PDObject import TileDetailsD3PDObject
        alg += TileDetailsD3PDObject ( 1, sgkey=calocell_sgkey, prefix='tile_', Kinematics_WriteEtaPhi=False)

    calocluster_sgkey = 'CaloCalTopoClusters'
    if testSGKey ('xAOD::CaloClusterContainer', calocluster_sgkey):
        from CaloD3PDMaker.xAODClusterD3PDObject import xAODClusterD3PDObject
        alg += xAODClusterD3PDObject (10)
