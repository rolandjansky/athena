from LArROD.LArRODFlags import larRODFlags
from AthenaCommon.GlobalFlags import globalflags

def LArRawChannelBuilderDefault():
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr

    if larRODFlags.readDigits() and globalflags.InputFormat() == 'bytestream':

        if not "LArDigitContainer/FREE" in svcMgr.ByteStreamAddressProviderSvc.TypeNames:
            svcMgr.ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/FREE"]
        
        if not larRODFlags.keepDSPRaw():
            svcMgr.ByteStreamAddressProviderSvc.TypeNames += ["LArRawChannelContainer/LArRawChannels"]
            
        from LArRecUtils.LArADC2MeVCondAlgDefault import LArADC2MeVCondAlgDefault

        LArADC2MeVCondAlgDefault()

        from LArROD.LArRODConf import LArRawChannelBuilderAlg
        theLArRawChannelBuilder=LArRawChannelBuilderAlg()
        if larRODFlags.keepDSPRaw():
            theLArRawChannelBuilder.LArRawChannelKey=larRODFlags.RawChannelFromDigitsContainerName()
        topSequence += theLArRawChannelBuilder

        #Useless here but for backward compatiblity
        from AthenaCommon.AppMgr import ToolSvc
        from LArRecUtils.LArADC2MeVToolDefault import LArADC2MeVToolDefault
        theADC2MeVTool = LArADC2MeVToolDefault()
        ToolSvc += theADC2MeVTool
