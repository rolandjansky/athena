

def LArBadChannelAccess(algname="LArBadChannelCondAlg",dbString=None):
    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")

    if hasattr(condSeq,algname):
        print "Access to bad Feb info already set up"
        return
    
    
    from IOVDbSvc.CondDB import conddb

    if dbString is not None:
        foldername=conddb.extractFolder(dbString)
        conddb.addFolder("",dbString,className="CondAttrListCollection")
    else:
        if conddb.isOnline:
            foldername="/LAR/BadChannels/BadChannels"
            conddb.addFolder("LAR",foldername,className="CondAttrListCollection")
        else:
            foldername="/LAR/BadChannelsOfl/BadChannels"
            conddb.addFolder("LAR_OFL",foldername,className="CondAttrListCollection")
            pass
        pass
    

    from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelCondAlg
    theLArBadChannelCondAlg=LArBadChannelCondAlg(algname)
    theLArBadChannelCondAlg.ReadKey=foldername
    condSeq+=theLArBadChannelCondAlg
    return



    



