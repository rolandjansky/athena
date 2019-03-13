# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

class PixelLorentzAngleToolSetup:
  "Class to simplify setup of PixelLorentzAngleTool"
  def __init__(self):
    hvFolder = "/PIXEL/DCS/HV"
    tempFolder = "/PIXEL/DCS/TEMPERATURE"
    dbInstance = "DCS_OFL"

    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    if athenaCommonFlags.isOnline():
      hvFolder = "/PIXEL/HLT/DCS/HV"
      tempFolder = "/PIXEL/HLT/DCS/TEMPERATURE"
      dbInstance = "PIXEL_ONL"

    from IOVDbSvc.CondDB import conddb
    if not conddb.folderRequested(hvFolder):
      conddb.addFolder(dbInstance, hvFolder, className="CondAttrListCollection")
    if not conddb.folderRequested(tempFolder):
      conddb.addFolder(dbInstance, tempFolder, className="CondAttrListCollection")

    from AthenaCommon.GlobalFlags import globalflags
    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")
 
    if not hasattr(condSeq,"PixelDCSCondHVAlg"):
      from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondHVAlg
      condSeq += PixelDCSCondHVAlg(name="PixelDCSCondHVAlg", ReadKey=hvFolder, UseConditions=True)

    if not hasattr(condSeq,"PixelDCSCondTempAlg"):
      from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondTempAlg
      condSeq += PixelDCSCondTempAlg(name="PixelDCSCondTempAlg", ReadKey=tempFolder, UseConditions=True)

    if not hasattr(condSeq, "PixelSiPropertiesCondAlg"):
      from SiPropertiesTool.SiPropertiesToolConf import PixelSiPropertiesCondAlg
      condSeq += PixelSiPropertiesCondAlg(name="PixelSiPropertiesCondAlg")

    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr(ToolSvc, "PixelSiPropertiesTool"):
      from SiPropertiesTool.SiPropertiesToolConf import SiPropertiesTool
      ToolSvc += SiPropertiesTool(name = "PixelSiPropertiesTool",
                                  DetectorName = "Pixel",
                                  ReadKey = "PixelSiliconPropertiesVector")

    if not hasattr(condSeq, "PixelSiLorentzAngleCondAlg"):
      from SiLorentzAngleTool.SiLorentzAngleToolConf import PixelSiLorentzAngleCondAlg
      condSeq += PixelSiLorentzAngleCondAlg(name = "PixelSiLorentzAngleCondAlg",
                                            SiPropertiesTool = ToolSvc.PixelSiPropertiesTool,
                                            UseMagFieldSvc = True,
                                            UseMagFieldDcs = (not athenaCommonFlags.isOnline()))
      pixelSiLorentzAngleCondAlg = condSeq.PixelSiLorentzAngleCondAlg

    if not hasattr(ToolSvc, "PixelLorentzAngleTool"):
      from SiLorentzAngleTool.SiLorentzAngleToolConf import SiLorentzAngleTool
      ToolSvc += SiLorentzAngleTool(name="PixelLorentzAngleTool", DetectorName="Pixel", SiLorentzAngleCondData="PixelSiLorentzAngleCondData")
      pixelLorentzAngleTool = ToolSvc.PixelLorentzAngleTool
      # Pass the silicon conditions tool to the Lorentz angle tool
      # Also make sure UseMagFieldTool is True as AtlasGeoModel sets this to False
      # if loaded first.
      pixelLorentzAngleTool.UseMagFieldSvc = True
      self.PixelLorentzAngleTool = pixelLorentzAngleTool

