from MadGraphControl.MadGraphUtils import *

# General settings
minevents=5000
nevents=30000
mode=0

# MG Particle cuts
mllcut=-1

# MG merging settings
maxjetflavor=5
ickkw=0


# Pythia8 merging settings
nJetMax=2
ktdurham=30
dparameter=0.4


### DSID lists (extensions can include systematics samples)
ttZllonshell_Np0  =[410069]
ttZllonshell_Np1  =[410070]

ttZlloffshell_Np0  =[410071]
ttZlloffshell_Np1  =[410072]

ttZnnqq_Np0        =[410073,410190,410191]
ttZnnqq_Np1        =[410074,410192,410193]
ttZnnqq_Np2        =[410075,410194,410195]
                                         
ttW_Np0            =[410066,410196,410197,421108]
ttW_Np1            =[410067,410198,410199]
ttW_Np2            =[410068,410200,410201]
                                         
ttee_5fl_Np0       =[410111,410202,410203]
ttee_5fl_Np1       =[410112,410204,410205]
ttmumu_5fl_Np0     =[410113,410206,410207]
ttmumu_5fl_Np1     =[410114,410208,410209]
tttautau_5fl_Np0   =[410115,410210,410211]
tttautau_5fl_Np1   =[410116,410212,410213]


systScalefactUp=[410190,
                 410192,
                 410194,
                 410196,
                 410198,
                 410200,
                 410202,
                 410204,
                 410206,
                 410208,
                 410210,
                 410212]

systScalefactDn=[410191,
                 410193,
                 410195,
                 410197,
                 410199,
                 410201,
                 410203,
                 410205,
                 410207,
                 410209,
                 410211,
                 410213]

if hasattr(runArgs,'runNumber'):
   runNum = int(runArgs.runNumber)
else:
   parDSID = str(runArgs.jobConfig[0])
   if len(parDSID)==6 and parDSID.isdigit():
      runNum = int(parDSID)
   else:
      runNum = int(parDSID[-6:])

if runNum in ttZllonshell_Np0:
    mgproc="""generate p p > t t~ z, z > l+ l-"""
    name='ttllonshell_Np0'
    mllcut=10
    process="pp>tt~z"
    keyword=['SM','ttZ','2lepton'] 
    nJetMax=1
elif runNum in ttZllonshell_Np1:
    mgproc="""generate p p > t t~ z j, z > l+ l-"""
    name='ttllonshell_Np1'
    mllcut=10
    process="pp>tt~z"
    keyword=['SM','ttZ','2lepton'] 
    nJetMax=1

elif runNum in ttZlloffshell_Np0:
    mgproc="""generate p p > t t~ l+ l- $ z h"""
    name='ttlloffshell_Np0'
    mllcut=10
    process="pp>tt~LEPTONS,NEUTRINOS"
    keyword=['SM','ttZ','2lepton'] 
    nJetMax=1
elif runNum in ttZlloffshell_Np1:
    mgproc="""generate p p > t t~ l+ l- j $ z h"""
    name='ttlloffshell_Np1'
    mllcut=10
    process="pp>tt~LEPTONS,NEUTRINOS"
    keyword=['SM','ttZ','2lepton'] 
    nJetMax=1

elif runNum in ttZnnqq_Np0:
    mgproc="""generate p p > t t~ z, z > zonshell zonshell~"""
    name='ttZnnqq_Np0'
    process="pp>tt~z"
    nJetMax=2
    keyword=['SM','ttZ','neutrino','quark'] 
elif runNum in ttZnnqq_Np1:
    mgproc="""generate p p > t t~ z j, z > zonshell zonshell~"""
    name='ttZnnqq_Np1'
    process="pp>tt~z"
    nJetMax=2
    keyword=['SM','ttZ','neutrino','quark'] 
elif runNum in ttZnnqq_Np2:
    mgproc="""generate p p > t t~ z j j, z > zonshell zonshell~"""
    name='ttZnnqq_Np2'
    process="pp>tt~z"
    nJetMax=2
    minevents=1000
    nevents=5000
    keyword=['SM','ttZ','neutrino','quark'] 



elif runNum in ttW_Np0:
    mgproc="""generate p p > t t~ w"""
    name='ttW_Np0'
    process="pp>tt~W"
    nJetMax=2
    keyword=['SM','ttW'] 
elif runNum in ttW_Np1:
    mgproc="""generate p p > t t~ w j"""
    name='ttW_Np1'
    process="pp>tt~W"
    nJetMax=2
    keyword=['SM','ttW'] 
