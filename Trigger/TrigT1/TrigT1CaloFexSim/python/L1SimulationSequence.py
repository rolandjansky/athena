

from AthenaCommon.Include import include  # to include old style job options

def setupRun3L1CaloSimulationSequence(skipCTPEmulation = False, useAlgSequence = False, setupOutput = False):
    setupOutput = True

    outputCollections = list()

    from AthenaCommon.Logging import logging
    log = logging.getLogger( 'TrigT1CaloFexSim.L1Sim' )
    log.setLevel(logging.INFO)


    ## print some information about the conditions the simulation is running in
    from RecExConfig.RecFlags import rec
    from AthenaCommon.GlobalFlags import globalflags, jobproperties
    log.info("Begin setup of L1 Calo Run 3 simulation chain")
    log.info("rec.read.*              : RDO: %s, ESD: %s, AOD: %s" % (rec.readRDO(), rec.readESD(), rec.readAOD()) )
    log.info("rec.do*                 : RDOTrigger: %s, ESD: %s, AOD: %s" % (rec.doRDOTrigger(), rec.doESD(), rec.doAOD()))
    log.info("rec.doWrite.*           : ESD: %s, AOD: %s, TAG: %s" % (rec.doWriteESD(), rec.doWriteAOD(), rec.doWriteTAG()) )
    log.info("globalflags.DataSource  : %s" % globalflags.DataSource())
    from TrigT1CaloFexSim.L1SimulationControlFlags import L1Phase1SimFlags as simflags
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


    ## Setup the histogramming, if it does not exist yet
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    if not hasattr(svcMgr,"THistSvc"):
        import AthenaCommon.CfgMgr as CfgMgr
        svcMgr += CfgMgr.THistSvc()
    (hStream , hFile) = simflags.OutputHistFile().split('#')  # e.g. EXPERT#l1Simulation.root
    if not hStream in [entry.split()[0] for entry in svcMgr.THistSvc.Output]:
        svcMgr.THistSvc.Output += ["%s DATAFILE='%s' OPT='RECREATE'" % tuple(simflags.OutputHistFile().split('#')) ]


    ## Compiling the sequence
    # The L1 simulation algorithms will be added to a special sequence
    # It will be merged later into the AthAlgSequence
    from AthenaCommon.AlgSequence import AthSequencer, AlgSequence
    l1simAlgSeq = AthSequencer("L1Run3SimulationSequence")
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
    if(simflags.Calo.DataNoPedestal()==False and globalflags.DataSource()!='data' ):
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


    ## Setup the provider of the SuperCells
    if simflags.Calo.SCellType() == "Pulse":
        # These are fully simulated supercells, from supercell pulse 
        # collection is CaloCellContainer#SCell
        SCIn="SCellnoBCID"
    elif simflags.Calo.SCellType() == "BCID":
        # These are fully simulated supercells with applied BCID corrections
        # This is the only kind of supercells where BCID corrections are applied
        #from TrigT1CaloFexSim.TrigT1CaloFexSimConfig import createSuperCellBCIDAlg
        #l1simAlgSeq += createSuperCellBCIDAlg(SCellContainerIn="SCell",SCellContainerOut="SCellBCID")
        #Pedestal corrections are now applied from HITS->RDO, which is what is needed to work correctly. 
        SCIn="SCell"
    elif simflags.Calo.SCellType() == "Emulated" and simflags.Calo.DataNoPedestal() == True:
        # If the pedestal correction is broken, then disable the things 
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
        theNoiseTool=CaloNoiseToolDefault()
        svcMgr.ToolSvc += theNoiseTool
        from LArL1Sim.LArL1SimConf import LArSCSimpleMaker
        l1simAlgSeq += LArSCSimpleMaker( SCellContainer="SimpleSCell",CaloNoiseTool=theNoiseTool, addBCID = False) 
        SCIn="SimpleSCell" 
    elif simflags.Calo.SCellType() == "Emulated" and simflags.Calo.DataNoPedestal() == False:
        # Supercells are reconstructed from the ET sum of the constituent calo cells 
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
        theNoiseTool=CaloNoiseToolDefault()
        svcMgr.ToolSvc += theNoiseTool
        from CaloTools.CaloLumiBCIDToolDefault import CaloLumiBCIDToolDefault
        theBCIDTool=CaloLumiBCIDToolDefault()
        svcMgr.ToolSvc += theBCIDTool
        from LArL1Sim.LArL1SimConf import LArSCSimpleMaker
        #Give the supercell maker the BCID tool, so it can undo the BCID correction per cell. Then add noise, and return the container SimpleSCellNoBCID
        #Normally needs ot read SimpleSCellNoBCID
        l1simAlgSeq += LArSCSimpleMaker( SCellContainer="SimpleSCellNoBCID", CaloNoiseTool=theNoiseTool, LumiBCIDTool=theBCIDTool )
        #Now run the BCID correction per supercell. Take the container generated above, then return SimpleSCell
        from TrigT1CaloFexSim.TrigT1CaloFexSimConfig import createSuperCellBCIDEmAlg
        l1simAlgSeq += createSuperCellBCIDEmAlg(SCellContainerIn="SimpleSCellNoBCID",SCellContainerOut="SimpleSCell")
        #Tell the rest of the sequence we want to use SimpleSCell, e.g., for building electrons, towers, jets and triggering on dark matter! 
        SCIn="SimpleSCell"
    else:
        SCIn="SCell" # default

    # Schedule towermaker
    from TrigT1CaloFexSim.TrigT1CaloFexSimConfig import createJGTowerReader, createJGTowerMaker
    l1simAlgSeq += createJGTowerMaker( useSCQuality = simflags.Calo.ApplySCQual(),
                                       useAllCalo = simflags.Calo.UseAllCalo(),
                                       SuperCellType = SCIn,
                                       EmulateSuperCellTiming=False,
                                       SuperCellQuality = simflags.Calo.QualBitMask(), 
                                       minSCETp = simflags.Calo.minSCETp(), 
                                       maxSCETm = simflags.Calo.maxSCETm(),
                                       minTowerET = simflags.Calo.minTowerET()
                                   )


    # Schedule fex algorithms
    if simflags.Calo.RunFexAlgorithms():

        # eFEX
        from TrigL1CaloUpgrade.TrigL1CaloUpgradeConfig import enableEfexAlgorithms
        enableEfexAlgorithms ( l1simAlgSeq,
                               SuperCellContainer = SCIn, 
                               useTDR = False,
                               doLArFex = False,
                               ApplySCQual = simflags.Calo.ApplySCQual(), 
                               SCBitMask = simflags.Calo.QualBitMask() )
        # j/gFEX
        l1simAlgSeq += createJGTowerReader(SuperCellType=SCIn) # too much debug output
        
        #jFEX taus 
        from TrigT1CaloFexSim.TrigT1CaloFexSimConf import JFexEleTau
        l1simAlgSeq += JFexEleTau(RegenerateSeeds = True, NoiseStrategy=0, ApplyNoise = False, CheckMax = True, UseRun2=False, SingleTowerSeed=True) 

        #include L1Topo Simulation
        if simflags.Topo.RunTopoAlgorithms():

            if 'L1TopoSimulation' not in algseq:
                from L1TopoSimulation.L1TopoSimulationConfig import L1TopoSimulation
                include ( "TrigT1CaloFexSim/EnableTopoAlgorithms.py" )
                enableL1TopoSimulation()

    # Schedule CTP Simulation
    if not skipCTPEmulation and simflags.CTP.RunCTPEmulation():
        from TrigT1CTP.TrigT1CTP_EnableCTPEmulation import enableCTPEmulation
        enableCTPEmulation(l1simAlgSeq)

    if useAlgSequence:
        sequence = AthSequencer("AthAlgSeq") # everything is added to the AthAlqSequence (which is then put in front of everything) 
    else:
        sequence = AlgSequence() # everything is added to the topSequence

    for alg in l1simAlgSeq:
        sequence += alg
    l1simAlgSeq.removeAll()



    if simflags.EnableDebugOutput():
        log.debug("Algorithm sequence after L1 simulation setup")
        from AthenaCommon.AlgSequence import dumpSequence
        dumpSequence(sequence)


    ## Configure the output content if requested
    # usually this is done as part of the framework according to TrigEDMConfig/TriggerEDM.py
    if setupOutput:
        if rec.doWriteAOD():
            from TrigT1CaloFexSim.SetupOutput import fillAOD
            stream = fillAOD( SuperCellContainer=SCIn,
                              WriteAllCalo=False, otherOutput = outputCollections)

        if rec.doRDOTrigger():
            from TrigT1CaloFexSim.SetupOutput import fillRDO
            stream = fillRDO( SuperCellContainer=SCIn,
                              WriteAllCalo=False, otherOutput = outputCollections)

        if simflags.EnableDebugOutput():
            from TrigT1CaloFexSim.SetupOutput import printStreamingInfo
            printStreamingInfo()

    log.info("Finished setup of L1 Calo Run 3 simulation chain")
