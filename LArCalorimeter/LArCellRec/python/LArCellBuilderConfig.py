# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from LArCellRec.LArCellRecConf import LArCellBuilderFromLArRawChannelTool, LArCellMerger, LArCellNoiseMaskingTool
from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg 

def LArCellBuilderCfg(configFlags):
    result=LArOnOffIdMappingCfg(configFlags)
    theLArCellBuilder = LArCellBuilderFromLArRawChannelTool()

    theLArCellBuilder.addDeadOTX = False #Create flag? Requires bad-feb DB access
    return result,theLArCellBuilder



def LArCellCorrectorCfg(configFlags):
    result=ComponentAccumulator()
    
    correctionTools=[]

    if configFlags.get("LAr.RawChannelSource")=="both":
        theMerger=LArCellMerger(RawChannelsName="LArRawChannels_FromDigits")
        correctionTools.append(theMerger)
    
    if configFlags.get("LAr.doCellNoiseMasking") or configFlags.get("LAr.doCellSporadicNoiseMasking"):
        from LArBadChannelTool.LArBadChannelConfig import LArBadChannelMaskerCfg
        theNoiseMasker=LArCellNoiseMaskingTool()
        if configFlags.get("LAr.doCellNoiseMasking"):
            acc,cellNoiseMaskingTool= LArBadChannelMaskerCfg(configFlags,problemsToMask=["highNoiseHG","highNoiseMG","highNoiseLG","deadReadout","deadPhys"],ToolName="CellNoiseMask")
            result.merge(acc)
            theNoiseMasker.MaskingTool=cellNoiseMaskingTool
            pass
        if configFlags.get("LAr.doCellSporadicNoiseMasking"):
            acc,sporadicNoiseMaskingTool=LArBadChannelMaskerCfg(configFlags,problemsToMask=["sporadicBurstNoise",],ToolName="SporadicNoiseMask")
            result.merge(acc)
            theNoiseMasker.MaskingSporadicTool=sporadicNoiseMaskingTool
            pass
        correctionTools.append(theNoiseMasker)
    #Many more tools to be added, eg HV correction

    
    return result,correctionTools
        
        
        