elif runNum in ttW_Np2:
    mgproc="""generate p p > t t~ w j j"""
    name='ttW_Np2'
    process="pp>tt~W"
    nJetMax=2
    keyword=['SM','ttW'] 



elif runNum in ttee_5fl_Np0:
    mgproc="""generate p p > t t~ e+ e-"""
    name='ttee_Np0'
    mllcut=5
    process="pp>tt~e+e-"
    nJetMax=1
    keyword=['SM','ttZ'] 
elif runNum in ttee_5fl_Np1:
    mgproc="""generate p p > t t~ e+ e- j"""
    name='ttee_Np1'
    mllcut=5
    process="pp>tt~e+e-"
    nJetMax=1
    keyword=['SM','ttZ'] 


elif runNum in ttmumu_5fl_Np0:
    mgproc="""generate p p > t t~ mu+ mu-"""
    name='ttmumu_Np0'
    mllcut=5
    process="pp>tt~mu+mu-"
    nJetMax=1
    keyword=['SM','ttZ'] 
elif runNum in ttmumu_5fl_Np1:
    mgproc="""generate p p > t t~ mu+ mu- j"""
    name='ttmumu_Np1'
    mllcut=5
    process="pp>tt~mu+mu-"
    nJetMax=1
    keyword=['SM','ttZ'] 


elif runNum in tttautau_5fl_Np0:
    mgproc="""generate p p > t t~ ta+ ta- $ h"""
    name='tttautau_Np0'
    mllcut=5
    process="pp>tt~ta+ta-"
    nJetMax=1
    keyword=['SM','ttZ'] 
elif runNum in tttautau_5fl_Np1:
    mgproc="""generate p p > t t~ ta+ ta- j $ h"""
    name='tttautau_Np1'
    mllcut=5
    process="pp>tt~ta+ta-"
    nJetMax=1
    keyword=['SM','ttZ'] 



else: 
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runNum)


stringy = 'madgraph.'+str(runNum)+'.MadGraph_'+str(name)


fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model sm-no_b_mass
define p = g u c d s b u~ c~ d~ s~ b~
define j = g u c d s b u~ c~ d~ s~ b~
define w = w+ w-
define q = u c d s b
define q~ = u~ c~ d~ s~ b~
define zonshell = u c d s b vl
define zonshell~ = u~ c~ d~ s~ b~ vl~
"""+mgproc+"""
output -f
""")
fcard.close()


beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")




pdflabel="nn23lo1"

#Fetch default LO run_card.dat and set parameters
extras = { 'lhe_version'  : '3.0',
           'cut_decays'   : 'F', 
           'pdlabel'      : "'"+pdflabel+"'",
           'maxjetflavor' : maxjetflavor,
           'asrwgtflavor' : maxjetflavor,
           'ickkw'        : 0,
           'ptj'          : 20,
           'ptb'          : 20,
           'mmll'         : mllcut,      
           'mmjj'         : 0,
           'drjj'         : 0,
           'drll'         : 0,
           'drjl'         : 0.4,
           'ptl'          : 0,
           'etal'         : 10,
           'etab'         : 6,
           'etaj'         : 6,
           'ktdurham'     : ktdurham,    
           'dparameter'   : dparameter  }



scalefact = 1.0
if runNum in systScalefactUp:
    name=name+'_scaleUp'
    scalefact = 2.0
elif runNum in systScalefactDn:
    name=name+'_scaleDown'
    scalefact = 0.5




process_dir = new_process()

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat', 
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,xqcut=0.,
               extras=extras,scalefact=scalefact)

print_cards()

generate(run_card_loc='run_card.dat',param_card_loc=None,mode=mode,proc_dir=process_dir)
arrange_output(proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz')



#### Shower 
evgenConfig.description = 'MadGraph_'+str(name)
evgenConfig.keywords+=keyword 
evgenConfig.inputfilecheck = stringy
evgenConfig.minevents = minevents
runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")


PYTHIA8_TMS=ktdurham
PYTHIA8_nJetMax=nJetMax
PYTHIA8_Dparameter=dparameter
PYTHIA8_Process=process
PYTHIA8_nQuarksMerge=maxjetflavor
include("Pythia8_i/Pythia8_CKKWL_kTMerge.py")
genSeq.Pythia8.Commands+=["Merging:mayRemoveDecayProducts=on"]

