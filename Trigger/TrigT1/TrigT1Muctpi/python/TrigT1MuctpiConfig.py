# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


# Local (generated) configurable(s):
from TrigT1Muctpi.TrigT1MuctpiConf import LVL1MUCTPI__L1Muctpi
from TrigT1Muctpi.TrigT1MuctpiConf import LVL1MUCTPI__L1MuctpiTool

class DefaultL1Muctpi( LVL1MUCTPI__L1Muctpi ):

  """
    Base configurable module for the L1Muctpi algorithm configurations
  """

  def __init__( self, name = "L1Muctpi" ):

    LVL1MUCTPI__L1Muctpi.__init__( self, name )

    # Create a logger:
    from AthenaCommon.Logging import logging
    logger = logging.getLogger( "L1Muctpi" )

    # Set properties of the LUT overlap handling:
    self.OverlapStrategyName = "LUT"
    self.DumpLUT = False
    self.LUTXMLFile = "UNDEFINED"
    self.RunPeriod = "UNDEFINED"
    self.FlaggingMode = False
    self.MultiplicityStrategyName = "INCLUSIVE"

    # Decide which LUT to use, based on which run we are simulating:
    from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as commonGeoFlags
    from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags as geoFlags
    if ( commonGeoFlags.Run() == "RUN1" ) or ( ( commonGeoFlags.Run() == "UNDEFINED" ) and
                                               ( geoFlags.isIBL() is False ) ):
      self.LUTXMLFile = "TrigConfMuctpi/data10_7TeV.periodI.physics_Muons.MuCTPI_LUT.NoBEOverlaps_composedEF.v002.xml"
      self.RunPeriod = "RUN1"
      logger.info( "Configuring MuCTPI simulation with Run 1 configuration file:" )
      logger.info( "  TrigConfMuctpi/data10_7TeV.periodI.physics_Muons.MuCTPI_LUT.NoBEOverlaps_composedEF.v002.xml" )
      logger.info( "  with a RunPeriod=RUN1" )
    elif ( commonGeoFlags.Run() == "RUN2" ) or ( ( commonGeoFlags.Run() == "UNDEFINED" ) and
                                                 ( geoFlags.isIBL() is True ) ):
      self.LUTXMLFile = "TrigConfMuctpi/data10_7TeV.periodI.physics_Muons.MuCTPI_LUT.NoBEOverlaps_composedEF.v002_modifiedBB.xml"
      self.RunPeriod = "RUN2"
      logger.info( "Configuring MuCTPI simulation with Run 2 configuration file:" )
      logger.info( "  TrigConfMuctpi/data10_7TeV.periodI.physics_Muons.MuCTPI_LUT.NoBEOverlaps_composedEF.v002_modifiedBB.xml" )
      logger.info( "  with a RunPeriod=RUN2" )
    else:
      self.LUTXMLFile = "TrigConfMuctpi/data10_7TeV.periodI.physics_Muons.MuCTPI_LUT.NoBEOverlaps_composedEF.v002_modifiedBB.xml"
      self.RunPeriod = "RUN2"
      logger.warning( "Couldn't determine which run to simulate, using Run 2 configuration file:" )
      logger.warning( "  TrigConfMuctpi/data10_7TeV.periodI.physics_Muons.MuCTPI_LUT.NoBEOverlaps_composedEF.v002_modifiedBB.xml" )
      logger.warning( "  with a RunPeriod=RUN2" )

      pass

    # Set properties for NIM outputs ot L1Topo conversion from RoI to eta/phi
    self.GeometryXMLFile = "TrigConfMuctpi/TestMioctGeometry_2016_05_30_CS_2600573263.xml"

    # Turn on the NIM output creation by default:
    self.DoNIMOutput = True
    # The bit settings were extracted from here:
    #   https://savannah.cern.ch/bugs/?90300#comment14
    self.NIMBarrelBit = 29
    self.NIMEndcapBit = 30

    ### this should not be done at all, since it instantiates a config svc without proper configuration!!
    ### instead rely on the TriggerConfigGetter being setup in the top job properties
    # Ensure that the LVL1ConfigSvc is loaded:
    #from AthenaCommon.AppMgr import ServiceMgr # Has to be imported here for some reason...
    #if not hasattr( ServiceMgr, "LVL1ConfigSvc" ):  
    #  from TrigConfigSvc.TrigConfigSvcConfig import LVL1ConfigSvc
    #  lvl1ConfigSvc = LVL1ConfigSvc("LVL1ConfigSvc")
#   #   lvl1ConfigSvc.XMLFile = "LVL1config13.0.20.xml"
    #  ServiceMgr += lvl1ConfigSvc
    #  self.LVL1ConfigSvc = lvl1ConfigSvc

