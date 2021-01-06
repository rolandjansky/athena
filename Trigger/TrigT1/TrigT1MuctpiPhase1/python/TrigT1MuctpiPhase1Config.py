# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


# Local (generated) configurable(s):
from TrigT1MuctpiPhase1.TrigT1MuctpiPhase1Conf import LVL1MUCTPIPHASE1__MUCTPI_AthAlg
from TrigT1MuctpiPhase1.TrigT1MuctpiPhase1Conf import LVL1MUCTPIPHASE1__MUCTPI_AthTool

class DefaultL1MuctpiPhase1( LVL1MUCTPIPHASE1__MUCTPI_AthAlg ):

  """
    Base configurable module for the L1MuctpiPhase1 algorithm configurations
  """

  def __init__( self, name = "MUCTPI_AthAlg" ):

    LVL1MUCTPIPHASE1__MUCTPI_AthAlg.__init__( self, name )


class L1MuctpiPhase1( DefaultL1MuctpiPhase1 ):

  """
    Module configuring the default MuCTPI simulation to be used as
    part of the LVL1 simulation.
  """

  def __init__( self, name = "MUCTPI_AthAlg" ):

    DefaultL1MuctpiPhase1.__init__( self, name )


class L1MuctpiPhase1_on_RDO( DefaultL1MuctpiPhase1 ):

  """
    Module configuring the MuCTPI simulation to be re-run on an RDO file
    (that already has a result from a previous MuCTPI simulation running)
  """

  def __init__( self, name = "L1MuctpiPhase1_on_RDO" ):

    DefaultL1MuctpiPhase1.__init__( self, name )

    self.InputSource = "RDO"
    self.RDOLocID = "MUCTPI_RDO"
    self.RDOOutputLocID = "MUCTPI_RDO+"
    self.RoIOutputLocID = "not_used_1"
    self.CTPOutputLocID = "not_used_2"

class L1MuctpiPhase1_on_Data( DefaultL1MuctpiPhase1 ):

  """
    Module configuring the MuCTPI simulation to be re-run on data BS file
    (that already has a result from a previous MuCTPI simulation running)
  """

  def __init__( self, name = "L1MuctpiPhase1_on_Data" ):

    DefaultL1MuctpiPhase1.__init__( self, name )

    self.InputSource = "RDO"
    self.RDOLocID = "MUCTPI_RDO"
    self.RDOOutputLocID = "MUCTPI_RDO+"
    self.RoIOutputLocID = "not_used_1"
    self.CTPOutputLocID = "/Run/L1MuCTPItoCTPLocation"
    self.OverlapStrategyName = "LUT"
    self.LUTXMLFile = "TrigConfMuctpi/overlapRun3_20201214.xml"
    self.IsData=1
    self.FlaggingMode = False

class L1MuctpiPhase1_on_AOD( DefaultL1MuctpiPhase1 ):

  """
    Module configuring the MuCTPI simulation to be re-run on an AOD file
    (that contains the LVL1_ROI object)
  """

  def __init__( self, name = "L1MuctpiPhase1_on_AOD" ):

    DefaultL1MuctpiPhase1.__init__( self, name )

    self.InputSource = "AOD"
    self.AODLocID = "LVL1_ROI"
    self.RDOOutputLocID = "MUCTPI_RDO+"
    self.RoIOutputLocID = "not_used_1"
    self.CTPOutputLocID = "not_used_2"


# for now just duplicate the configuration for the L1MuctpiPhase1Tool, to be consolidated
# ------------------------------------------------------------------------------------------------------



class DefaultL1MuctpiPhase1Tool( LVL1MUCTPIPHASE1__MUCTPI_AthTool ):

  """
    Base configurable module for the L1MuctpiPhase1Tool algorithm configurations
  """

  def __init__( self, name = "MUCTPI_AthTool" ):

    LVL1MUCTPIPHASE1__MUCTPI_AthTool.__init__( self, name )

    # Create a logger:
    from AthenaCommon.Logging import logging
    logger = logging.getLogger( "MUCTPI_AthTool" )

    # Set properties of the LUT overlap handling:
    self.OverlapStrategyName = "LUT"
    self.DumpLUT = False
    self.FlaggingMode = False
    self.MultiplicityStrategyName = "INCLUSIVE"
    self.GeometryXMLFile = "TrigConfMuctpi/L1MuonGeometry_20200629.xml"

    # Decide which LUT to use, based on which run we are simulating:
    #from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as commonGeoFlags
    #from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags as geoFlags
    self.LUTXMLFile = "TrigConfMuctpi/overlapRun3_20201214.xml"
    self.RunPeriod  = "RUN3"
    logger.info( "Configuring MuCTPI simulation with " + self.RunPeriod +" configuration file:" )
    logger.info( "  "+self.LUTXMLFile )
    logger.info( "  with a RunPeriod=" + self.RunPeriod )


class L1MuctpiPhase1Tool( DefaultL1MuctpiPhase1Tool ):

  """
    Module configuring the default MuCTPI simulation to be used as
    part of the LVL1 simulation.
  """

  def __init__( self, name = "L1MuctpiPhase1Tool" ):

    DefaultL1MuctpiPhase1Tool.__init__( self, name )

    self.InputSource = "DIGITIZATION"

class L1MuctpiPhase1Tool_on_RDO( DefaultL1MuctpiPhase1Tool ):

  """
    Module configuring the MuCTPI simulation to be re-run on an RDO file
    (that already has a result from a previous MuCTPI simulation running)
  """

  def __init__( self, name = "L1MuctpiPhase1Tool_on_RDO" ):

    DefaultL1MuctpiPhase1Tool.__init__( self, name )

    self.InputSource = "RDO"
    self.RDOLocID = "MUCTPI_RDO"
    self.RDOOutputLocID = "MUCTPI_RDO+"
    self.RoIOutputLocID = "not_used_1"
    self.CTPOutputLocID = "not_used_2"

class L1MuctpiPhase1Tool_on_Data( DefaultL1MuctpiPhase1Tool ):

  """
    Module configuring the MuCTPI simulation to be re-run on data BS file
    (that already has a result from a previous MuCTPI simulation running)
  """

  def __init__( self, name = "L1MuctpiPhase1Tool_on_Data" ):

    DefaultL1MuctpiPhase1Tool.__init__( self, name )

    self.InputSource = "RDO"
    self.RDOLocID = "MUCTPI_RDO"
    self.RDOOutputLocID = "MUCTPI_RDO+"
    self.RoIOutputLocID = "not_used_1"
    self.CTPOutputLocID = "/Run/L1MuCTPItoCTPLocation"
    self.OverlapStrategyName = "LUT"
    self.LUTXMLFile = "TrigConfMuctpi/overlapRun3_20201214.xml"
    self.IsData=1
    self.FlaggingMode = False

class L1MuctpiPhase1Tool_on_AOD( DefaultL1MuctpiPhase1Tool ):

  """
    Module configuring the MuCTPI simulation to be re-run on an AOD file
    (that contains the LVL1_ROI object)
  """

  def __init__( self, name = "L1MuctpiPhase1Tool_on_AOD" ):

    DefaultL1MuctpiPhase1Tool.__init__( self, name )

    self.InputSource = "AOD"
    self.AODLocID = "LVL1_ROI"
    self.RDOOutputLocID = "MUCTPI_RDO+"
    self.RoIOutputLocID = "not_used_1"
    self.CTPOutputLocID = "not_used_2"

