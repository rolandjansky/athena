from MadGraphControl.MadGraphUtils import *
import math
import os

# Compute decay widths as a function of m_taup for use in param card
def taup_decay_width(m_taup):
    epsilon=0.04
    mZ=91.187
    mH=125.1
    rz=mZ**2 / m_taup**2
    rh=mH**2 / m_taup**2

    decaywidth = (1-rz)**2 * (2+1/rz) * rz
    decaywidth += (1-rh)**2
    decaywidth *= m_taup * epsilon**2
    decaywidth /= 64 * math.pi
    return decaywidth

def vtaup_decay_width(m_taup):
    epsilon=0.04
    mW=79.956
    rw=mW**2 / m_taup**2

    decaywidth = m_taup * (1-rw)**2 * (2+1/rw) * epsilon**2 * rw / (32*math.pi)
    return decaywidth

# Define constants
minevents=5000
mode=0

dsid_to_mtaup = { 999993 : 100,
                 999998 : 130,
                 999999 : 200,
                 310515 : 300,
                 999996 : 400,
                 999995 : 500,
                 999994 : 600,
                }

# Get arguments
DSID=-1
beamEnergy=-1
randSeed=-1
nevents=-1
if hasattr(runArgs,'runNumber'):
    DSID=runArgs.runNumber
else:
    raise RuntimeError("Run number (DSID) not given.")

if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2
else:
    raise RuntimeError("Center of mass energy not given.")

if hasattr(runArgs,'randomSeed'):
    randSeed = runArgs.randomSeed % (30081*30081) # MG5 random seed must be less than 30081*30081
else:
    raise RuntimeError("Random seed not given.")

if hasattr(runArgs,'maxEvents'):
    nevents=runArgs.maxEvents
else:
    nevents=5000

mtaup = dsid_to_mtaup.get(DSID)
if mtaup == None:
    raise RuntimeError("VLL joboptions run with invalid runNumber: {}".format(DSID))

evt_multiplier = 3.2
if mtaup <= 100:
    evt_multiplier = 15.0
elif mtaup <= 130:
    evt_multiplier = 10.0
elif mtaup <= 300:
    evt_multiplier = 5.0
evt_multiplier *= 1.05

# Write MG5 process card
proc_card = open('proc_card_mg5.dat','w')
proc_card.write("""
                import model sm
                define l+ e+ mu+ ta+
                define l- e- mu- ta-
                import model Doublet_Vector_like_Leptons_Model_UFO
                generate p p > taup taup~, taup > z ta-, taup~ > z ta+
                add process p p > taup taup~, taup > h ta-, taup~ > h ta+
                add process p p > taup taup~, taup > z ta-, taup~ > h ta+
                add process p p > taup taup~, taup > h ta-, taup~ > z ta+
                add process p p > taup vtaup~, taup > h ta-, vtaup~ > w- ta+
                add process p p > vtaup taup~, vtaup > w+ ta-, taup~ > h ta+
                add process p p > taup vtaup~, taup > z ta-, vtaup~ > w- ta+
                add process p p > vtaup taup~, vtaup > w+ ta-, taup~ > z ta+
                add process p p > vtaup vtaup~, vtaup > w+ ta-, vtaup~ > w- ta+
                output -f
                """)
proc_card.close()

process_dir = new_process()

# Set up the run card with mostly default values
run_card_extras = {
    'pdlabel' : 'lhapdf',
    'lhaid' : 247000,
    'sys_pdf' : 'cteq6l1',
    'event_norm' : 'sum',
    'scale' : str(mtaup),
    'dsqrt_q2fact1' : str(mtaup),
    'dsqrt_q2fact2' : str(mtaup)
}

# Set up the param card, varying the taup mass and decay widths based on the DSID
param_card_extras = {
    'MASS' : {
        'MTAUP' : mtaup,
        'MVTAUP' : mtaup
    },
    'VECTORLIKE' : {
        'EPSILON' : 0.04
    },
    'DECAY' : {
        'WTAUP' : str(taup_decay_width(mtaup)),
        'WVTAUP' : str(vtaup_decay_width(mtaup))
    }
}

ret_flag = build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir), run_card_new='run_card.dat', nevts=nevents*evt_multiplier, rand_seed=randSeed, beamEnergy=beamEnergy, extras=run_card_extras)
if ret_flag == -1:
    raise RuntimeError("Error building run_card.dat.")

ret_flag = build_param_card(param_card_old='{}/Cards/param_card.dat'.format(process_dir), param_card_new='param_card.dat', params=param_card_extras)
if ret_flag == -1:
    raise RuntimeError("Error building param_card.dat.")

print_cards()

# Generate LHE file from MG5
generate(run_card_loc='run_card.dat', param_card_loc='param_card.dat', mode=mode, proc_dir=process_dir, cluster_type=None, cluster_queue=None)

output_DS_prefix = 'madgraph.'+str(DSID)+'.MadGraph_VLL_mtaup'+str(mtaup)
output_DS = output_DS_prefix+'._00001.events.tar.gz'
arrange_output(proc_dir=process_dir, outputDS=output_DS)

# Change to serial mode for Pythia8
if 'ATHENA_PROC_NUMBER' in os.environ:
    njobs = os.environ.pop('ATHENA_PROC_NUMBER')
    if hasattr(opts,'nprocs'):
        opts.nprocs = 0

# Shower
evgenConfig.description = 'MadGraph5+Pythia8 for Vectorlike leptons of mass {}'.format(mtaup)
evgenConfig.contact = ['Daniel Wilbern <daniel.wilbern@cern.ch>']
evgenConfig.keywords += ['exotic', 'multilepton', 'tau']
evgenConfig.generators += ['MadGraph','Pythia8']
evgenConfig.inputfilecheck = output_DS_prefix
evgenConfig.minevents = minevents
runArgs.inputGeneratorFile=output_DS

include('Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py')
include('Pythia8_i/Pythia8_MadGraph.py')

# Filter
include('GeneratorFilters/MultiLeptonFilter.py')

filtSeq.MultiLeptonFilter.Ptcut = 12000.
filtSeq.MultiLeptonFilter.Etacut = 2.7
filtSeq.MultiLeptonFilter.NLeptons = 2
