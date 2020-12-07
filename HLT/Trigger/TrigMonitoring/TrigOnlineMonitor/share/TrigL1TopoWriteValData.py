#**************************************************************
#
# TrigL1TopoWriteValData.py
#
#==============================================================

from AthenaCommon.Logging import logging
log = logging.getLogger('TrigL1TopoWriteValData.py')

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


# Add simulation algorithm which provided input to monitoring: 
# new instance with non-default output location and prescale
from L1TopoSimulation.L1TopoSimulationConfig import L1TopoSimulation

# Check topsequence and add L1TopoSimulation only if it has not already been added.

if 'doL1TopoSimMon' not in dir():
    doL1TopoSimMon=True
# default True
log.info ('doL1TopoSimMon=%s' % doL1TopoSimMon)
if doL1TopoSimMon:
    topSequenceAlgNames=[alg.getName() for alg in topSequence.getChildren()]
    steerIndex=topSequenceAlgNames.index('TrigSteer_HLT')

    if 'L1TopoSimulation' not in topSequenceAlgNames:
        from L1TopoSimulation.L1TopoSimulationConfig import L1TopoSimulation
        topSequence.insert(steerIndex,L1TopoSimulation())
        #topSequence += L1TopoSimulation()
        log.info( "adding L1TopoSimulation() to topSequence" )

        from TrigT1Muctpi.TrigT1MuctpiConfig import L1MuctpiTool
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += L1MuctpiTool()
        topSequence.L1TopoSimulation.MuonInputProvider.MuctpiSimTool = L1MuctpiTool()

        # enable the reduced (coarse) granularity topo simulation
        # currently only for MC
        from AthenaCommon.GlobalFlags  import globalflags
        if globalflags.DataSource()!='data':
            log.info("Muon eta/phi encoding with reduced granularity for MC (L1 Simulation)")
            topSequence.L1TopoSimulation.MuonInputProvider.MuonEncoding = 1
        else:
            log.info("Muon eta/phi encoding with full granularity for data (L1 Simulation) - should be faced out")
            topSequence.L1TopoSimulation.MuonInputProvider.MuonEncoding = 1

    else:
        log.info( "not adding L1TopoSimulation() to topSequence as it already exists" )
    topSequenceAlgNames=[alg.getName() for alg in topSequence.getChildren()]
    log.debug("topSequence: %s" % topSequenceAlgNames)
else:
    log.info("did not check and add L1TopoSimulation to topSequence")


# Add an instance of TrigL1TopoWriteValData to the sequence
#from AthenaCommon import CfgMgr
#l1topoWriteValData = CfgMgr.TrigL1TopoWriteValData("l1topoWriteValData")
#topSequence += l1topoWriteValData
#log.debug("added l1topoWriteValData to topSequence")
#l1topoWriteValData.OutputLevel=1

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
#
#
#==============================================================
#
# End of TrigL1TopoWriteValData.py
#
#**************************************************************
