evgenConfig.description = 'MadGraph5_aMC@NLO+Pythia8 bbA production'
evgenConfig.keywords    = [ 'Higgs', 'BSMHiggs', 'ZHiggs', 'bottom' ]
evgenConfig.contact     = [ 'Xiaohu.Sun@cern.ch' ]
evgenConfig.generators  = [ "aMcAtNlo", "Pythia8", "EvtGen"] 

evgenConfig.minevents = 5000

import math
import re

#parse channel, component and mass from joboption name
jo_name = runArgs.jobConfig[0]
if "llbb" in jo_name:
    channel = "llbb"
elif "vvbb" in jo_name:
    channel = "vvbb"
else:
    raise Exception("Unable to parse subchannel from joboption name.")
mA = int(jo_name.split("_")[3][2:])
mH = int(jo_name.split("_")[4][2:])
print "Setting mA {} mH {} channel {}".format(mA, mH, channel)

# factor of events more needed 
if runArgs.maxEvents > 0:
    nEvents = runArgs.maxEvents
else:
    nEvents = evgenConfig.minevents

# bbA has lower lepton pT than ggF, i.e. lower filter eff, use 2.0 intead of 1.4
if channel == "llbb":
    lhe_factor = 2.0
elif channel == "vvbb":
    lhe_factor = 2.0

nLHEvents = int(nEvents * lhe_factor)

# event generation and lhe manipulation
include("MadGraphControl/MadGraphControl_bbAZH4FS.py")

# Pythia fragmentation
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_aMcAtNlo.py")

genSeq.Pythia8.Commands += ["SpaceShower:pTmaxMatch = 1",
                            "SpaceShower:pTmaxFudge = 1",
                            "SpaceShower:MEcorrections = off",
                            "TimeShower:pTmaxMatch = 1",
                            "TimeShower:pTmaxFudge = 1",
                            "TimeShower:MEcorrections = off",
                            "TimeShower:globalRecoil = on",
                            "TimeShower:limitPTmaxGlobal = on",
                            "TimeShower:nMaxGlobalRecoil = 1",
                            "TimeShower:globalRecoilMode = 2",
                            "TimeShower:nMaxGlobalBranch = 1.",
                            "Check:epTolErr = 1e-2",
                            "SLHA:readFrom = 2", # take A0 from slha file - issues with Pythia readding lhe 
                            "SLHA:file = narrowWidth.slha", 
                            "35:m0 {}".format(mH), # H mass
                            "36:m0 {}".format(mA), # A mass
                            "23:onMode = off"] 

# Z decay
if channel == "llbb":
    genSeq.Pythia8.Commands += ["23:onIfAny = 11 13 15"]
elif channel == "vvbb":
    genSeq.Pythia8.Commands += ["23:onIfAny = 12 14 16"]


# set width for heavy Higgs
nwf=open('./narrowWidth.slha', 'w')
nwfinput = """#           PDG      WIDTH
DECAY   36  0.001
#          BR         NDA          ID1       ID2       ID3       ID4
1       2       23      35
#           PDG      WIDTH
DECAY   35  0.001
#          BR         NDA          ID1       ID2       ID3       ID4
1       2       -5      5
"""
nwf.write(nwfinput)
nwf.close()

# lepton pT filter
from GeneratorFilters.GeneratorFiltersConf import MultiElecMuTauFilter
filtSeq += MultiElecMuTauFilter("LepTauPtFilter")
filtSeq.LepTauPtFilter.IncludeHadTaus = False
filtSeq.LepTauPtFilter.NLeptons = 1
filtSeq.LepTauPtFilter.MinPt = 18000.
filtSeq.LepTauPtFilter.MaxEta = 5.
