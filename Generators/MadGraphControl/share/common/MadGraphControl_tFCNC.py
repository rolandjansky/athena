from MadGraphControl.MadGraphUtils import *
import fileinput

nevents=int(1.1*runArgs.maxEvents)
mode=0

### DSID lists 
allDIDS = [410650, 410653]

if runArgs.runNumber not in range(allDIDS[0], allDIDS[1]+1):
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)
  
NLO = True # for testing, set to false to reduce runtime
order = " [QCD]" if NLO else ""

is_u_quark     = True if (runArgs.runNumber == 410650 or runArgs.runNumber == 410651) else False
is_left_handed = True if (runArgs.runNumber == 410650 or runArgs.runNumber == 410652) else False

restrict = "onlytugam" if is_u_quark else "onlytcgam"
  
fcard = open("proc_card_mg5.dat","w")
fcard.write("""
    import model TopFCNC-%s
    
    define p = g u c d s b u~ c~ d~ s~ b~
    define j = g u c d s b u~ c~ d~ s~ b~
    
    define l+ = e+ mu+ ta+
    define l- = e- mu- ta-
    define vl = ve vm vt
    define vl~ = ve~ vm~ vt~    
    
    generate p p > t a $$ t~ %s
    add process p p > t~ a $$ t %s
    
    output -f"""%(restrict,order,order))
fcard.close()

runName = "mcFCNCtgam"+str(runArgs.runNumber)+"NLO5FS"

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy/2.
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

extras = { 'lhaid'          : lhaid,
           'pdlabel'        : "'"+pdflabel+"'",
           'parton_shower'  : parton_shower,
           'maxjetflavor'   : maxjetflavor,
           'reweight_scale' : reweight_scale,
           'reweight_PDF'   : reweight_PDF,
           'PDF_set_min'    : pdfmin, 
           'PDF_set_max'    : pdfmax,
           'muR_over_ref'   : muR_over_ref,
           'muF1_over_ref'  : muF1_over_ref,        
           'muF2_over_ref'  : muF2_over_ref,
           'dynamical_scale_choice' : dyn_scale, 
           
           'use_syst'       : 'T',
           'sys_scalefact'  : '1 0.5 2',
           'sys_alpsfact'   : 'None',
           'sys_matchscale' : 'None',
           'sys_pdf'        : 'T',           
}

## For MadSpin
madspin_card_loc = 'madspin_card.dat'
madspin_seed     = runArgs.randomSeed

mscard = open(madspin_card_loc,'w')
mscard.write("""
set Nevents_for_max_weigth 250 # number of events for the estimate of the max. weight (default: 75)
set max_weight_ps_point 1000   # number of PS to estimate the maximum for each event  (default: 400)

set seed %i                                              

define l+ = e+ mu+ ta+
define l- = e- mu- ta-
define vl = ve vm vt
define vl~ = ve~ vm~ vt~ 

decay t > w+ b, w+ > l+ vl
decay t~ > w- b~, w- > l- vl~

launch
"""%runArgs.randomSeed)
mscard.close()

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
RCuW  = 0. 
RCtB  = 0.  # vertex tuy (left-handed)  
RCuB  = 0.  # vertex tuy (right-handed)  
RCtcW = 0.
RCctW = 0.
RCtcB = 0.  # vertex tcy (left-handed)  
RCctB = 0.  # vertex tcy (right-handed)  

# tensor for tgq
RCuG  = 0.
RCtG  = 0.
RCctG = 0.
RCtcG = 0.

keyName = 'photon'

if is_left_handed:
  if is_u_quark:
    RCtB = 12.0    # vertex tuy (left-handed)          
  else:
    RCtcB = 12.0   # vertex tcy (left-handed)    
else:
  if is_u_quark:
    RCuB = 12.0    # vertex tuy (right-handed)      
  else:
    RCctB = 12.0   # vertex tcy (right-handed)      
  
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
    'RCctB'   : RCctB,
    
    'RCuG'    : RCuG,  
    'RCtG'    : RCtG,  
    'RCctG'   : RCctG,  
    'RCtcG'   : RCtcG,
    }

process_dir = new_process()

paramFileName  = 'MadGraph_param_card_ttFCNC_NLO.dat'
paramFileNameN = 'param_card.dat'
paramFile      = subprocess.Popen(['get_files','-data',paramFileName]).communicate()
if not os.access(paramFileName, os.R_OK):
    print 'ERROR: Could not get param card'
    raise RuntimeError("parameter card '%s' missing!"%paramFileName)

build_param_card(param_card_old=paramFileName,param_card_new=paramFileNameN,params={'dim6':coup})
shutil.copyfile(paramFileNameN,process_dir+'/Cards/'+paramFileNameN)

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)
print_cards()

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
c         mass of the top quark (that is the most massive particle produced)
c         Q^2=m_top          
          tmp=0d0  
          do i=3,nexternal
             tmp=max(tmp, pp(0,i)*pp(0,i)-pp(3,i)*pp(3,i)-pp(2,i)*pp(2,i)-pp(1,i)*pp(1,i))
          enddo       
          tmp=dsqrt(tmp)
          temp_scale_id='m_top'
     """
#### End of setscales cooking

generate(run_card_loc='run_card.dat',param_card_loc=None,
         madspin_card_loc=madspin_card_loc,
         mode=mode,njobs=1,
         proc_dir=process_dir,run_name=runName,
         nevents=nevents,random_seed=runArgs.randomSeed)
outputDS = arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz',lhe_version=lhe_version)  
  
#### Shower
evgenConfig.description = 'aMcAtNlo+Pythia8+EvtGen single top production for FCNC'
evgenConfig.keywords += ["singleTop", "FCNC"]
evgenConfig.keywords += [keyName]
evgenConfig.contact = ["gregor.gessner@cern.ch"]
runArgs.inputGeneratorFile = outputDS

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_aMcAtNlo.py")