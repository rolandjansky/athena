# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


from LArByteStream.LArByteStreamConf import LArRawDataContByteStreamTool



def LArRawDataContByteStreamToolConfig (name="LArRawDataContByteStreamTool",
                                        InitializeForWriting = False,
                                        stream=None,
                                        **kwargs):
      tool = LArRawDataContByteStreamTool (name, **kwargs)
      if InitializeForWriting:
         from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
         noisealg = CaloNoiseCondAlg ('totalNoise')
         if stream:
            key = noisealg.OutputKey
            if key.find ('+') < 0:
               key = 'ConditionStore+' + key
            stream.ExtraInputs += [('CaloNoise', key)]
      tool.InitializeForWriting = InitializeForWriting
      return tool
