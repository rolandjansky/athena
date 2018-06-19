BadChannelsFolder="/LAR/BadChannelsOfl/BadChannels"
MissingFEBsFolder="/LAR/BadChannelsOfl/MissingFEBs"

from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool
    
theLArBadChanTool= LArBadChanTool(CoolFolder=BadChannelsFolder,
                                  CoolMissingFEBsFolder=MissingFEBsFolder,
                                  )

ToolSvc+=theLArBadChanTool


from AthenaCommon.AlgSequence import AthSequencer
from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelCondAlg,LArBadFebCondAlg
condSeq = AthSequencer("AthCondSeq")
condSeq+=LArBadFebCondAlg(ReadKey=MissingFEBsFolder)
condSeq+=LArBadChannelCondAlg(ReadKey=BadChannelsFolder)


