import os

os.environ['PYTHIA8DATA']='/afs/cern.ch/sw/lcg/external/MCGenerators_hepmc2.03.11/pythia8/162/i686-slc5-gcc43-opt/xmldoc/'
os.system("echo $MADPATH")
os.system("echo $PYTHIA8DATA")

from MadGraphControl.MadGraphUtils import *
from MadGraphControl.SetupMadGraphEventsForSM import setupMadGraphEventsForSM

fcard = open('proc_card_mg5.dat','w')

fcard.write("""
import model RPVMSSM_UFO
define p = g u c d s u~ c~ d~ s~
define j = u d s u~ d~ s~ b b~
define l+ = e+ mu+
define l- = e- mu-
define vl = ve vm vt
define vl~ = ve~ vm~ vt~
generate p p > go go, go > j j j
output -f
""")

fcard.close()

process_dir = new_process()

jetmin = 50
xqcut = 12

# Grab the run card and move it into place
runcard = subprocess.Popen(['get_files','-data','run_card.SM.dat'])
runcard.wait()
if not os.access('run_card.SM.dat',os.R_OK):
    print 'ERROR: Could not get run card'
#elif os.access('run_card.dat',os.R_OK):
#    print 'ERROR: Old run card in the current directory.  Dont want to clobber it.  Please move it first.'
else:
    oldcard = open('run_card.SM.dat','r')
    newcard = open('run_card.dat','w')
    for line in oldcard:
        if ' xqcut ' in line:
            newcard.write('%f   = xqcut   ! minimum kt jet measure between partons \n'%(xqcut))
        elif ' nevents ' in line:
            newcard.write('  %i       = nevents ! Number of unweighted events requested \n'%(1000))
        elif ' iseed ' in line:
            newcard.write('   %i      = iseed   ! rnd seed (0=assigned automatically=default)) \n'%(runArgs.randomSeed))
        elif ' xptj ' in line:
            newcard.write('   %i      = xptj ! minimum pt for at least one jet \n'%(jetmin))
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()

if runArgs.runNumber==156302:
    Mgo = '1.000000e+02'
elif runArgs.runNumber==156303:
    Mgo = '2.000000e+02'
elif runArgs.runNumber==156304:
    Mgo = '3.000000e+02'
elif runArgs.runNumber==156305:
    Mgo = '4.000000e+02'
elif runArgs.runNumber==156306:
    Mgo = '5.000000e+03'
elif runArgs.runNumber==156307:
    Mgo = '6.000000e+03'
else:
    print 'ERROR - no Mgo specified'

paramcard = subprocess.Popen(['get_files','-data','param_card.RPV_Gluino.dat'])
paramcard.wait()
if not os.access('param_card.RPV.gogo.dat',os.R_OK):
    print 'ERROR: Could not get run card'
else:
    oldcard = open('param_card.RPV.gogo.dat','r')
    newcard = open('param_card.dat','w')
    for line in oldcard:
        if ' Mgo' in line:
            newcard.write('  1000021 %s # Mgo \n'%(Mgo))
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()


generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=2,njobs=8,run_name='Test',proc_dir=process_dir)

stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_Pythia8_RPV_Gluino_'+Mgo[0:3]
if hasattr(runArgs,'skipEvents'): skip_events=runArgs.skipEvents

skip_events=0

arrange_output(run_name='Test',proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz',skip_events=skip_events)

####PYTHIA 8 SHOWERING TO EVGEN############################

from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence("TopAlg")

from Pythia8_i.Pythia8_iConf import Pythia8_i
Pythia8 = Pythia8_i()
topAlg += Pythia8
Pythia8.Commands += ["Tune:pp=5"]
Pythia8.Commands += [ "Main:timesAllowErrors = 1000"]
Pythia8.Commands += ["6:m0 = 172.5"]
Pythia8.Commands += ["23:m0 = 91.1876"]
Pythia8.Commands += ["23:mWidth = 2.4952"]
Pythia8.Commands += ["24:m0 = 80.399"]
Pythia8.Commands += ["24:mWidth = 2.085"]
Pythia8.Commands += ["ParticleDecays:limitTau0 = on"]  # set K_S, Lambda stable
Pythia8.Commands += ["PDF:useLHAPDF=on"]
Pythia8.Commands += ["PDF:LHAPDFset = cteq6ll.LHpdf"]

Pythia8.LHEFile = stringy+'._00001.events'
Pythia8.MaxFailures = 1000000

from MC11JobOptions.MadGraphEvgenConfig import evgenConfig
evgenConfig.inputfilebase = stringy+'._00001'

runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'

for i in xrange(len(evgenConfig.generators) ):
    if evgenConfig.generators[i]=='Pythia':
        evgenConfig.generators[i]='Pythia8'



