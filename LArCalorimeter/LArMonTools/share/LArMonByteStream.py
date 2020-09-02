
#
# Provides ByteStreamInputSvc name of the data file to process in the offline context
#
if not online:
    theByteStreamInputSvc = svcMgr.ByteStreamInputSvc
    svcMgr.EventSelector.Input = FullFileName
    svcMgr.EventSelector.MaxBadEvents = 0
else:
    theApp.CreateSvc += ["ByteStreamCnvSvc"]



#
# don't know if following lines working or not... should work with release 14 
#
#from LArByteStream.LArByteStreamConf import LArRodDecoder
#theLArRodDecoder = LArRodDecoder(name="LArRodDecoder",
#                                 IgnoreCheckFEBs = [ 0x38080000 ]
#                                 )



#
#  Read LAr Digit depending of the type of the run 
#
theByteStreamAddressProviderSvc = svcMgr.ByteStreamAddressProviderSvc
theByteStreamAddressProviderSvc.TypeNames += ["LArFebHeaderContainer/LArFebHeader"]
if Type == 'Pedestal' or Type == 'Cosmic':
    theByteStreamAddressProviderSvc.TypeNames +=["LArDigitContainer/LOW"]
    theByteStreamAddressProviderSvc.TypeNames +=["LArDigitContainer/MEDIUM"]
    theByteStreamAddressProviderSvc.TypeNames +=["LArDigitContainer/HIGH"]
else:
    if runAccumulator:
        include("LArCalibUtils/LArCalib_CalibrationPatterns.py")
    else:
        theByteStreamAddressProviderSvc.TypeNames +=["LArAccumulatedCalibDigitContainer/LOW"]
        theByteStreamAddressProviderSvc.TypeNames +=["LArAccumulatedCalibDigitContainer/MEDIUM"]
        theByteStreamAddressProviderSvc.TypeNames +=["LArAccumulatedCalibDigitContainer/HIGH"]

