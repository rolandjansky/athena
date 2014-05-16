BadChannelsFolder="/LAR/BadChannelsOfl/BadChannels"
MissingFEBsFolder="/LAR/BadChannelsOfl/MissingFEBs"

from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool
    
theLArBadChanTool= LArBadChanTool(CoolFolder=BadChannelsFolder,
                                  CoolMissingFEBsFolder=MissingFEBsFolder,
                                  )

ToolSvc+=theLArBadChanTool

