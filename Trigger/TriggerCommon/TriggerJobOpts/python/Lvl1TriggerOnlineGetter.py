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
    from TrigT1CTP.TrigT1CTPConfig import CTPSimulation
    from TrigT1RoIB.TrigT1RoIBConf import ROIB__RoIBuilder

    import TrigT1RPCRecRoiSvc.TrigT1RPCRecRoiConfig
    import TrigT1TGCRecRoiSvc.TrigT1TGCRecRoiConfig
    import TrigT1RPCsteering.TrigT1RPCsteeringConfig
    import TrigT1TGC.TrigT1TGCConfig
    from TrigT1Muctpi.TrigT1MuctpiConfig import L1Muctpi
    from TrigT1MBTS.TrigT1MBTSConf import LVL1__TrigT1MBTS


from RecExConfig.Configured import Configured 



class Lvl1SimulationGetter (Configured):
    _output = {"CTP_Decision":"CTP_Decision", "LVL1_ROI":"LVL1_ROI"}

    def configure(self):

        log = logging.getLogger( "Lvl1TriggerOnlineGetter.py" )

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

        if (not TriggerFlags.fakeLVL1()) and TriggerFlags.doLVL1():
            if TriggerFlags.useCaloTTL():
                include( "TrigT1CaloSim/TrigT1CaloSimJobOptions_TTL1.py")
            else:
                include( "TrigT1CaloSim/TrigT1CaloSimJobOptions_Cell.py")
            topSequence += LVL1__TrigT1MBTS()

            topSequence += L1Muctpi()

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

            log.info("adding ctp simulation to the topSequence")
            topSequence += CTPSimulation("CTPSimulation")
            
            log.info("adding ROIB simulation to the topSequence")
            topSequence += ROIB__RoIBuilder("RoIBuilder")

        return True
# end of class  Lvl1SimulationGetter
