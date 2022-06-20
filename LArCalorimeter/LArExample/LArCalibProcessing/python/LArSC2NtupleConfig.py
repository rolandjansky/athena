#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def LArSC2NtupleCfg(flags, **kwargs):

       kwargs['isSC'] = True

       cfg=ComponentAccumulator()

       from LArByteStream.LArRawSCDataReadingConfig import LArRawSCDataReadingCfg
       cfg.merge(LArRawSCDataReadingCfg(flags,OutputLevel=kwargs['OutputLevel']))

       from LArCabling.LArCablingConfig import LArOnOffIdMappingSCCfg,LArCalibIdMappingSCCfg,LArLATOMEMappingCfg
       cfg.merge(LArOnOffIdMappingSCCfg(flags))
       cfg.merge(LArCalibIdMappingSCCfg(flags))
       cfg.merge(LArLATOMEMappingCfg(flags))

       if flags.LArSCDump.doRawChan:
          from LArByteStream.LArRawDataReadingConfig import LArRawDataReadingCfg
          cfg.merge(LArRawDataReadingCfg(flags))
          from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg
          cfg.merge(LArOnOffIdMappingCfg(flags))
          
       alg=CompFactory.LArSC2Ntuple('LArSC2Ntuple',**kwargs)
       # if debug is needed, uncomment:
       #from AthenaCommon.Constants import DEBUG
       #alg.OutputLevel=DEBUG

       cfg.addEventAlgo(alg)

       return cfg


