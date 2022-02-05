# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

def LArBadChannelAccess(algname="LArBadChannelCondAlg",dbString=None):
    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")

    if hasattr(condSeq,algname):
        print ("Access to bad Cell info already set up")
        return


    from LArCabling.LArCablingAccess import LArOnOffIdMapping
    LArOnOffIdMapping()

    from IOVDbSvc.CondDB import conddb

    if dbString is not None:
        foldername=conddb.extractFolder(dbString)
        conddb.addFolder("",dbString,className="CondAttrListCollection")
    else:
        if conddb.isOnline:
            foldername="/LAR/BadChannels/BadChannels"
            if not conddb.folderRequested(foldername):
              conddb.addFolder("LAR",foldername,className="CondAttrListCollection")
        else:
            foldername="/LAR/BadChannelsOfl/BadChannels"
            if not conddb.folderRequested(foldername):
              conddb.addFolder("LAR_OFL",foldername,className="CondAttrListCollection")
            pass
        pass
    

    from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelCondAlg
    theLArBadChannelCondAlg=LArBadChannelCondAlg(algname)
    theLArBadChannelCondAlg.ReadKey=foldername
    condSeq+=theLArBadChannelCondAlg
    return



    



