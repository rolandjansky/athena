# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# -------------------------------------------------------------
# L1 configuration
# -------------------------------------------------------------
from TriggerJobOpts.TriggerFlags import TriggerFlags
from AthenaCommon.GlobalFlags import GlobalFlags
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.Constants import *
from AthenaCommon.Logging import logging  # loads logger
from AthenaCommon.Include import include  # to include old style job options
from AthenaCommon.AppMgr import theApp
        
if (not TriggerFlags.fakeLVL1()) and TriggerFlags.doLVL1():

    import TrigT1RPCRecRoiSvc.TrigT1RPCRecRoiConfig
    import TrigT1TGCRecRoiSvc.TrigT1TGCRecRoiConfig
    import TrigT1RPCsteering.TrigT1RPCsteeringConfig
    import TrigT1TGC.TrigT1TGCConfig
    from TrigT1MBTS.TrigT1MBTSConf import LVL1__TrigT1MBTS


from RecExConfig.Configured import Configured 

"""
This LvlTriggerOnlineGetter.Lvl1SimulationGetter is primarily called from
  TriggerRelease/Trigger_topOptions_standalone.py
which is called primarily from
  TriggerRelease/runHLT_standalone.py

In Trigger_topOptions_standalone.py TriggerFlags.doLVL1 is set to False when running on bytestream.
When running on bytestream, the running of LVL1 is enabled through the rerunLVL1 and rerunLVL1Phase-I modifiers

However, these modifiers are executed either before or after the full trigger setup, which makes it difficult to
execute the LVL1 simulation setup at the right place

Therefor, the modifier rerunLVL1PhaseI only sets TriggerFlags.doLVL1PhaseI=True and relies on the work being done
in here.  
"""


