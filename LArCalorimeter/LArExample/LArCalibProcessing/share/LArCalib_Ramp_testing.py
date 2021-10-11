## second instance of the validation tools to detect "bad" channels
theBadRamp.OutputLevel = INFO
theBadRamp.ListOfDevFEBs='BadRampFebs.txt'
theBadRamp.MsgLevelForDeviations=ERROR
theBadRamp.RampTolerance=["0.1,0.1,0.1"]
theBadRamp.RampToleranceFEB=["0.1,0.1,0.1"]
theBadRamp.RawRampTimeTolerance=20.
theBadRamp.UseCorrChannels=False

    

## first instance of the validation tools to detect "deviated" channels
theRampValidationAlg.OutputLevel = INFO
theRampValidationAlg.PatchMissingFEBs=True
theRampValidationAlg.ListOfDevFEBs='RampFebs.txt'
theRampValidationAlg.MsgLevelForDeviations=ERROR
theRampValidationAlg.RampTolerance=["0.1,0.1,0.1"]
theRampValidationAlg.RampToleranceFEB=["0.1,0.1,0.1"]
theRampValidationAlg.RawRampTimeTolerance=20.
theRampValidationAlg.UseCorrChannels=False

theLArRampPatcher.OutputLevel = INFO

svcMgr.MessageSvc.OutputLevel = WARNING
svcMgr.MessageSvc.defaultLimit = 9999999  # all messages

conddb.addOverride("/LAR/Align","LARAlign-UPD4-00")
conddb.addOverride("/LAR/LArCellPositionShift","LArCellPositionShift-ideal")
print(svcMgr.IOVDbSvc.Folders)
