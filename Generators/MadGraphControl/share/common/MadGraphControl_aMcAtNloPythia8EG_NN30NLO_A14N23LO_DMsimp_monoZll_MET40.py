from MadGraphControl.MadGraphUtils import *
import math

# write process card
fcard = open('proc_card_mg5.dat','w')
# generate ... QED=0 QCD=3
fcard.write("""
import model DMsimp_s_spin1 -modelname

define p = g d u s c b d~ u~ s~ c~ b~ 
generate p p > xd xd~ z [QCD]

output -f
""")
fcard.close()

# require beam energy to be set as argument
beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    raise RuntimeError("No center of mass energy found.")

process_dir = new_process()

extras = {'lhe_version':'3.0',
          'pdlabel'    : "'lhapdf'",
          'lhaid': '260000 13100 25100',
          'reweight_scale':'.true.',
          'rw_Rscale_down': 0.5,
          'rw_Rscale_up' : 2.0,
          'rw_Fscale_down': 0.5,
          'rw_Fscale_up' : 2.0,
          'reweight_PDF':'True False False',
          'PDF_set_min' : 260001,
          'PDF_set_max' : 260100,
          'parton_shower' :'PYTHIA8',
          'ptj' : '20',
          'etaj' : '5',
          'ptl' : '10',
          'etal' : '2.5',
          'drll' : '0.1',
          'mll_sf' : '0.0',
          'ptgmin' : '10',
          'etagamma' : '2.5',
          }

#from https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/MadGraph5aMCatNLOForAtlas#Problems_with_run_card_dat_in_ne
build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat', xqcut=0.0,
               nevts=runArgs.maxEvents*LHE_EventMultiplier,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)
print_cards()

# write parameter card
paramcard = subprocess.Popen(['get_files','-data','MadGraph_param_card_DMsimp_s_spin1.dat'])
paramcard.wait()
if not os.access('MadGraph_param_card_DMsimp_s_spin1.dat',os.R_OK):
    print 'ERROR: Could not get param card'
elif os.access('param_card.dat',os.R_OK):
    print 'ERROR: Old param card in the current directory.  Dont want to clobber it.  Please move it first.'
else:
    oldcard = open('MadGraph_param_card_DMsimp_s_spin1.dat','r')
    newcard = open('param_card.dat','w')
    for line in oldcard:
        if '# gVXd' in line:
            newcard.write('   2 %e # gVXd \n'%(gVXd))
        elif '# gAXd' in line:
            newcard.write('   3 %e # gAXd \n'%(gAXd))
        elif '# gVd11' in line:
            newcard.write('   4 %e # gVd11 \n'%(gVd11))
        elif '# gVu11' in line:
            newcard.write('   5 %e # gVu11 \n'%(gVu11))
        elif '# gVd22' in line:
            newcard.write('   6 %e # gVd22 \n'%(gVd22))
        elif '# gVu22' in line:
            newcard.write('   7 %e # gVu22 \n'%(gVu22))
        elif '# gVd33' in line:
            newcard.write('   8 %e # gVd33 \n'%(gVd33))
        elif '# gVu33' in line:
            newcard.write('   9 %e # gVu33 \n'%(gVu33))
        elif '# gVl11' in line:
            newcard.write('   10 %e # gVl11 \n'%(gVl11))
        elif '# gVl22' in line:
            newcard.write('   11 %e # gVl22 \n'%(gVl22))
        elif '# gVl33' in line:
            newcard.write('   12 %e # gVl33 \n'%(gVl33))
        elif '# gAd11' in line:
            newcard.write('   13 %e # gAd11 \n'%(gAd11))
        elif '# gAu11' in line:
            newcard.write('   14 %e # gAu11 \n'%(gAu11))
        elif '# gAd22' in line:
            newcard.write('   15 %e # gAd22 \n'%(gAd22))
        elif '# gAu22' in line:
            newcard.write('   16 %e # gAu22 \n'%(gAu22))
        elif '# gAd33' in line:
            newcard.write('   17 %e # gAd33 \n'%(gAd33))
        elif '# gAu33' in line:
            newcard.write('   18 %e # gAu33 \n'%(gAu33))
        elif '# gAl11' in line:
            newcard.write('   19 %e # gAl11 \n'%(gAl11))
        elif '# gAl22' in line:
            newcard.write('   20 %e # gAl22 \n'%(gAl22))
        elif '# gAl33' in line:
            newcard.write('   21 %e # gAl33 \n'%(gAl33))
        elif '# gnu11' in line:
            newcard.write('   22 %e # gnu11 \n'%(gnu11))
        elif '# gnu22' in line:
            newcard.write('   23 %e # gnu22 \n'%(gnu22))
        elif '# gnu33' in line:
            newcard.write('   24 %e # gnu33 \n'%(gnu33))
        elif ' MY1 ' in line:
            newcard.write('   5000001 %e # MY1 \n'%(MY1))
        elif 'DECAY 5000001' in line :
            newcard.write('DECAY 5000001 %e #WY1 \n'%(WY1))
        elif ' xd : MXd ' in line:
            newcard.write('   1000022 %e # xd : MXd \n'%(MXd))
        elif ' # MXd ' in line:
            newcard.write('   1000022 %e # MXd \n'%(MXd))
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()

### creating mad spin card to specify decay of z boson
madspin_card_loc='madspin_card.dat'
mscard = open(madspin_card_loc,'w')
mscard.write("""
#************************************************************
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
set spinmode full
define l+ = e+ mu+
define l- = e- mu-
decay z > l+ l-
# running the actual code
launch"""%runArgs.randomSeed)

mscard.close()

runName='run_01'

generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name=runName,proc_dir=process_dir,madspin_card_loc=madspin_card_loc)

arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz')


# Multi-core capability
if 'ATHENA_PROC_NUMBER' in os.environ:
    evgenLog.info('Noticed that you have run with an athena MP-like whole-node setup.  Will re-configure now to make sure that the remainder of the job runs serially.')
    njobs = os.environ.pop('ATHENA_PROC_NUMBER')
    # Try to modify the opts underfoot
    if not hasattr(opts,'nprocs'): mglog.warning('Did not see option!')
    else: opts.nprocs = 0
    print opts


#### Shower
evgenConfig.description = "Wimp dmV mediator from DMSimp"
evgenConfig.keywords = ["exotic","BSM"]
evgenConfig.process = "generate pb pb > xd xd~ z [QCD]"
evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'
evgenConfig.contact = ["Christopher Anelli <cranelli@cern.ch>"]

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_aMcAtNlo.py")

#particle data = name antiname spin=2s+1 3xcharge colour mass width (left out, so set to 0: mMin mMax tau0)
genSeq.Pythia8.Commands += ["1000022:all = xd xd~ 2 0 0 %d 0" %(MXd),
                            "1000022:isVisible = false"]
#,                            
#                            "UncertaintyBands:doVariations=on",
#                            'UncertaintyBands:List = {Var3cUp isr:muRfac=0.549241,Var3cDown isr:muRfac=1.960832,isr:muRfac=0.5_fsr:muRfac=0.5 isr:muRfac=0.5 fsr:muRfac=0.5,isr:muRfac=2.0_fsr:muRfac=2.0 isr:muRfac=2.0 fsr:muRfac=2.0}']
#genSeq.Pythia8.ShowerWeightNames = ["Var3cUp", "Var3cDown","isr:muRfac=0.5_fsr:muRfac=0.5", "isr:muRfac=2.0_fsr:muRfac=2.0"]
 

include("GeneratorFilters/MissingEtFilter.py")
filtSeq.MissingEtFilter.METCut = 40*GeV
