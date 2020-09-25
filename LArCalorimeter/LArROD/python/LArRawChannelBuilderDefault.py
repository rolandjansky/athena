# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from LArROD.LArRODFlags import larRODFlags
from AthenaCommon.GlobalFlags import globalflags
from LArByteStream.LArByteStreamConf import LArRawDataReadingAlg

def LArRawChannelBuilderDefault():
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()

    if larRODFlags.readDigits() and globalflags.InputFormat() == 'bytestream':
        if LArRawDataReadingAlg() not in topSequence:
            print ("Adding LArRawDataReaderAlg")
            topSequence+=LArRawDataReadingAlg()

        from LArRecUtils.LArADC2MeVCondAlgDefault import LArADC2MeVCondAlgDefault

        LArADC2MeVCondAlgDefault()

        from LArROD.LArRODConf import LArRawChannelBuilderAlg
        theLArRawChannelBuilder=LArRawChannelBuilderAlg()
        if larRODFlags.keepDSPRaw():
            theLArRawChannelBuilder.LArRawChannelKey=larRODFlags.RawChannelFromDigitsContainerName()

        obj = "AthenaAttributeList"
        db = 'LAR_ONL'
        if globalflags.DataSource() == 'data':
            from IOVDbSvc.CondDB import conddb
            if conddb.GetInstance() == 'COMP200':
                fld='/LAR/Configuration/DSPThreshold/Thresholds'
                theLArRawChannelBuilder.Run1DSPThresholdsKey='LArDSPThresholds'
                obj='LArDSPThresholdsComplete'
            else:
                fld="/LAR/Configuration/DSPThresholdFlat/Thresholds"
                theLArRawChannelBuilder.Run2DSPThresholdsKey=fld
        else:   
           fld="/LAR/NoiseOfl/DSPThresholds"
           theLArRawChannelBuilder.Run2DSPThresholdsKey=fld
           db = 'LAR_OFL'

        from IOVDbSvc.CondDB import conddb
        conddb.addFolder (db, fld, className=obj)

        topSequence += theLArRawChannelBuilder

        #Useless here but for backward compatiblity
        #from AthenaCommon.AppMgr import ToolSvc
        #from LArRecUtils.LArADC2MeVToolDefault import LArADC2MeVToolDefault
        #theADC2MeVTool = LArADC2MeVToolDefault()
        #ToolSvc += theADC2MeVTool
