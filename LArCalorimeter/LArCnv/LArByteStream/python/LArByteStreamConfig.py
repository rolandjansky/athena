# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


from LArByteStream.LArByteStreamConf import LArRawDataContByteStreamTool



def LArRawDataContByteStreamToolConfig (name="LArRawDataContByteStreamTool",
                                        InitializeForWriting = False,
                                        stream=None,
                                        **kwargs):
      tool = LArRawDataContByteStreamTool (name, **kwargs)
      if InitializeForWriting:
         from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
         from LArCabling.LArCablingAccess import LArOnOffIdMapping, LArFebRodMapping
         noisealg = CaloNoiseCondAlg ('totalNoise')
         LArOnOffIdMapping()
         LArFebRodMapping()
         if stream:
            key = str(noisealg.OutputKey)
            if key.find ('+') < 0:
               key = 'ConditionStore+' + key
            stream.ExtraInputs += [('CaloNoise', key)]

            stream.ExtraInputs += [('LArOnOffIdMapping', 'ConditionStore+LArOnOffIdMap')]

            stream.ExtraInputs += [('LArFebRodMapping', 'ConditionStore+LArFebRodMap')]
      tool.InitializeForWriting = InitializeForWriting
      return tool
