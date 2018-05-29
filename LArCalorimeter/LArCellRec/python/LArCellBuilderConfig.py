# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from LArCellRec.LArCellRecConf import LArCellBuilderFromLArRawChannelTool, LArCellMerger, LArCellNoiseMaskingTool
from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg 

def LArCellBuilderCfg(configFlags):
    result=ComponentAccumulator()
    result.addConfig(LArOnOffIdMappingCfg,configFlags)
    theLArCellBuilder = LArCellBuilderFromLArRawChannelTool()

    theLArCellBuilder.addDeadOTX = False #Create flag? Requires bad-feb DB access
    result.addAlgTool(theLArCellBuilder)

    
    return result



def LArCellCorrectorCfg(configFlags):
    result=ComponentAccumulator()
    
    correctionTools=[]

    if configFlags.get("LAr.RawChannelSource")=="both":
        theMerger=result.addConfig(LArCellMerger,configFlags,RawChannelsName="LArRawChannels_FromDigits")
        correctionTools.append(theMerger[0])
    
    if configFlags.get("LAr.doCellNoiseMasking") or configFlags.get("LAr.doCellSporadicNoiseMasking"):
        from LArBadChannelTool.LArBadChannelConfig import LArBadChannelMaskerCfg
        theNoiseMasker=LArCellNoiseMaskingTool()
        if configFlags.get("LAr.doCellNoiseMasking"):
            cellNoiseMaskingTool=result.addConfig(LArBadChannelMaskerCfg,configFlags,
                                                  problemsToMask=["highNoiseHG","highNoiseMG","highNoiseLG","deadReadout","deadPhys"],
                                                  ToolName="CellNoiseMask")
            theNoiseMasker.MaskingTool=cellNoiseMaskingTool[0]
            pass
        if configFlags.get("LAr.doCellSporadicNoiseMasking"):
            sporadicNoiseMaskingTool=result.addConfig(LArBadChannelMaskerCfg,configFlags,
                                                      problemsToMask=["sporadicBurstNoise",],
                                                      ToolName="SporadicNoiseMask")
            theNoiseMasker.MaskingSporadicTool=sporadicNoiseMaskingTool[0]
            pass
        correctionTools.append(theNoiseMasker)
        pass

    #Many more tools to be added, eg HV correction

    
    result.clearAlgTools()
    for t in correctionTools:
        result.addAlgTool(t)

    return result
        
        
        
