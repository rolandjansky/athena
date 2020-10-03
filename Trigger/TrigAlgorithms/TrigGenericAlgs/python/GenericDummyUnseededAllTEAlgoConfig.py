# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo

class GenericDummyUnseededAllTEAlgo( PESA__DummyUnseededAllTEAlgo ) :
  __slots__ = []
  def __init__(self, name="GenericAllTEAlgo" ) :
    super ( GenericDummyUnseededAllTEAlgo, self ).__init__(name)

    from RegionSelector.RegSelToolConfig import makeRegSelTool_Pixel
    from RegionSelector.RegSelToolConfig import makeRegSelTool_SCT

    self.RegSelTool_Pixel = makeRegSelTool_Pixel()
    self.RegSelTool_SCT   = makeRegSelTool_SCT()

