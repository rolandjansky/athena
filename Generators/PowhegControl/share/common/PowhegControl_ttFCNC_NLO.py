#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = 'POWHEG+Pythia8 ttbar production with Powheg hdamp equal 1.5*top mass, A14 tune, ME NNPDF30 NLO, A14 NNPDF23 LO, FCNC Top decays'
evgenConfig.keywords    = [ 'top', 'ttbar', 'Higgs', 'FCNC' ]
evgenConfig.contact     = [ 'james.robinson@cern.ch','andrea.helen.knue@cern.ch','onofrio@liverpool.ac.uk','ian.connelly@cern.ch']

include('PowhegControl/PowhegControl_tt_Common.py')
# Initial settings
if hasattr(PowhegConfig, "topdecaymode"):
    # Use PowhegControl-00-02-XY (and earlier) syntax
    PowhegConfig.topdecaymode = 22222 # inclusive top decays
else:
    # Use PowhegControl-00-03-XY (and later) syntax
    PowhegConfig.decay_mode      = "t t~ > undecayed"
    PowhegConfig.MadSpin_enabled = False

PowhegConfig.hdamp        = 258.75                                        # 1.5 * mtop
DoSingleWeight = False
if DoSingleWeight:
    PowhegConfig.mu_F         = 1.0
    PowhegConfig.mu_R         = 1.0 
    PowhegConfig.PDF          = 260000 
else:
    PowhegConfig.mu_F         = [1.0, 2.0, 0.5, 1.0, 1.0, 0.5, 2.0, 0.5, 2.0] # List of factorisation scales which pairs with renormalisation scale below
    PowhegConfig.mu_R         = [1.0, 1.0, 1.0, 2.0, 0.5, 0.5, 2.0, 2.0, 0.5] # List of renormalisation scales
    PowhegConfig.PDF          = [260000, 25200, 13165, 90900]                 # NNPDF30, MMHT, CT14, PDF4LHC - PDF variations with nominal scale variation
    PowhegConfig.PDF.extend(range(260001, 260101))                            # Include the NNPDF error set
    PowhegConfig.PDF.extend(range(90901 , 90931 ))                            # Include the PDF4LHC error set

PowhegConfig.nEvents     *= 1.1 # compensate filter efficiency
PowhegConfig.generate()

#--------------------------------------------------------------
# Now preparing for MadSpin
#--------------------------------------------------------------

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

#t-c-H or t-u-H
istcH = any("Q2cH" in JO for JO in runArgs.jobConfig)

# Get the DSID
thisDSID = runArgs.runNumber
# Apply the offset
if istcH:
    thisDSID = thisDSID - 64
else:
    thisDSID = thisDSID - 48
   
model            = 'TopFCNC-onlyh' #eventually, change for tqZ, tqphoton, tqgluon
madspin_card_rep = 'madspin_card.dat'
madspin_in       = 'import run.lhe'
madspin_rep      = 'set ms_dir MadSpin'
madspin_seed     = runArgs.randomSeed

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
## The SM top
ttyp = thisDSID%2
if ttyp == 0:
    t2str = 'decay t~ > w- b~'
else:
    t2str = 'decay t > w+ b'

# the tqH part
if ( (thisDSID >= 410700 and thisDSID <= 410779) or (thisDSID >= 410588  and thisDSID <= 410595) or (thisDSID >= 411106  and thisDSID <= 411129) ):

    keyName = 'Higgs'

    if thisDSID >= 410592 and thisDSID <= 410595 :
        theDIDS = thisDSID - 410700 + 104
    elif thisDSID >= 410588 and thisDSID <= 410591:
        theDIDS = thisDSID - 410700 + 108
    elif ( thisDSID >= 411106  and thisDSID <= 411129 ):
        theDIDS = thisDSID - 411106
    else :
        theDIDS = thisDSID - 410700

    #semi-manually setting the couplings (for the inclusive H decay cases)..
    if theDIDS == -4 or theDIDS == -3 :
        RCtphi  = 1.
    elif theDIDS == -2 or theDIDS == -1 :
        RCtcphi = 1.
    #other channels
    elif theDIDS >= 0 and theDIDS < 20:
        RCtcphi = 1.
    elif theDIDS < 40:
        RCtphi  = 1.
    elif theDIDS < 60:
        RCctphi = 1.
    else:
        RCuphi  = 1.

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

