#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from TriggerMenuMT.HLT.Menu.MenuComponents import RecoFragmentsPool, MenuSequence
from AthenaCommon.CFElements import seqAND, parOR
from TrigEDMConfig.TriggerEDMRun3 import recordable
from .FullScanDefs import caloFSRoI

class CaloMenuDefs(object):
      """Static Class to collect all string manipulations in Calo sequences """
      from TrigEDMConfig.TriggerEDMRun3 import recordable
      L2CaloClusters= recordable("HLT_FastCaloEMClusters")


#
# central or forward fast calo sequence 
#
def fastCaloSequence(flags, name="fastCaloSequence"):
    """ Creates Fast Calo reco sequence"""
    
    from TrigT2CaloCommon.CaloDef import fastCaloEVCreator
    from TrigT2CaloCommon.CaloDef import fastCaloRecoSequence
    (fastCaloViewsMaker, InViewRoIs) = fastCaloEVCreator()
    # reco sequence always build the rings
    (fastCaloInViewSequence, sequenceOut) = fastCaloRecoSequence(InViewRoIs, doRinger=True)
     # connect EVC and reco
    fastCaloSequence = seqAND(name, [fastCaloViewsMaker, fastCaloInViewSequence ])
    return (fastCaloSequence, fastCaloViewsMaker, sequenceOut)



def fastCaloMenuSequence(flags, name, doRinger=True, is_probe_leg=False):
    """ Creates Egamma Fast Calo  MENU sequence
    The Hypo name changes depending on name, so for different implementations (Electron, Gamma,....)
    """
    (sequence, fastCaloViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(fastCaloSequence, flags=flags)
    # check if use Ringer and are electron because there aren't ringer for photons yet:

    # hypo
    if doRinger:
      from TrigEgammaHypo.TrigEgammaFastCaloHypoTool import createTrigEgammaFastCaloHypoAlg
    else:
      from TrigEgammaHypo.TrigEgammaFastCaloHypoTool import createTrigEgammaFastCaloHypoAlg_noringer as createTrigEgammaFastCaloHypoAlg

    theFastCaloHypo = createTrigEgammaFastCaloHypoAlg(name+"EgammaFastCaloHypo", sequenceOut)
    CaloMenuDefs.L2CaloClusters = sequenceOut

    from TrigEgammaHypo.TrigEgammaFastCaloHypoTool import TrigEgammaFastCaloHypoToolFromDict
    return MenuSequence( Sequence    = sequence,
                         Maker       = fastCaloViewsMaker,
                         Hypo        = theFastCaloHypo,
                         HypoToolGen = TrigEgammaFastCaloHypoToolFromDict,
                         IsProbe     = is_probe_leg )



def cellRecoSequence(flags, name="HLTCaloCellMakerFS", RoIs=caloFSRoI, outputName="CaloCellsFS"):
    """ Produce the full scan cell collection """
    if not RoIs:
        from HLTSeeding.HLTSeedingConfig import mapThresholdToL1RoICollection
        RoIs = mapThresholdToL1RoICollection("FSNOSEED")
    from TrigT2CaloCommon.CaloDef import setMinimalCaloSetup
    setMinimalCaloSetup()
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from TrigCaloRec.TrigCaloRecConfig import HLTCaloCellMaker
    alg = HLTCaloCellMaker(name)
    alg.RoIs=RoIs
    alg.TrigDataAccessMT=svcMgr.TrigCaloDataAccessSvc
    alg.CellsName=outputName
    return parOR(name+"RecoSequence", [alg]), str(alg.CellsName)

def caloClusterRecoSequence(
        flags, name="HLTCaloClusterMakerFS", RoIs=caloFSRoI,
        outputName="HLT_TopoCaloClustersFS"):
    """ Create the EM-level fullscan clusters """
    cell_sequence, cells_name = RecoFragmentsPool.retrieve(cellRecoSequence, flags=flags, RoIs=RoIs)
    from TrigCaloRec.TrigCaloRecConfig import TrigCaloClusterMaker_topo
    alg = TrigCaloClusterMaker_topo(
            name,
            doMoments=True,
            doLC=False,
            cells=cells_name)
    alg.CaloClusters = recordable(outputName)
    return parOR(name+"RecoSequence", [cell_sequence, alg]), str(alg.CaloClusters)

def LCCaloClusterRecoSequence(
        flags, name="HLTCaloClusterCalibratorLCFS", RoIs=caloFSRoI,
        outputName="HLT_TopoCaloClustersLCFS"):
    """ Create the LC calibrated fullscan clusters

    The clusters will be created as a shallow copy of the EM level clusters
    """
    em_sequence, em_clusters = RecoFragmentsPool.retrieve(caloClusterRecoSequence, flags=flags, RoIs=RoIs)
    from TrigCaloRec.TrigCaloRecConfig import TrigCaloClusterCalibrator_LC
    alg = TrigCaloClusterCalibrator_LC(
            name,
            InputClusters = em_clusters,
            OutputClusters = outputName,
            OutputCellLinks = outputName+"_cellLinks")
    return parOR(name+"RecoSequence", [em_sequence, alg]), str(alg.OutputClusters)

def caloTowerHIRecoSequence(
        flags, name="HLTHICaloTowerMakerFS", RoIs=caloFSRoI,
        outputName="HLT_HICaloTowerFS"):
    """ Create the EM-level fullscan clusters for heavy-ion"""
    cell_sequence, cells_name = RecoFragmentsPool.retrieve(cellRecoSequence, flags=flags, RoIs=RoIs)
    from TrigCaloRec.TrigCaloRecConfig import TrigCaloTowerMaker_hijet
    alg = TrigCaloTowerMaker_hijet(
            name,
            )
    alg.RoIs=RoIs
    alg.Cells=cells_name
    alg.CaloTowers=outputName
    return parOR(name+"RecoSequence", [cell_sequence, alg]), str(alg.CaloTowers), str(cells_name)
