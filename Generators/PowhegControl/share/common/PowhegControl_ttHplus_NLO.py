#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = 'POWHEG+Madspin+Pythia8 ttbar production with Powheg hdamp equal 1.5*top mass, A14 tune, ME NNPDF30 NLO, A14 NNPDF23 LO, Higgs plus decays for masses from 80-160'
evgenConfig.keywords    =['Higgs','MSSM','BSMHiggs','chargedHiggs','top','ttbar']
evgenConfig.contact     = [ 'anna.ivina@cern.ch']
evgenConfig.minevents = 10000


include('PowhegControl/PowhegControl_tt_Common.py')
# Initial settings
if hasattr(PowhegConfig, "topdecaymode"):
    # Use PowhegControl-00-02-XY (and earlier) syntax
    PowhegConfig.topdecaymode = 22222 # inclusive top decays
else:
    # Use PowhegControl-00-03-XY (and later) syntax
    PowhegConfig.decay_mode      = "t t~ > undecayed"
    PowhegConfig.MadSpin_enabled = False

PowhegConfig.hdamp        = 258.75                                             # 1.5 * mtop
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

#PowhegConfig.nEvents     *= 1.1 # compensate filter efficiency, this will probably is not needed for Hplus
PowhegConfig.generate()

#--------------------------------------------------------------
# Now preparing for MadSpin
#--------------------------------------------------------------
# Get the DSID and also make the strings
thisDSID = runArgs.runNumber #you can use it if you want to decay tops or W to something else
model            = '2HDMtypeII'
madspin_card_rep = 'madspin_card.dat'
madspin_in       = 'import run.lhe'
madspin_rep      = 'set ms_dir MadSpin'
madspin_seed     = runArgs.randomSeed

#open the mscard and write the parameters, set seed,and decay
#included into the define p and j the b quark
mscard = open(madspin_card_rep,'w')
mscard.write("""
set Nevents_for_max_weigth 250 # number of events for the estimate of the max. weight (default: 75)
set max_weight_ps_point 1000  # number of PS to estimate the maximum for each event   (default: 400)
set seed %i
%s
%s
define l+ = e+ mu+ ta+
define l- = e- mu- ta-
define vl = ve vm vt
define vl~ = ve~ vm~ vt~
define p = g u c d b s u~ c~ d~ s~ b~
define j = g u c d b s u~ c~ d~ s~ b~
\n
"""%(madspin_seed,madspin_in,madspin_rep))


#make the mscard
#W decay
wstr = ', w- > l- vl~'
## The SM top
t2str = 'decay t~ > b~ w-'
t1str = 'decay t > b h+'
mscard.write("""%s\n%s%s\nlaunch"""%(t1str,t2str,wstr))
mscard.close()

#--------------------------------------------------------------
# Charge Higgs, and all other masses in GeV
#--------------------------------------------------------------
###take the mass from the JO line
mhc_str = str(runArgs.jobConfig[0])  # JOB OPTION NAME MUST CONTAIN THE MASS WE WANT TO SIMULATE IN FORMAT LIKE: *_H160_*
mhc=0
int(s)
for s in mhc_str.split("_"):
    ss=s.replace("H","")
    if ss.isdigit():
        mhc = int(ss)
        print  "Charged Higgs mass set to %i"%mhc
if mhc==0:
   raise RuntimeError("Charged Higgs mass not set, mhc=0, check joOption name %s"%mhc_str)
###
import math
mh1=1.250e+02
mh2=math.sqrt(math.pow(mhc,2)+math.pow(8.0399e+01,2))
mh3=mh2


masses = {'25':str(mh1)+'  #  mh1',
          '35':str(mh2)+'  #  mh2',
          '36':str(mh2)+'  #  mh2',
          '37':str(mhc)+'  #  mhc'}
decayss={'37': 'DECAY 37 1.300000e+02 # whc'}


#write_param_card is part of the 2HDMTypeII model
import shutil,os,subprocess

paramFileName  = 'MadGraph_param_card_ttHplus_NLO.dat'
paramFileNameN = 'param_card.dat'

from MadGraphControl.MadGraphUtils import *
build_param_card(param_card_old=paramFileName,param_card_new=paramFileNameN,masses=masses,decays=decayss)


##### Start LHE cooking for MadSpin
fin  = open('PowhegOTF._1.events','r')
fout = open('run.lhe','w')
line = fin.readline()
while line != "-->\n":
  fout.write(line)
  line = fin.readline()
fout.write(line)


# add the process
#to use 5 FS you need to set b quark mass to zero at model level, use restrict_nobmass provided in the model folder
fout.write('<MG5ProcCard>\n')
fout.write('import model /cvmfs/atlas.cern.ch/repo/sw/Generators/madgraph/models/latest/%s-nobmass\n'%(model))
fout.write('generate p p > t t~ [QCD]  \n')
fout.write('</MG5ProcCard>\n')

# add run parameters
eline = str(PowhegConfig.nEvents)+' = nevents\n'
fout.write('<MGRunCard>\n')
fout.write('#0.01 = req_acc_FO\n') #Need because of a new check introduced in 2.5.0 : if not there, RunCardLO will be considered, which wants a line like that "x = nhel ! x = 0,1"
fout.write(eline)
fout.write('50.0 = bwcutoff\n')
fout.write('</MGRunCard>\n')



#add model parameters
fout.write('<slha>\n')
shutil.copyfileobj(open(paramFileNameN, 'r'), fout)
fout.write('</slha>\n')
fout.write('<montecarlomasses>\n')
fout.write('       1   0.330000E+00\n')
fout.write('       2   0.330000E+00\n')
fout.write('       3   0.500000E+00\n')
fout.write('       4   0.1550000E+01\n')
fout.write('       5   0.470000E+01\n')
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
