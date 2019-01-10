# This control file is an update to the original FCNC script which used LO NP model
# The processes are defined using the DSIDs and the numbering.
# To simplify things, the script will be largely unchanged and an offset will be applied to get the same syntax
# Starting DSID: 410700 -> 410804 (changed to) : offset is -104
# runArgs.runNumber -> thisDSID for numerical comparisons

from MadGraphControl.MadGraphUtils import *
import fileinput
from AthenaCommon import Logging
JOlog = Logging.logging.getLogger('FCNCJobOption')

nevents=int(1.1*runArgs.maxEvents)
mode=0

gridpack_dir  = 'madevent/'
gridpack_mode = False

### DSID lists
# Original FCNC range (-104 for NLO)
allDIDS  = range(410700,410892)
# Additional DSIDs (-104 for NLO)
allDIDS += range(410588,410596)

NLO = True
if NLO:
    proc    = 'generate p p > t t~ [QCD]'
else:
    # this is just for quick checks
    proc    = 'generate p p > t t~'

fcard = open('proc_card_mg5.dat','w')
model   = 'TopFCNC-onlyh'
runName = 'mcttqHWb'+str(runArgs.runNumber)+'NLO5FS'

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

# Get the DSID
thisDSID = runArgs.runNumber
# Apply the offset
thisDSID = thisDSID - 104
### JO logging
JOlog.info(" --- DSID shifting for NLO model --- ")
JOlog.info(" DSID : "+str(runArgs.runNumber)+" -> Shifting to : "+str(thisDSID))
JOlog.info(" Generating : "+str(nevents)+" events")

if thisDSID >= 410780 and thisDSID <= 410875:
    model   = 'TopFCNC'
    runName = 'mcttqZWb'+str(runArgs.runNumber)+'NLO5FS'
elif thisDSID >= 410876 and thisDSID <= 410892:
    model   = 'TopFCNC-onlyGam'
    runName = 'mcttqgamWb'+str(runArgs.runNumber)+'NLO5FS'

# Check that the DSID is in the range we expect
if thisDSID in allDIDS:
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
pdfmin=260001
pdfmax=260100

reweight_scale = '.true.'
reweight_PDF   = '.true.'

pdflabel='lhapdf'

maxjetflavor  = 5
parton_shower = 'PYTHIA8'

muR_over_ref  = 1.0
muF1_over_ref = 1.0
muF2_over_ref = 1.0
dyn_scale     = 10

lhe_version   = 3

extras = { 'lhaid'         : lhaid,
           'pdlabel'       : "'"+pdflabel+"'",
           'parton_shower' : parton_shower,
           'maxjetflavor'  : maxjetflavor,
           'reweight_scale': reweight_scale,
           'reweight_PDF'  : reweight_PDF,
           'PDF_set_min'   : pdfmin,
           'PDF_set_max'   : pdfmax,
           'muR_over_ref'  : muR_over_ref,
           'muF1_over_ref' : muF1_over_ref,
           'muF2_over_ref' : muF2_over_ref,
           'dynamical_scale_choice' : dyn_scale,
           'req_acc'       : 0.001
       }

# The couplings. Only real part.
# The tqH relevant couplings (c,u) x (LH,RH) (scalar of course)
RCtphi  = 0.
RCuphi  = 0.
RCtcphi = 0.
RCctphi = 0.

# The tqZ,tqg relevant couplings (c,u) x (Vector,Tensor) x (LH,RH)
# vector for tqZ
RC1utR = 0.
RC1utL = 0.
RC3utL = 0.
RC1ctR = 0.
RC1ctL = 0.
RC3ctL = 0.

# tensor for tqZ and tqgamma
RCtW  = 0.
RCtB  = 0.
RCuW  = 0.
RCuB  = 0.
RCtcW = 0.
RCtcB = 0.
RCctW = 0.
RCctB = 0.

## For MadSpin
madspin_card_loc = 'madspin_card.dat'
madspin_card_rep = madspin_card_loc
madspin_in       = 'import Events/'+runName+'/events.lhe'
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
wtyp = thisDSID%4
if wtyp == 0:
    wstr = ', w- > l- vl~'
elif wtyp == 1:
    wstr = ', w+ > l+ vl'
elif wtyp == 2:
    wstr = ', w- > j j'
elif wtyp == 3:
    wstr = ', w+ > j j'
