from MadGraphControl.MadGraphUtils import *

# General settings
nevents=int(1.1*runArgs.maxEvents)
mode=0
gridpack_mode=False
gridpack_dir='madevent/'

# MG Particle cuts
mllcut=-1
ptgmincut=20
# Shower/merging settings
maxjetflavor=5
parton_shower='PYTHIA8'

### DSID lists (extensions can include systematics samples)

ttee           = [410218,410370,410371,410276]
ttmumu         = [410219,410372,410373,410277]
tttautau       = [429804,410220,410374,410375,410278]

ttZnunu        = [429805,410156,410378,410379]
ttZqq          = [410157,410380,410381]
ttgamma        = [407320]

ttW            = [429803,410155,410376,410377]

a14Var3Up      = [410370,410372,410374,410376,410378,410380]
a14Var3Down    = [410371,410373,410375,410377,410379,410381]

mll_1_5GeV       = [410276,410277,410278]

zdecay="decay z > all all"

if runArgs.runNumber in ttee:
    mgproc="""generate p p > t t~ e+ e- [QCD]"""
    name='ttee_NLO'
    if runArgs.runNumber in mll_1_5GeV:
        mllcut=1
    else:
        mllcut=5
    keyword=['SM','ttZ','2lepton']
elif runArgs.runNumber in ttmumu:
    mgproc="""generate p p > t t~ mu+ mu- [QCD]"""
    name='ttmumu_NLO'
    if runArgs.runNumber in mll_1_5GeV:
        mllcut=1
    else:
        mllcut=5
    keyword=['SM','ttZ','2lepton']
elif runArgs.runNumber in tttautau:
    mgproc="""generate p p > t t~ ta+ ta- $$ h [QCD]"""
    name='tttautau_NLO'
    if runArgs.runNumber in mll_1_5GeV:
        mllcut=1
    else:
        mllcut=5
    keyword=['SM','ttZ','2lepton']
elif runArgs.runNumber in ttZnunu:
    mgproc="""generate p p > t t~ z [QCD]"""
    name='ttZqq_NLO'
    keyword=['SM','ttZ']
    zdecay="decay z > vl vl~"
elif runArgs.runNumber in ttZqq:
    mgproc="""generate p p > t t~ z [QCD]"""
    name='ttZnunu_NLO'
    keyword=['SM','ttZ']
    zdecay="decay z > q q~"
elif runArgs.runNumber in ttW:
    mgproc="""generate p p > t t~ w [QCD]"""
    name='ttW_NLO'
    keyword=['SM','ttW']
elif runArgs.runNumber in ttgamma:
    mgproc="""generate p p > t t~ a [QCD]"""
    name='ttgamma_NLO'
    keyword=['SM','ttV','photon']

else:
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)


stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(name)


fcard = open('proc_card_mg5.dat','w')

fcard.write("""
import model loop_sm-no_b_mass
define p = g u c d s b u~ c~ d~ s~ b~
define j = g u c d s b u~ c~ d~ s~ b~
define w = w+ w-
define l+ = e+ mu+ ta+
define l- = e- mu- ta-
"""+mgproc+"""
output -f
""")
fcard.close()


beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    raise RuntimeError("No center of mass energy found.")


lhaid=260000
pdflabel='lhapdf'

#Fetch default LO run_card.dat and set parameters
extras = { 'lhaid'         : lhaid,
           'pdlabel'       : "'"+pdflabel+"'",
           'maxjetflavor'  : maxjetflavor,
           'parton_shower' : parton_shower,
           'ptgmin': ptgmincut,
           'mll_sf'        : mllcut}
doSyst=True

if doSyst:
    lhe_version=3
    extras.update({'reweight_scale': '.true.',
                   'rw_Rscale_down':  0.5,
                   'rw_Rscale_up'  :  2.0,
                   'rw_Fscale_down':  0.5,
                   'rw_Fscale_up'  :  2.0,
                   'reweight_PDF'  : '.true.',
                   'PDF_set_min'   : 260001,
                   'PDF_set_max'   : 260100})
else:
    lhe_version=1
    extras.update({'reweight_scale': '.false.',
                   'reweight_PDF'  : '.false.'})



madspin_card_loc='madspin_card.dat'

