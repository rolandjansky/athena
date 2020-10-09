# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


from LArByteStream.LArByteStreamConf import LArRawDataContByteStreamTool



class LArRawDataContByteStreamToolConfig(LArRawDataContByteStreamTool):
   __slots__ = []
   def __init__(self, name="LArRawDataContByteStreamTool",
                InitializeForWriting = False,
                stream=None,
                **kwargs):
      super( LArRawDataContByteStreamToolConfig, self).__init__(name)
      if InitializeForWriting:
         from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
         noisealg = CaloNoiseCondAlg ('totalNoise')
         if stream:
            key = noisealg.OutputKey
            if key.find ('+') < 0:
               key = 'ConditionsStore+' + key
            stream.ExtraInputs += [('CaloNoise', key)]
      for n,v in kwargs.items():
         setattr(self, n, v)
      self.InitializeForWriting = InitializeForWriting
      return
