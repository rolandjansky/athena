# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Include import include  # to include old style job options
from GaudiKernel import Constants


def setupRun3L1CaloPerfSequence(skipCTPEmulation = False,  useAlgSequence=True, setupOutput = False):

    outputCollections = list()

    from AthenaCommon.Logging import logging
    log = logging.getLogger( 'TrigT1CaloFexPerf.L1Perf' )
    log.setLevel(logging.INFO)


    ## print some information about the conditions the simulation is running in
    from RecExConfig.RecFlags import rec
    from AthenaCommon.GlobalFlags import globalflags, jobproperties
    log.info("Begin setup of L1 Calo Run 3 simulation chain")
    log.info("rec.read.*              : RDO: %s, ESD: %s, AOD: %s" % (rec.readRDO(), rec.readESD(), rec.readAOD()) )
    log.info("rec.do*                 : RDOTrigger: %s, ESD: %s, AOD: %s" % (rec.doRDOTrigger(), rec.doESD(), rec.doAOD()))
    log.info("rec.doWrite.*           : ESD: %s, AOD: %s, TAG: %s" % (rec.doWriteESD(), rec.doWriteAOD(), rec.doWriteTAG()) )
    log.info("globalflags.DataSource  : %s" % globalflags.DataSource())
    from TrigT1CaloFexPerf.L1PerfControlFlags import L1Phase1PerfFlags as simflags
    log.info(simflags._context_name)
    simflags.print_JobProperties('tree&value')


    ## SuperCell setup
    # consistency check:
    # * on data rerun we need to emulate SuperCells for now
    # * on MC we need to simulate them (in the previous, HITs->DIGI step)
    if globalflags.DataSource()=='data':
        if simflags.Calo.SCellType() != "Emulated":
            log.error("Running on data and SuperCell-type is set to %s. On data SuperCells currently need to be emulated." % simflags.Calo.SCellType())
    else:
        if simflags.Calo.SCellType() == "Emulated":
            log.warning("Running on simulation and SuperCell-type is set to Emulated. Simulation should run from simulated SuperCells")
    # if SuperCells are not emulated, they need to be present in the input data
    from RecExConfig.InputFilePeeker import inputFileSummary
    if simflags.Calo.SCellType() != "Emulated" \
       and not ('CaloCellContainer','SCell') in inputFileSummary['eventdata_items']:
        log.warning("No container 'CaloCellContainer#SCell' available in input data %s. Forcing SuperCell-type to 'Emulated'" % inputFileSummary['file_name'] )
        simflags.Calo.SCellType.set_Value_and_Lock("Emulated")
    # printout
    if simflags.Calo.SCellType() == "Emulated":
        log.info("Running with emulated SuperCells")
    else:
        log.info("Running with simulated SuperCells")

    ## Compiling the sequence
    # The L1 simulation algorithms will be added to a special sequence
    # It will be merged later into the AthAlgSequence
    from AthenaCommon.AlgSequence import AthSequencer, AlgSequence
    l1simAlgSeq = AthSequencer("L1Run3PerfSequence")
    topSequence = AlgSequence()


    ## scheduling EventInfo unless it exists already
    from RecExConfig.ObjKeyStore import objKeyStore
    if not objKeyStore.isInInput( "xAOD::EventInfo" ):
        if not hasattr( topSequence, "xAODMaker::EventInfoCnvAlg" ):
            from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
            topSequence += xAODMaker__EventInfoCnvAlg()
    else:
        if not hasattr( topSequence, "xAODMaker::EventInfoNonConstCnvAlg" ):
            topSequence += CfgMgr.xAODMaker__EventInfoNonConstCnvAlg()

    ## when running on data the ROBDataProviderSvc needs to be setup 
    if  jobproperties.Global.InputFormat() == 'bytestream':
        include('TrigT1CaloByteStream/ReadLVL1CaloBSRun2_jobOptions.py')

    #python options for supercells
    if(globalflags.DataSource()!='data' ):
        include('LArROD/LArConfigureCablingSCFolder.py')

    ## CaloCells need to be created from LAr and Tile data when running on raw data
    ## (when running on ESD, CaloCells should be taken from the file)
    # CaloCellGetter schedules the offline CaloCellMaker
    # CaloCellMaker produces CaloCellContainer#AllCalo
    # only needed by the LarSCSimpleMaker, when emulating the SuperCells
    doCaloCellCreation = (rec.readRDO() and simflags.Calo.SCellType() == "Emulated")
    if doCaloCellCreation:
        # Setting up the CaloCell creation
        # turn off the digits->raw channel reconstruction
        from LArROD.LArRODFlags import larRODFlags
        from TileRecUtils.TileRecFlags import jobproperties as tileFlags
        from CaloRec.CaloCellFlags import jobproperties as caloCellFlags
        larRODFlags.readDigits = False
        tileFlags.TileRecFlags.readDigits = False
        caloCellFlags.CaloCellFlags.doLArCreateMissingCells = False

        log.info("Calling CaloCellGetter to setup creation of calo cells")
        # this is a bit ugly but it will make sure the CaloCellGetter gets into the L1Run3SimulationSequence
        topSequenceSnapshot = [c for c in topSequence]
        from CaloRec.CaloCellGetter import CaloCellGetter
        CaloCellGetter()
        for alg in topSequence:
            if alg not in topSequenceSnapshot:
                l1simAlgSeq += alg
        topSequence.removeAll()
        for alg in topSequenceSnapshot:
            topSequence += alg 

    log.info("Super cell type : %s", simflags.Calo.SCellType())
        
    ## Setup the provider of the SuperCells
    if simflags.Calo.SCellType() == "Pulse":
        # These are fully simulated supercells, from supercell pulse 
        # collection is CaloCellContainer#SCell
        SCIn="SCellnoBCID"
    elif simflags.Calo.SCellType() == "BCID":
        # These are fully simulated supercells with applied BCID corrections
        # This is the only kind of supercells where BCID corrections are applied
        #Pedestal corrections are now applied from HITS->RDO, which is what is needed to work correctly. 
        SCIn="SCell"
    elif simflags.Calo.SCellType() == "Emulated":
        # Supercells are reconstructed from the ET sum of the constituent calo cells 
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
        theNoiseTool=CaloNoiseToolDefault()
        svcMgr.ToolSvc += theNoiseTool
        from CaloTools.CaloLumiBCIDToolDefault import CaloLumiBCIDToolDefault
        theBCIDTool=CaloLumiBCIDToolDefault()
        svcMgr.ToolSvc += theBCIDTool
        #from TrigT1CaloFexPerf.TrigT1CaloFexPerfConf SCEmulation
        import AthenaCommon.CfgMgr as CfgMgr
        l1simAlgSeq +=  CfgMgr.LVL1__SCEmulation(OutputSuperCells = "SimpleSCellNoBCID", CaloNoiseTool=theNoiseTool, LumiBCIDTool=theBCIDTool)
        SCIn="SimpleSCellNoBCID"
        if(simflags.Calo.ApplyEmulatedPedestal() == True):
            from TrigL1CaloUpgrade.TrigL1CaloUpgradeConf import SuperCellBCIDEmAlg
            l1simAlgSeq += CfgMgr.SuperCellBCIDEmAlg(SCellContainerIn="SimpleSCellNoBCID",SCellContainerOut="SimpleSCell")
        SCIn="SimpleSCell"
    else:
        SCIn="SCell" # default

    # Schedule towermaker
    from TrigT1CaloFexPerf.TrigT1CaloFexPerfConf import (
        LVL1__JGTowerBuilder, LVL1__JTowerMappingMaker, LVL1__GTowerMappingMaker, LVL1__JGTowerNoiseAlg, LVL1__GBlockBuilder,
        LVL1__JTowerRhoSubtractionAlg, LVL1__GTowersFromGCaloTowers, LVL1__GTowerRhoSubtractionAlg, LVL1__METNoiseCutPerfFex, LVL1__METJWoJPerfFex)
    from AthenaCommon.AppMgr import ToolSvc
    global ToolSvc
    ToolSvc += LVL1__JTowerMappingMaker(
        "JTowerMappingMaker",
        TowerAreasFile = "Run3L1CaloSimulation/Noise/jTowerCorrection.20210308.r12406.root",
        MapTileCells = simflags.Calo.UseAllCalo())
    ToolSvc += LVL1__GTowerMappingMaker(
        "GTowerMappingMaker",
        MapTileCells = simflags.Calo.UseAllCalo())
    
    l1simAlgSeq += LVL1__JGTowerBuilder("JTowerBuilder",
        UseSCQuality = simflags.Calo.ApplySCQual(),
        UseAllCalo = simflags.Calo.UseAllCalo(),
        InputSuperCells = SCIn,
        EmulateSuperCellTiming = False,
        SuperCellQuality = simflags.Calo.QualBitMask(),
        MinSCETp = simflags.Calo.minSCETp(),
        MaxSCETm = simflags.Calo.maxSCETm(),
        MinTowerET = simflags.Calo.minTowerET(),
        MappingMaker = ToolSvc.JTowerMappingMaker,
        OutputTowers = "JTowersPerf",
    )

    l1simAlgSeq += LVL1__JGTowerNoiseAlg(
        "JTowerNoiseAlg",
        NoiseFile = "Run3L1CaloSimulation/Noise/jTowergTowerNoise.20210304.r12406.root", 
        InputTowers = l1simAlgSeq.JTowerBuilder.OutputTowers,
        DoJFEX = True,
    )

    l1simAlgSeq += LVL1__JGTowerBuilder("GTowerBuilder",
        UseSCQuality = simflags.Calo.ApplySCQual(),
        UseAllCalo = simflags.Calo.UseAllCalo(),
        InputSuperCells = SCIn,
        EmulateSuperCellTiming = False,
        SuperCellQuality = simflags.Calo.QualBitMask(),
        MinSCETp = simflags.Calo.minSCETp(),
        MaxSCETm = simflags.Calo.maxSCETm(),
        MinTowerET = simflags.Calo.minTowerET(),
        MappingMaker = ToolSvc.GTowerMappingMaker,
        OutputTowers = "GCaloTowersPerf",
    )

    l1simAlgSeq += LVL1__JTowerRhoSubtractionAlg(
        "JTowerRhoSubtractionAlg",
        InputTowers = l1simAlgSeq.JTowerBuilder.OutputTowers,
        OutputTowers = "JTowersRhoSubtractedPerf",
        MappingMaker = ToolSvc.JTowerMappingMaker,
        OutputRho = "JFEXRhoPerf",
    )

    l1simAlgSeq += LVL1__GTowersFromGCaloTowers(
        "GTowersFromGCaloTowers",
        InputTowers = l1simAlgSeq.GTowerBuilder.OutputTowers,
        OutputTowers = "GTowersPerf",
        MappingMaker = ToolSvc.GTowerMappingMaker,
    )

    l1simAlgSeq += LVL1__JGTowerNoiseAlg(
        "GTowerNoiseAlg",
        NoiseFile = "Run3L1CaloSimulation/Noise/jTowergTowerNoise.20210304.r12406.root",
        InputTowers = l1simAlgSeq.GTowersFromGCaloTowers.OutputTowers,
        DoJFEX = False,
    )

    l1simAlgSeq += LVL1__GTowerRhoSubtractionAlg(
        "GTowerRhoSubtractionAlg",
        InputTowers = l1simAlgSeq.GTowersFromGCaloTowers.OutputTowers,
        OutputTowers = "GTowerRhoSubtracted",
        OutputRho = "GFEXRhoPerf",
    )

    l1simAlgSeq += LVL1__GBlockBuilder(
        "gBlockBuilder",
        InputTowers = l1simAlgSeq.GTowersFromGCaloTowers.OutputTowers,
        OutputBlocks = "GBlocksPerf"
    )

    l1simAlgSeq += LVL1__GBlockBuilder(
        "RhoSubtractedGBlockBuilder",
        InputTowers = l1simAlgSeq.GTowerRhoSubtractionAlg.OutputTowers,
        OutputBlocks = "GBlocksRhoSubtractedPerf"
    )

    l1simAlgSeq += LVL1__METNoiseCutPerfFex(
        "jXERHOFex",
        InputTowers = l1simAlgSeq.JTowerRhoSubtractionAlg.OutputTowers,
        OutputMET = "jXERHOPerf",
        InputTowerNoise = "",
        UseNegativeTowers = True,
    )

    l1simAlgSeq += LVL1__METNoiseCutPerfFex(
        "jNOISECUTFex",
        InputTowers = l1simAlgSeq.JTowerBuilder.OutputTowers,
        OutputMET = "jXENOISECUTPerf",
        InputTowerNoise = "noise",
        UseNegativeTowers = False,
    )

    l1simAlgSeq += LVL1__METNoiseCutPerfFex(
        "gXERHOFex",
        InputTowers = l1simAlgSeq.GTowerRhoSubtractionAlg.OutputTowers,
        OutputMET = "gXERHOPerf",
        InputTowerNoise = "",
        UseNegativeTowers = True,
    )

    l1simAlgSeq += LVL1__METNoiseCutPerfFex(
        "gXENOISECUTFex",
        InputTowers = l1simAlgSeq.GTowersFromGCaloTowers.OutputTowers,
        OutputMET = "gXENOISECUTPerf",
        InputTowerNoise = "noise",
        UseNegativeTowers = True,
    )

    l1simAlgSeq += LVL1__METJWoJPerfFex(
        "gXEJWOJFex",
        InputGBlocks = l1simAlgSeq.gBlockBuilder.OutputBlocks,
        OutputMET = "gXEJWOJPerf",
        OutputMHT = "gXEJWOJPerfMHT",
        OutputMST = "gXEJWOJPerfMST",
        UseNegativeTowers = True,
        UseRho = False
    )

    l1simAlgSeq += LVL1__METJWoJPerfFex(
        "gXEJWOJRHOFex",
        InputGBlocks = l1simAlgSeq.RhoSubtractedGBlockBuilder.OutputBlocks,
        OutputMET = "gXEJWOJRHOPerf",
        OutputMHT = "gXEJWOJRHOPerfMHT",
        OutputMST = "gXEJWOJRHOPerfMST",
        UseNegativeTowers = False,
        UseRho = False,
    )

    l1simAlgSeq += LVL1__METJWoJPerfFex(
        "gXEJWOJRHOHTFex",
        InputGBlocks = l1simAlgSeq.gBlockBuilder.OutputBlocks,
        OutputMET = "gXEJWOJRHOHTPerf",
        OutputMHT = "gXEJWOJRHOHTPerfMHT",
        OutputMST = "gXEJWOJRHOHTPerfMST",
        UseNegativeTowers = True,
        UseRho = True,
        InputRho = l1simAlgSeq.GTowerRhoSubtractionAlg.OutputRho,
    )

    from TrigT1CaloFexPerf.EFexConfig import enableEfexAlgorithms
    enableEfexAlgorithms(l1simAlgSeq, 
                         SuperCellContainer=SCIn, 
                         )
    

    if simflags.EnableDebugOutput():
        log.debug("Algorithm sequence after L1 simulation setup")
        from AthenaCommon.AlgSequence import dumpSequence
        dumpSequence(sequence)

    log.info("Finished setup of L1 Calo Run 3 simulation chain")
    
    if useAlgSequence:
        sequence = AthSequencer("AthAlgSeq") # everything is added to the AthAlqSequence (which is then put in front of everything) 
    else:
        sequence = AlgSequence() # everything is added to the topSequence

    for alg in l1simAlgSeq:
        sequence += alg
    l1simAlgSeq.removeAll()

    itemList = []

    # New JTowers and GTowers
    for alg in ["JTowerBuilder", "GTowerBuilder", "GTowersFromGCaloTowers"]:
        tower = getattr(l1simAlgSeq, alg).OutputTowers
        itemList += [ "xAOD::JGTowerContainer#%s" % tower, 
                    "xAOD::JGTowerAuxContainer#%sAux.*" % tower ]

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
            "xAOD::EnergySumRoIAuxInfo#{}Aux.".format(met)
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

    itemList += ["xAOD::GBlockContainer#GBlocks", "xAOD::GBlockAuxContainer#GBlocksAux."]

    if rec.doRDOTrigger():
        from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
        stream = MSMgr.GetStream( "StreamRDO" )
        from OutputStreamAthenaPool.MultipleStreamManager import AugmentedPoolStream
        if isinstance(stream,AugmentedPoolStream):
            stream.AddItem(itemList)
        else:
            stream.ItemList += itemList

