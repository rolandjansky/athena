# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


from AthenaConfiguration.ComponentFactory import CompFactory


def LArRawDataContByteStreamToolConfig (name="LArRawDataContByteStreamTool",
                                        InitializeForWriting = False,
                                        stream=None,
                                        **kwargs):
      tool = CompFactory.LArRawDataContByteStreamTool (name, **kwargs)
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

# ComponentAccumulator version
def LArRawDataContByteStreamToolCfg (flags,
                                     name="LArRawDataContByteStreamTool",
                                     InitializeForWriting = False,
                                     **kwargs):
      from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
      acc = ComponentAccumulator()
      tool = CompFactory.LArRawDataContByteStreamTool(name, **kwargs)
      tool.InitializeForWriting = InitializeForWriting
      acc.addPublicTool(tool)

      extraOutputs = []

      if InitializeForWriting:
         from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
         acc.merge(CaloNoiseCondAlgCfg(flags, noisetype="totalNoise"))

         from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg, LArFebRodMappingCfg
         acc.merge(LArOnOffIdMappingCfg(flags))
         acc.merge(LArFebRodMappingCfg(flags))

         extraOutputs = [
            ('CaloNoise', 'ConditionStore+totalNoise'),
            ('LArOnOffIdMapping', 'ConditionStore+LArOnOffIdMap'),
            ('LArFebRodMapping', 'ConditionStore+LArFebRodMap'),
            ('CaloDetDescrManager', 'ConditionStore+CaloDetDescrManager')
         ]

      return acc, extraOutputs
