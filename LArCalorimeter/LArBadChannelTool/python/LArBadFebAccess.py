

def LArBadFebAccess(algname="LArBadFebCondAlg", dbString=None):
    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")

    if hasattr(condSeq,algname):
        print "Access to bad Feb info already set up"
        return

    from IOVDbSvc.CondDB import conddb

    if dbString is not None:
        foldername=conddb.extractFolder(dbString)
        conddb.addFolder("",dbString,className='AthenaAttributeList')
    else:
        if conddb.isOnline:
            foldername="/LAR/BadChannels/MissingFEBs"
            conddb.addFolder("LAR",foldername,className='AthenaAttributeList')
        else:
            foldername="/LAR/BadChannelsOfl/MissingFEBs"
            conddb.addFolder("LAR_OFL",foldername,className='AthenaAttributeList')
            pass
        pass
    


    from LArBadChannelTool.LArBadChannelToolConf import LArBadFebCondAlg
    theLArBadFebCondAlg=LArBadFebCondAlg(algname)
    theLArBadFebCondAlg.ReadKey=foldername
    condSeq+=theLArBadFebCondAlg
    return
