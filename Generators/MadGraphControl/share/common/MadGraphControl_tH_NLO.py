from MadGraphControl.MadGraphUtils import *

# General settings
nevents=5500
gridpack_dir=None
gridpack_mode=False
cluster_type='lsf'
cluster_queue='1nd'
mode=1
njobs=20
runName='run_01'

tH_4fl = [ 346188 ]
tH_5fl = [ 346221 ]

if runArgs.runNumber in tH_4fl:
    
    mgproc="generate p p > h t1 b1 j $$ w+ w- [QCD]"
    name='tH'
    process="pp>tHbj"
    topdecay='''decay t > w+ b, w+ > all all
    decay t~ > w- b~, w- > all all'''
    gridpack_mode=True
    gridpack_dir='madevent/'

    fcard = open('proc_card_mg5.dat','w')
    fcard.write("""
    import model loop_sm
    define p = g u c d s u~ c~ d~ s~
    define j = g u c d s u~ c~ d~ s~
    define t1 = t t~
    define b1 = b b~
    """+mgproc+"""
    output -f
    """)
    fcard.close()

    extras = {'pdlabel'        : "'lhapdf'",
              'lhaid'          : 260400,
              'parton_shower'  :'PYTHIA8',
              'reweight_scale' : 'True',
              'reweight_PDF'   : 'True',
              'PDF_set_min'    : 260401,
              'PDF_set_max'    : 260500,
              'bwcutoff'       : 50.,
              'dynamical_scale_choice' : 3 }

elif runArgs.runNumber in tH_5fl:
    
    mgproc="generate p p > h t j $$ w+ w- [QCD]"
    mgprocadd="add process p p > h t~ j $$ w+ w- [QCD]"
    name='tH'
    process="pp>tHj"
    topdecay='''decay t > w+ b, w+ > all all
    decay t~ > w- b~, w- > all all'''
    gridpack_mode=True
    gridpack_dir='madevent/'

    fcard = open('proc_card_mg5.dat','w')
    fcard.write("""
    import model loop_sm-no_b_mass
    define p = g u c d s b u~ c~ d~ s~ b~
    define j = g u c d s b u~ c~ d~ s~ b~
    define q = u c d s b t
    define q~ = u~ c~ d~ s~ b~ t~
    """+mgproc+"""
    """+mgprocadd+"""
    output -f
    """)
    fcard.close()
    
    extras = {'pdlabel'        : "'lhapdf'",
              'lhaid'          : 260000,
              'parton_shower'  :'PYTHIA8',
              'reweight_scale' : 'True',
              'reweight_PDF'   : 'True',
              'PDF_set_min'    : 260001,
              'PDF_set_max'    : 260100,
              'bwcutoff'       : 50.,
              'dynamical_scale_choice' : 3 }

else:
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)


runName = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(name)

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    raise RuntimeError("No center of mass energy found.")

process_dir = new_process(grid_pack=gridpack_dir)

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
fMadSpinCard.write('''set Nevents_for_max_weigth 2000 # number of events for the estimate of the max. weight (default: 75)
set max_weight_ps_point 500  # number of PS to estimate the maximum for each event (default: 400)
'''+topdecay+'''
launch''')
fMadSpinCard.close()
    
    
    
generate(run_card_loc='run_card.dat',param_card_loc=None,mode=mode,njobs=njobs,proc_dir=process_dir,run_name=runName,madspin_card_loc=madspin_card_loc,
         grid_pack=gridpack_mode,gridpack_dir=gridpack_dir,nevents=nevents,random_seed=runArgs.randomSeed,required_accuracy=0.001,cluster_type=cluster_type,cluster_queue=cluster_queue)
outputDS=arrange_output(run_name=runName,proc_dir=process_dir,lhe_version=3,saveProcDir=True)



if 'ATHENA_PROC_NUMBER' in os.environ:
    print 'Noticed that you have run with an athena MP-like whole-node setup.  Will re-configure now to make sure that the remainder of the job runs serially.'
    njobs = os.environ.pop('ATHENA_PROC_NUMBER')
    # Try to modify the opts underfoot
    if not hasattr(opts,'nprocs'): print 'Did not see option!'
    else: opts.nprocs = 0
    print opts

evgenConfig.description = 'aMcAtNlo tH'
evgenConfig.keywords+=['Higgs', 'SMHiggs', 'tHiggs']
evgenConfig.inputfilecheck = outputDS
evgenConfig.contact = ['liza.mijovic@cern.ch', 'a.hasib@cern.ch' ]
runArgs.inputGeneratorFile=outputDS

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_aMcAtNlo.py") 
include("Pythia8_i/Pythia8_SMHiggs125_inc.py")
include("Pythia8_i/Pythia8_ShowerWeights.py")


