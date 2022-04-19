#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def LArDigits2NtupleCfg(flags, **kwargs):
       cfg=ComponentAccumulator()
       if kwargs['isSC']:
          from LArByteStream.LArRawSCDataReadingConfig import LArRawSCDataReadingCfg
          cfg.merge(LArRawSCDataReadingCfg(flags, kwargs))
       else:
          from LArByteStream.LArRawDataReadingConfig import LArRawDataReadingCfg
          cfg.merge(LArRawDataReadingCfg(flags, kwargs))

       cfg.addEventAlgo(CompFactory.LArDigits2Ntuple(kwargs))

       return cfg
