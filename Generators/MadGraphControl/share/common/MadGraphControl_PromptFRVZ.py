from MadGraphControl.MadGraphUtils import *

# 301537 run number for FRVZ 2zd Prompt mH125
# 301538 run number for FRVZ 4zd Prompt mH125

# Variables that depend on run number: which process and which Higgs mass
process = { 
    2:  'generate g g > h > nd2 nd2, (nd2 > nd1 zd, zd > f- f+), (nd2 > nd1 zd, zd > f- f+)',
    4:  'generate g g > h > nd2 nd2, (nd2 > nd1 hd1, (hd1 > zd zd, zd > f- f+)), (nd2 > nd1 hd1, (hd1 > zd zd, zd > f- f+))'
    }
modelcode='usrmodv4_lj_Run2'

if runArgs.runNumber==301537:    #check
    mH=125
    nGamma=2
    LJ='Prompt'
elif runArgs.runNumber==301538:       #check
    mH=125
    nGamma=4
    LJ='Prompt'
else:
    raise RuntimeError('Bad runNumber')

if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    beamEnergy = 7500.

rcard='MadGraph_run_card_FRVZprompt.dat'
pcard='MadGraph_param_card_FRVZzd%smH%s.dat' % (nGamma, mH)

# initialise random number generator/sequence
import random
random.seed(runArgs.randomSeed)
# lifetime function
def lifetime(avgtau = 21):
    import math
    t = random.random()
    return -1.0 * avgtau * math.log(t)

# basename for madgraph LHEF file
rname = 'run_lj'+str(runArgs.runNumber)

# do not run MadGraph if config only is requested
if not opts.config_only:

    # writing proc card for MG
    fcard = open('proc_card_mg5.dat','w')
    fcard.write("""
import model_v4 %s
define p = g u c d s u~ c~ d~ s~
define f- = me- mmu- pi-
define f+ = me+ mmu+ pi+
%s
output -f
""" % (modelcode, process[nGamma]))
    fcard.close()


    # getting run cards
    from PyJobTransformsCore.trfutil import get_files
    get_files( rcard, keepDir=False, errorIfNotFound=True )
    get_files( pcard, keepDir=False, errorIfNotFound=True )

    # generating events in MG
    process_dir = new_process()

    generate(run_card_loc=rcard,param_card_loc=pcard,mode=0,njobs=1,run_name=rname,proc_dir=process_dir)

    # replacing lifetime of dark photon, manually
    unzip1 = subprocess.Popen(['gunzip',process_dir+'/Events/'+rname+'/unweighted_events.lhe.gz'])
    unzip1.wait()

    oldlhe = open(process_dir+'/Events/'+rname+'/unweighted_events.lhe','r')
    newlhe = open(process_dir+'/Events/'+rname+'/unweighted_events2.lhe','w')
    init = True
    for line in oldlhe:
        if init==True:
            newlhe.write(line)
            if '</init>' in line:
                init = False
        else:
            newline = line.rstrip('\n')
            columns=newline.split()
            pdgid=columns[0]
            if pdgid == '3000001':
                if LJ=='Displaced':
                   part1 = line[:-7]
                   part2 = "%.11E" % (lifetime(avgtau))
                   part3 = line[-5:]
                   newlhe.write(part1+part2+part3)
                else:
                   newlhe.write(line)
            elif pdgid == '-3000011':
                part1 = '      -11'
                part2 = line[10:]
                newlhe.write(part1+part2)
            elif pdgid == '3000011':
                part1 = '       11'
                part2 = line[10:]
                newlhe.write(part1+part2)
            elif pdgid == '-3000013':
                part1 = '      -13'
                part2 = line[10:]
                newlhe.write(part1+part2)
            elif pdgid == '3000013':
                part1 = '       13'
                part2 = line[10:]
                newlhe.write(part1+part2)
            else:
                newlhe.write(line)

    oldlhe.close()
    newlhe.close()

    zip1 = subprocess.Popen(['gzip',process_dir+'/Events/'+rname+'/unweighted_events2.lhe'])
    zip1.wait()
    shutil.move(process_dir+'/Events/'+rname+'/unweighted_events2.lhe.gz',process_dir+'/Events/'+rname+'/unweighted_events.lhe.gz')
    os.remove(process_dir+'/Events/'+rname+'/unweighted_events.lhe')

    arrange_output(run_name=rname,proc_dir=process_dir,outputDS=rname+'._00001.events.tar.gz')

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
include ( "Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py" )
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
include("Pythia8_i/Pythia8_MadGraph.py")
genSeq.Pythia8.Commands += ["Main:timesAllowErrors = 60000"]

#relax the cuts on displaced vertices and non G4 particles
testSeq.TestHepMC.MaxTransVtxDisp = 100000000 #in mm
testSeq.TestHepMC.MaxVtxDisp = 100000000 #in mm
testSeq.TestHepMC.MaxNonG4Energy = 100000000 #in MeV

#--------------------------------------------------------------
# Configuration for EvgenJobTransforms
#--------------------------------------------------------------
evgenConfig.description = 'FRVZ process Higgs -> %sgamma_d + X with mH=%sGeV, %s LJs' % (nGamma,mH,LJ)
evgenConfig.keywords = ["exotic", "BSMHiggs", "BSM"]
evgenConfig.contact  = ['bmeirose@cern.ch']
evgenConfig.process="LJ_FRVZ"
evgenConfig.inputfilecheck = rname
runArgs.inputGeneratorFile=rname+'._00001.events.tar.gz'

