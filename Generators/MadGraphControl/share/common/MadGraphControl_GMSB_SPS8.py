from MadGraphControl.MadGraphUtils import *

#TestHepMC max vtx
testSeq.TestHepMC.MaxVtxDisp = 1000*1000 #in mm
testSeq.TestHepMC.MaxTransVtxDisp = 1000*1000

# Event multipliers for getting more events out of madgraph to feed through athena (esp. for filters)
evt_multiplier = 2.0
if evt_multiplier>0:
    if runArgs.maxEvents>0:
        nevts=runArgs.maxEvents*evt_multiplier
    else:
        nevts=5000*evt_multiplier

keepOutput = False

splitjo = runArgs.jobConfig[0].rstrip('.py').split('_')
ID = runArgs.runNumber
Lambda = float(splitjo[4])
raw_lifetime = splitjo[5]
lifetime = float(raw_lifetime.replace('p','0.'))
slha_file = 'susy.%s.GMSB_SPS8_%s_%s.slha'  % (ID, int(Lambda), raw_lifetime)

process = '''
generate p p > x1+ x1- $ susystrong @1
add process p p > x1+ n2 $ susystrong @1
add process p p > x1- n2 $ susystrong @1
add process p p > x1+ x1- j $ susystrong @2
add process p p > x1+ n2 j $ susystrong @2
add process p p > x1- n2 j $ susystrong @2
'''
njets = 1
    
pdlabel='nn23lo1'
lhaid=247000

# Set beam energy
beamEnergy = 6500.
if hasattr(runArgs,'ecmEnergy'): beamEnergy = runArgs.ecmEnergy * 0.5

# Set random seed
rand_seed=1234
if hasattr(runArgs, "randomSeed"):
    # Add DSID to ensure a totally unique offset
    rand_seed = 1000*int( str(ID)[1:6] ) + runArgs.randomSeed

if not os.environ.has_key('MADGRAPH_DATA'):
    os.environ['MADGRAPH_DATA']=os.getcwd()
    mglog.warning('Setting your MADGRAPH_DATA environmental variable to the working directory')

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

# Generate the new process!
full_proc = """
import model mssm-full
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

# Grab the param card and move the new masses into place
from PyJobTransformsCore.trfutil import get_files
get_files(slha_file, keepDir=False, errorIfNotFound=True)

include ( 'MadGraphControl/SUSYMetadata.py' )
(m_neutralino, m_gravitino, m_chargino1, m_neutralino2) = mass_extract(slha_file, ['1000022', '1000039', '1000024', '1000023'])
mglog.info('chargino1 mass = '+m_chargino1+' neutralino2 mass = '+m_neutralino2)

build_param_card(param_card_old=slha_file,param_card_new='param_card.dat')

qcut=(abs(float(m_chargino1))+abs(float(m_neutralino2)))*0.125

xqcut=-1
if njets>0:
    xqcut=qcut
# Grab the run card and move it into place
extras = { 'ktdurham':xqcut , 'lhe_version':'2.0' , 'cut_decays':'F' , 'pdlabel':pdlabel , 'lhaid':lhaid , 'drjj':0.0 }
build_run_card(run_card_old=get_default_runcard(),run_card_new='run_card.dat',xqcut=qcut,
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

if qcut<0 or outputDS is None or ''==outputDS:
    evgenLog.warning('Looks like something went wrong with the MadGraph generation - bailing out!')
    raise RuntimeError('Error in MadGraph generation')

import os
if 'ATHENA_PROC_NUMBER' in os.environ:
    evgenLog.info('Noticed that you have run with an athena MP-like whole-node setup.  Will re-configure now to make sure that the remainder of the job runs serially.')
    njobs = os.environ.pop('ATHENA_PROC_NUMBER')
    # Try to modify the opts underfoot
    if not hasattr(opts,'nprocs'): mglog.warning('Did not see option!')
    else: opts.nprocs = 0
    print opts

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
                                "Merging:Process = pp>{x1+,1000024}{x1-,-1000024}{n2,1000023}",
                                "1000024:spinType = 1",
                                "1000023:spinType = 1"]


# Configuration for EvgenJobTransforms
#--------------------------------------------------------------
evgenLog.info('Registered generation of GMSB SPS8')

evgenConfig.contact  = [ "Kalliopi.Iordanidou@cern.ch" ]
evgenConfig.keywords += ['SUSY','GMSB','longLived']
evgenConfig.description = 'GMSB SPS8 model with Lambda = %s TeV, N1 lifetime = %s ns'%(Lambda,lifetime)

if not hasattr(runArgs,'inputGeneratorFile'):
    print 'ERROR: something wasnt write in file name transfer from the fragment.'
    runArgs.inputGeneratorFile='madgraph.*._events.tar.gz'
evgenConfig.inputfilecheck = runArgs.inputGeneratorFile.split('._0')[0]

# Clean up
del splitjo, m_neutralino, m_gravitino, m_chargino1, m_neutralino2, Lambda, raw_lifetime, lifetime, slha_file
