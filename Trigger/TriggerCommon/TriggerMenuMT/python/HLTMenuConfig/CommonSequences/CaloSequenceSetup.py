#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool, MenuSequence
from AthenaCommon.CFElements import seqAND, parOR
from TrigEDMConfig.TriggerEDMRun3 import recordable

class CaloMenuDefs(object):
      """Static Class to collect all string manipulations in Calo sequences """
      from TrigEDMConfig.TriggerEDMRun3 import recordable
      L2CaloClusters= recordable("HLT_FastCaloEMClusters")




def fastCaloSequence(doRinger):
    """ Creates Fast Calo sequence"""
    # EV creator
    from TrigT2CaloCommon.CaloDef import fastCaloEVCreator
    (fastCaloViewsMaker, InViewRoIs) = fastCaloEVCreator()

    # reco sequence always build the rings
    from TrigT2CaloCommon.CaloDef import fastCaloRecoSequence
    (fastCaloInViewSequence, sequenceOut) = fastCaloRecoSequence(InViewRoIs, doRinger=doRinger)

     # connect EVC and reco
    fastCaloSequence = seqAND("fastCaloSequence", [fastCaloViewsMaker, fastCaloInViewSequence ])
    return (fastCaloSequence, fastCaloViewsMaker, sequenceOut)


def fastCaloMenuSequence(name, doRinger):
    """ Creates Egamma Fast Calo  MENU sequence
    The Hypo name changes depending on name, so for different implementations (Electron, Gamma,....)
    The doRinger flag is to use or not the Ringer hypo
    """
    (sequence, fastCaloViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(fastCaloSequence, {'doRinger' : doRinger})
    # check if use Ringer and are electron because there aren't ringer for photons yet:
    # hypo
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaFastCaloHypoAlgMT
    theFastCaloHypo = TrigEgammaFastCaloHypoAlgMT(name+"EgammaFastCaloHypo")
    theFastCaloHypo.CaloClusters = sequenceOut
    CaloMenuDefs.L2CaloClusters = sequenceOut

    from TrigEgammaHypo.TrigEgammaFastCaloHypoTool import TrigEgammaFastCaloHypoToolFromDict
    return MenuSequence( Sequence    = sequence,
                         Maker       = fastCaloViewsMaker,
                         Hypo        = theFastCaloHypo,
                         HypoToolGen = TrigEgammaFastCaloHypoToolFromDict )


def cellRecoSequence(flags, name="HLTCaloCellMakerFS", RoIs="HLT_FSJETRoI", outputName="CaloCellsFS"):
    """ Produce the full scan cell collection """
    if not RoIs:
        from L1Decoder.L1DecoderConfig import mapThresholdToL1RoICollection
        RoIs = mapThresholdToL1RoICollection("FSNOSEED")
    from TrigT2CaloCommon.CaloDef import setMinimalCaloSetup
    setMinimalCaloSetup()
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from TrigCaloRec.TrigCaloRecConfig import HLTCaloCellMaker
    alg = HLTCaloCellMaker(name)
    alg.RoIs=RoIs
    alg.TrigDataAccessMT=svcMgr.TrigCaloDataAccessSvc
    alg.CellsName=outputName
    return parOR(name+"RecoSequence", [alg]), alg.CellsName

def caloClusterRecoSequence(
        flags, name="HLTCaloClusterMakerFS", RoIs="HLT_FSJETRoI",
        outputName="HLT_TopoCaloClustersFS"):
    """ Create the EM-level fullscan clusters """
    cell_sequence, cells_name = RecoFragmentsPool.retrieve(cellRecoSequence, flags=None, RoIs=RoIs)
    from TrigCaloRec.TrigCaloRecConfig import TrigCaloClusterMakerMT_topo
    alg = TrigCaloClusterMakerMT_topo(
            name,
            doMoments=True,
            doLC=False,
            cells=cells_name)
    alg.CaloClusters = recordable(outputName)
    return parOR(name+"RecoSequence", [cell_sequence, alg]), alg.CaloClusters

def LCCaloClusterRecoSequence(
        flags, name="HLTCaloClusterCalibratorLCFS", RoIs="HLT_FSJETRoI",
        outputName="HLT_TopoCaloClustersLCFS"):
    """ Create the LC calibrated fullscan clusters

    The clusters will be created as a shallow copy of the EM level clusters
    """
    em_sequence, em_clusters = RecoFragmentsPool.retrieve(caloClusterRecoSequence, flags=None, RoIs=RoIs)
    from TrigCaloRec.TrigCaloRecConfig import TrigCaloClusterCalibratorMT_LC
    alg = TrigCaloClusterCalibratorMT_LC(
            name,
            InputClusters = em_clusters,
            OutputClusters = outputName,
            OutputCellLinks = outputName+"_cellLinks")
    return parOR(name+"RecoSequence", [em_sequence, alg]), alg.OutputClusters