class L1Muctpi( DefaultL1Muctpi ):

  """
    Module configuring the default MuCTPI simulation to be used as
    part of the LVL1 simulation.
  """

  def __init__( self, name = "L1Muctpi" ):

    DefaultL1Muctpi.__init__( self, name )

    self.InputSource = "DIGITIZATION"

class L1Muctpi_on_RDO( DefaultL1Muctpi ):

  """
    Module configuring the MuCTPI simulation to be re-run on an RDO file
    (that already has a result from a previous MuCTPI simulation running)
  """

  def __init__( self, name = "L1Muctpi_on_RDO" ):

    DefaultL1Muctpi.__init__( self, name )

    self.InputSource = "RDO"
    self.RDOLocID = "MUCTPI_RDO"
    self.RDOOutputLocID = "MUCTPI_RDO+"
    self.RoIOutputLocID = "not_used_1"
    self.CTPOutputLocID = "not_used_2"

class L1Muctpi_on_Data( DefaultL1Muctpi ):

  """
    Module configuring the MuCTPI simulation to be re-run on data BS file
    (that already has a result from a previous MuCTPI simulation running)
  """

  def __init__( self, name = "L1Muctpi_on_Data" ):

    DefaultL1Muctpi.__init__( self, name )

    self.InputSource = "RDO"
    self.RDOLocID = "MUCTPI_RDO"
    self.RDOOutputLocID = "MUCTPI_RDO+"
    self.RoIOutputLocID = "not_used_1"
    self.OverlapStrategyName = "LUT"
    self.LUTXMLFile = "TrigConfMuctpi/data10_7TeV.periodI.physics_Muons.MuCTPI_LUT.NoBEOverlaps_composedEF.v002_modifiedBB_crc_3385316356.xml"
    self.IsData=1
    self.FlaggingMode = False

class L1Muctpi_on_AOD( DefaultL1Muctpi ):

  """
    Module configuring the MuCTPI simulation to be re-run on an AOD file
    (that contains the LVL1_ROI object)
  """

  def __init__( self, name = "L1Muctpi_on_AOD" ):

    DefaultL1Muctpi.__init__( self, name )

    self.InputSource = "AOD"
    self.AODLocID = "LVL1_ROI"
    self.RDOOutputLocID = "MUCTPI_RDO+"
    self.RoIOutputLocID = "not_used_1"
    self.CTPOutputLocID = "not_used_2"


# for now just duplicate the configuration for the L1MuctpiTool, to be consolidated
# ------------------------------------------------------------------------------------------------------



class DefaultL1MuctpiTool( LVL1MUCTPI__L1MuctpiTool ):

  """
    Base configurable module for the L1MuctpiTool algorithm configurations
  """

  def __init__( self, name = "L1MuctpiTool" ):

    LVL1MUCTPI__L1MuctpiTool.__init__( self, name )

    # Create a logger:
    from AthenaCommon.Logging import logging
    logger = logging.getLogger( "L1MuctpiTool" )

    # Set properties of the LUT overlap handling:
    self.OverlapStrategyName = "LUT"
    self.DumpLUT = False
    self.LUTXMLFile = "UNDEFINED"
    self.RunPeriod = "UNDEFINED"
    self.FlaggingMode = False
    self.MultiplicityStrategyName = "INCLUSIVE"

    # Decide which LUT to use, based on which run we are simulating:
    from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as commonGeoFlags
    from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags as geoFlags
    if ( commonGeoFlags.Run() == "RUN1" ) or ( ( commonGeoFlags.Run() == "UNDEFINED" ) and
                                               ( geoFlags.isIBL() is False ) ):
      self.LUTXMLFile = "TrigConfMuctpi/data10_7TeV.periodI.physics_Muons.MuCTPI_LUT.NoBEOverlaps_composedEF.v002.xml"
      self.RunPeriod = "RUN1"
      logger.info( "Configuring MuCTPI simulation with Run 1 configuration file:" )
      logger.info( "  TrigConfMuctpi/data10_7TeV.periodI.physics_Muons.MuCTPI_LUT.NoBEOverlaps_composedEF.v002.xml" )
      logger.info( "  with a RunPeriod=RUN1" )
    elif ( commonGeoFlags.Run() == "RUN2" ) or ( ( commonGeoFlags.Run() == "UNDEFINED" ) and
                                                 ( geoFlags.isIBL() is True ) ):
      self.LUTXMLFile = "TrigConfMuctpi/data10_7TeV.periodI.physics_Muons.MuCTPI_LUT.NoBEOverlaps_composedEF.v002_modifiedBB.xml"
      self.RunPeriod = "RUN2"
      logger.info( "Configuring MuCTPI simulation with Run 2 configuration file:" )
      logger.info( "  TrigConfMuctpi/data10_7TeV.periodI.physics_Muons.MuCTPI_LUT.NoBEOverlaps_composedEF.v002_modifiedBB.xml" )
      logger.info( "  with a RunPeriod=RUN2" )
    else:
      self.LUTXMLFile = "TrigConfMuctpi/data10_7TeV.periodI.physics_Muons.MuCTPI_LUT.NoBEOverlaps_composedEF.v002_modifiedBB.xml"
      self.RunPeriod = "RUN2"
      logger.warning( "Couldn't determine which run to simulate, using Run 2 configuration file:" )
      logger.warning( "  TrigConfMuctpi/data10_7TeV.periodI.physics_Muons.MuCTPI_LUT.NoBEOverlaps_composedEF.v002_modifiedBB.xml" )
      logger.warning( "  with a RunPeriod=RUN2" )

      pass

    # Set properties for NIM outputs ot L1Topo conversion from RoI to eta/phi
    self.GeometryXMLFile = "TrigConfMuctpi/TestMioctGeometry_2016_05_30_CS_2600573263.xml"

    # Turn on the NIM output creation by default:
    self.DoNIMOutput = True
    # The bit settings were extracted from here:
    #   https://savannah.cern.ch/bugs/?90300#comment14
    self.NIMBarrelBit = 29
    self.NIMEndcapBit = 30

    ### this should not be done at all, since it instantiates a config svc without proper configuration!!
    ### instead rely on the TriggerConfigGetter being setup in the top job properties
    # Ensure that the LVL1ConfigSvc is loaded:
    #from AthenaCommon.AppMgr import ServiceMgr # Has to be imported here for some reason...
    #if not hasattr( ServiceMgr, "LVL1ConfigSvc" ):  
    #  from TrigConfigSvc.TrigConfigSvcConfig import LVL1ConfigSvc
    #  lvl1ConfigSvc = LVL1ConfigSvc("LVL1ConfigSvc")
