from MadGraphControl.MadGraphUtils import *

# General settings
nevents=int(1.1*runArgs.maxEvents)
gridpack_dir='madevent/'
gridpack_mode=True
cluster_type='lsf'
cluster_queue='1nw'
mode=0
njobs=1
runName='run_01'

# MG Particle cuts
mllcut=5

# Shower/merging settings
maxjetflavor=5
parton_shower='PYTHIA8'

### DSID lists (extensions can include systematics samples)
zdecay="decay z > all all"

keyword=['top', 'ttV']

#DSIDs: 412008-412040
DSID = runArgs.runNumber
if DSID>=412008 and DSID <=412018:
    mgproc="""generate p p > t t~ e+ e- EFT=1 QED=2 QCD=2 [QCD]"""
    if DSID==412008:
        name='ttee_NLO_EFT_sm'
    elif DSID==412009:
        name='ttee_NLO_EFT_cfq3p'
    elif DSID==412010:
        name='ttee_NLO_EFT_cfq3m'
    elif DSID==412011:
        name='ttee_NLO_EFT_cftp'
    elif DSID==412012:
        name='ttee_NLO_EFT_cftm'
    elif DSID==412013:
        name='ttee_NLO_EFT_ctwp'
    elif DSID==412014:
        name='ttee_NLO_EFT_ctwm'
    elif DSID==412015:
        name='ttee_NLO_EFT_ctbp'
    elif DSID==412016: 
        name='ttee_NLO_EFT_ctbm'
    elif DSID==412017:
        name='ttee_NLO_EFT_ctgp'
    elif DSID==412018:
        name='ttee_NLO_EFT_ctgm'
elif DSID>=412019 and DSID <=412029:
    mgproc="""generate p p > t t~ mu+ mu- EFT=1 QED=2 QCD=2 [QCD]"""
    if DSID==412019:
        name='ttmm_NLO_EFT_sm'
    elif DSID==412020:
        name='ttmm_NLO_EFT_cfq3p'
    elif DSID==412021:
        name='ttmm_NLO_EFT_cfq3m'
    elif DSID==412022:
        name='ttmm_NLO_EFT_cftp'
    elif DSID==412023:
        name='ttmm_NLO_EFT_cftm'
    elif DSID==412024:
        name='ttmm_NLO_EFT_ctwp'
    elif DSID==412025:
        name='ttmm_NLO_EFT_ctwm'
    elif DSID==412026:
        name='ttmm_NLO_EFT_ctbp'
    elif DSID==412027:
        name='ttmm_NLO_EFT_ctbm'
    elif DSID==412028:
        name='ttmm_NLO_EFT_ctgp'
    elif DSID==412029:
        name='ttmm_NLO_EFT_ctgm'
elif DSID>=412030 and DSID <=412040:
    mgproc="""generate p p > t t~ ta+ ta- $$ h  EFT=1 QED=2 QCD=2 [QCD]"""
    if DSID==412030:
        name='tttata_NLO_EFT_sm'
    elif DSID==412031:
        name='tttata_NLO_EFT_cfq3p'
    elif DSID==412032:
        name='tttata_NLO_EFT_cfq3m'
    elif DSID==412033:
        name='tttata_NLO_EFT_cftp'
    elif DSID==412034:
        name='tttata_NLO_EFT_cftm'
    elif DSID==412035:
        name='tttata_NLO_EFT_ctwp'
    elif DSID==412036:
        name='tttata_NLO_EFT_ctwm'
    elif DSID==412037:
        name='tttata_NLO_EFT_ctbp'
    elif DSID==412038:
        name='tttata_NLO_EFT_ctbm'
    elif DSID==412039:
        name='ttata_NLO_EFT_ctgp'
    elif DSID==412040:
        name='tttata_NLO_EFT_ctgm'

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
           }

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



