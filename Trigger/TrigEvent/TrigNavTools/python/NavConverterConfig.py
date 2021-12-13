#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from TrigEDMConfig.TriggerEDM import getTriggerEDMList

def createNavConverterAlg():
    cnvAlg = CompFactory.Run2ToRun3TrigNavConverter("TrigRun2ToRun3NavConverter")
    cnvAlg.Rois = ["forID","forID1","forID2","forMS","forSA","forTB","forMT","forCB"]
    cnvAlg.Chains = ["HLT_mu4"] #TODO expand or replace with wildcard covering all chains
    cnvAlg.Collections = [ x.split("#")[0] for x in getTriggerEDMList("AODCONV", 2) if "Aux" not in x]
    # to read navigation from TDT rather directly from xAOD::TrigNavigation
    cnvAlg.TrigDecisionTool = "TrigDecisionTool"
    cnvAlg.TrigNavReadKey = ""

    cnvAlg.TrigNavWriteKey = "HLTNav_R2ToR3"
    return cnvAlg


def NavConverterCfg(flags):
    acc = ComponentAccumulator()
    from TrigDecisionTool.TrigDecisionToolConfig import TrigDecisionToolCfg
    tdt_ca = TrigDecisionToolCfg(flags)
    tdt = tdt_ca.getPrimary()
    acc.merge(tdt_ca)

    cnvAlg = createNavConverterAlg()
    cnvAlg.TrigDecisionTool = tdt
    cnvAlg.TrigConfigSvc = tdt.TrigConfigSvc
    acc.addEventAlgo(cnvAlg)

    return acc


if __name__ == "__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()

    acc = NavConverterCfg(ConfigFlags)
    acc.printConfig(withDetails=True, summariseProps=True)
    acc.wasMerged()
