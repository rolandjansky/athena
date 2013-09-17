# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from LArByteStream.LArByteStreamConf import LArRawDataContByteStreamTool



class LArRawDataContByteStreamToolConfig(LArRawDataContByteStreamTool):
   __slots__ = []
   def __init__(self, name="LArRawDataContByteStreamTool", **kwargs):
      super( LArRawDataContByteStreamToolConfig, self).__init__(name)
      from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
      theCaloNoiseTool=CaloNoiseToolDefault()
      from AthenaCommon.AppMgr import ToolSvc
      ToolSvc+=theCaloNoiseTool
      self.NoiseTool = theCaloNoiseTool
      for n,v in kwargs.items():
       setattr(self, n, v)
