#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

def init_mask(tool_name):
    from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
    theLArBadChannelsMasker=LArBadChannelMasker(tool_name) #,this)
    theLArBadChannelsMasker.DoMasking=True
    theLArBadChannelsMasker.ProblemsToMask=[
        "deadReadout","deadPhys","short","almostDead",
        "highNoiseHG","highNoiseMG","highNoiseLG","sporadicBurstNoise"]
    return theLArBadChannelsMasker
