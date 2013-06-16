# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TrigT1MuctpiConfig.py 515239 2012-08-28 11:39:45Z krasznaa $

# Local (generated) configurable(s):
from TrigT1Muctpi.TrigT1MuctpiConf import LVL1MUCTPI__L1Muctpi

class DefaultL1Muctpi( LVL1MUCTPI__L1Muctpi ):

  """
    Base configurable module for the L1Muctpi algorithm configurations
  """

  def __init__( self, name = "L1Muctpi" ):

    LVL1MUCTPI__L1Muctpi.__init__( self, name )

    # Set properties of the LUT overlap handling:
    self.OverlapStrategyName = "LUT"
    self.DumpLUT = False
    self.LUTXMLFile = "TrigConfMuctpi/data10_7TeV.periodI.physics_Muons.MuCTPI_LUT.NoBEOverlaps_composedEF.v002.xml"
    self.FlaggingMode = False
    self.MultiplicityStrategyName = "INCLUSIVE"

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

    from AthenaCommon.GlobalFlags import globalflags
    from RecExConfig.RecFlags import rec
    pass

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
    self.CTPOutputLocID = "/Run/L1MuCTPItoCTPLocation"
    self.OverlapStrategyName = "LUT"
    self.LUTXMLFile = "TrigConfMuctpi/data10_7TeV.periodI.physics_Muons.MuCTPI_LUT.NoBEOverlaps_composedEF.v002_crc_1614217507.xml"
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
