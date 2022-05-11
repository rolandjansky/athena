#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def LArDigitsToNtupleCfg(flags):
    cfg=ComponentAccumulator()
    cfg.addEventAlgo(CompFactory.LArDigitsToNtuple(ReadTrigger=False,
                                                   ReadScint=False,
                                                   ReadPedestal=False,
                                                   ReadSCA=False,
                                                   ReadPhase=False,
                                                   NSamples=flags.LAr.ROD.nSamples,
                                                   ContainerKey="Bkg_LArDigitContainer_MC" if flags.Input.isMC else "FREE"
                                               ))

    cfg.addService(CompFactory.NTupleSvc(Output=["FILE1 DATAFILE='lardigits.root' TYP='ROOT' OPT='NEW'"]))
    cfg.setAppProperty("HistogramPersistency","ROOT")

    return cfg
