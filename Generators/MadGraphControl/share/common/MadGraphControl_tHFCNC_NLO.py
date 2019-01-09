from MadGraphControl.MadGraphUtils import *
import fileinput
from AthenaCommon import Logging
JOlog = Logging.logging.getLogger('FCNCJobOption')

nevents=int(1.1*runArgs.maxEvents)
mode=0

gridpack_dir  = 'madevent/'
gridpack_mode = False

### DSID lists
allDIDS  = range(410752,410764)

NLO = True
if NLO:
    proc    = 'generate p p > t h $$ t~ [QCD]\nadd process p p > t~ h $$ t [QCD]'
else:
    # this is just for quick checks
    proc    = 'define mt = t t~\ndefine mw = w+ w-\ndefine mb = b b~\ndefine lep = l+ l- ta+ ta- vl vl~\ngenerate p p > mt h'

fcard = open('proc_card_mg5.dat','w')
model   = 'TopFCNC-onlyh'
runName = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_mctHWb_NLO5FS'

isHbb = any("Hbb" in JO for JO in runArgs.jobConfig)

### Nevent adjustment for event filters
if any("Htautau" in JO for JO in runArgs.jobConfig):
    nevents = int(2.5*nevents)
elif any("HLepFilter" in JO for JO in runArgs.jobConfig):
    nevents = int(7.0*nevents)
elif any("2LepFilter" in JO for JO in runArgs.jobConfig):
    JOlog.info(" Apply 2LepFilter for t -> Hq and H->VV generation")
    nevents = int(6.0*nevents)
else:
    # No event filter was recognised so just pass through unchanged
    pass

JOlog.info(runArgs.jobConfig)

thisDSID = runArgs.runNumber-410752

# Check that the DSID is in the range we expect
if runArgs.runNumber in allDIDS:
    fcard.write("""import model %s\n%s\noutput -f"""%(model,proc))
else:
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)
fcard.close()

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    raise RuntimeError("No center of mass energy found.")

##Fetch default NLO run_card.dat and set parameters
##nnpdf3.0 nlo as 0.118 5fs
lhaid=260000

reweight_scale = '.true.'
reweight_PDF   = '.true.'

pdflabel='lhapdf'

parton_shower = 'PYTHIA8'

muR_over_ref  = 1.0
dyn_scale     = -1
# I think this is the choice of 1412.5594
fixed_ren_scale = '.true.'
fixed_fac_scale = '.true.'
muR_ref_fixed   = 172.5+125.
muF_ref_fixed   = 172.5+125.

lhe_version   = 3

extras = { 'lhaid'         : lhaid,
           'pdlabel'       : "'"+pdflabel+"'",
           'parton_shower' : parton_shower,
           'reweight_scale': reweight_scale,
           'reweight_PDF'  : reweight_PDF,
           'muR_over_ref'  : muR_over_ref,
           'dynamical_scale_choice' : dyn_scale,
           'fixed_ren_scale' : fixed_ren_scale,
           'fixed_fac_scale' : fixed_fac_scale,
           'muR_ref_fixed' : muR_ref_fixed,  
           'muF_ref_fixed' : muF_ref_fixed,           
           'req_acc'       : 0.001
       }

# The couplings. Only real part.
# The tH relevant couplings (c,u) x (LH,RH) x (tqH,tqg)
RCtphi  = 0.
RCuphi  = 0.
RCtcphi = 0.
RCctphi = 0.
RCtG    = 0.
RCuG    = 0.
RCtcG   = 0.
RCctG   = 0.

## For MadSpin
madspin_card_loc = 'madspin_card.dat'
madspin_card_rep = madspin_card_loc
madspin_in       = 'import Events/'+runName+'/events.lhe'
if not NLO:
    madspin_in       = 'import Events/'+runName+'/unweighted_events.lhe'
madspin_rep      = 'set ms_dir MadSpin'
madspin_seed     = runArgs.randomSeed
if hasattr(runArgs, 'inputGenConfFile'):
    madspin_card_rep = gridpack_dir+'Cards/'+madspin_card_loc
    madspin_in       = 'import '+gridpack_dir+'Events/'+runName+'/events.lhe'
    madspin_rep      = 'set ms_dir '+gridpack_dir+'MadSpin'
    madspin_seed     = 10000000+int(runArgs.randomSeed)

mscard = open(madspin_card_rep,'w')
mscard.write("""
set Nevents_for_max_weigth 250 # number of events for the estimate of the max. weight (default: 75)
set max_weight_ps_point 1000  # number of PS to estimate the maximum for each event   (default: 400)
set seed %i
%s
%s
define l+ = l+ ta+
define l- = l- ta-
define All = l+ l- vl vl~ j
\n
"""%(madspin_seed,madspin_in,madspin_rep))

## The W decay
wtyp = thisDSID%2
if wtyp == 0 or thisDSID > 7:
    wstrm = ', w- > l- vl~'
    wstrp = ', w+ > l+ vl'
