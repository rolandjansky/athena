

def setupMenuProvider():

    # after proof of concept this will go to new joboptions
    from AthenaCommon.AlgSequence import AthSequencer      
    condSequence = AthSequencer("AthCondSeq")             

    # L1 menu setup

    from TriggerJobOpts.TriggerFlags import TriggerFlags
    if TriggerFlags.readLVL1configFromXML():
        xmlFile = TriggerFlags.inputLVL1configFile()
    else:
        xmlFile = TriggerFlags.outputLVL1configFile()

    l1InputType = "file"
    l1JsonFile = "%s.json" % xmlFile.rsplit(".",1)[0]

    from TrigConfIO.TrigConfIOConf import TrigConf__L1MenuCondAlg
    condSequence += TrigConf__L1MenuCondAlg( InputType = l1InputType,
                                             JsonFileName = l1JsonFile
    )

    # this is a temporary measure until we have L1 menu json files being written out directly 
    from TrigConfIO.TransformXML2JSON import transformXML2Json
    transformXML2Json(xmlFile)
