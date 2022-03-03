#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Logging import logging
log = logging.getLogger("NavConverterConfig")


def NavConverterCfg(flags):
    """Configures Run 1/2 to Run 3 navigation conversion algorithm for all triggers"""
    acc = ComponentAccumulator()
    from TrigDecisionTool.TrigDecisionToolConfig import TrigDecisionToolCfg
    tdt_ca = TrigDecisionToolCfg(flags)
    tdt = tdt_ca.getPrimary()
    acc.merge(tdt_ca)

    cnvAlg = CompFactory.Run2ToRun3TrigNavConverterV2("TrigRun2ToRun3NavConverter")
    cnvAlg.TrigDecisionTool = tdt
    cnvAlg.TrigNavReadKey = ""
    cnvAlg.TrigConfigSvc = tdt.TrigConfigSvc
    outputName = "HLTNav_R2ToR3Summary"
    cnvAlg.OutputNavKey = outputName
    from OutputStreamAthenaPool.OutputStreamConfig import addToAOD, addToESD
    collections = [f"xAOD::TrigCompositeContainer#{outputName}", f"xAOD::TrigCompositeAuxContainer#{outputName}Aux."]
    acc.merge(addToAOD(flags, collections))
    acc.merge(addToESD(flags, collections))

    cnvAlg.Rois = ["initialRoI","forID","forID1","forID2","forMS","forSA","forTB","forMT","forCB"]

    from TrigEDMConfig.TriggerEDM import getTriggerEDMList
    edm = getTriggerEDMList("AODCONV", flags.Trigger.EDMVersion)
    types = [ t for t in edm ]
    log.info("Assuming these collections are relevant for trigger: %s", " ".join(types))
    cnvAlg.Collections = types
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