import shutil,os,subprocess

paramFileName  = 'MadGraph_param_card_ttFCNC_NLO_FixedFCNCBR.dat'
paramFileNameN = 'param_card.dat'
paramFile      = subprocess.Popen(['get_files','-data',paramFileName]).communicate()
if not os.access(paramFileName, os.R_OK):
    print 'ERROR: Could not get param card'
    raise RuntimeError("parameter card '%s' missing!"%paramFileName)

from MadGraphControl.MadGraphUtils import *
build_param_card(param_card_old=paramFileName,param_card_new=paramFileNameN,
                 params={'dim6':coup})

##### Start LHE cooking for MadSpin

fin  = open('PowhegOTF._1.events','r')
fout = open('run.lhe','w')
line = fin.readline()
while line != "-->\n":
  fout.write(line)
  line = fin.readline()
fout.write(line)

# add the process
fout.write('<MG5ProcCard>\n')
fout.write('import model /cvmfs/atlas.cern.ch/repo/sw/Generators/madgraph/models/latest/%s\n'%(model)) 
fout.write('generate p p > t t~ [QCD]  \n')                                             
fout.write('</MG5ProcCard>\n')

# add run parameters
eline = str(PowhegConfig.nEvents)+' = nevents\n'
fout.write('<MGRunCard>\n')
fout.write('#0.01 = req_acc_FO\n') #Need because of a new check introduced in 2.5.0 : if not there, RunCardLO will be considered, which wants a line like that "x = nhel ! x = 0,1"
fout.write(eline)
fout.write('50.0 = bwcutoff\n')
fout.write('</MGRunCard>\n')

# add model parameters
fout.write('<slha>\n')
shutil.copyfileobj(open(paramFileNameN, 'r'), fout)
fout.write('</slha>\n')
fout.write('<montecarlomasses>\n')
fout.write('       1   0.330000E+00\n')
fout.write('       2   0.330000E+00\n')
fout.write('       3   0.500000E+00\n')
fout.write('       4   0.150000E+01\n')
fout.write('       5   0.480000E+01\n')
fout.write('      11   0.510999E-03\n')
fout.write('      13   0.105658E+00\n')
fout.write('      15   0.177682E+01\n')
fout.write('      21   0.000000E+00\n')
fout.write('</montecarlomasses>\n')

# add the events !
line = fin.readline()
while 'LesHouchesEvents' not in line:
  fout.write(line)
  line = fin.readline()
fout.write(line)
fout.close()

#### End of LHE cooking for MadSpin

# run MadSpin
os.system('$MADPATH/MadSpin/madspin < madspin_card.dat')

# This is a Powheg job, so expect lhe file to be called PowhegOTF._1.events
unzip = subprocess.Popen(['gunzip','run_decayed.lhe.gz'])
unzip.wait()
os.system('cp run_decayed.lhe PowhegOTF._1.events')

# Does MadSpin add some spurious lines that make the weight names unreadable ?
# see https://bugs.launchpad.net/mg5amcnlo/+bug/1720979
with open('PowhegOTF._1.events', 'r+') as f:
    t = f.read()
    to_delete = [ '<![CDATA[', ']]>' ]
    f.seek(0)
    for line in t.split('\n'):
        if line not in to_delete:
            f.write(line + '\n')
    f.truncate()

#--------------------------------------------------------------
# Pythia8 showering
#--------------------------------------------------------------
include('Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py')
include("Pythia8_i/Pythia8_Powheg_Main31.py")
genSeq.Pythia8.Commands += [ 'POWHEG:pThard = 0' ]
genSeq.Pythia8.Commands += [ 'POWHEG:nFinal = 2' ]
genSeq.Pythia8.Commands += [ 'POWHEG:pTdef = 2' ]
genSeq.Pythia8.Commands += [ 'POWHEG:vetoCount = 3' ]
genSeq.Pythia8.Commands += [ 'POWHEG:pTemt  = 0' ]
genSeq.Pythia8.Commands += [ 'POWHEG:emitted = 0' ]
genSeq.Pythia8.Commands += [ 'POWHEG:MPIveto = 0' ]

