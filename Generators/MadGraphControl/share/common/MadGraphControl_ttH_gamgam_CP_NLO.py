from MadGraphControl.MadGraphUtils import *

# General settings
nevents=5500
gridpack_dir=None
gridpack_mode=True
cluster_type='lsf'
cluster_queue='8nh'
mode=0
njobs=1
runName='run_01'
parameters={}

### DSID lists (extensions can include systematics samples)
ttH_CP   = [346303, 346304]

mgproc="generate p p > t t~ x0 [QCD]"
name='ttbarH_CP'
process="pp>tt~x0"  
topdecay='''decay t > w+ b, w+ > all all
decay t~ > w- b~, w- >  all all'''
nevents=5300
gridpack_mode=True
gridpack_dir='madevent/'


runName = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(name)


fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model HC_NLO_X0_UFO-no_b_mass
define p = g u c d s b u~ c~ d~ s~ b~
define j = g u c d s b u~ c~ d~ s~ b~
define q = u c d s b t
define q~ = u~ c~ d~ s~ b~ t~
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


process_dir = new_process(grid_pack=gridpack_dir)

#Fetch default LO run_card.dat and set parameters
extras = {'pdlabel'        : "'lhapdf'",
          'lhaid'          : 260000,
          'parton_shower'  :'PYTHIA8',
          'reweight_scale' : 'True',
          'reweight_PDF'   : 'True',
          'PDF_set_min'    : 260001,
          'PDF_set_max'    : 260100,
          'bwcutoff'       : 50.}


#---------------------------------------------------------------------------------------------------                                                               
# Setting EFT parameters in HC model                                                                                                                                                                   
#---------------------------------------------------------------------------------------------------                                                                                                  
      
parameters_cosa_1={
    'frblock':{
        'Lambda':'1.000000e+03',
        'cosa':  '1.000000e+00',
        'kSM':   '1.000000e+00',
        'kHtt':  '1.000000e+00',
        'kAtt':  '1.000000e+00'}
    }

parameters_cosa_0={
    'frblock':{
        'Lambda':'1.000000e+03',
        'cosa':  '0.000000e+00',
        'kSM':   '0.000000e+00',
        'kHtt':  '0.000000e+00',
        'kAtt':  '1.000000e+00'}
    }

parameters_cosa_sqrt2={
    'frblock':{
        'Lambda':'1.000000e+03',
        'cosa':  '0.707106781',
        'kSM':   '1.414213562',
        'kHtt':  '1.000000e+00',
        'kAtt':  '1.000000e+00'}
    }


parameters = parameters_cosa_1

if runArgs.runNumber in {346303}:
    parameters = parameters_cosa_0
if runArgs.runNumber in {346304}:
    parameters = parameters_cosa_sqrt2

#---------------------------------------------------------------------------------------------------                                                                                             
# Using the helper function from MadGraphControl for setting up the param_card                                                                                                                            
# Build a new param_card.dat from an existing one                                                                                                                                                         
# Used values given in "parameters" for EFT parameters, if not set there, default values are used                                                                                                         
#---------------------------------------------------------------------------------------------------                                                                                                     

build_param_card(param_card_old=process_dir+'/Cards/param_card.dat',param_card_new='param_card_new.dat',params=parameters)

# Param card
#paramNameToCopy      = 'aMcAtNlo_param_card_loopsmnobmass.dat' #'aMcAtNlo_param_card_tt.dat'
#paramNameDestination = 'param_card_new.dat'
#paramcard            = subprocess.Popen(['get_files','-data',paramNameToCopy])
#paramcard.wait()
#if not os.access(paramNameToCopy,os.R_OK):
#    raise RuntimeError("ERROR: Could not get %s"%(paramNameToCopy))
#shutil.copy(paramNameToCopy,paramNameDestination)


build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,xqcut=0.,
               extras=extras)


madspin_card_loc='madspin_card.dat'

if not hasattr(runArgs, 'inputGenConfFile'):
    fMadSpinCard = open('madspin_card.dat','w')
    fMadSpinCard.write('import Events/'+runName+'/events.lhe.gz\n')
    fMadSpinCard.write('set ms_dir MadSpin\n')
else:
    os.unlink(gridpack_dir+'Cards/madspin_card.dat')
    fMadSpinCard = open(gridpack_dir+'Cards/madspin_card.dat','w')
    fMadSpinCard.write('import '+gridpack_dir+'Events/'+runName+'/events.lhe.gz\n')
    fMadSpinCard.write('set ms_dir '+gridpack_dir+'MadSpin\n')
    fMadSpinCard.write('set seed '+str(10000000+int(runArgs.randomSeed))+'\n')
fMadSpinCard.write('''set Nevents_for_max_weigth 250 # number of events for the estimate of the max. weight (default: 75)
set max_weight_ps_point 1000  # number of PS to estimate the maximum for each event (default: 400)
'''+topdecay+'''
launch''')
fMadSpinCard.close()    
    
generate(run_card_loc='run_card.dat',param_card_loc='param_card_new.dat',mode=mode,njobs=njobs,proc_dir=process_dir,run_name=runName,madspin_card_loc=madspin_card_loc,
         grid_pack=gridpack_mode,gridpack_dir=gridpack_dir,nevents=nevents,random_seed=runArgs.randomSeed,required_accuracy=0.001,cluster_type=cluster_type,cluster_queue=cluster_queue)


outputDS=arrange_output(run_name=runName,proc_dir=process_dir,lhe_version=3,saveProcDir=True)


if 'ATHENA_PROC_NUMBER' in os.environ:
    print 'Noticed that you have run with an athena MP-like whole-node setup.  Will re-configure now to make sure that the remainder of the job runs serially.'
    njobs = os.environ.pop('ATHENA_PROC_NUMBER')
    # Try to modify the opts underfoot
    if not hasattr(opts,'nprocs'): print 'Did not see option!'
    else: opts.nprocs = 0
    print opts

evgenConfig.description = 'aMcAtNlo_ttH'
evgenConfig.keywords+=['ttHiggs','ttbar','Higgs','BSM']
evgenConfig.inputfilecheck = outputDS
evgenConfig.contact = ['mcfayden@cern.ch', 'aknue@cern.ch']
runArgs.inputGeneratorFile=outputDS

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_aMcAtNlo.py")

genSeq.Pythia8.Commands += [
    '25:onMode = off', # switch OFF all Higgs decay channels
    '25:onIfMatch = 22 22', # H -> gamma gamma
    "PartonLevel:MPI = on"
    ]

