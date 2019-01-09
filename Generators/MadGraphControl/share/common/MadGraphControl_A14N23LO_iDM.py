from MadGraphControl.MadGraphUtils import *

# initialise random number generator/sequence
import random
random.seed(runArgs.randomSeed)
# lifetime function
def lifetime(avgtau = 1000):
    import math
    if (avgtau == 0) :
        return 0
    else:
        t = random.random()
        return -1.0 * avgtau * math.log(t)

# run parameters
avgtau_chi = 0.0
avgtau_zd = 0.0
mzd = 0
mchi = 0
dmchi = 0

if runArgs.runNumber == 305100:
    mchi=60
    mzd = 10
    dmchi = 3
    avgtau_zd = 480.0
    avgtau_chi = 11.5
elif runArgs.runNumber == 305101:
    mchi=60
    mzd = 3
    dmchi = 3
    avgtau_zd = 800.0
    avgtau_chi = 16.8
elif runArgs.runNumber == 305102:
    mchi=60
    mzd = 10
    dmchi = 1
    avgtau_zd = 370.0
    avgtau_chi = 9.5
elif runArgs.runNumber == 305103:
    mchi=100
    mzd=3
    dmchi=1
    avgtau_zd = 850.0
    avgtau_chi = 21.0

if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    beamEnergy = 6500.

# specify run card, param card, run base name
rcard='MadGraph_run_card_iDM.dat'
pcard='MadGraph_param_card_iDM_mzd%s_mchi%s_dmchi0%s.dat' % (mzd,mchi,dmchi)
rname = 'run_lj'+str(runArgs.runNumber)

# do not run MadGraph if config only is requested
if not opts.config_only:

	# writing proc card for MG
	fcard = open('proc_card_mg5.dat','w')
	fcard.write("""
import model iDM
define p = g u c d s u~ c~ d~ s~
generate p p > j chi1 chi2 /hs, chi2 > chi1 l+ l- /hs
output -f
""" )
	fcard.close()

	# getting run and param cards
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
            		# chi1, chi2: removing one zero in pdgID
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
           		 # chi2: removing one zero in pdgID, and changing lifetime
            		if (pdgid == '-30000016') :
                		part0 = ' -3000016'
                		part1 = line[10:-7]
                		part2 = "%.11E" % (lifetime(avgtau_chi))
                		part3 = line[-5:]
                		newlhe.write(part0+part1+part2+part3)
            		elif (pdgid == '30000016') :
                		part0 = '  3000016'
                		part1 = line[10:-7]
                		part2 = "%.11E" % (lifetime(avgtau_chi))
                		part3 = line[-5:]
                		newlhe.write(part0+part1+part2+part3)
            		# chi1: removing one zero in pdgID
            		elif (pdgid == '-30000015') :
                		part0 = ' -3000015'
                		part1 = line[10:]
                		newlhe.write(part0+part1)
            		elif (pdgid == '30000015') :
                		part0 = '  3000015'
                		part1 = line[10:]
                		newlhe.write(part0+part1)
            		# zd: changing lifetime
            		elif (pdgid == '-3000001') :
                		part1 = line[:-7]
                		part2 = "%.11E" % (lifetime(avgtau_zd))
                		part3 = line[-5:]
                		newlhe.write(part1+part2+part3)
            		elif (pdgid == '3000001') :
                		part1 = line[:-7]
                		part2 = "%.11E" % (lifetime(avgtau_zd))
                		part3 = line[-5:]
                		newlhe.write(part1+part2+part3)
            		else:
                		newlhe.write(line)
	oldlhe.close()
	newlhe.close()

# re-zipping hacked LHE file
zip1 = subprocess.Popen(['gzip',process_dir+'/Events/'+rname+'/unweighted_events2.lhe'])
zip1.wait()
shutil.move(process_dir+'/Events/'+rname+'/unweighted_events2.lhe.gz',process_dir+'/Events/'+rname+'/unweighted_events.lhe.gz')
os.remove(process_dir+'/Events/'+rname+'/unweighted_events.lhe')

arrange_output(run_name=rname,proc_dir=process_dir,outputDS=rname+'._00001.events.tar.gz')

runArgs.inputGeneratorFile=rname+'._00001.events.tar.gz'
import os
if 'ATHENA_PROC_NUMBER' in os.environ:
    opts.nprocs = 0

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
evgenConfig.description = 'iDM LJ+j+MET, m_chi1=%sGeV, m_zd=%s, dm_chi=%s' % (mchi, mzd, dmchi)
evgenConfig.keywords = ["exotic", "BSM", "longLived", "darkPhoton", "WIMP"]
evgenConfig.contact  = ['antonio.policicchio@cern.ch']
evgenConfig.process="iDM LJ+jet+MET"
evgenConfig.inputfilecheck = rname

