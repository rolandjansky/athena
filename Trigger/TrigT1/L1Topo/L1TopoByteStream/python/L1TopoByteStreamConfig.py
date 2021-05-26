# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
'''
Functions creating ComponentAccumulator with ByteStream converters for L1Topo objects
'''

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

# This function could likely be moved somewhere central
def GenericByteStreamCnvCfg(flags, typeNamesToDecode):
    if flags.Trigger.Online.isPartition:
        from TrigByteStreamCnvSvc.TrigByteStreamConfig import TrigByteStreamCfg
        servicesCfgFunction = TrigByteStreamCfg
    else:
        from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
        servicesCfgFunction = ByteStreamReadCfg

    acc = ComponentAccumulator()
    acc.merge(servicesCfgFunction(flags, type_names=typeNamesToDecode))
    return acc

def L1TopoRDOCollectionBSCnvCfg(flags):
    typeNamesToDecode = ["L1TopoRDOCollection/L1TopoRDOCollection",
                         "SG::AuxVectorBase/L1TopoRDOCollection"]
    return GenericByteStreamCnvCfg(flags, typeNamesToDecode)

def L1TopoRawDataContainerBSCnvCfg(flags):
    typeNamesToDecode = ["xAOD::L1TopoRawDataContainer/L1TopoRawData",
                         "xAOD::L1TopoRawDataAuxContainer/L1TopoRawDataAux."]
    return GenericByteStreamCnvCfg(flags, typeNamesToDecode)

def L1TopoByteStreamCfg(flags):
    acc = ComponentAccumulator()
    acc.merge(L1TopoRDOCollectionBSCnvCfg(flags))
    acc.merge(L1TopoRawDataContainerBSCnvCfg(flags))
    return acc


if __name__ == '__main__':
    '''Run a functional unit test if module is executed'''

    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaCommon.Constants import DEBUG

    flags.Exec.OutputLevel = DEBUG  # noqa: ATL900 - fine for short unit test
    flags.Input.Files = defaultTestFiles.RAW
    flags.lock()

    acc = MainServicesCfg(flags)
    acc.merge(L1TopoByteStreamCfg(flags))
    acc.printConfig()

    with open('L1TopoByteStreamConfig.pkl', 'wb') as pkl:
        acc.store(pkl)

    acc.run(10)