class Lvl1SimulationGetter (Configured):
    _output = {"CTP_Decision":"CTP_Decision", "LVL1_ROI":"LVL1_ROI"}

    def configure(self):

        log = logging.getLogger( "Lvl1TriggerOnlineGetter.py" )
        log.info("TriggerFlags")
        log.info("   doLVL1 = %s" % TriggerFlags.doLVL1() )
        log.info("   doLVL1PhaseI = %s" % TriggerFlags.doLVL1PhaseI() )
        log.info("   doL1Topo = %s" % TriggerFlags.doL1Topo() )
        log.info("   fakeLVL1 = %s" % TriggerFlags.fakeLVL1() )

        from AthenaServices.AthenaServicesConf import AthenaOutputStream
        from AthenaCommon.AppMgr import ServiceMgr
        from AthenaCommon.AlgSequence import AlgSequence 
        topSequence = AlgSequence()
        
        # initialize LVL1ConfigSvc
        if not hasattr( ServiceMgr, 'LVL1ConfigSvc' ):
            from TrigConfigSvc.TrigConfigSvcConfig import LVL1ConfigSvc
            LVL1ConfigSvc = LVL1ConfigSvc("LVL1ConfigSvc")
            LVL1ConfigSvc.XMLMenuFile = TriggerFlags.inputLVL1configFile()
            ServiceMgr += LVL1ConfigSvc
        else:
            log.info( "LVL1ConfigSvc already created. Will ignore configuration from xml file="+TriggerFlags.inputLVL1configFile()\
                      +" and use file="+ServiceMgr.LVL1ConfigSvc.XMLFile )

        ## Run 2 L1Calo
        if (not TriggerFlags.fakeLVL1()) and (TriggerFlags.doLVL1() or TriggerFlags.doLVL1PhaseI()):
            if TriggerFlags.doLVL1PhaseI():
                include ("TrigT1CaloByteStream/ReadLVL1CaloBS_jobOptions.py")
                include ("TrigT1CaloSim/TrigT1CaloSimJobOptions_ReadTT.py" )
            else:
                if TriggerFlags.useCaloTTL():
                    include( "TrigT1CaloSim/TrigT1CaloSimJobOptions_TTL1.py")
                else:
                    include( "TrigT1CaloSim/TrigT1CaloSimJobOptions_Cell.py")
                topSequence += LVL1__TrigT1MBTS()

        ## Run 3 L1Calo
        if TriggerFlags.doLVL1PhaseI():
            log.info("setting up the Run 3 L1 calo simulation")
            from TrigT1CaloFexPerf.L1PerfControlFlags import L1Phase1PerfFlags as simflags
            from TrigT1CaloFexSim.L1SimulationControlFlags import L1Phase1SimFlags as simflags_old
            simflags.Calo.SCellType = "Emulated"
            simflags_old.Calo.SCellType = "PreEmulated" # as we have no SuperCells yet
            from TrigT1CaloFexPerf.L1PerfSequence import setupRun3L1CaloPerfSequence
            setupRun3L1CaloPerfSequence(skipCTPEmulation=True, useAlgSequence=False)
            from TrigT1CaloFexSim.L1SimulationSequence import setupRun3L1CaloSimulationSequence
            setupRun3L1CaloSimulationSequence(skipCTPEmulation=True)

        ## MUCTPI
        if (not TriggerFlags.fakeLVL1()) and ( TriggerFlags.doLVL1() or TriggerFlags.doLVL1PhaseI() ):
            if TriggerFlags.doLVL1PhaseI():
                log.info("adding MUCTPI (on RDO) simulation to the topSequence")
                from TrigT1Muctpi.TrigT1MuctpiConfig import L1Muctpi_on_RDO
                topSequence += L1Muctpi_on_RDO()
                topSequence.L1Muctpi_on_RDO.CTPOutputLocID = "/Run/L1MuCTPItoCTPLocation"
                topSequence.L1Muctpi_on_RDO.RoIOutputLocID = "/Run/L1MuCTPItoRoIBLocation"
            else:
                from TrigT1Muctpi.TrigT1MuctpiConfig import L1Muctpi
                topSequence += L1Muctpi()

        ## L1 Topo
        if (not TriggerFlags.fakeLVL1()) and ( TriggerFlags.doLVL1() or TriggerFlags.doLVL1PhaseI() ):
            if TriggerFlags.doL1Topo():
                log.info("adding l1topo simulation to the topSequence")
                from L1TopoSimulation.L1TopoSimulationConfig import L1TopoSimulation
                topSequence += L1TopoSimulation()
                topSequence.L1TopoSimulation.OutputLevel = DEBUG

                try: # this is temporary until TrigT1Muctpi-00-06-29 is in the release
                    from TrigT1Muctpi.TrigT1MuctpiConfig import L1MuctpiTool
                    from AthenaCommon.AppMgr import ToolSvc
                    ToolSvc += L1MuctpiTool()
                    topSequence.L1TopoSimulation.MuonInputProvider.MuctpiSimTool = L1MuctpiTool()
                except ImportError:
                    pass

                # enable the reduced (coarse) granularity topo simulation
                # currently only for MC
                from AthenaCommon.GlobalFlags  import globalflags
                if globalflags.DataSource()!='data':
                    log.info("Muon eta/phi encoding with reduced granularity for MC (L1 Simulation)")
                    topSequence.L1TopoSimulation.MuonInputProvider.MuonEncoding = 1
                else:
                    log.info("Muon eta/phi encoding with full granularity for data (L1 Simulation) - should be faced out")
                    topSequence.L1TopoSimulation.MuonInputProvider.MuonEncoding = 1


        if (not TriggerFlags.fakeLVL1()) and ( TriggerFlags.doLVL1() or TriggerFlags.doLVL1PhaseI() ):
            if TriggerFlags.doLVL1PhaseI(): # doLVL1PhaseI
                log.info("adding Run 3 CTP emulation to the topSequence from with Lvl1TriggerOnlineGetter")
                from TrigT1CTP.TrigT1CTP_EnableCTPEmulation import enableCTPEmulation
                enableCTPEmulation(topSequence)
            else: # doLVL1
                log.info("adding CTP simulation to the topSequence")
                from TrigT1CTP.TrigT1CTPConfig import CTPSimulation
                topSequence += CTPSimulation("CTPSimulation")

        if ((not TriggerFlags.fakeLVL1()) and TriggerFlags.doLVL1()) or TriggerFlags.doLVL1PhaseI():
            log.info("adding ROIB simulation to the topSequence")
            from TrigT1RoIB.TrigT1RoIBConf import ROIB__RoIBuilder
            topSequence += ROIB__RoIBuilder("RoIBuilder")
            topSequence.RoIBuilder.CTPSLinkLocation = '/Event/CTPSLinkLocation_Rerun'

        return True
# end of class  Lvl1SimulationGetter
