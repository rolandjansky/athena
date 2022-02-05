# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
'''
Functions creating ComponentAccumulator for ByteStream access
'''

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def ByteStreamReadCfg(flags, type_names=[]):
    '''
    Returns ComponentAccumulator with all components required to read objects from ByteStream input

    Params:
        flags (AthConfigFlags): The configuration flags container
        type_names (list): Types to register converters for reading from BS, format ["typeA/key1", "typeB/key2"]

    This wrapper function registers the type_names with either the online (HLT), the offline or the EMon BS services
    depending on the configuration flags.
    '''

    if flags.Common.isOnline and not any(flags.Input.Files) and not (flags.Trigger.doHLT or flags.Trigger.doLVL1):
        # Running online reconstruction at P1 (EMon)
        from ByteStreamEmonSvc.EmonByteStreamConfig import EmonByteStreamCfg
        servicesCfgFunction = EmonByteStreamCfg
    elif flags.Trigger.Online.isPartition:
        # Running HLT at P1 or with athenaHLT.py
        from TrigByteStreamCnvSvc.TrigByteStreamConfig import TrigByteStreamCfg
        servicesCfgFunction = TrigByteStreamCfg
    else:
        # Running offline athena
        from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
        servicesCfgFunction = ByteStreamReadCfg

    acc = ComponentAccumulator()
    acc.merge(servicesCfgFunction(flags, type_names=type_names))
    return acc


def ByteStreamWriteCfg(flags, type_names=[], extra_inputs=[]):
    '''
    Returns ComponentAccumulator with all components required to write objects to ByteStream output

    Params:
        flags (AthConfigFlags): The configuration flags container
        type_names (list): Objects to register converters for reading from BS, format ["typeA/key1", "typeB/key2"]
        extra_inputs (list): Objects which need to be produced before the output stream algorithm runs
            (scheduler dependencies), format [('typeC', 'key3'), ('typeD', 'key4')]
    '''

    # Allow calling this only in offline athena (i.e. not in athenaHLT)
    if flags.Trigger.Online.isPartition:
        raise RuntimeError('Cannot write arbitrary objects to ByteStream in online HLT configuration')

    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamWriteCfg
    acc = ByteStreamWriteCfg(flags, type_names=type_names)
    acc.getPrimary().ExtraInputs = extra_inputs
    acc.getService('ByteStreamEventStorageOutputSvc').StreamType = 'unknown'
    acc.getService('ByteStreamEventStorageOutputSvc').StreamName = 'SingleStream'
    return acc
