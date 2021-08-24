# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Athena import(s):
import AthenaCommon.CfgMgr as CfgMgr

# Import the genConf made configurable:
import AnalysisTriggerAlgs.AnalysisTriggerAlgsConf as Conf

class RoIBResultToxAOD( Conf.RoIBResultToxAOD ):

    def __init__( self, name = "RoIBResultToxAOD" ):
        super( RoIBResultToxAOD, self ).__init__( name )

        # Get a logger:
        from AthenaCommon.Logging import logging
        log = logging.getLogger( 'RoIBResultToxAOD' )

        #
        # Set up the L1Calo tools:
        #
        log.info( "will add L1CPMTools instance to the algorithm" )
        self.L1CPMTools = CfgMgr.LVL1__L1CPMTools( 'L1CPMTools' )

        log.info( "will add L1JEMJetTools instance to the algorithm" )
        self.L1JEMJetTools = CfgMgr.LVL1__L1JEMJetTools( 'L1JEMJetTools' )

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
            rpcRecRoiTool = LVL1__TrigT1RPCRecRoiTool("RPCRecRoiTool", UseRun3Config=ConfigFlags.Trigger.enableL1MuonPhase1)
            self.RecRpcRoiTool = rpcRecRoiTool
            tgcRecRoiTool = LVL1__TrigT1TGCRecRoiTool("TGCRecRoiTool", UseRun3Config=ConfigFlags.Trigger.enableL1MuonPhase1)
            self.RecTgcRoiTool = tgcRecRoiTool
        else:
            self.RecRpcRoiTool=""
            self.RecTgcRoiTool=""

    def setDefaults( self, handle ):
        # switch off reading of Muon/Calo inputs if subsystem is not running
        from AthenaCommon.DetFlags import DetFlags
        handle.DoCalo = DetFlags.detdescr.Calo_on()
        handle.DoMuon = DetFlags.detdescr.Muon_on()
