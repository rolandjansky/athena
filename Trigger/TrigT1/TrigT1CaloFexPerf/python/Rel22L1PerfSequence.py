# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from GaudiKernel import Constants
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.CFElements import seqAND


def setupRun3L1CaloPerfSequence(
    skipCTPEmulation=False, useAlgSequence=True, setupOutput=False, sequence=None
):

    if sequence is None:
        if useAlgSequence:
            sequence = AlgSequence(
                "AthAlgSeq"
            )  # everything is added to the AthAlqSequence (which is then put in front of everything)
        else:
            sequence = AlgSequence()  # everything is added to the topSequence

    from AthenaCommon.Logging import logging

    log = logging.getLogger("TrigT1CaloFexPerf.L1Perf")
    log.setLevel(logging.INFO)

    ## print some information about the conditions the simulation is running in
    from RecExConfig.RecFlags import rec
    from AthenaCommon.GlobalFlags import globalflags

    log.info("Begin setup of L1 Calo Run 3 simulation chain")
    log.info(
        "rec.read.*              : RDO: %s, ESD: %s, AOD: %s",
        rec.readRDO(),
        rec.readESD(),
        rec.readAOD(),
    )
    log.info(
        "rec.do*                 : RDOTrigger: %s, ESD: %s, AOD: %s",
        rec.doRDOTrigger(),
        rec.doESD(),
        rec.doAOD(),
    )
    log.info(
        "rec.doWrite.*           : ESD: %s, AOD: %s, TAG: %s",
        rec.doWriteESD(),
        rec.doWriteAOD(),
        rec.doWriteTAG(),
    )
    log.info("globalflags.DataSource  : %s", globalflags.DataSource())
    from TrigT1CaloFexPerf.L1PerfControlFlags import L1Phase1PerfFlags as simflags

    log.info(simflags._context_name)
    simflags.print_JobProperties("tree&value")

    ## Setup the provider of the SuperCells
    if simflags.Calo.SCellType() == "Pulse":
        # These are fully simulated supercells, from supercell pulse
        # collection is CaloCellContainer#SCell
        SCIn = "SCellnoBCID"
    elif simflags.Calo.SCellType() == "BCID":
        # These are fully simulated supercells with applied BCID corrections
        # This is the only kind of supercells where BCID corrections are applied
        # Pedestal corrections are now applied from HITS->RDO, which is what is needed to work correctly.
        SCIn = "SCell"
    else:
        SCIn = "SCell"  # default

    # Schedule towermaker
    from TrigT1CaloFexPerf.TrigT1CaloFexPerfConf import (
        LVL1__JGTowerBuilder,
        LVL1__JTowerMappingDataCondAlg,
        LVL1__GTowerMappingDataCondAlg,
        LVL1__JGTowerNoiseAlg,
        LVL1__GBlockBuilder,
        LVL1__JTowerRhoSubtractionAlg,
        LVL1__GTowersFromGCaloTowers,
        LVL1__GTowerRhoSubtractionAlg,
        LVL1__METNoiseCutPerfFex,
        LVL1__METJWoJPerfFex,
    )

    from AthenaCommon.AlgSequence import AthSequencer

    condSequence = AthSequencer("AthCondSeq")
    condSequence += LVL1__JTowerMappingDataCondAlg(
        "JTowerMappingDataCondAlg",
        TowerAreasFile = "Run3L1CaloSimulation/Noise/jTowerCorrection.20210308.r12406.root",
        MappingData="JTowerMappingData",
        MapTileCells=simflags.Calo.UseAllCalo(),
    )
    condSequence += LVL1__GTowerMappingDataCondAlg(
        "GTowerMappingDataCondAlg",
        MappingData="GTowerMappingData",
        MapTileCells=simflags.Calo.UseAllCalo(),
    )

    l1simAlgSeq = seqAND("L1SimAlgSequence")
    l1simAlgSeq += LVL1__JGTowerBuilder(
        "JTowerBuilder",
        UseSCQuality=simflags.Calo.ApplySCQual(),
        UseAllCalo=simflags.Calo.UseAllCalo(),
        InputSuperCells=SCIn,
        EmulateSuperCellTiming=False,
        SuperCellQuality=simflags.Calo.QualBitMask(),
        MinSCETp=simflags.Calo.minSCETp(),
        MaxSCETm=simflags.Calo.maxSCETm(),
        MinTowerET=simflags.Calo.minTowerET(),
        MappingData="JTowerMappingData",
        OutputTowers="JTowersPerf",
    )

    l1simAlgSeq += LVL1__JGTowerNoiseAlg(
        "JTowerNoiseAlg",
        NoiseFile = "Run3L1CaloSimulation/Noise/jTowergTowerNoise.20210304.r12406.root",
        InputTowers=l1simAlgSeq.JTowerBuilder.OutputTowers,
        DoJFEX=True,
    )

    l1simAlgSeq += LVL1__JGTowerBuilder(
        "GTowerBuilder",
        UseSCQuality=simflags.Calo.ApplySCQual(),
        UseAllCalo=simflags.Calo.UseAllCalo(),
        InputSuperCells=SCIn,
        EmulateSuperCellTiming=False,
        SuperCellQuality=simflags.Calo.QualBitMask(),
        MinSCETp=simflags.Calo.minSCETp(),
        MaxSCETm=simflags.Calo.maxSCETm(),
        MinTowerET=simflags.Calo.minTowerET(),
        MappingData="GTowerMappingData",
        OutputTowers="GCaloTowersPerf",
    )

    l1simAlgSeq += LVL1__JTowerRhoSubtractionAlg(
        "JTowerRhoSubtractionAlg",
        InputTowers=l1simAlgSeq.JTowerBuilder.OutputTowers,
        OutputTowers="JTowerRhoSubtracted",
        OutputRho="JFEXRhoPerf",
    )

    l1simAlgSeq += LVL1__GTowersFromGCaloTowers(
        "GTowersFromGCaloTowers",
        InputTowers=l1simAlgSeq.GTowerBuilder.OutputTowers,
        OutputTowers="GTowersPerf",
        MappingData="GTowerMappingData",
    )

    l1simAlgSeq += LVL1__JGTowerNoiseAlg(
        "GTowerNoiseAlg",
        NoiseFile = "Run3L1CaloSimulation/Noise/jTowergTowerNoise.20210304.r12406.root",
        InputTowers=l1simAlgSeq.GTowersFromGCaloTowers.OutputTowers,
        DoJFEX=False,
    )

    l1simAlgSeq += LVL1__GTowerRhoSubtractionAlg(
        "GTowerRhoSubtractionAlg",
        InputTowers=l1simAlgSeq.GTowersFromGCaloTowers.OutputTowers,
        OutputTowers="GTowerRhoSubtracted",
        OutputRho="GFEXRhoPerf",
    )

    l1simAlgSeq += LVL1__GBlockBuilder(
        "gBlockBuilder",
        InputTowers=l1simAlgSeq.GTowersFromGCaloTowers.OutputTowers,
        OutputBlocks="GBlocks",
    )

    l1simAlgSeq += LVL1__GBlockBuilder(
        "RhoSubtractedGBlockBuilder",
        InputTowers=l1simAlgSeq.GTowerRhoSubtractionAlg.OutputTowers,
        OutputBlocks="GBlocksRhoSubtractedPerf",
    )

    l1simAlgSeq += LVL1__METNoiseCutPerfFex(
        "jXERHOFex",
        InputTowers=l1simAlgSeq.JTowerRhoSubtractionAlg.OutputTowers,
        OutputMET="jXERHOPerf",
        InputTowerNoise="",
        UseNegativeTowers=True,
    )

    l1simAlgSeq += LVL1__METNoiseCutPerfFex(
        "jNOISECUTFex",
        InputTowers=l1simAlgSeq.JTowerBuilder.OutputTowers,
        OutputMET="jNOISECUTPerf",
        InputTowerNoise="noise",
        UseNegativeTowers=False,
    )

    l1simAlgSeq += LVL1__METNoiseCutPerfFex(
        "gXERHOFex",
        InputTowers=l1simAlgSeq.GTowerRhoSubtractionAlg.OutputTowers,
        OutputMET="gXERHOPerf",
        InputTowerNoise="",
        UseNegativeTowers=True,
    )

    l1simAlgSeq += LVL1__METNoiseCutPerfFex(
        "gXENOISECUTFex",
        InputTowers=l1simAlgSeq.GTowersFromGCaloTowers.OutputTowers,
        OutputMET="gXENOISECUTPerf",
        InputTowerNoise="noise",
        UseNegativeTowers=True,
    )

    l1simAlgSeq += LVL1__METJWoJPerfFex(
        "gXEJWOJFex",
        InputGBlocks=l1simAlgSeq.gBlockBuilder.OutputBlocks,
        OutputMET="gXEJWOJPerf",
        OutputMHT="gXEJWOJPerfMHT",
        OutputMST="gXEJWOJPerfMST",
        UseNegativeTowers=True,
        UseRho=False,
    )

    l1simAlgSeq += LVL1__METJWoJPerfFex(
        "gXEJWOJRHOFex",
        InputGBlocks=l1simAlgSeq.RhoSubtractedGBlockBuilder.OutputBlocks,
        OutputMET="gXEJWOJRHOPerf",
        OutputMHT="gXEJWOJRHOPerfMHT",
        OutputMST="gXEJWOJRHOPerfMST",
        UseNegativeTowers=False,
        UseRho=False,
    )

    l1simAlgSeq += LVL1__METJWoJPerfFex(
        "gXEJWOJRHOHTFex",
        InputGBlocks=l1simAlgSeq.gBlockBuilder.OutputBlocks,
        OutputMET="gXEJWOJRHOHTPerf",
        OutputMHT="gXEJWOJRHOHTPerfMHT",
        OutputMST="gXEJWOJRHOHTPerfMST",
        UseNegativeTowers=True,
        UseRho=True,
        InputRho=l1simAlgSeq.GTowerRhoSubtractionAlg.OutputRho,
    )

   #from TrigT1CaloFexPerf.EFexConfig import enableEfexAlgorithms

   # enableEfexAlgorithms(
   #     l1simAlgSeq,
   #     SuperCellContainer=SCIn,
   # )

    if simflags.EnableDebugOutput() or True:
        log.debug("Algorithm sequence after L1 simulation setup")
        from AthenaCommon.AlgSequence import dumpSequence

        for alg in l1simAlgSeq:
            alg.OutputLevel = Constants.VERBOSE

        dumpSequence(l1simAlgSeq)

    log.info("Finished setup of L1 Calo Run 3 simulation chain")

    sequence += l1simAlgSeq

    if simflags.EnableExtraXAODOutput() or True:

        itemList = []

        # New JTowers and GTowers
        for alg in ["JTowerBuilder", "GTowerBuilder", "GTowersFromGCaloTowers"]:
            tower = getattr(l1simAlgSeq, alg).OutputTowers
            itemList += [
                "xAOD::JGTowerContainer#%s" % tower,
                "xAOD::JGTowerAuxContainer#%sAux.*" % tower,
            ]

        for alg in ["JTowerRhoSubtractionAlg", "GTowerRhoSubtractionAlg"]:
            tower = getattr(l1simAlgSeq, alg).OutputTowers
            itemList += [
                "xAOD::JGTowerContainer#{}".format(tower),
                "xAOD::ShallowAuxContainer#{}Aux.*".format(tower),
            ]

        for alg in ["jXERHOFex", "jNOISECUTFex", "gXERHOFex", "gXENOISECUTFex"]:
            met = getattr(l1simAlgSeq, alg).OutputMET
            itemList += [
                "xAOD::EnergySumRoI#{}".format(met),
                "xAOD::EnergySumRoIAuxInfo#{}Aux.".format(met),
            ]

        for alg_name in "GTowerRhoSubtractionAlg", "JTowerRhoSubtractionAlg":
            alg = getattr(l1simAlgSeq, alg_name)
            itemList += [
                "xAOD::EnergySumRoI#{}".format(alg.OutputRho),
                "xAOD::EnergySumRoIAuxInfo#{}Aux.".format(alg.OutputRho),
            ]

        for alg_name in ["gXEJWOJFex", "gXEJWOJRHOFex", "gXEJWOJRHOHTFex"]:
            alg = getattr(l1simAlgSeq, alg_name)
            for prop in "OutputMET", "OutputMHT", "OutputMST":
                met = getattr(alg, prop)
                itemList += [
                    "xAOD::EnergySumRoI#{}".format(met),
                    "xAOD::EnergySumRoIAuxInfo#{}Aux.".format(met),
                ]

        itemList += [
            "xAOD::GBlockContainer#GBlocks",
            "xAOD::GBlockAuxContainer#GBlocksAux.",
        ]

        if rec.doRDOTrigger():
            from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

            stream = MSMgr.GetStream("StreamAOD")
            from OutputStreamAthenaPool.MultipleStreamManager import AugmentedPoolStream

            if isinstance(stream, AugmentedPoolStream):
                stream.AddItem(itemList)
            else:
                stream.ItemList += itemList
