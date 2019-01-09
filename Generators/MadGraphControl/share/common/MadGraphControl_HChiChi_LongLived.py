from MadGraphControl.MadGraphUtils import *

# 304600 run number for xchi -> tau tau nu (mChi10)
# 304601 run number for xchi -> tau tau nu (mChi30)
# 304602 run number for xchi -> tau tau nu (mChi50)
# 304603 run number for xchi -> tau tau nu (mChi100)

# 304604 run number for xchi -> c b s (mChi10)
# 304605 run number for xchi -> c b s (mChi30)
# 304606 run number for xchi -> c b s (mChi50)
# 304607 run number for xchi -> c b s (mChi100)

# 304608 run number for xchi -> l c b (mChi10)
# 304609 run number for xchi -> l c b (mChi30)
# 304610 run number for xchi -> l c b (mChi50)
# 304611 run number for xchi -> l c b (mChi100)

# 304612 run number for xchi -> nu b b (mChi10)
# 304613 run number for xchi -> nu b b (mChi30)
# 304614 run number for xchi -> nu b b (mChi50)
# 304615 run number for xchi -> nu b b (mChi100)

process = {
    1:  'generate p p > xchi xchi , xchi > ta- ta+ vl , xchi > ta+ ta- vl~',
    2:  'generate p p > xchi xchi , xchi > c b s      , xchi > c~ b~ s~',
    3:  'generate p p > xchi xchi , xchi > l+ c~ b    , xchi > l- c b~',
    4:  'generate p p > xchi xchi , xchi > vl b b~    , xchi > vl~ b~ b'
}

if runArgs.runNumber==304600:
    mChi=10
    avgtau = 920
    nProcess = 1
elif runArgs.runNumber==304601:
    mChi=30
    avgtau = 2750
    nProcess = 1
elif runArgs.runNumber==304602:
    mChi=50
    avgtau = 5550
    nProcess = 1
elif runArgs.runNumber==304603:
    mChi=100
    avgtau = 3500
    nProcess = 1
elif runArgs.runNumber==304604:
    mChi=10
    avgtau = 920
    nProcess = 2
elif runArgs.runNumber==304605:
    mChi=30
    avgtau = 2750
    nProcess = 2
elif runArgs.runNumber==304606:
    mChi=50
    avgtau = 5550
    nProcess = 2
elif runArgs.runNumber==304607:
     mChi=100
     avgtau = 3500
     nProcess = 2
elif runArgs.runNumber==304608:
     mChi=10
     avgtau = 920
     nProcess = 3
elif runArgs.runNumber==304609:
    mChi=30
    avgtau = 2750
    nProcess = 3
elif runArgs.runNumber==304610:
    mChi=50
    avgtau = 5550
    nProcess = 3
elif runArgs.runNumber==304611:
    mChi=100
    avgtau = 3500
    nProcess = 3
elif runArgs.runNumber==304612:
    mChi=10
    avgtau = 920
    nProcess = 4
elif runArgs.runNumber==304613:
    mChi=30
    avgtau = 2750
    nProcess = 4
elif runArgs.runNumber==304614:
    mChi=50
    avgtau = 5550
    nProcess = 4
elif runArgs.runNumber==304615:
    mChi=100
    avgtau = 3500
    nProcess = 4
else:
    raise RuntimeError('Bad runNumber')
beamEnergy = -999.
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
   raise RuntimeError("No center of mass energy found")

# initialise random number generator/sequence
import random
random.seed(runArgs.randomSeed)
# lifetime function
def lifetime(avgtau = 21):
    import math
    t = random.random()
    return -1.0 * avgtau * math.log(t)

# basename for madgraph LHEF file
rname = 'run_'+str(runArgs.runNumber)

# do not run MadGraph if config only is requested
if not opts.config_only:
    
