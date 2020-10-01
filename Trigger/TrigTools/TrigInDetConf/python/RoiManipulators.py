# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigGenericAlgs.TrigGenericAlgsConf import PESA__TrigRoiUpdater

class IDTrigRoiUpdater ( PESA__TrigRoiUpdater ):
  __slots__ = []
  def __init__(self, name="IDTrigRoiUpdater_Electron_IDT", type="electron"):
    super ( PESA__TrigRoiUpdater, self ).__init__(name)

    from RegionSelector.RegSelToolConfif import makeRegSelTool_Pixel
    from RegionSelector.RegSelToolConfif import makeRegSelTool_SCT

    self.RegSelTool_Pixel = makeRegSelTool_Pixel()
    self.RegSelTool_SCT   = makeRegSelTool_SCT()

    from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
    self.EtaHalfWidth = InDetTrigSliceSettings[('etaHalfWidth',type)]
    self.PhiHalfWidth = InDetTrigSliceSettings[('phiHalfWidth',type)]

