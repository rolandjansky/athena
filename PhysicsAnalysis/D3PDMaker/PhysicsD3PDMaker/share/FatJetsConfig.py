include.block("PhysicsD3PDMaker/FatJetsConfig.py")

from JetRec.JetRecFlags import jetFlags
jetFlags.doBTagging = False
jetFlags.inputFileType = 'AOD'

preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())

###
from HSG5DPDUtils.JSjets import createJSJets,getGroomingTools

from HSG5DPDUtils.GroomedJetsConfig import getGroomedJetsConfig
dictsConfig = getGroomedJetsConfig()

sj_n = []
for dC in dictsConfig:
    myjetfinder = dC[0]["JetFinder"]
    myjetdr = dC[0]["JetdR"]
    myjetinput = dC[0]["JetInput"]
    myGroomingTools, catNames = getGroomingTools(dC[1])
    sj_n += [(myjetfinder+"%d"%(myjetdr*10) + myjetinput+x) for x in catNames if "Subjets" in x]
    
from BTagging.BTaggingFlags import BTaggingFlags
for sj in sj_n:
   BTaggingFlags.Jets += [sj]
   BTaggingFlags.JetsWithInfoPlus += [sj]
   BTaggingFlags.CalibrationChannelAliases += [sj+"->AntiKt4LCTopo" ]

include("BTagging/BTagging_LoadTools.py")

myJetKeys = []
for dC in dictsConfig:
    xx = createJSJets(dC[0], dC[1], preseq)
    myJetKeys += [xx]
