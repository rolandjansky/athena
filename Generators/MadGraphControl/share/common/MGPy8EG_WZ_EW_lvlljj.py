from MadGraphControl.MadGraphUtils import *

# General settings
nevents=6000
gridpack_dir=None
gridpack_mode=False
#runName='run_01'

### DSID lists (extensions can include systematics samples)
WZ_EW=[364499]

if runArgs.runNumber in WZ_EW:
    name='VBS_ew_lvlljj'
    process="pp>WZ>lvll~"

    gridpack_mode=True
    gridpack_dir='madevent/'

else: 
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)


stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(name)


fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model sm-no_b_mass
define p = g u c d s b u~ c~ d~ s~ b~
define j = g u c d s b u~ c~ d~ s~ b~
define l+ = e+ mu+ ta+
define l- = e- mu- ta-
generate p p > l+ l- l+ vl j j QCD=0 @1
add process p p > l+ l- l- vl~ j j QCD=0 @2
output -f
""")
fcard.close()


beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")


process_dir = new_process(grid_pack=gridpack_dir)

#Fetch default LO run_card.dat and set parameters
extras = {'lhe_version':'3.0', 
          'cut_decays' :'F', 
          'pdlabel'    : "'lhapdf'",
          'lhaid'      : 260000,
          'sys_pdf'    : 'NNPDF30_nlo_as_0118',
          'gridpack'   : 'T',
          'ptj'        : '10.0',
          'ptb'        : '10.0',
          'pta'        : '10.0',
          'ptl'        : '10.0',
          'etaj'       : '-1.0',
          'etab'       : '-1.0',
          'etal'       : '-1.0',
          'drjj'       : '0.2',
          'drbb'       : '0.2',
          'drll'       : '0.1',
          'draa'       : '0.4',
          'drbj'       : '0.1',
          'draj'       : '0.4',
          'drjl'       : '0.1',
          'dral'       : '0.4',
          'mmjj'       : '20.0',
          'mmbb'       : '20.0',
          'mmaa'       : '0.0',
          'mmll'       : '0.0',
          'mmnl'       : '10.0',
          'use_syst'   : 'T',
}


build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat', 
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)

print_cards()

runName='VBS_ew_lvlljj'

#madspin_card_loc='madspin_card.dat'                                                                                                                                    

#mscard = open(madspin_card_loc,'w')                                                                                                                                    
#mscard.write("""#************************************************************
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
# set max_weight_ps_point 400  # number of PS to estimate the maximum for each event
#
#set seed %i
# specify the decay for the final state particles
#decay t > w+ b, w+ > all all
#decay t~ > w- b~, w- > all all
#decay w+ > all all
#decay w- > all all
#decay z > all all
# running the actual code
#launch"""%runArgs.randomSeed)                                                                                                                                              
#mscard.close()

    
#print_cards()

   
generate(run_card_loc='run_card.dat',param_card_loc=None,mode=0,njobs=1,proc_dir=process_dir,run_name=runName,
         grid_pack=gridpack_mode,gridpack_dir=gridpack_dir,nevents=nevents,random_seed=runArgs.randomSeed)
#grid_pack=gridpack_mode,gridpack_dir=gridpack_dir,
#madspin_card_loc=madspin_card_loc,

#arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz',lhe_version=3,saveProcDir=True)
arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz')



############################
# Shower JOs will go here

import os
if 'ATHENA_PROC_NUMBER' in os.environ:
   njobs = os.environ.pop('ATHENA_PROC_NUMBER')
   # Try to modify the opts underfoot
   if not hasattr(opts,'nprocs'): print 'Warning: Did not see option!'
   else: opts.nprocs = 0
   print opts


include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")
#include("Pythia8_i/Pythia8_LHEF.py")
#include("Pythia8_i/Pythia8_EvtGen.py")

genSeq.Pythia8.Commands += [
    "Init:showAllParticleData = on",
    "Next:numberShowLHA = 10",
    "Next:numberShowEvent = 10"]
	
evgenConfig.generators = ["MadGraph", "Pythia8", "EvtGen"]
evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'
	
evgenConfig.contact = ['Louis Helary <Louis.Helary@cern.ch>','Benjamin Freund <benjamin.freund@cern.ch>']
#evgenConfig.keywords+=keyword
#evgenConfig.description = description	
evgenConfig.description = 'VBS_WZ'
evgenConfig.keywords+=['SM','VBS']
runArgs.inputGeneratorFile=name+'._00001.events.tar.gz'

