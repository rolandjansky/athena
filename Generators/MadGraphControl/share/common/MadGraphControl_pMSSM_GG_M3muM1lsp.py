# SUSY MadGraph+Pythia8 jobOptions for pMSSM M3muM1lsp simulation
from MadGraphControl.MadGraphUtils import *

# Event multipliers for getting more events out of madgraph to feed through athena (esp. for filters)
evt_multiplier = 4.0

num_events=5000
if hasattr(runArgs,'maxEvents') and runArgs.maxEvents>0: num_events=runArgs.maxEvents
nevts=num_events*evt_multiplier

# in case someone needs to be able to keep the output directory for testing
keepOutput = False

if not hasattr(runArgs,'jobConfig') or len(runArgs.jobConfig)==0:
  log.error('No jobConfig set - cannot figure out configuration')

#naming examples: MC15.371200.MadGraphPythia8EvtGen_A14N_GG_pMSSM_M3muM1lsp_900_170
generatorType='GG'

process = '''
import model MSSM_SLHA2-full
define susystrong = go ul ur dl dr cl cr sl sr t1 t2 b1 b2 ul~ ur~ dl~ dr~ cl~ cr~ sl~ sr~ t1~ t2~ b1~ b2~
define susylq = ul ur dl dr cl cr sl sr
define susylq~ = ul~ ur~ dl~ dr~ cl~ cr~ sl~ sr~
define susysq = ul ur dl dr cl cr sl sr t1 t2 b1 b2
define susysq~ = ul~ ur~ dl~ dr~ cl~ cr~ sl~ sr~ t1~ t2~ b1~ b2~
generate p p > go go $ susysq susysq~ @1
add process p p > go go j $ susysq susysq~ @2
add process p p > go go j j $ susysq susysq~ @3
output -f
'''

njets=2

pdlabel='nn23lo1'
lhaid=247000

slha_file = 'susy.%s.pMSSM_M3muM1lsp.slha' % runArgs.runNumber
log.info("Using paramCard %s" % slha_file)
import subprocess
proccard = subprocess.Popen(['get_files','-data',slha_file])
proccard.wait()

#--------------------------------------------------------------
# Find Gluino Mass
#--------------------------------------------------------------
# Grab the param card and move the new masses into place
glMass=1000
include ( 'MadGraphControl/SUSYMetadata.py' )
(glMass,nuMass) = mass_extract(slha_file, ['1000021','1000023'])
mglog.info('gluino mass = '+glMass)

#--------------------------------------------------------------
# Beam energy
#--------------------------------------------------------------
### As suggested by MC generator experts
beamEnergy=4000
if hasattr(runArgs,'ecmEnergy'):
  beamEnergy = runArgs.ecmEnergy / 2.
else:
  raise RunTimeError("No center of mass energy found.")

#--------------------------------------------------------------
# xqcut
#--------------------------------------------------------------
### set this to 1/4 of gluino mass
xqcut = float(glMass)*0.25
qcut = xqcut

# Set random seed
rand_seed=1234
if hasattr(runArgs, "randomSeed"): rand_seed=runArgs.randomSeed

if not os.environ.has_key('MADGRAPH_DATA'):
    os.environ['MADGRAPH_DATA']=os.getcwd()
    mglog.warning('Setting your MADGRAPH_DATA environmental variable to the working directory')

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

# Generate the new process!
full_proc = """
import model MSSM_SLHA2-full
"""+helpful_definitions()+"""
# Specify process(es) to run

"""+process+"""
# Output processes to MadEvent directory
output -f
"""
thedir = new_process(card_loc=full_proc)
if 1==thedir:
    mglog.error('Error in process generation!')
mglog.info('Using process directory '+thedir)

shutil.move(slha_file,'original_param_card.dat')
update_param_card_blocks( process_dir=thedir , from_param_card='original_param_card.dat' , to_param_card=slha_file )
build_param_card(param_card_old=slha_file,param_card_new='param_card.dat')

# Grab the run card and move it into place
extras = { 'ktdurham':xqcut , 'lhe_version':'2.0' , 'cut_decays':'F' , 'pdlabel':pdlabel , 'lhaid':lhaid , 'drjj':0.0 , 'ickkw':0}
# Temporary solution to inconsistent weights, see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/SUSYMcRequestProcedure#AGENE_1542_Inconsistent_Weights
extras['event_norm']='sum'
extras['use_syst']='F'
build_run_card(run_card_old=get_default_runcard(proc_dir=thedir),run_card_new='run_card.dat',xqcut=0,
               nevts=nevts,rand_seed=rand_seed,beamEnergy=beamEnergy,extras=extras)

if generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name='Test',proc_dir=thedir):
    mglog.error('Error generating events!')

# Move output files into the appropriate place, with the appropriate name
the_spot = arrange_output(run_name='Test',proc_dir=thedir,outputDS='madgraph_OTF._00001.events.tar.gz')
if the_spot == '':
    mglog.error('Error arranging output dataset!')

mglog.info('Removing process directory...')
shutil.rmtree(thedir,ignore_errors=True)

mglog.info('All done generating events!!')

outputDS = the_spot

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
                                "Merging:Process = pp>{go,1000021}{go,1000021}"]


# Configuration for EvgenJobTransforms
#--------------------------------------------------------------
evgenLog.info('Registered generation of pMSSM M3muM1lsp')

joparts = runArgs.jobConfig[0].rstrip('.py').split('_')

evgenConfig.contact  = [ "christopher.young@cern.ch" ]
evgenConfig.keywords += ['SUSY','pMSSM','gluino']
evgenConfig.description = 'pMSSM scan of M3 and mu with M1 lsp of 60 GeV, M3 = %s GeV, mu = %s GeV'%(joparts[5],joparts[6])

evgenConfig.generators += ["EvtGen"]

if not hasattr(runArgs,'inputGeneratorFile'):
    print 'ERROR: something wasnt write in file name transfer from the fragment.'
    runArgs.inputGeneratorFile='madgraph.*._events.tar.gz'
evgenConfig.inputfilecheck = runArgs.inputGeneratorFile.split('._0')[0]

# Clean up
del slha_file

