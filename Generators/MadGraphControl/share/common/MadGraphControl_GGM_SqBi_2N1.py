# SUSY MadGraph+Pythia8 jobOptions for GGM squark-bino grid 
from MadGraphControl.MadGraphUtils import *
# ---------- Start of Pre-processes --------------------------
include("MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py")
# ----------- End of Pre-processes -------------------------

# ----------- Start of Main Processes ----------------------

if not hasattr(runArgs,'jobConfig') or len(runArgs.jobConfig)==0:
  log.error('No jobConfig set - cannot figure out configuration')

for q in squarks: masses[q] = float(runArgs.jobConfig[0].split('_')[5])
masses['1000022'] = float(runArgs.jobConfig[0].split('_')[6].split('.')[0])
if masses['1000022']<0.5: masses['1000022']=0.5

process = '''
generate p p > susysq susysq~ $ go susyweak @1
add process p p > susysq susysq~ j $ go susyweak @2
add process p p > susysq susysq~ j j $ go susyweak @3
'''
njets = 2
runArgs.EventMultiplier = 4

evgenConfig.contact  = [ "osamu.jinnouchi@cern.ch" ]
evgenConfig.keywords += ['SUSY','squark','bino']
evgenConfig.description = 'GGM squark-bino grid generation with m_sq = %s GeV, m_N1 = %s GeV ' % (masses[squarks[0]],masses['1000022'])

slha_file = 'susy.%s.sq_bino_%d_%d.slha' % (runArgs.runNumber,masses[squarks[0]],masses['1000022'])
log.info("Using paramCard %s" % slha_file)
import subprocess
proccard = subprocess.Popen(['get_files','-data',slha_file])
proccard.wait()

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{ul,1000002}{ul~,-1000002}{ur,2000002}{ur~,-2000002}{dl,1000001}{dl~,-1000001}{dr,2000001}{dr~,-2000001}{sl,1000003}{sl~,-1000003}{sr,2000003}{sr~,-2000003}{cl,1000004}{cl~,-1000004}{cr,2000004}{cr~,-2000004}{t1,1000006}{t1~,-1000006}{t2,2000006}{t2~,-2000006}{b1,1000005}{b1~,-1000005}{b2,2000005}{b2~,-2000005}"]

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
### set this to 1/4 of squark mass
qcut = float(masses[squarks[0]])*0.25

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
extras = { 'ktdurham':qcut , 'lhe_version':'2.0' , 'cut_decays':'F' , 'pdlabel':pdlabel , 'lhaid':lhaid , 'drjj':0.0, 'ickkw':0 }
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
evgenLog.info('Registered generation of GGM squark-bino grid')

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

# Clean up
del slha_file
