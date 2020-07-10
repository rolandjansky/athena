#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool, MenuSequence
from AthenaCommon.CFElements import seqAND, parOR
from TrigEDMConfig.TriggerEDMRun3 import recordable


class CaloMenuDefs(object):
      """Static Class to collect all string manipulations in Calo sequences """
      from TrigEDMConfig.TriggerEDMRun3 import recordable
      L2CaloClusters= recordable("HLT_L2CaloEMClusters")




def fastCaloSequence(doRinger):
    """ Creates L2 Fast Calo sequence"""
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
    """ Creates L2 Fast Calo  MENU sequence
    The Hypo name changes depending on name, so for different implementations (Electron, Gamma,....)
    The doRinger flag is to use or not the Ringer hypo
    """
    (sequence, fastCaloViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(fastCaloSequence, {'doRinger' : doRinger})
    # check if use Ringer and are electron because there aren't ringer for photons yet:
    # hypo
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoAlgMT
    theFastCaloHypo = TrigL2CaloHypoAlgMT(name+"L2CaloHypo")
    theFastCaloHypo.CaloClusters = sequenceOut
    CaloMenuDefs.L2CaloClusters = sequenceOut

    from TrigEgammaHypo.TrigL2CaloHypoTool import TrigL2CaloHypoToolFromDict
    return MenuSequence( Sequence    = sequence,
                         Maker       = fastCaloViewsMaker,
                         Hypo        = theFastCaloHypo,
                         HypoToolGen = TrigL2CaloHypoToolFromDict )

def cellRecoSequence(flags, name="HLTCaloCellMakerFS", RoIs="FSJETRoI", outputName="CaloCellsFS"):
    """ Produce the full scan cell collection """
    if not inputEDM:
        from L1Decoder.L1DecoderConfig import mapThresholdToL1RoICollection
        inputEDM = mapThresholdToL1RoICollection("FSNOSEED")
    from TrigT2CaloCommon.CaloDef import setMinimalCaloSetup
    setMinimalCaloSetup()
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from TrigCaloRec.TrigCaloRecConfig import HLTCaloCellMaker
    alg = HLTCaloCellMaker(
            name,
            RoIs=inputEDM,
            TrigDataAccessMT=svcMgr.TrigCaloDataAccessSvc,
            CellsName=outputName)
    return parOR(name+"RecoSequence", [alg]), alg.CellsName

def caloClusterRecoSequence(
        flags, name="HLTCaloClusterMakerFS", RoIs="FSJETRoI",
        outputName="HLT_TopoCaloClustersFS"):
    """ Create the EM-level fullscan clusters """
    cell_sequence, cells_name = RecoFragmentsPool.retrieve(cellRecoSequence, flags=None, RoIs=RoIs)
    from TrigCaloRec.TrigCaloRecConfig import TrigCaloClusterMakerMT_topo
    alg = TrigCaloClusterMakerMT_topo(
            name,
            doMoments=True,
            doLC=False,
            cells=cells_name,
            CaloClusters = recordable(outputName))
    return parOR(name+"RecoSequence", [cell_sequence, alg]), alg.CaloClusters

def LCCaloClusterRecoSequence(
        flags, name="HLTCaloClusterCalibratorLCFS", RoIs="FSJETRoI",
        outputName="HLT_TopoCaloClustersLCFS"):
    """ Create the LC calibrated fullscan clusters

    The clusters will be created as a shallow copy of the EM level clusters
    """
    em_sequence, em_clusters = caloClusterRecoSequence(flags=None, RoIs=RoIs)
    from TrigCaloRec.TrigCaloRecConfig import TrigCaloClusterCalibratorMT_LC
    alg = TrigCaloClusterCalibratorMT_LC(
            name,
            InputClusters = em_clusters,
            OutputClusters = recordable(outputName))
    return parOR(name+"RecoSequence", [em_sequence, alg]), alg.OutputClusters

