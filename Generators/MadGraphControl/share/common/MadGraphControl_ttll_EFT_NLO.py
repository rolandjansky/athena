from MadGraphControl.MadGraphUtils import *

# General settings
nevents=5500
mode=0

### DSID lists (extensions can include systematics samples)

ttee_EFT    = [410290,410291,410292,410293,410294,410295,410296,410297,410298,410299,410300]
ttmm_EFT    = [410301,410302,410303,410304,410305,410306,410307,410308,410309,410310,410311]
tttata_EFT  = [410312,410313,410314,410315,410316,410317,410318,410319,410320,410321,410322]

if runArgs.runNumber in ttee_EFT:
    name='ttee_NLO_EFT'
elif runArgs.runNumber in ttmm_EFT:
    name='ttmm_NLO_EFT'
elif runArgs.runNumber in tttata_EFT:
    name='tttata_NLO_EFT'
else:
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)

############
#EVGEN Configuration
############
evgenConfig.description = 'aMcAtNlo_'+str(name)
evgenConfig.keywords+=['ttZ','2lepton']
evgenConfig.contact = ["olga.bylund@cern.ch"]
# String to check the input LHE dataset name eg DSID.name_eft
lhecheckstring = str(runArgs.runNumber)+"."+name.split("_")[0]+"_eft"
evgenConfig.inputfilecheck = lhecheckstring
evgenConfig.generators += ["aMcAtNlo","Pythia8"]

#############
#Pythia8 shower
#############
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_aMcAtNlo.py")
