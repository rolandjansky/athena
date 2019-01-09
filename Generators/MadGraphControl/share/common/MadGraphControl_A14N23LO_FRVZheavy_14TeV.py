from MadGraphControl.MadGraphUtils import *

# run parameters
mH= 0
nGamma = 0
avgtau = 0
category = 'prompt'

if runArgs.runNumber == 415200:
    mH=125
    nGamma=2
    avgtau = 856.0
if avgtau > 0:
	category = 'displaced'

if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    beamEnergy = 7000.

# MadGraph info
process = {
    2:  'generate g g > h > chi2 chi2, (chi2 > chi1 zp, zp > f- f+), (chi2 > chi1 zp, zp > f- f+)' ,
    4:  'generate g g > h > chi2 chi2, (chi2 > chi1 hs, (hs > zp zp, zp > f- f+)), (chi2 > chi1 hs, (hs > zp zp, zp > f- f+))'
    }
modelcode = 'iDM'
rcard='MadGraph_run_card_FRVZheavy_14TeV.dat'
pcard='MadGraph_param_card_FRVZheavy_mH%s.dat' % (mH)
rname = 'run_lj'+str(runArgs.runNumber)

# initialise random number generator/sequence
import random
random.seed(runArgs.randomSeed)
# lifetime function
def lifetime(avgtau = 21):
    import math
    t = random.random()
    return -1.0 * avgtau * math.log(t)

# do not run MadGraph if config only is requested
if not opts.config_only:
    # writing proc card for MG
    fcard = open('proc_card_mg5.dat','w')
    fcard.write("""
import model --modelname %s
define f- = e- m- d s u~ c~
define f+ = e+ m+ d~ s~ u c
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

    # hacking LHE file
    unzip1 = subprocess.Popen(['gunzip',process_dir+'/Events/'+rname+'/unweighted_events.lhe.gz'])
    unzip1.wait()
    oldlhe = open(process_dir+'/Events/'+rname+'/unweighted_events.lhe','r')
    newlhe = open(process_dir+'/Events/'+rname+'/unweighted_events2.lhe','w')
    init = True
    for line in oldlhe:
        if init==True:
            if '30000016' in line:
                line = line.replace('30000016','3000016')
            elif '30000015' in line:
                line = line.replace('30000015','3000015')
            newlhe.write(line)
            if '</init>' in line:
                init = False
        else:
            newline=line.rstrip('\n')
            columns=newline.split()
            pdgid=columns[0]
            if pdgid == '3000001' and avgtau>0:
                part1 = line[:-7]
                part2 = "%.11E" % (lifetime(avgtau))
                part3 = line[-5:]
                newlhe.write(part1+part2+part3)
            elif (pdgid == '-30000016') :
                part1 = ' -3000016'
                part2 = line[10:]
                newlhe.write(part1+part2)
            elif (pdgid == '30000016') :
                part1 = '  3000016'
                part2 = line[10:]
                newlhe.write(part1+part2)
            elif (pdgid == '-30000015') :
                part1 = ' -3000015'
                part2 = line[10:]
                newlhe.write(part1+part2)
            elif (pdgid == '30000015') :
                part1 = '  3000015'
                part2 = line[10:]
                newlhe.write(part1+part2)
            else:
                newlhe.write(line)
    oldlhe.close()
    newlhe.close()

    # re-zipping hacked LHE
    zip1 = subprocess.Popen(['gzip',process_dir+'/Events/'+rname+'/unweighted_events2.lhe'])
    zip1.wait()
    shutil.move(process_dir+'/Events/'+rname+'/unweighted_events2.lhe.gz',process_dir+'/Events/'+rname+'/unweighted_events.lhe.gz')
    os.remove(process_dir+'/Events/'+rname+'/unweighted_events.lhe')

    arrange_output(run_name=rname,proc_dir=process_dir,outputDS=rname+'._00001.events.tar.gz')

import os
if 'ATHENA_PROC_NUMBER' in os.environ:
    opts.nprocs = 0
runArgs.inputGeneratorFile=rname+'._00001.events.tar.gz'

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
evgenConfig.description = 'FRVZ H -> heavy %sgamma_d + X, mH=%sGeV %s' % (nGamma,mH, category)
evgenConfig.keywords = ["exotic", "BSMHiggs", "BSM", "darkPhoton"]
evgenConfig.contact  = ['antonio.policicchio@cern.ch']
evgenConfig.process="LJ_FRVZheavy"
evgenConfig.inputfilecheck = rname
runArgs.inputGeneratorFile=rname+'._00001.events.tar.gz'
