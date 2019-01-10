from MadGraphControl.MadGraphUtils import *

# General settings
nevents=int(2.1*runArgs.maxEvents)
mode=2

# MG Particle cuts
mllcut=-1
ptjCut=15
ptlCut=15
ptgCut=15
drajCut=0.4
dralCut=0.4

# Shower/merging settings
maxjetflavor=5
parton_shower='PYTHIA8'

### DSID lists (extensions can include systematics samples)
ttgamma = [410382, 410383, 410384, 410385, 410386, 410387, 410388]
zdecay="decay z > all all"

DSID = runArgs.runNumber
if DSID == 410382:
    mgproc="""generate p p > t t~ a EFT=1 QED=1 QCD=2 [QCD]"""
    name='ttgamma_NLO_EFT_sm'
    keyword=['top', 'ttV', 'photon']
elif DSID == 410383:
    mgproc="""generate p p > t t~ a EFT=1 QED=1 QCD=2 [QCD]"""
    name='ttgamma_NLO_EFT_ctbm'
    keyword=['top', 'ttV', 'photon']
elif DSID == 410384:
    mgproc="""generate p p > t t~ a EFT=1 QED=1 QCD=2 [QCD]"""
    name='ttgamma_NLO_EFT_ctbp'
    keyword=['top', 'ttV', 'photon']
elif DSID == 410385:
    mgproc="""generate p p > t t~ a EFT=1 QED=1 QCD=2 [QCD]"""
    name='ttgamma_NLO_EFT_ctgm'
    keyword=['top', 'ttV', 'photon']
elif DSID == 410386:
    mgproc="""generate p p > t t~ a EFT=1 QED=1 QCD=2 [QCD]"""
    name='ttgamma_NLO_EFT_ctgp'
    keyword=['top', 'ttV', 'photon']
elif DSID == 410387:
    mgproc="""generate p p > t t~ a EFT=1 QED=1 QCD=2 [QCD]"""
    name='ttgamma_NLO_EFT_ctwm'
    keyword=['top', 'ttV', 'photon']
elif DSID == 410388:
    mgproc="""generate p p > t t~ a EFT=1 QED=1 QCD=2 [QCD]"""
    name='ttgamma_NLO_EFT_ctwp'
    keyword=['top', 'ttV', 'photon']

else: 
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)

strigy = 'MG5_aMCatNLO.'+str(runArgs.runNumber)+'.'+str(name)+'_13TeV.TXT.mc15_v1'

fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model TEFT_EW
define p = g u c d s b u~ c~ d~ s~ b~
define j = g u c d s b u~ c~ d~ s~ b~
define w = w+ w-
define l+ = e+ mu+ ta+
define l- = e- mu- ta-
define vl = ve vm vt
define vl~ = ve~ vm~ vt~
"""+mgproc+"""
output -f
""")
fcard.close()


beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")


lhaid=260000
pdflabel='lhapdf'

#Fetch default LO run_card.dat and set parameters
extras = { 'lhaid'         : lhaid,
           'pdlabel'       : "'"+pdflabel+"'",
           'maxjetflavor'  : maxjetflavor,
           'parton_shower' : parton_shower,
           'mll_sf'        : mllcut,
           'ptj'           :ptjCut,
           'ptl'           :ptlCut,
	   'ptgmin'        :ptgCut}

doSyst=True

if doSyst:
    lhe_version=3
    extras.update({'reweight_scale': '.true.',
                   'rw_Rscale_down':  0.5,
                   'rw_Rscale_up'  :  2.0,
                   'rw_Fscale_down':  0.5,  
                   'rw_Fscale_up'  :  2.0, 
                   'reweight_PDF'  : '.false.',
                   'PDF_set_min'   : 260001, 
                   'PDF_set_max'   : 260100,
                   'store_rwgt_info': '.true.'})
else:
    lhe_version=1
    extras.update({'reweight_scale': '.false.',
                   'reweight_PDF'  : '.false.'})



madspin_card_loc='madspin_card.dat'                                                                                                                                    

mscard = open(madspin_card_loc,'w')                                                                                                                                    
mscard.write("""#************************************************************
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
define q = u d s c b
define q~ = u~ d~ s~ c~ b~
define sc~ = s c~
define cs~ = c s~
define du~ = d u~
define ud~ = u d~
decay t > w+ b, w+ > all all  
decay t~ > w- b~, w- > all all
decay w+ > all all            
decay w- > all all
%s
# running the actual code     
launch"""%(runArgs.randomSeed,zdecay))                                                                                                                                              
mscard.close()



process_dir = new_process()

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat', 
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,xqcut=0.,
               extras=extras)

# Param card
paramNameToCopy      = 'aMcAtNlo_ttg_EFT_param_card.dat'
paramNameDestination = 'param_card.dat'
paramcard            = subprocess.Popen(['get_files','-data',paramNameToCopy])
paramcard.wait()
if not os.access(paramNameToCopy,os.R_OK):
    raise RuntimeError("ERROR: Could not get %s"%(paramNameToCopy))
shutil.copy(paramNameToCopy,paramNameDestination)

dsid_to_coup = {
    410382:{'Lambda':'1.000000e+03', 'ccc':'1.000000e+00', 'ctG':'1.000000e-06', 'cft':'1.000000e-06', 'cfb':'1.000000e-06', 
            'cfQ1':'0.000000e+00', 'cfQ3':'4.000000e-06', 'ctW':'1.000000e-06', 'ctB':'1.000000e-06', 'cff':'1.000000e-06', 
            'cbW':'1.000000e-06', 'muprime':'1.725000e+02'},
    410383:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'1.000000e-04', 'cfb':'1.000000e-04', 
            'cfQ1':'0.000000e+00', 'cfQ3':'4.000000e-04', 'ctW':'1.000000e-04', 'ctB':'-5.000000e+02', 'cff':'1.000000e-04', 
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},       
    410384:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'4.000000e-04', 'ctW':'1.000000e-04', 'ctB':'5.000000e+02', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    410385:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'-3.000000e+01', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'4.000000e-04', 'ctW':'1.000000e-04', 'ctB':'1.000000e-04', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    410386:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'3.000000e+01', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'4.000000e-04', 'ctW':'1.000000e-04', 'ctB':'1.000000e-04', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    410387:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'4.000000e-04', 'ctW':'-2.000000e+02', 'ctB':'1.000000e-04', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    410388:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'4.000000e-04', 'ctW':'2.000000e+02', 'ctB':'1.000000e-04', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'}

}

if DSID not in dsid_to_coup.keys():
    raise RuntimeError("BSM parameters not available for DSID %i "%DSID)

with open(paramNameDestination, 'w') as new_file:
    with open(paramNameToCopy) as old_file:
        for line in old_file:
            found=False
            for par in dsid_to_coup[DSID].keys():
                if(line.find(par) != -1):
                    found=True
                    old_value = line.split('#')[0].strip('\t').lstrip()[2:].strip(' ')
                    new_file.write(line.replace(old_value, str(dsid_to_coup[DSID][par])))
            if (found == False):
                new_file.write(line)    

print_cards()

generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=mode,proc_dir=process_dir,madspin_card_loc=madspin_card_loc)
outputDS=arrange_output(proc_dir=process_dir,outputDS=strigy+'._00001.tar.gz',lhe_version=lhe_version)

# Go to serial mode for Pythia8
if 'ATHENA_PROC_NUMBER' in os.environ:
    print 'Noticed that you have run with an athena MP-like whole-node setup.  Will re-configure now to make sure that the remainder of the job runs serially.'
    njobs = os.environ.pop('ATHENA_PROC_NUMBER')
    # Try to modify the opts underfoot
    if not hasattr(opts,'nprocs'): print 'Did not see option!'
    else: opts.nprocs = 0
    print opts

evgenConfig.generators += ["aMcAtNlo","Pythia8"]
evgenConfig.description = 'aMcAtNlo_'+str(name)
evgenConfig.keywords+=keyword
evgenConfig.contact = ["mazoza.ghneimat@cern.ch"]
runArgs.inputGeneratorFile=outputDS
#############
#Pythia8 shower
#############
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_aMcAtNlo.py")
#########
#Filter
#########
include("GeneratorFilters/TTbarWToLeptonFilter.py")
filtSeq.TTbarWToLeptonFilter.NumLeptons = -1 #(non-all had)
filtSeq.TTbarWToLeptonFilter.Ptcut = 15000.0  #MeV
