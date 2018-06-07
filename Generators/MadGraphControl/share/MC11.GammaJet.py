from MadGraphControl.MadGraphUtils import *
from MadGraphControl.SetupMadGraphEventsForSM import setupMadGraphEventsForSM

fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model sm
define p = g u c d s u~ c~ d~ s~
define j = g u c d s u~ c~ d~ s~
define l+ = e+ mu+
define l- = e- mu-
define vl = ve vm vt
define vl~ = ve~ vm~ vt~
generate p p > a j @1
add process p p > a j j @2
add process p p > a j j j @3
output -f
""")
fcard.close()

process_dir = new_process()

if runArgs.runNumber==126717:
    jetmin = 25
    xqcut = 15
elif runArgs.runNumber==126718:
    jetmin = 50
    xqcut = 25
elif runArgs.runNumber==126719:
    jetmin = 100
    xqcut = 40
elif runArgs.runNumber==126720:
    jetmin = 200
    xqcut = 60
elif runArgs.runNumber==126721:
    jetmin = 400
    xqcut = 100
elif runArgs.runNumber==126722:
    jetmin = 600
    xqcut = 150
elif runArgs.runNumber==126723:
    jetmin = 800
    xqcut = 200

# Grab the run card and move it into place
runcard = subprocess.Popen(['get_files','-data','run_card.SM.dat'])
runcard.wait()
if not os.access('run_card.SM.dat',os.R_OK):
    print 'ERROR: Could not get run card'
elif os.access('run_card.dat',os.R_OK):
    print 'ERROR: Old run card in the current directory.  Dont want to clobber it.  Please move it first.'
else:
    oldcard = open('run_card.SM.dat','r')
    newcard = open('run_card.dat','w')
    for line in oldcard:
        if ' xqcut ' in line:
            newcard.write('%f   = xqcut   ! minimum kt jet measure between partons \n'%(xqcut))
        elif ' nevents ' in line:
            newcard.write('  %i       = nevents ! Number of unweighted events requested \n'%(20000))
        elif ' iseed ' in line:
            newcard.write('   %i      = iseed   ! rnd seed (0=assigned automatically=default)) \n'%(runArgs.randomSeed))
        elif ' xptj ' in line:
            newcard.write('   %i      = xptj ! minimum pt for at least one jet \n'%(jetmin))
        elif ' xpta ' in line:
            newcard.write('   50      = xpta ! minimum pt for at least one photon \n')
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()

generate(run_card_loc='run_card.dat',param_card_loc=None,mode=0,njobs=1,run_name='Test',proc_dir=process_dir)

stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_GammaJet_'+str(jetmin)

skip_events=0
if hasattr(runArgs,'skipEvents'): skip_events=runArgs.skipEvents
arrange_output(run_name='Test',proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz',skip_events=skip_events)

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence("TopAlg")
include ( "MC11JobOptions/MC11_PythiaAUET2B_CTEQ6L1_Common.py" ) # Because the CTEQ6L1 PDF was used!
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
Pythia = topAlg.Pythia
Pythia.PythiaCommand = ["pyinit user madgraph"]
#--------------------------------------------------------------
Pythia.PythiaCommand +=  [
            "pystat 1 3 4 5",
            "pyinit dumpr 1 5",
            "pyinit pylistf 1",
            ]
# ... TAUOLA and Photos
Pythia.PythiaCommand += ["pydat1 parj 90 20000",
                          "pydat3 mdcy 15 1 0" ]
# ... Tauola
include ( "MC11JobOptions/MC11_Tauola_Fragment.py" )
# ... Photos
include ( "MC11JobOptions/MC11_Photos_Fragment.py" )
#--------------------------------------------------------------
# Configuration for EvgenJobTransforms
#--------------------------------------------------------------
from MC11JobOptions.MadGraphEvgenConfig import evgenConfig
evgenConfig.minevents = 5000
evgenConfig.inputfilebase = stringy
runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'

# Additional bit for ME/PS matching
phojf=open('./pythia_card.dat', 'w')
phojinp = """
      !...Matching parameters...
      IEXCFILE=0
      showerkt=T
      qcut=%i
      imss(21)=24
      imss(22)=24  
    """%(xqcut)

phojf.write(phojinp)
phojf.close()