else :
    raise RuntimeError("No W decays are generated please check the job option")

## The W decay - Overwrite for inclusive samples
if thisDSID >=410592  and thisDSID <=410595 :
    if wtyp == 0 or wtyp == 2:
        wstr = ', w- > All All'
    if wtyp == 1 or wtyp == 3:
        wstr = ', w+ > All All'

## The W decay - Overwrite for inclusive samples H->VV
if thisDSID >=410588  and thisDSID <=410591 :
    if wtyp == 0 or wtyp == 2:
        wstr = ', w- > All All'
    if wtyp == 1 or wtyp == 3:
        wstr = ', w+ > All All'

## The SM top
ttyp = thisDSID%2
if ttyp == 0:
    t2str = 'decay t~ > w- b~'
else:
    t2str = 'decay t > w+ b'

# the tqH part
if ( (thisDSID >= 410700 and thisDSID <= 410779) or (thisDSID >= 410588  and thisDSID <= 410595) ):

    keyName = 'Higgs'

    if thisDSID >= 410592 and thisDSID <= 410595 :
        theDIDS = thisDSID - 410700 + 104
    elif thisDSID >= 410588 and thisDSID <= 410591:
        theDIDS = thisDSID - 410700 + 108
    else :
        theDIDS = thisDSID - 410700

    #semi-manually setting the couplings (for the inclusive H decay cases)..
    if theDIDS == -4 or theDIDS == -3 :
        RCtphi  = 12.
    elif theDIDS == -2 or theDIDS == -1 :
        RCtcphi = 12.
    #other channels
    elif theDIDS >= 0 and theDIDS < 20:
        RCtcphi = 12.
    elif theDIDS < 40:
        RCtphi  = 12.
    elif theDIDS < 60:
        RCctphi = 12.
    else:
        RCuphi  = 12.

    qtyp = theDIDS/20
    if theDIDS >= 40:
        qtyp = (theDIDS-40)/20
    #For the inclusive H decay samples
    elif theDIDS <= 0:
        if theDIDS == -4 or theDIDS == -3 :
            qtyp=1
        elif theDIDS == -2 or theDIDS == -1 :
            qtyp=0

    if ttyp%2 == 0 and qtyp == 0:
        t1str = 'decay t > c h'
    elif ttyp%2 == 1 and qtyp == 0:
        t1str = 'decay t~ > c~ h'
    elif ttyp%2 == 0 and qtyp == 1:
        t1str = 'decay t > u h'
    elif ttyp%2 == 1 and qtyp == 1:
        t1str = 'decay t~ > u~ h'

    mscard.write("""%s\n%s%s\nlaunch"""%(t1str,t2str,wstr))

# the tqZ part
elif thisDSID >= 410780 and thisDSID <= 410875:

    keyName = 'Z'

    theDIDS = thisDSID - 410780

    if theDIDS < 12:
        RC1ctL = 7.0
    elif theDIDS < 24:
        RC1utL = 7.0
    elif theDIDS < 36:
        RCctW = 0.05
    elif theDIDS < 48:
        RCtW  = 0.05
    elif theDIDS < 60:
        RC1ctR = 7.0
    elif theDIDS < 72:
        RC1utR = 7.0
    elif theDIDS < 84:
        RCtcW  = 0.05
    elif theDIDS < 96:
        RCuW = 0.05

    ztyp = (theDIDS/4)%3
    if ztyp == 0:
        zstr = ', z > l- l+'
    elif ztyp == 1:
        zstr = ', z > vl vl~'
    else:
        zstr = ', z > j j'

    qtyp = theDIDS/12
    if (24 <= theDIDS < 48):
        qtyp = (theDIDS-24)/12
    elif (48 <= theDIDS < 72):
        qtyp = (theDIDS-48)/12
    elif (72 <= theDIDS < 96):
        qtyp = (theDIDS-72)/12

    if ttyp%2 == 0 and qtyp == 0:
        t1str = 'decay t > c z'
    elif ttyp%2 == 1 and qtyp == 0:
        t1str = 'decay t~ > c~ z'
    elif ttyp%2 == 0 and qtyp == 1:
        t1str = 'decay t > u z'
    elif ttyp%2 == 1 and qtyp == 1:
        t1str = 'decay t~ > u~ z'

    mscard.write("""%s%s\n%s%s\nlaunch"""%(t1str,zstr,t2str,wstr))

