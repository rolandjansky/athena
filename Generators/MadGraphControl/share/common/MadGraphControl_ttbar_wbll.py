from MadGraphControl.MadGraphUtils import *

nevents=int(1.1*runArgs.maxEvents)
mode=0
njobs=1
gridpack_mode=True
gridpack_dir='madevent/'

### DSID lists (extensions can include systematics samples)
ee=[410397]
mm=[410398]
tt=[410399]

if runArgs.runNumber in ee:
    lepstring = 'e+ e-'
elif runArgs.runNumber in mm:
    lepstring = 'mu+ mu-'
elif runArgs.runNumber in tt:
    lepstring = 'ta+ ta-'
else:
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)

fcard = open('proc_card_mg5.dat','w')
fcard.write("""
    import model sm-lepton_masses
    define p = g u c d s u~ c~ d~ s~
    define j = g u c d s u~ c~ d~ s~
    define all = u c d s u~ c~ d~ s~ e+ mu+ ta+ e- mu- ta- ve vm vt ve~ vm~ vt~
    define l+ = e+ mu+ ta+
    define l- = e- mu- ta-
    define vl = ve vm vt
    define vl~ = ve~ vm~ vt~
""")
fcard.write("""
    generate p p > t t~, t > l+ vl b %(lepstring)s, (t~ > w- b~, w- > all all)
    add process p p > t t~, t > j j b %(lepstring)s, (t~ > w- b~, w- > all all)
    add process p p > t t~, t~ > l- vl~ b~ %(lepstring)s, (t > w+ b, w+ > all all)
    add process p p > t t~, t~ > j j b~ %(lepstring)s, (t > w+ b, w+ > all all)
    output -f
""" % {'lepstring': lepstring})

fcard.close()

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")


runName='run_01'     


process_dir = new_process(grid_pack=gridpack_dir)
#process_dir='madevent/'

#Fetch default LO run_card.dat and set parameters
extras = {'lhe_version':'2.0', 
          'cut_decays' :'T', 
          'pdlabel'    : "'lhapdf'",
          'lhaid'      : 263000,
          'sys_pdf'    : 'NNPDF30_lo_as_0130_qed',
          'mmll'       : '1.0',
          'ptj'        : '0.0',
          'ptl'        : '0.0',
          'etaj'       : '-1.0',
          'etal'       : '-1.0',
          'drjj'       : '0.0',
          'drll'       : '0.0',
          'drjl'       : '0.0',
          }

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)

print_cards()
    
generate(run_card_loc='run_card.dat',param_card_loc=None,mode=mode,proc_dir=process_dir,run_name=runName,grid_pack=gridpack_mode,gridpack_dir=gridpack_dir,cluster_type='condor',cluster_queue='None',njobs=njobs,nevents=nevents,random_seed=runArgs.randomSeed,gridpack_compile=True)



outputDS=arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz',lhe_version=3,saveProcDir=True)  
   

evgenConfig.generators = ["MadGraph"]

############################
# Shower JOs will go here
#### Shower

evgenConfig.description = 'MadGraph_ttbar_rare_%s' % lepstring
evgenConfig.keywords+=['SM','ttbar']
evgenConfig.contact = ['ponyisi@cern.ch','narayan@cern.ch']
runArgs.inputGeneratorFile=outputDS


include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")

import os
if 'ATHENA_PROC_NUMBER' in os.environ:
    evgenLog.info('Noticed that you have run with an athena MP-like whole-node setup.  Will re-configure now to make sure that the remainder of the job runs serially.')
    njobs = os.environ.pop('ATHENA_PROC_NUMBER')
    # Try to modify the opts underfoot
    if not hasattr(opts,'nprocs'): 
        mglog.warning('Did not see option!')
    else: opts.nprocs = 0

include("Pythia8_i/Pythia8_MadGraph.py")


