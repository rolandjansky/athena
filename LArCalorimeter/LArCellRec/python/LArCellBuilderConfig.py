# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
LArCellBuilderFromLArRawChannelTool, LArCellMerger, LArCellNoiseMaskingTool=CompFactory.getComps("LArCellBuilderFromLArRawChannelTool","LArCellMerger","LArCellNoiseMaskingTool",)
from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg
from LArBadChannelTool.LArBadChannelConfig import LArBadChannelCfg, LArBadFebCfg
from LArCalibUtils.LArHVScaleConfig import LArHVScaleCfg
from LArConfiguration.LArConfigFlags import RawChannelSource 

def LArCellBuilderCfg(configFlags):
    result=ComponentAccumulator()
    result.merge(LArOnOffIdMappingCfg(configFlags))
    result.merge(LArBadFebCfg(configFlags))
    theLArCellBuilder = LArCellBuilderFromLArRawChannelTool()
    theLArCellBuilder.LArCablingKey = "ConditionStore+LArOnOffIdMap"
    theLArCellBuilder.MissingFebKey = "ConditionStore+LArBadFeb"
    if configFlags.LAr.RawChannelSource is RawChannelSource.Calculated:
       theLArCellBuilder.RawChannelsName="LArRawChannels_FromDigits"
    else:
       theLArCellBuilder.RawChannelsName = "LArRawChannels"
    theLArCellBuilder.addDeadOTX = True #Create flag? Requires bad-feb DB access
    result.setPrivateTools(theLArCellBuilder)
    return result


def LArCellCorrectorCfg(configFlags):
    result=ComponentAccumulator()

    correctionTools=[]

    if configFlags.LAr.RawChannelSource in (RawChannelSource.Both, RawChannelSource.Input) and not configFlags.Input.isMC and not configFlags.Overlay.DataOverlay:
        theMerger=LArCellMerger(RawChannelsName="LArRawChannels_FromDigits")
        correctionTools.append(theMerger)

    if configFlags.LAr.doCellNoiseMasking or configFlags.LAr.doCellSporadicNoiseMasking:
        result.merge(LArBadChannelCfg(configFlags))
        theNoiseMasker=LArCellNoiseMaskingTool(qualityCut = 4000)
        if configFlags.LAr.doCellNoiseMasking:
            theNoiseMasker.ProblemsToMask=["highNoiseHG","highNoiseMG","highNoiseLG","deadReadout","deadPhys"]
            pass
        if configFlags.LAr.doCellSporadicNoiseMasking:
            theNoiseMasker.SporadicProblemsToMask=["sporadicBurstNoise",]
            pass
        correctionTools.append(theNoiseMasker)

    result.setPrivateTools(correctionTools)
    return result


def LArHVCellContCorrCfg(configFlags):
    acc=ComponentAccumulator()
    acc.merge(LArHVScaleCfg(configFlags)) #CondAlgo & co for HVScale Corr
    LArCellContHVCorrTool=CompFactory.LArCellContHVCorrTool
    theLArCellHVCorrTool = LArCellContHVCorrTool()
    acc.setPrivateTools(theLArCellHVCorrTool)
    return acc


