# SUSY MadGraph+Pythia8 jobOptions for GGM gluino-bino grid 
from MadGraphControl.MadGraphUtils import *
# ---------- Start of Pre-processes --------------------------
runArgs.EventMultiplier = 4
include("MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py")
njets = 2
# ----------- End of Pre-processes -------------------------

# ----------- Stard of Main Processes ----------------------

if not hasattr(runArgs,'jobConfig') or len(runArgs.jobConfig)==0:
  log.error('No jobConfig set - cannot figure out configuration')

masses['1000021'] = float(runArgs.jobConfig[0].split('_')[6])
masses['1000022'] = float(runArgs.jobConfig[0].split('_')[7].split('.')[0])
if masses['1000022']<0.5: masses['1000022']=0.5

process = '''
generate p p > go go $ susysq susysq~ @1
add process p p > go go j $ susysq susysq~ @2
add process p p > go go j j $ susysq susysq~ @3
'''

evgenConfig.contact  = [ "osamu.jinnouchi@cern.ch" ]
evgenConfig.keywords += ['SUSY','gluino','bino']
evgenConfig.description = 'GGM gluino-bino grid generation with m_glu = %s GeV, m_N1 = %s GeV ' % (masses['1000021'],masses['1000022'])

if runArgs.jobConfig[0].split('_')[8].split('.')[0]=="0p5":
  slha_file = 'susy.%s.gl_bino_tau_%d_%d_0p5.slha' % (runArgs.runNumber,masses['1000021'],masses['1000022'])
else:
  lft = runArgs.jobConfig[0].split('_')[8].split('.')[0]
  slha_file = 'susy.%s.gl_bino_tau_%d_%d_%s.slha' % (runArgs.runNumber,masses['1000021'],masses['1000022'],lft)

log.info("Using paramCard %s" % slha_file)

import subprocess
proccard = subprocess.Popen(['get_files','-data',slha_file])
proccard.wait()

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{go,1000021}{go,1000021}"]

# --------------------- End of main processes --------------
#--------------------------------------------------------------
# Beam energy
#--------------------------------------------------------------
### As suggested by MC generator experts
beamEnergy=6500
if hasattr(runArgs,'ecmEnergy'):
  beamEnergy = runArgs.ecmEnergy / 2.
else:
  raise RunTimeError("No center of mass energy found.")

#--------------------------------------------------------------
# xqcut
#--------------------------------------------------------------
### set this to 1/4 of gluino mass
qcut = float(masses['1000021'])*0.25

# Set random seed
rand_seed=1234
if hasattr(runArgs, "randomSeed"): rand_seed=runArgs.randomSeed

full_proc = """
import model mssm
"""+helpful_definitions()+"""
# Specify process(es) to run

"""+process+"""
# Output processes to MadEvent directory
output -f
"""
thedir = new_process(card_loc=full_proc)
mglog.info('Using process directory '+thedir)

pdlabel='nn23lo1'
lhaid=247000

# Grab the run card and move it into place
extras = { 'ktdurham':qcut , 'lhe_version':'2.0' , 'cut_decays':'F' , 'pdlabel':pdlabel , 'lhaid':lhaid , 'drjj':0.0 , 'ickkw': 0 }
build_run_card(run_card_old=get_default_runcard(),run_card_new='run_card.dat',xqcut=qcut,nevts=nevts,rand_seed=rand_seed,beamEnergy=beamEnergy,extras=extras)

if generate(run_card_loc='run_card.dat',param_card_loc=slha_file,mode=0,njobs=1,run_name='Test',proc_dir=thedir):
    mglog.error('Error generating events!')

# Move output files into the appropriate place, with the appropriate name
outputDS = arrange_output(run_name='Test',proc_dir=thedir,outputDS='madgraph_OTF._00001.events.tar.gz',saveProcDir=keepOutput)
if outputDS == '':
    mglog.error('Error arranging output dataset!')

runArgs.qcut = qcut
runArgs.inputGeneratorFile = outputDS

# Pythia8 setup
genSeq.Pythia8.Commands += ["Init:showAllParticleData = on",
                            "Next:numberShowLHA = 10",
                            "Next:numberShowEvent = 10",
                            ]
if njets>0:
    genSeq.Pythia8.Commands += ["Merging:mayRemoveDecayProducts = on",
                                "Merging:nJetMax = "+str(njets),
                                "Merging:doKTMerging = on",
                                "Merging:TMS = "+str(qcut),
                                "Merging:ktType = 1",
                                "Merging:Dparameter = 0.4",
                                "Merging:nQuarksMerge = 4"]

# Configuration for EvgenJobTransforms
#--------------------------------------------------------------
evgenLog.info('Registered generation of GGM gluino-bino grid')

evgenConfig.generators += ["EvtGen"]

evgenConfig.inputfilecheck = runArgs.inputGeneratorFile.split('._0')[0]

from GeneratorFilters.GeneratorFiltersConf import ParticleFilter
filtSeq += ParticleFilter("ParticleFilter")
filtSeq.ParticleFilter.Ptcut = 0.
filtSeq.ParticleFilter.Etacut = 10.0
filtSeq.ParticleFilter.StatusReq = -1
filtSeq.ParticleFilter.PDG = 1000022
filtSeq.ParticleFilter.MinParts = 2
filtSeq.ParticleFilter.Exclusive = True
testSeq.TestHepMC.MaxVtxDisp = 10000*10000 #in mm
testSeq.TestHepMC.MaxTransVtxDisp = 10000*10000
# Clean up
del slha_file

