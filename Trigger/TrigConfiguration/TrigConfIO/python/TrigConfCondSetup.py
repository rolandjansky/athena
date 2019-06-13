

def setupMenuProvider():

    from AthenaCommon.Logging import logging
    msg = logging.getLogger( 'TrigConfCondSetup' )

    # after proof of concept this will go to new joboptions
    from AthenaCommon.AlgSequence import AthSequencer      
    condSequence = AthSequencer("AthCondSeq")             

    # L1 menu setup
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    if TriggerFlags.readLVL1configFromXML():
        xmlFile = TriggerFlags.inputLVL1configFile()
    else:
        xmlFile = TriggerFlags.outputLVL1configFile()

    # this is a temporary measure until we have L1 menu json files being written out directly
    from TrigConfIO.TransformXML2JSON import transformXML2Json
    l1JsonFile = transformXML2Json(xmlFile)


    l1InputType = "file"
    from TrigConfIO.TrigConfIOConf import TrigConf__L1MenuCondAlg
    condSequence += TrigConf__L1MenuCondAlg( InputType = l1InputType,
                                             JsonFileName = l1JsonFile
    )
    msg.info("Configured L1MenuCondAlg with InputType=%s and JsonFileName=%s" % (l1InputType, l1JsonFile))

