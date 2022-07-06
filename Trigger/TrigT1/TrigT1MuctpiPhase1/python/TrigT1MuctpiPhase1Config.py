# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.Logging import logging

#Muon RecRoiTools
from TrigT1MuonRecRoiTool.TrigT1MuonRecRoiToolConfig import getRun3RPCRecRoiTool, getRun3TGCRecRoiTool

# Local (generated) configurable(s):
from TrigT1MuctpiPhase1.TrigT1MuctpiPhase1Conf import LVL1MUCTPIPHASE1__MUCTPI_AthAlg

#####
##New style config
#####

def getTrigThresholdDecisionTool(name="TrigThresholdDecisionTool"):
  tool = CompFactory.getComp("LVL1::TrigThresholdDecisionTool")(name)
  tool.RPCRecRoiTool = getRun3RPCRecRoiTool("RPCRecRoiTool", useRun3Config=True)
  tool.TGCRecRoiTool = getRun3TGCRecRoiTool("TGCRecRoiTool", useRun3Config=True)
  return tool


def MUCTPI_AthToolCfg(flags, name):
  tool = CompFactory.getComp("LVL1MUCTPIPHASE1::MUCTPI_AthTool")(name)
  tool.RPCRecRoiTool = getRun3RPCRecRoiTool("RPCRecRoiTool", useRun3Config=True)
  tool.TGCRecRoiTool = getRun3TGCRecRoiTool("TGCRecRoiTool", useRun3Config=True)
  tool.TrigThresholdDecisionTool = getTrigThresholdDecisionTool("TrigThresholdDecisionTool")
  #tool.MUCTPI_xAODLocation = ["LVL1MuonRoIsBCm2", "LVL1MuonRoIsBCm1", "LVL1MuonRoIs", "LVL1MuonRoIsBCp1", "LVL1MuonRoIsBCp2"]
  # Create a logger:
  logger = logging.getLogger( "MUCTPI_AthTool" )

  # Set properties of the LUT overlap handling:
  tool.OverlapStrategyName = "LUT"
  
  # Decide which LUT to use, based on which run we are simulating:
  tool.LUTXMLFile = "TrigConfMuctpi/overlapRun3_20201214.xml"
  logger.info( "Configuring MuCTPI simulation with configuration file: %s", tool.LUTXMLFile )

  if flags.Trigger.doHLT:
      # Check the RoI EDM containers are registered in HLT outputs
      from TrigEDMConfig.TriggerEDMRun3 import recordable
      for key in tool.MUCTPI_xAODLocation:
        logger.info( "Configuring MuCTPI simulation with configuration outputs: %s", key )
        assert key==recordable(key), f'recordable() check failed for {key}'
  logger.info( "Configuring MuCTPI: post flags.Trigger.doHLT" )
  return tool


def MUCTPI_AthAlgCfg(flags):
  acc = ComponentAccumulator()
  alg = CompFactory.getComp("LVL1MUCTPIPHASE1::MUCTPI_AthAlg")(name="MUCTPI_AthAlg")
  alg.MUCTPI_AthTool = MUCTPI_AthToolCfg(flags, name="MUCTPI_AthTool")
  acc.addEventAlgo(alg)
  return acc


#####
##Old style config
#####


class L1MuctpiPhase1_on_Data( LVL1MUCTPIPHASE1__MUCTPI_AthAlg ):

  """
    Module configuring the MuCTPI simulation to be re-run on data BS file
    (that already has a result from a previous MuCTPI simulation running)
  """

  def __init__( self, name = "L1MuctpiPhase1_on_Data" ):

    LVL1MUCTPIPHASE1__MUCTPI_AthAlg.__init__( self, name )

    logger = logging.getLogger( "L1MuctpiPhase1_on_Data" )
    logger.info( "Configuring MuCTPI" )

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    self.MUCTPI_AthTool = MUCTPI_AthToolCfg(ConfigFlags, "MUCTPI_AthTool")
    self.InputSource = "RDO"
    self.RDOLocID = "MUCTPI_RDO"
    self.RDOOutputLocID = "MUCTPI_RDO+"
    self.RoIOutputLocID = "not_used_1"
    self.CTPOutputLocID = "/Run/L1MuCTPItoCTPLocation"
    self.OverlapStrategyName = "LUT"
    self.LUTXMLFile = "TrigConfMuctpi/overlapRun3_20201214.xml"
    self.IsData=1
    self.FlaggingMode = False
