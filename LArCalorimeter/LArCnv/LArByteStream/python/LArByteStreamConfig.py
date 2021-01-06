# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


from AthenaConfiguration.ComponentFactory import CompFactory


def LArRawDataContByteStreamToolConfig (name="LArRawDataContByteStreamTool",
                                        InitializeForWriting = False,
                                        stream=None,
                                        **kwargs):
      tool = CompFactory.LArRawDataContByteStreamTool (name, **kwargs)
      if InitializeForWriting:
         from LArCabling.LArCablingAccess import LArOnOffIdMapping, LArFebRodMapping
         noisealg = CompFactory.CaloNoiseCondAlg ('totalNoise')
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
