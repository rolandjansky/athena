#-----------------------------------------------------------

from TrigMinBiasNtuple.TrigMinBiasNtupleConf import TrigMinBiasNtupleAlg
TrigMinBiasNtuple = TrigMinBiasNtupleAlg ()

TrigMinBiasNtuple.ntupleFile = TriggerFlags.NtupleProductionFlags.FileNames.forSlice('MinBias')


# A string vector enable and configure Ntuple components
#  "NtComponent[,Class:key][,opt:Option]"
# where ',' is used to separate the items and [] are optional.
TrigMinBiasNtuple.ntupleComponents = [#
    # components for Tier0
    "MbtsContainer",
    "BcmRdo",
    "CtpRdo",
    "RawLucid",
    "ZdcDigits", 
    "MbtsLvl2",
    "SpLvl2",
    "TrkCntsEf",
    "VxCntsEf",
    "TrtLvl2"
    ]

if globalflags.DataSource == 'data':
    TrigMinBiasNtuple.ntupleComponents += ["EventInfo,EventInfo:ByteStreamEventInfo"]
else:
    TrigMinBiasNtuple.ntupleComponents += ["EventInfo"]

topSequence += TrigMinBiasNtuple
print          TrigMinBiasNtuple

theApp.HistogramPersistency = "ROOT"