mscard = open(madspin_card_loc,'w')
mscard.write("""#************************************************************
#*                        MadSpin                           *
#*                                                          *
#*    P. Artoisenet, R. Frederix, R. Rietkerk, O. Mattelaer *
#*                                                          *
#*    Part of the MadGraph5_aMC@NLO Framework:              *
#*    The MadGraph5_aMC@NLO Development Team - Find us at   *
#*    https://server06.fynu.ucl.ac.be/projects/madgraph     *
#*                                                          *
#************************************************************
#Some options (uncomment to apply)
#
# set seed 1
# set Nevents_for_max_weigth 75 # number of events for the estimate of the max. weight
# set BW_cut 15                # cut on how far the particle can be off-shell
 set max_weight_ps_point 400  # number of PS to estimate the maximum for each event
#
set seed %i
# specify the decay for the final state particles
define q = u d s c b
define q~ = u~ d~ s~ c~ b~
decay t > w+ b, w+ > all all
decay t~ > w- b~, w- > all all
decay w+ > all all
decay w- > all all
%s
# running the actual code
launch"""%(runArgs.randomSeed,zdecay))
mscard.close()



process_dir = new_process(grid_pack=gridpack_dir)

# set shower starting scale to old scale s hat
mccounter_path=os.path.join(process_dir, 'SubProcesses/montecarlocounter.f')
mccounter_file=open(mccounter_path,'r')
mccounter_content=mccounter_file.read().replace('parameter(i_scale=1)','parameter(i_scale=0)')
mccounter_file.close()
mccounter_file=open(mccounter_path,'w')
mccounter_file.write(mccounter_content)
mccounter_file.close()


build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,xqcut=0.,
               extras=extras)

get_param_file = subprocess.Popen(['get_files','-data', 'aMcAtNlo_param_card_loop_sm-no_b_mass.dat'])
get_param_file.wait()
import os, shutil
if not os.path.exists('aMcAtNlo_param_card_loop_sm-no_b_mass.dat'):
    raise RuntimeError("Cannot find aMcAtNlo_param_card_loop_sm-no_b_mass.dat")

if runArgs.runNumber in mll_1_5GeV:
    get_cuts_file = subprocess.Popen(['get_files','-data','MadGraphControl_ttV_lowmass_cuts.f'])
    get_cuts_file.wait()
    import os, shutil
    if not os.path.exists('MadGraphControl_ttV_lowmass_cuts.f'):
        raise RuntimeError("Cannot find MadGraphControl_ttV_lowmass_cuts.f")
    shutil.copy('MadGraphControl_ttV_lowmass_cuts.f', os.path.join(process_dir, 'SubProcesses/cuts.f'))
    gridpack_mode = True

print_cards()

generate(run_card_loc='run_card.dat',param_card_loc='aMcAtNlo_param_card_loop_sm-no_b_mass.dat',mode=mode,proc_dir=process_dir,madspin_card_loc=madspin_card_loc,grid_pack=gridpack_mode,gridpack_dir=gridpack_dir,gridpack_compile=True,random_seed=runArgs.randomSeed,nevents=nevents)
outputDS=arrange_output(proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz',lhe_version=lhe_version)



#### Shower
evgenConfig.description = 'aMcAtNlo_'+str(name)
evgenConfig.keywords+=keyword
evgenConfig.contact = ['mcfayden@cern.ch', 'ponyisi@cern.ch']
runArgs.inputGeneratorFile=outputDS


if runArgs.runNumber in a14Var3Up:
    include("Pythia8_i/Pythia8_A14_NNPDF23LO_Var3cUp_EvtGen_Common.py")
    evgenConfig.description += "A14 var3c up"
elif runArgs.runNumber in a14Var3Down:
    include("Pythia8_i/Pythia8_A14_NNPDF23LO_Var3cDown_EvtGen_Common.py")
    evgenConfig.description += "A14 var3c down"
else:
    include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")

import os
if 'ATHENA_PROC_NUMBER' in os.environ:
    evgenLog.info('Noticed that you have run with an athena MP-like whole-node setup.  Will re-configure now to make sure that the remainder of the job runs serially.')
    njobs = os.environ.pop('ATHENA_PROC_NUMBER')
    # Try to modify the opts underfoot
    if not hasattr(opts,'nprocs'): 
        mglog.warning('Did not see option!')
    else: opts.nprocs = 0
    print opts

include("Pythia8_i/Pythia8_aMcAtNlo.py")
