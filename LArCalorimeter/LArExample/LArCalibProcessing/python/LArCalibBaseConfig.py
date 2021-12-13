# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def LArCalibBaseCfg(flags):  
    result=ComponentAccumulator()
    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    result.merge(ByteStreamReadCfg(flags))

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    result.merge(LArGMCfg(flags))

    from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg, LArCalibIdMappingCfg, LArOnOffIdMappingSCCfg#, LArCalibIdMappingSCCfg
    if flags.LArCalib.isSC:
        #Setup SuperCell cabling
        result.merge(LArOnOffIdMappingSCCfg(flags))
    else:
        #Setup regular cabling

        result.merge(LArOnOffIdMappingCfg(flags))
        result.merge(LArCalibIdMappingCfg(flags))
    
    #Set up bad-channel config
    from LArCalibProcessing.LArCalib_BadChannelConfig import LArCalibBadChannelCfg

    result.merge(LArCalibBadChannelCfg(flags))
    return result




#Helper method to maipulate channel selection string
def chanSelStr(flags):
    inp=flags.LArCalib.Input.ChannelSelection
    if inp=="": return ""
    if inp.startswith("<channelSelection>"): return inp
    
    return "<channelSelection>"+inp+"</channelSelection>"
