
from AthenaCommon.Logging import logging
log = logging.getLogger( 'TrigT1CaloFexSim.L1Sim' )
log.setLevel(logging.DEBUG)

from RecExConfig.RecFlags import rec
from TrigT1CaloFexSim.L1SimulationControlFlags import L1Phase1SimFlags as simflags

log.debug("dir() = %r" % dir())

simflags.Calo.SCellType = "Emulated"
simflags.CTP.RunCTPEmulation = True

log.info("rec.read.*     : RDO: %s, ESD: %s, AOD: %s" % (rec.readRDO(), rec.readESD(), rec.readAOD()) )
log.info("rec.do*        : RDOTrigger: %s, ESD: %s, AOD: %s" % (rec.doRDOTrigger(), rec.doESD(), rec.doAOD()))
log.info("rec.doWrite.*  : ESD: %s, AOD: %s, TAG: %s" % (rec.doWriteESD(), rec.doWriteAOD(), rec.doWriteTAG()) )
log.info(simflags._context_name)
simflags.print_JobProperties('tree&value')


if rec.readRDO():

    # algorithm shuffling
    # * at this stage (Modifier.post_exec) the HLT is set up and in sequence TopAlg -> move to HLTAlgSeq
    # * CaloCellGetter by default writes to TopAlg -> need to move to AthAlgSeq before everything else is added
    # * the rest of the L1simulation is being added to AthAlgSeq
    
    from AthenaCommon.AlgSequence import AlgSequence
    topSeq = AlgSequence()
    algSeq = CfgMgr.AthSequencer("AthAlgSeq")
    hltSeq = CfgMgr.AthSequencer("HLTSeq")

    for alg in topSeq:
        if alg.name() in ['ALFAROBMonitor', 'l1topoWriteValData', 'MuCTPiROBMonitor']:
            continue
        if alg.name() == 'xAODMaker::EventInfoCnvAlg':
            algSeq += alg # event info converter has to come first
            continue
        hltSeq += alg
    topSeq.removeAll()

    # Setting up the CaloCell creation
    # turn off the digits->raw channel reconstruction
    from LArROD.LArRODFlags import larRODFlags
    from TileRecUtils.TileRecFlags import jobproperties as tileFlags
    from CaloRec.CaloCellFlags import jobproperties as caloCellFlags
    larRODFlags.readDigits=False
    tileFlags.TileRecFlags.readDigits=False
    caloCellFlags.CaloCellFlags.doLArCreateMissingCells = False

    #from LArConditionsCommon.LArCondFlags import larCondFlags
    #larCondFlags.useShape = False

    from CaloRec.CaloCellGetter import CaloCellGetter
    log.info("Calling CaloCellGetter to setup creation of calo cells")
    CaloCellGetter()
    for alg in topSeq:
        algSeq += alg
    topSeq.removeAll()

    include('TrigT1CaloByteStream/ReadLVL1CaloBSRun2_jobOptions.py')

    # Determine input supercell approach
    if simflags.Calo.SCellType() == "Pulse":
        SCIn="SCell"
    elif simflags.Calo.SCellType() == "BCID":
        include("TrigT1CaloFexSim/ApplyBCIDCorrections.py")
        SCIn="SCellBCID"
    elif simflags.Calo.SCellType() == "Emulated":
        algSeq += CfgMgr.LArSCSimpleMaker (
            SCellContainer = "SimpleSCell"
            )
        SCIn="SimpleSCell"



    # Schedule towermaker
    include ( "TrigT1CaloFexSim/EnableJGTowerMaker.py" )
    enableJGTowerMaker( SuperCellContainer = SCIn,
                        ApplySCQual = simflags.Calo.ApplySCQual(), 
                        SCBitMask = simflags.Calo.QualBitMask())


    if simflags.Calo.RunFexAlgorithms():

        # eFEX
        include ("TrigL1CaloUpgrade/EnableEfexAlgorithms.py")
        enableEfexAlgorithms ( SuperCellContainer = SCIn, 
                               useTDR = False,
                               doLArFex = False,
                               ApplySCQual = simflags.Calo.ApplySCQual(), 
                               SCBitMask = simflags.Calo.QualBitMask() )

        # j/gFEX
        include ( "TrigT1CaloFexSim/EnableFexAlgorithms.py" )
        enableJGTowerReader(debug = False, SuperCellType=SCIn) # too much debug output

    # Schedule CTP Simulation
    if simflags.CTP.RunCTPEmulation():
        include ( "TrigT1CTP/TrigT1CTP_EnableCTPEmulation.py" )
        enableCTPEmulation()

    #algSeq.removeAll()

    # now append the HLT to the algorithm sequence
    for alg in hltSeq:
        algSeq += alg
    hltSeq.removeAll()

    if simflags.EnableDebugOutput():
        include( "TrigT1CaloFexSim/enableDebugOutput.py" )

if rec.doRDOTrigger():
    print "ADDING CONTAINERS TO RDOTrigger" 
    include ("TrigT1CaloFexSim/FillAODStream.py")

    # from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    # from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
    # streamRDO = AthenaPoolOutputStream("StreamRDO", athenaCommonFlags.PoolRDOOutput.get_Value(), asAlg=True)
 
    fillStream( StreamRDO,
                SuperCellContainer=SCIn,
                WriteAllCalo=False)

    