process_dir = new_process(grid_pack=gridpack_dir)

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
    412008:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'1.000000e-04', 'cfb':'1.000000e-04', 
            'cfQ1':'0.000000e+00', 'cfQ3':'4.000000e-04', 'ctW':'1.000000e-04', 'ctB':'1.000000e-04', 'cff':'1.000000e-04', 
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412019:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'4.000000e-04', 'ctW':'1.000000e-04', 'ctB':'1.000000e-04', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412030:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'4.000000e-04', 'ctW':'1.000000e-04', 'ctB':'1.000000e-04', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412009:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'1.000000e-04', 'cfb':'1.000000e-04', 
            'cfQ1':'0.000000e+00', 'cfQ3':'6.000000e+02', 'ctW':'1.000000e-04', 'ctB':'1.000000e-04', 'cff':'1.000000e-04', 
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},       
    412020:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'6.000000e+02', 'ctW':'1.000000e-04', 'ctB':'1.000000e-04', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412031:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'6.000000e+02', 'ctW':'1.000000e-04', 'ctB':'1.000000e-04', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412010:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'-6.000000e+02', 'ctW':'1.000000e-04', 'ctB':'1.000000e-04', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412021:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'-6.000000e+02', 'ctW':'1.000000e-04', 'ctB':'1.000000e-04', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412032:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'-6.000000e+02', 'ctW':'1.000000e-04', 'ctB':'1.000000e-04', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412011:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'6.000000e+02', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'1.000000e-04', 'ctW':'1.000000e-04', 'ctB':'1.000000e-04', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412022:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'6.000000e+02', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'1.000000e-04', 'ctW':'1.000000e-04', 'ctB':'1.000000e-04', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412033:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'6.000000e+02', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'1.000000e-04', 'ctW':'1.000000e-04', 'ctB':'1.000000e-04', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412012:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'-6.000000e+02', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'1.000000e-04', 'ctW':'1.000000e-04', 'ctB':'1.000000e-04', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412023:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'-6.000000e+02', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'1.000000e-04', 'ctW':'1.000000e-04', 'ctB':'1.000000e-04', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412034:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'-6.000000e+02', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'1.000000e-04', 'ctW':'1.000000e-04', 'ctB':'1.000000e-04', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412013:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'1.000000e-04', 'ctW':'2.000000e+02', 'ctB':'1.000000e-04', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412024:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'1.000000e-04', 'ctW':'2.000000e+02', 'ctB':'1.000000e-04', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412035:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'1.000000e-04', 'ctW':'2.000000e+02', 'ctB':'1.000000e-04', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412014:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'1.000000e-04', 'ctW':'-2.000000e+02', 'ctB':'1.000000e-04', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412025:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'1.000000e-04', 'ctW':'-2.000000e+02', 'ctB':'1.000000e-04', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412036:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'1.000000e-04', 'ctW':'-2.000000e+02', 'ctB':'1.000000e-04', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412015:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'1.000000e-04', 'ctW':'1.000000e-04', 'ctB':'5.000000e+02', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412026:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'1.000000e-04', 'ctW':'1.000000e-04', 'ctB':'5.000000e+02', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412037:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'1.000000e-04', 'ctW':'1.000000e-04', 'ctB':'5.000000e+02', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412016:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'1.000000e-04', 'ctW':'1.000000e-04', 'ctB':'-5.000000e+02', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412027:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'1.000000e-04', 'ctW':'1.000000e-04', 'ctB':'-5.000000e+02', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412038:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'1.000000e-04', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'1.000000e-04', 'ctW':'1.000000e-04', 'ctB':'-5.000000e+02', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412017:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'3.000000e+01', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'1.000000e-04', 'ctW':'1.000000e-04', 'ctB':'1.000000e-04', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412028:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'3.000000e+01', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'1.000000e-04', 'ctW':'1.000000e-04', 'ctB':'1.000000e-04', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412039:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'3.000000e+01', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'1.000000e-04', 'ctW':'1.000000e-04', 'ctB':'1.000000e-04', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412018:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'-3.000000e+01', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'1.000000e-04', 'ctW':'1.000000e-04', 'ctB':'1.000000e-04', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412029:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'-3.000000e+01', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'1.000000e-04', 'ctW':'1.000000e-04', 'ctB':'1.000000e-04', 'cff':'1.000000e-04',
            'cbW':'1.000000e-04', 'muprime':'1.725000e+02'},
    412040:{'Lambda':'1.000000e+04', 'ccc':'1.000000e+00', 'ctG':'-3.000000e+01', 'cft':'1.000000e-04', 'cfb':'1.000000e-04',
            'cfQ1':'0.000000e+00', 'cfQ3':'1.000000e-04', 'ctW':'1.000000e-04', 'ctB':'1.000000e-04', 'cff':'1.000000e-04',
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

if not hasattr(runArgs, 'inputGenConfFile'):
    with open("amcatnlo_configuration.txt", 'w') as new_file:
        with open("PROCNLO_TEFT_EW_0/Cards/amcatnlo_configuration.txt", 'r') as old_file:
            for line in old_file:
                if (line.find("# cluster_nb_retry = 1") != -1):
                    line = "cluster_nb_retry = 5"
    		
                new_file.write(line)    
    
    shutil.copy("amcatnlo_configuration.txt", "PROCNLO_TEFT_EW_0/Cards/amcatnlo_configuration.txt")

print_cards()

generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=mode,njobs=njobs,proc_dir=process_dir,run_name=runName,madspin_card_loc=madspin_card_loc,grid_pack=gridpack_mode,gridpack_dir=gridpack_dir,nevents=nevents,random_seed=runArgs.randomSeed,required_accuracy=0.001,cluster_type=cluster_type,cluster_queue=cluster_queue)

outputDS=arrange_output(run_name=runName,proc_dir=process_dir,outputDS=strigy+'._00001.tar.gz',lhe_version=lhe_version,saveProcDir=True)

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
evgenConfig.contact = ["olga.bylund@cern.ch","maria.moreno.llacer@cern.ch"]
runArgs.inputGeneratorFile=outputDS
#############
#Pythia8 shower
#############
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_aMcAtNlo.py")
#########
