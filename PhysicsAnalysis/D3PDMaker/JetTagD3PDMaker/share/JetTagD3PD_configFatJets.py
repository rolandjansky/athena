include.block("JetTagD3PDMaker/JetTagD3PD_configFatJets.py")

from JetRec.JetRecFlags import jetFlags
jetFlags.doBTagging = False

###
from HSG5DPDUtils.JSjets import getGroomingTools

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
