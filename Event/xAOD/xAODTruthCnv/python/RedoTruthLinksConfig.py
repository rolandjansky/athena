# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def RedoTruthLinksAlgCfg(ConfigFlags) :
    acc = ComponentAccumulator()
    if 'xAODTruthLinks' not in ConfigFlags.Input.Collections :
        acc.addEventAlgo( CompFactory.xAODMaker.RedoTruthLinksAlg("GEN_AOD2xAOD_links") )
    return acc
