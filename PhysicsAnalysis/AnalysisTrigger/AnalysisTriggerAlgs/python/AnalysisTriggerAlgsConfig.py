# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Athena import(s):
import AthenaCommon.CfgMgr as CfgMgr

# Import the genConf made configurable:
from AnalysisTriggerAlgs.AnalysisTriggerAlgsConf import RoIBResultToxAOD as genConfRoIBResultToAOD

class DefaultRoIBResultToAOD( genConfRoIBResultToAOD ):

    def __init__( self, name = "DefaultRoIBResultToAOD" ):
        super( DefaultRoIBResultToAOD, self ).__init__( name )

        # Get a logger:
        from AthenaCommon.Logging import logging
        log = logging.getLogger( 'RoIBResultToxAOD' )

        #
        # Get the handle to the LVL1 config service:
        #
        from AthenaCommon.AppMgr import ServiceMgr
        if hasattr( ServiceMgr, "TrigConfigSvc" ):
            log.info( "Using ServiceMgr.TrigConfigSvc for LVL1 configuration" )
            lvl1ConfigSvc = ServiceMgr.TrigConfigSvc
        elif hasattr( ServiceMgr, "LVL1ConfigSvc" ):
            log.info( "Using ServiceMgr.LVL1ConfigSvc for LVL1 configuration" )
            lvl1ConfigSvc = ServiceMgr.LVL1ConfigSvc
        else:
            log.warning( "Did not find a configured LVL1 configuration service!" )
            log.warning( "Will assume that one called 'TrigConf::TrigConfigSvc/TrigConfigSvc'" )
            log.warning( "will be available at runtime. --> The job could crash later on!" )
            lvl1ConfigSvc = "TrigConf::TrigConfigSvc/TrigConfigSvc" # Hopefully it will be configured correctly
                                                                    # later on in the job configuration...

        # Set the handle for the algorithm:
        self.LVL1ConfigSvc = lvl1ConfigSvc
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        self.UseNewConfig = ConfigFlags.Trigger.readLVL1FromJSON

        #
        # Set up the L1Calo tools:
        #
        log.info( "will add L1CPMTools instance to the algorithm" )
        self.L1CPMTools = CfgMgr.LVL1__L1CPMTools( 'L1CPMTools' )
        self.L1CPMTools.LVL1ConfigSvc = lvl1ConfigSvc
        self.L1CPMTools.UseNewConfig = ConfigFlags.Trigger.readLVL1FromJSON

        log.info( "will add L1JEMJetTools instance to the algorithm" )
        self.L1JEMJetTools = CfgMgr.LVL1__L1JEMJetTools( 'L1JEMJetTools' )
        self.L1JEMJetTools.LVL1ConfigSvc = lvl1ConfigSvc
        self.L1JEMJetTools.UseNewConfig = ConfigFlags.Trigger.readLVL1FromJSON

        #
        # Set up the muon RoI tools:
        #
        from AthenaCommon.DetFlags import DetFlags
        if DetFlags.detdescr.Muon_on():
            #Configure alignemnt and muon detector conditions algs
            from MuonRecExample import MuonAlignConfig  # noqa: F401
            #TGC and RPC RecRoiTools
            from TrigT1MuonRecRoiTool.TrigT1MuonRecRoiToolConf import LVL1__TrigT1RPCRecRoiTool, LVL1__TrigT1TGCRecRoiTool
            from AthenaConfiguration.AllConfigFlags import ConfigFlags
            rpcRecRoiTool = LVL1__TrigT1RPCRecRoiTool("RPCRecRoiTool", UseRun3Config=ConfigFlags.Trigger.enableL1Phase1)
            self.RecRpcRoiTool = rpcRecRoiTool
            tgcRecRoiTool = LVL1__TrigT1TGCRecRoiTool("TGCRecRoiTool", UseRun3Config=ConfigFlags.Trigger.enableL1Phase1)
            self.RecTgcRoiTool = tgcRecRoiTool
        else:
            self.RecRpcRoiTool=""
            self.RecTgcRoiTool=""

    def setDefaults( self, handle ):
        # switch off reading of Muon/Calo inputs if subsystem is not running
        from AthenaCommon.DetFlags import DetFlags
        handle.DoCalo = DetFlags.detdescr.Calo_on()
        handle.DoMuon = DetFlags.detdescr.Muon_on()

class RoIBResultToAOD( DefaultRoIBResultToAOD ):

    def __init__( self, name = "RoIBResultToAOD" ):
        super( RoIBResultToAOD, self ).__init__( name )

    def setDefaults( self, handle ):
        DefaultRoIBResultToAOD.setDefaults( handle )

class RoIBResultToAODLegacy( DefaultRoIBResultToAOD ):

    def __init__(self, name = "RoIBResultToAODLegacy"):
        super( RoIBResultToAODLegacy, self ).__init__( name )

        # configure legacy trigger type building
        self.EGammaItem = ["iEM25I","iEM15I","iMU10+EM15I"]
        self.TauHadItem = ["iHA25I+TM30"]
        self.JetItem    = ["iJT200","i3JT90","i4JT65","iJT60+TM60"]
        self.ESumItem   = ["iHA25I+TM30","iJT60+TM60"]
        self.HighMuItem = ["iMU20"]
        self.LowMuItem  = ["i2MU6","iMU10+EM15I"]

    def setDefaults( self, handle ):
        DefaultRoIBResultToAOD.setDefaults( handle )
