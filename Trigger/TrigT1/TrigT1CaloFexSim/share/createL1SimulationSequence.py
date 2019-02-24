
from AthenaCommon.Logging import logging
log = logging.getLogger( 'TrigT1CaloFexSim.L1Sim' )
log.setLevel(logging.DEBUG)

from RecExConfig.RecFlags import rec
from TrigT1CaloFexSim.L1SimulationControlFlags import L1Phase1SimFlags as simflags


CfgMgr.MessageSvc().setError+=["HepMcParticleLink"];

SCIn="SCell" # Initialize to use default supercells from ESD files

log.debug("dir() = %r" % dir())


log.info("rec.read.*     : RDO: %s, ESD: %s, AOD: %s" % (rec.readRDO(), rec.readESD(), rec.readAOD()) )
log.info("rec.do*        : RDOTrigger: %s, ESD: %s, AOD: %s" % (rec.doRDOTrigger(), rec.doESD(), rec.doAOD()))
log.info("rec.doWrite.*  : ESD: %s, AOD: %s, TAG: %s" % (rec.doWriteESD(), rec.doWriteAOD(), rec.doWriteTAG()) )
log.info(simflags._context_name)
simflags.print_JobProperties('tree&value')


if rec.doAOD():
    #if ( ("StreamAOD" in dir()) or ("StreamAOD_Augmented" in dir()) ):

    algseq = CfgMgr.AthSequencer("AthAlgSeq") 

    # Determine input supercell approach
    if simflags.Calo.SCellType() == "Pulse":
        SCIn="SCell"
    elif simflags.Calo.SCellType() == "BCID":
        include("TrigT1CaloFexSim/ApplyBCIDCorrections.py")
        SCIn="SCellBCID"
    elif simflags.Calo.SCellType() == "Emulated":
        algseq += CfgMgr.LArSCSimpleMaker (
            SCellContainer="SimpleSCell"
            )
        SCIn="SimpleSCell"

    # Schedule towermaker
    include ( "TrigT1CaloFexSim/EnableJGTowerMaker.py" )
    enableJGTowerMaker( SuperCellContainer = SCIn,
                        ApplySCQual = simflags.Calo.ApplySCQual(), 
                        SCBitMask = simflags.Calo.QualBitMask())

    # Schedule fex algorithms
    if simflags.Calo.RunFexAlgorithms():

        # eFEX
        include ("TrigL1CaloUpgrade/EnableEfexAlgorithms.py")
        enableEfexAlgorithms ( SuperCellContainer = SCIn, 
                               useTDR = False,
                               doLArFex = False)
        # j/gFEX
        include ( "TrigT1CaloFexSim/EnableFexAlgorithms.py" )
        enableJGTowerReader(debug = False) # too much debug output

    # Schedule CTP Simulation
    if simflags.CTP.RunCTPEmulation():
        include ( "TrigT1CTP/TrigT1CTP_EnableCTPEmulation.py" )
        enableCTPEmulation()


if rec.doAOD() and rec.doWriteAOD():
    print "ADDING CONTAINERS TO AOD" 
    include ("TrigT1CaloFexSim/FillAODStream.py")
    fillAOD(SuperCellContainer=SCIn,
            WriteAllCalo=False)

    