elif wtyp == 1:
    wstrm = ', w- > j j'
    wstrp = ', w+ > j j'
else :
    raise RuntimeError("No W decays are generated please check the job option")

## The top decay (SM)
tstrm = 'decay t~ > w- b~'
tstrp = 'decay t > w+ b'

mscard.write("""%s%s\n"""%(tstrm,wstrm))
mscard.write("""%s%s\nlaunch"""%(tstrp,wstrp))
mscard.close()

## The operator (only t-q-H for the time being, choose between c and u. No color dipole; expect to be small because of limit from pp --> t)
## it seems that both q flavours need to have non zero value ?? 
ope = thisDSID%8
if   (not isHbb and (ope == 0 or ope == 1)) or (isHbb and ope == 3):
    RCtphi  = 1
    RCtcphi = 1e-5
elif (not isHbb and (ope == 2 or ope == 3)) or (isHbb and ope == 0):
    RCtcphi = 1
    RCtphi  = 1e-5
elif (not isHbb and (ope == 4 or ope == 5)) or (isHbb and ope == 1):
    RCuphi = 1
    RCctphi  = 1e-5
elif (not isHbb and (ope == 6 or ope == 7)) or (isHbb and ope == 2):
    RCctphi = 1
    RCuphi  = 1e-5
else :
    raise RuntimeError("Unknow operateur please check the DSID")

coup = {
    'RCtphi'  : RCtphi ,
    'RCuphi'  : RCuphi ,
    'RCtcphi' : RCtcphi,
    'RCctphi' : RCctphi,

    'RCtG'  : RCtG ,
    'RCuG'  : RCuG ,
    'RCtcG' : RCtcG,
    'RCctG' : RCctG,
    }

process_dir = new_process(grid_pack=gridpack_dir)

paramFileName  = 'MadGraph_param_card_ttFCNC_NLO.dat'
#paramFileNameN = 'param_cardDecay.dat'
paramFileNameN = 'param_card.dat'
paramFile      = subprocess.Popen(['get_files','-data',paramFileName]).communicate()
if not os.access(paramFileName, os.R_OK):
    print 'ERROR: Could not get param card'
    raise RuntimeError("parameter card '%s' missing!"%paramFileName)

build_param_card(param_card_old=paramFileName,param_card_new=paramFileNameN,
                 params={'dim6':coup}
             )
shutil.copyfile(paramFileNameN,process_dir+'/Cards/'+paramFileNameN)

if NLO:
    build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',
                   nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)
else:
    build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',
                   nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy)

print_cards()

generate(run_card_loc='run_card.dat',param_card_loc=None,
         madspin_card_loc=madspin_card_loc,
         mode=mode,njobs=1,
         proc_dir=process_dir,run_name=runName,
         grid_pack=gridpack_mode,gridpack_dir=gridpack_dir,nevents=nevents,random_seed=runArgs.randomSeed)
outputDS = arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz',lhe_version=lhe_version)

#### Shower
evgenConfig.description = 'aMcAtNlo+Pythia8+EvtGen tH production for FCNC NLO model'
evgenConfig.keywords += ['top', 'ttbar', 'FCNC', 'Higgs']
#evgenConfig.inputfilecheck = runName
#evgenConfig.inputconfcheck = ''
runArgs.inputGeneratorFile = outputDS
runArgs.contact = ['Ian Connelly <ian.connelly@cern.ch>']

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_aMcAtNlo.py")
include("Pythia8_i/Pythia8_ShowerWeights.py")

### Additional filter(s)
### Tau filter
if any("Htautau" in JO for JO in runArgs.jobConfig):
    if not hasattr(filtSeq,"TauFilter"):
        from GeneratorFilters.GeneratorFiltersConf import TauFilter
        filtSeq += TauFilter()
    filtSeq.TauFilter.Ntaus    = 2
    filtSeq.TauFilter.Ptcute   = 13000.0 # MeV
    filtSeq.TauFilter.Ptcutmu  = 8000.0  # MeV
    filtSeq.TauFilter.Ptcuthad = 20000.0 # MeV

### Lepton filter
if any("HLepFilter" in JO for JO in runArgs.jobConfig):
    if not hasattr(filtSeq,"LeptonFilter"):
        from GeneratorFilters.GeneratorFiltersConf import LeptonFilter
        filtSeq += LeptonFilter()
    filtSeq.LeptonFilter.Ptcut = 15000.0 # MeV

### Multilepton filter
if any("2LepFilter" in JO for JO in runArgs.jobConfig):
    if not hasattr(filtSeq,"MultiLeptonFilter"):
        from GeneratorFilters.GeneratorFiltersConf import MultiLeptonFilter
        filtSeq += MultiLeptonFilter()
    filtSeq.MultiLeptonFilter.NLeptons = 2
    filtSeq.MultiLeptonFilter.Ptcut    = 10000.0 # MeV
    filtSeq.MultiLeptonFilter.Etacut   = 4.0     # Rapidity cut