# writing proc card for MG
    fcard = open('proc_card_mg5.dat','w')
    fcard.write("""
 import model WIMP_BG_higgsportal_full_loop
 define p = g u c d s u~ c~ d~ s~
 define l- = e- mu- ta-
 define l+ = e+ mu+ ta+
 define vl = ve vm vt
 define vl~ = ve~ vm~ vt~
 %s
 output -f
 """ % (process[nProcess]))
    fcard.close()
    
    param_card_extras = { "MASS": { 'Mxchi': mChi}, # chi mass
    }
    run_card_extras = { 'lhe_version':'2.0',
                   'ebeam1':'6500',
                   'ebeam2':'6500',
                   'cut_decays':'F',
                   'ptj':'0',
                   'ptb':'0',
                   'pta':'0',
                   'ptl':'0',
                   'etaj':'-1',
                   'etab':'-1',
                   'etaa':'-1',
                   'etal':'-1',
                   'drjj':'0',
                   'drbb':'0',
                   'drll':'0',
                   'draa':'0',
                   'drbj':'0',
                   'draj':'0',
                   'drjl':'0',
                   'drab':'0',
                   'drbl':'0',
                   'dral':'0' }
    try:
      proc_name
    except NameError:
      proc_name = ""
    try:
      param_card
    except NameError:
      print("No param_card given, will use the one from the process directory..");
      param_card = ""  

    try:
      param_card_extras
    except NameError:
      param_card_extras={}

    try:
      run_card
    except NameError:
      print("No run_card given, will use the default one for this process (LO: $MADPATH/Template/LO/Cards/run_card.dat)");
      run_card = ""

    try:
      run_card_extras
    except NameError:
      run_card_extras = {}  
#modify the run_card_extras
    if 'pdlabel' in run_card_extras:
      raise RuntimeError("Cannot specify a pdlabel in run_card_extras, as this is set for you")

    if 'lhaid' in run_card_extras:
      raise RuntimeError("Cannot specify a lhaid in run_card_extras, as this is set for you")

    run_card_extras['pdlabel'] = 'lhapdf'
    run_card_extras['lhaid'] = '247000'#NNDF23_lo_as_0130_qed pdf set

# generating events in MG
    process_dir = new_process()
    if proc_name != "" and process_dir != proc_name:
      raise RuntimeError("Unexpected process_dir %s when expected %s" % (process_dir,proc_name))

# create the param_card
### test readability of the path
    if os.access('param_card.data',os.R_OK): 
      print("Deleting old param_card.data")
      os.remove('param_card.data')
    if param_card == "": param_card = "%s/Cards/param_card.dat" % process_dir

    if( build_param_card(param_card_old=param_card,param_card_new='param_card.dat',params=param_card_extras) == -1):
      raise RuntimeError("Could not create param_card.dat")

#create the run card: FIXME: Should check for success
    safefactor=1.1 #generate extra 10% events in case any fail showering
    nevents = 5000*safefactor
    if runArgs.maxEvents > 0: nevents = runArgs.maxEvents*safefactor
  
    if os.access('run_card.dat',os.R_OK):
      print("Deleting old run_card.dat")
      os.remove('run_card.dat')
    if run_card == "": run_card = get_default_runcard(proc_dir=process_dir)
    if build_run_card(run_card_old=run_card,run_card_new='run_card.dat',nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=run_card_extras)==-1:
      raise RuntimeError("Could not create run_card.dat")


#ready to generate
    generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name=rname,proc_dir=process_dir)
 
# replacing lifetime of scalar, manually
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
            if 'vent' in line:
                newlhe.write(line)
                continue
            newline = line.rstrip('\n')
            columns = newline.split()
            pdgid = columns[0]
            if pdgid == '3000001':
                part1 = line[:-7]
                part2 = "%.11E" % (lifetime(avgtau))
                part3 = line[-5:]
                newlhe.write(part1+part2+part3)
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
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")

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
evgenConfig.description = 'Displaced hadronic jets process WIMP (Higgs portal) with mChi=%sGeV' % (mChi)
evgenConfig.keywords = ["exotic", "WIMP", "BSM", "longLived"]
evgenConfig.contact  = ['Cristiano.Alpigiani@cern.ch']
evgenConfig.process=" --> LLPs"
evgenConfig.inputfilecheck = rname
runArgs.inputGeneratorFile=rname+'._00001.events.tar.gz'
