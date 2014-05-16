## second instance of the validation tools to detect "bad" channels 
theBadPedestal.OutputLevel = INFO
theBadPedestal.ListOfDevFEBs='BadPedestaldFebs.txt'
theBadPedestal.MsgLevelForDeviations=ERROR

theBadAutoCorr.OutputLevel = INFO
theBadAutoCorr.ListOfDevFEBs='BadAutoCorrFebs.txt'
theBadAutoCorr.MsgLevelForDeviations=ERROR

## first instance of the validation tools to detect "deviated" channels 
thePedestalValidationAlg.OutputLevel = INFO
thePedestalValidationAlg.PatchMissingFEBs=True
thePedestalValidationAlg.UseCorrChannels=False
thePedestalValidationAlg.ListOfDevFEBs='PedestaldFebs.txt'
thePedestalValidationAlg.MsgLevelForDeviations=ERROR

theAutoCorrValidationAlg.OutputLevel = INFO
theAutoCorrValidationAlg.PatchMissingFEBs=True
theAutoCorrValidationAlg.UseCorrChannels=False
theAutoCorrValidationAlg.ListOfDevFEBs='AutoCorrFebs.txt'
theAutoCorrValidationAlg.MsgLevelForDeviations=ERROR

svcMgr.MessageSvc.OutputLevel = WARNING
svcMgr.MessageSvc.defaultLimit = 9999999  # all messages


