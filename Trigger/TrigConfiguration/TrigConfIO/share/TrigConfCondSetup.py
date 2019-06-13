

# after proof of concept this will go to new joboptions
from AthenaCommon.AlgSequence import AthSequencer      
condSequence = AthSequencer("AthCondSeq")             



# L1 menu setup

l1InputType = "none"
l1JsonFile = ""

if TriggerFlags.readLVL1configFromXML():
    l1InputType = "file"
    l1JsonFile = "%s.json" % TriggerFlags.inputLVL1configFile().rsplit(".",1)[0]


from TrigConfIO.TrigConfIOConf import TrigConf__L1MenuCondAlg
condSequence += TrigConf__L1MenuCondAlg( InputType = l1InputType,
                                         JsonFileName = l1JsonFile
)


