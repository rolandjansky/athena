from MadGraphControl.MadGraphUtils import *
import fileinput

nevents=11000
mode=0

### DSID lists 
allDIDS = range(410700,410892)

NLO = True
if NLO:
    runName = 'mcttqXWbNLO5FS'
    proc    = 'generate p p > t t~ [QCD]'
else:
    # this is just for quick checks
    runName = 'mcttqXWb'
    proc    = 'generate p p > t t~'
    
fcard = open('proc_card_mg5.dat','w')
if runArgs.runNumber in allDIDS:
    fcard.write("""import model loop_sm-no_b_mass\n%s\noutput -f"""%proc)
    fcard.close()
else: 
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)

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

maxjetflavor=5
parton_shower='PYTHIA8'

muR_over_ref  = 1.0
muF1_over_ref = 1.0
muF2_over_ref = 1.0
dyn_scale     = 0

lhe_version=3

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
           'dynamical_scale_choice' : dyn_scale 
       }

# The couplings. Only real part.
# The tqH relevant couplings (c,u) x (LH,RH) (scalar of course)
ReEtaLut = 0.
ReEtaRut = 0.
ReEtaLct = 0.
ReEtaRct = 0.

# The tqZ relevant couplings (c,u) x (Vector,Tensor) x (LH,RH)
ReXLut = 0.
ReXRut = 0.
ReXLct = 0.
ReXRct = 0.
ReKLut = 0.
ReKRut = 0.
ReKLct = 0.
ReKRct = 0.

# The tqgam relevant couplings (c,u) x (LH,RH) (it is a tensor)
ReLambdaLut = 0. 
ReLambdaRut = 0. 
ReLambdaLct = 0. 
ReLambdaRct = 0.

## For MadSpin
madspin_card_loc='madspin_card.dat'
mscard = open(madspin_card_loc,'w')
mscard.write("""
set max_weight_ps_point 400  # number of PS to estimate the maximum for each event  
set seed %i                                                        
import model topFCNC_UFO-cb_mass Cards/param_cardDecay.dat --bypass_check  
define l+ = l+ ta+
define l- = l- ta-
\n           
"""%runArgs.randomSeed)

## The W decay
wtyp = runArgs.runNumber%4
if wtyp == 0:
    wstr = ', w- > l- vl~'
elif wtyp == 1:
    wstr = ', w+ > l+ vl'
elif wtyp == 2:
    wstr = ', w- > j j'
elif wtyp == 3:
    wstr = ', w+ > j j'

## The SM top
ttyp = runArgs.runNumber%2
if ttyp == 0:
    t2str = 'decay t~ > w- b~'
else:
    t2str = 'decay t > w+ b'
        

# the tqH part
if runArgs.runNumber >= 410700 and runArgs.runNumber <= 410779:

    keyName = 'Higgs'
    
    theDIDS = runArgs.runNumber - 410700
    
    if theDIDS < 20:
        ReEtaLct = 0.05
    elif theDIDS < 40:
        ReEtaLut = 0.05
    elif theDIDS < 60:
        ReEtaRct = 0.05
    else:
        ReEtaRut = 0.05
        
    qtyp = theDIDS/20
    if theDIDS >= 40:
        qtyp = (theDIDS-40)/20
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
elif runArgs.runNumber >= 410780 and runArgs.runNumber <= 410875:

    keyName = 'Z'
    
    theDIDS = runArgs.runNumber - 410780

    if theDIDS < 12:
        ReXLct = 0.05
    elif theDIDS < 24:
        ReXLut = 0.05
    elif theDIDS < 36:
        ReKLct = 0.05
    elif theDIDS < 48:
        ReKLut = 0.05
    elif theDIDS < 60:
        ReXRct = 0.05
    elif theDIDS < 72:
        ReXRut = 0.05
    elif theDIDS < 84:
        ReKRct = 0.05
    elif theDIDS < 96:
        ReKRut = 0.05

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
elif runArgs.runNumber >= 410876 and runArgs.runNumber <= 410891:

    keyName = 'Photon'
    
    thDIDS = runArgs.runNumber - 410876
    
else:
    raise RuntimeError("No good runNumber")
mscard.close()

coup = {
    'ReEtaLut' : ReEtaLut,  
    'ReEtaRut' : ReEtaRut,  
    'ReEtaLct' : ReEtaLct,   
    'ReEtaRct' : ReEtaRct,   
                             
    'ReXLut' : ReXLut,     
    'ReXRut' : ReXRut,     
    'ReXLct' : ReXLct,     
    'ReXRct' : ReXRct,     
    'ReKLut' : ReKLut,     
    'ReKRut' : ReKRut,     
    'ReKLct' : ReKLct,     
    'ReKRct' : ReKRct,     
                             
    'ReLambdaLut' : ReLambdaLut,
    'ReLambdaRut' : ReLambdaRut,
    'ReLambdaLct' : ReLambdaLct,
    'ReLambdaRct' : ReLambdaRct
    }

process_dir = new_process()

paramFileName  = 'MadGraph_param_card_ttFCNC.dat'
paramFileNameN = 'param_cardDecay.dat'
paramFile      = subprocess.Popen(['get_files','-data',paramFileName]).communicate()
if not os.access(paramFileName, os.R_OK):
    print 'ERROR: Could not get param card'
    raise RuntimeError("parameter card '%s' missing!"%paramFileName)

build_param_card(param_card_old=paramFileName,param_card_new=paramFileNameN,
                 params={'newcoup':coup}
             )
shutil.copyfile(paramFileNameN,process_dir+'/Cards/'+paramFileNameN)

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)
print_cards()

#
# Cook the setscales file for the user defined dynamical scale
fileN = process_dir+'/SubProcesses/setscales.f'
mark  = '      elseif(dynamical_scale_choice.eq.0) then'
rmLines = ['ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc',
           'cc      USER-DEFINED SCALE: ENTER YOUR CODE HERE                                 cc',
           'cc      to use this code you must set                                            cc',
           'cc                 dynamical_scale_choice = 0                                    cc',
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

generate(run_card_loc='run_card.dat',param_card_loc=None,madspin_card_loc=madspin_card_loc,mode=mode,proc_dir=process_dir)
arrange_output(proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz',lhe_version=lhe_version)  
  
#### Shower
evgenConfig.description = 'aMcAtNlo+Pythia8+EvtGen ttbar production for FCNC'
evgenConfig.keywords += ['top', 'ttbar', 'FCNC', keyName]
evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile = runName+'._00001.events.tar.gz'

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_aMcAtNlo.py")