#   #   lvl1ConfigSvc.XMLFile = "LVL1config13.0.20.xml"
    #  ServiceMgr += lvl1ConfigSvc
    #  self.LVL1ConfigSvc = lvl1ConfigSvc

class L1MuctpiTool( DefaultL1MuctpiTool ):

  """
    Module configuring the default MuCTPI simulation to be used as
    part of the LVL1 simulation.
  """

  def __init__( self, name = "L1MuctpiTool" ):

    DefaultL1MuctpiTool.__init__( self, name )

    self.InputSource = "DIGITIZATION"

class L1MuctpiTool_on_RDO( DefaultL1MuctpiTool ):

  """
    Module configuring the MuCTPI simulation to be re-run on an RDO file
    (that already has a result from a previous MuCTPI simulation running)
  """

  def __init__( self, name = "L1MuctpiTool_on_RDO" ):

    DefaultL1MuctpiTool.__init__( self, name )

    self.InputSource = "RDO"
    self.RDOLocID = "MUCTPI_RDO"
    self.RDOOutputLocID = "MUCTPI_RDO+"
    self.RoIOutputLocID = "not_used_1"
    self.CTPOutputLocID = "not_used_2"

class L1MuctpiTool_on_Data( DefaultL1MuctpiTool ):

  """
    Module configuring the MuCTPI simulation to be re-run on data BS file
    (that already has a result from a previous MuCTPI simulation running)
  """

  def __init__( self, name = "L1MuctpiTool_on_Data" ):

    DefaultL1MuctpiTool.__init__( self, name )

    self.InputSource = "RDO"
    self.RDOLocID = "MUCTPI_RDO"
    self.RDOOutputLocID = "MUCTPI_RDO+"
    self.RoIOutputLocID = "not_used_1"
    self.OverlapStrategyName = "LUT"
    self.LUTXMLFile = "TrigConfMuctpi/data10_7TeV.periodI.physics_Muons.MuCTPI_LUT.NoBEOverlaps_composedEF.v002_modifiedBB_crc_3385316356.xml"
    self.IsData=1
    self.FlaggingMode = False

class L1MuctpiTool_on_AOD( DefaultL1MuctpiTool ):

  """
    Module configuring the MuCTPI simulation to be re-run on an AOD file
    (that contains the LVL1_ROI object)
  """

  def __init__( self, name = "L1MuctpiTool_on_AOD" ):

    DefaultL1MuctpiTool.__init__( self, name )

    self.InputSource = "AOD"
    self.AODLocID = "LVL1_ROI"
    self.RDOOutputLocID = "MUCTPI_RDO+"
    self.RoIOutputLocID = "not_used_1"
    self.CTPOutputLocID = "not_used_2"

