#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.Logging import logging
log = logging.getLogger('decodeBS_TLA_AOD.py')


def decodingCfg(flags):
    """ Configure deserialization of RAW byte stream """
    acc = ComponentAccumulator()

    from AthenaCommon.CFElements import seqAND
    from TrigEDMConfig import DataScoutingInfo

    acc.addSequence(seqAND("Decoding"))

    acc.addEventAlgo(CompFactory.HLTResultMTByteStreamDecoderAlg(), "Decoding")

    deserialiser = CompFactory.TriggerEDMDeserialiserAlg("TLATrigDeserialiser")
    deserialiser.ModuleID = DataScoutingInfo.DataScoutingIdentifiers["PhysicsTLA"]
    acc.addEventAlgo(deserialiser, "Decoding")

    return acc

# Configure AOD output
def outputCfg(flags):
    """ Configure AOD output """
    acc = ComponentAccumulator()

    from TrigEDMConfig.TriggerEDM import getTriggerEDMList
    edmList = getTriggerEDMList(flags.Trigger.ESDEDMSet, flags.Trigger.EDMVersion)

    ItemList = []
    for edmType, edmKeys in edmList.items():
        for key in edmKeys:
            ItemList.append(edmType+'#'+key)
    ItemList += [ "xAOD::EventInfo#EventInfo", "xAOD::EventAuxInfo#EventInfoAux." ]
    ItemList += [ 'xAOD::TrigCompositeContainer#*' ]
    ItemList += [ 'xAOD::TrigCompositeAuxContainer#*' ]

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge(OutputStreamCfg(flags, "AOD", ItemList=ItemList))

    return acc

def GetCustomAthArgs():
    from argparse import ArgumentParser
    parser = ArgumentParser(description='Parser for IDPVM configuration')
    parser.add_argument("--filesInput", action='append')
    return parser.parse_args()

if __name__ == "__main__":
    args = GetCustomAthArgs()

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True

    # Setup flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.Files = args.filesInput
    ConfigFlags.Input.isMC = False
    ConfigFlags.Output.AODFileName = 'AOD.pool.root'
    ConfigFlags.Trigger.triggerMenuSetup = 'Physics_pp_run3_v1'
    ConfigFlags.lock()

    # Setup main services
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(ConfigFlags)

    # Setup input

    # ByteStream input
    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    cfg.merge(ByteStreamReadCfg(ConfigFlags))

    # Decoding sequence
    cfg.merge(decodingCfg(ConfigFlags))

    # Trigger metadata
    from TriggerJobOpts.TriggerRecoConfig import TriggerMetadataWriterCfg
    metadataAcc, _ = TriggerMetadataWriterCfg(ConfigFlags)
    cfg.merge( metadataAcc )

    # Output
    cfg.merge(outputCfg(ConfigFlags))
    
    # Dump the pickle file
    with open("DecodeBS_TLA_AOD.pkl", "wb") as f:
        cfg.store(f)

    # Execute
    import sys
    sys.exit(not cfg.run().isSuccess())
