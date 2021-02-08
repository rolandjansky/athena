from RecExConfig.RecFlags  import rec
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
from D3PDMakerCoreComps.resolveSGKey import testSGKey


athenaCommonFlags.PoolESDInput.set_Value_and_Lock(['ESD.pool.root'])
rec.readESD.set_Value_and_Lock(True)
#rec.doESD.set_Value_and_Lock(False)
#rec.readRDO.set_Value_and_Lock(False)
#rec.doWriteESD.set_Value_and_Lock(False)
tuple_name = "calohitD3PD_from_esd.root" 

    

rec.AutoConfiguration=['everything']
athenaCommonFlags.EvtMax.set_Value_and_Lock(100)

rec.doHist.set_Value_and_Lock(False)
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteTAGCOM.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doMonitoring.set_Value_and_Lock(False)
rec.readAOD.set_Value_and_Lock(False)


# RecExCommon
include ("RecExCommon/RecExCommon_topOptions.py")

# D3PDMaker calo block
from CaloD3PDMaker.CaloHitD3PD import CaloHitD3PD
alg = CaloHitD3PD(file = tuple_name, tuplename = 'calohitD3PD')

if "writeCells" not in dir() :
    writeCells = False

if writeCells:
    calocell_sgkey = 'AllCalo'
    if testSGKey ('CaloCellContainer', calocell_sgkey):
        from CaloD3PDMaker.CaloInfoD3PDObject import CaloInfoD3PDObject
        alg += CaloInfoD3PDObject (10)
        from CaloD3PDMaker.CaloCellD3PDObject import SelCaloCellD3PDObject
        alg += SelCaloCellD3PDObject (10)
        from CaloD3PDMaker.TileDetailsD3PDObject import TileDetailsD3PDObject
        alg += TileDetailsD3PDObject ( 1, sgkey=calocell_sgkey, prefix='tile_', Kinematics_WriteEtaPhi=False)

    calocluster_sgkey = 'CaloCalTopoClusters'
    if testSGKey ('CaloClusterContainer', calocluster_sgkey):
        from CaloD3PDMaker.ClusterD3PDObject import ClusterD3PDObject
        alg += ClusterD3PDObject (10)

    if testSGKey ('xAOD::CaloClusterContainer', calocluster_sgkey):
        from CaloD3PDMaker.xAODClusterD3PDObject import xAODClusterD3PDObject
        alg += xAODClusterD3PDObject (10)