# the tqgam part
elif thisDSID >= 410876 and thisDSID <= 410891:

    keyName = 'photon'

    theDIDS = thisDSID - 410876

    if theDIDS < 4:
        RCtcB =  12.0
    elif theDIDS < 8:
        RCtB  = 12.0
    elif theDIDS < 12:
        RCctB = 12.0
    else:
        RCuB  = 12.0

    qtyp = theDIDS/4
    if theDIDS >= 8:
        qtyp = (theDIDS-8)/4
    if ttyp%2 == 0 and qtyp == 0:
        t1str = 'decay t >  c a'
    elif ttyp%2 == 1 and qtyp == 0:
        t1str = 'decay t~ > c~ a'
    elif ttyp%2 == 0 and qtyp == 1:
        t1str = 'decay t > u a'
    elif ttyp%2 == 1 and qtyp == 1:
        t1str = 'decay t~ > u~ a'

    mscard.write("""%s\n%s%s\nlaunch"""%(t1str,t2str,wstr))

else:
    raise RuntimeError("No good runNumber")
mscard.close()

coup = {
    'RCtphi'  : RCtphi ,
    'RCuphi'  : RCuphi ,
    'RCtcphi' : RCtcphi,
    'RCctphi' : RCctphi,

    'RC1utR'  : RC1utR,
    'RC1utL'  : RC1utL,
    'RC3utL'  : RC3utL,
    'RC1ctR'  : RC1ctR,
    'RC1ctL'  : RC1ctL,
    'RC3ctL'  : RC3ctL,
    'RCtW'    : RCtW,
    'RCtB'    : RCtB,
    'RCuW'    : RCuW,
    'RCuB'    : RCuB,
    'RCtcW'   : RCtcW,
    'RCtcB'   : RCtcB,
    'RCctW'   : RCctW,
    'RCctB'   : RCctB
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

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)
print_cards()

#
# Cook the setscales file for the user defined dynamical scale
fileN = process_dir+'/SubProcesses/setscales.f'
mark  = '      elseif(dynamical_scale_choice.eq.10) then'
rmLines = ['ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc',
           'cc      USER-DEFINED SCALE: ENTER YOUR CODE HERE                                 cc',
           'cc      to use this code you must set                                            cc',
           'cc                 dynamical_scale_choice = 10                                    cc',
           'cc      in the run_card (run_card.dat)                                           cc',
           'write(*,*) "User-defined scale not set"',
           'stop 1',
           'temp_scale_id=\'User-defined dynamical scale\' ! use a meaningful string',
           'tmp = 0',
           'cc      USER-DEFINED SCALE: END OF USER CODE                                     cc'
           ]
for line in fileinput.input(fileN, inplace=1):
    toKeep = True
    for rmLine in rmLines:
        if line.find(rmLine) >= 0:
           toKeep = False
           break
    if toKeep:
        print line,
    if line.startswith(mark):
        print """
          tmp=0d0
          do i=3,nexternal
             if (pp(0,i)*pp(0,i)-pp(3,i)*pp(3,i)-pp(2,i)*pp(2,i)-pp(1,i)*pp(1,i).ge.10.) then
                tmp=tmp+max(0d0,(pp(0,i)+pp(3,i))*(pp(0,i)-pp(3,i)))
             endif
          enddo
          tmp=dsqrt(tmp/2d0)
          temp_scale_id='sqrt(sum_i mT(i)**2/2), i=top quarks'
     """
#### End of setscales cooking

generate(run_card_loc='run_card.dat',param_card_loc=None,
         madspin_card_loc=madspin_card_loc,
         mode=mode,njobs=1,
         proc_dir=process_dir,run_name=runName,
         grid_pack=gridpack_mode,gridpack_dir=gridpack_dir,nevents=nevents,random_seed=runArgs.randomSeed)
arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz',lhe_version=lhe_version)

#### Shower
evgenConfig.description = 'aMcAtNlo+Pythia8+EvtGen ttbar production for FCNC NLO model'
evgenConfig.keywords += ['top', 'ttbar', 'FCNC', keyName]
evgenConfig.inputfilecheck = runName
#evgenConfig.inputconfcheck = ''
runArgs.inputGeneratorFile = runName+'._00001.events.tar.gz'
runArgs.contact = ['Ian Connelly <ian.connelly@cern.ch>']

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_aMcAtNlo.py")

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
