from MadGraphControl.MadGraphUtils import *

# 304710 run number for xchi -> tau tau nu (mChi500)
# 304711 run number for xchi -> tau tau nu (mChi1000)
# 304712 run number for xchi -> tau tau nu (mChi1500)

# 304713 run number for xchi -> t b s (mChi500)
# 304714 run number for xchi -> t b s (mChi1000)
# 304715 run number for xchi -> t b s (mChi1500)

# 304716 run number for xchi -> l t b (mChi500)
# 304717 run number for xchi -> l t b (mChi1000)
# 304718 run number for xchi -> l t b (mChi1500)

# 304719 run number for xchi -> nu b b (mChi500)
# 304720 run number for xchi -> nu b b (mChi1000)
# 304721 run number for xchi -> nu b b (mChi1500)


# MadGraph process
MGprocess = {
    1:  'generate p p > x1+ n2, x1+ > n2 u d~ \n add process p p > x1- n2, x1- > n2 u~ d'
}
    
# MadSpin process
MSprocess = {
    1:  'decay n2 > ta- ta+ vl \n  decay n2 > ta+ ta- vl~',
    2:  'decay n2 > t b s      \n  decay n2 > t~ b~ s~',
    3:  'decay n2 > l+ t~ b    \n  decay n2 > l- t b~',
    4:  'decay n2 > vl b b~    \n  decay n2 > vl~ b~ b'
    }


if runArgs.runNumber==304710:
    mX1=500.5
    mNeu2=500
    MGproc = 1
    MSproc = 1
    avgtauNeu2 = 6100
elif runArgs.runNumber==304711:
    mX1=1000.5
    mNeu2=1000
    MGproc = 1
    MSproc = 1
    avgtauNeu2 = 7800
elif runArgs.runNumber==304712:
    mX1=1500.5
    mNeu2=1500
    MGproc = 1
    MSproc = 1
    avgtauNeu2 = 9000
elif runArgs.runNumber==304713:
    mX1=500.5
    mNeu2=500
    MGproc = 1
    MSproc = 2
    avgtauNeu2 = 6100
elif runArgs.runNumber==304714:
    mX1=1000.5
    mNeu2=1000
    MGproc = 1
    MSproc = 2
    avgtauNeu2 = 7800
elif runArgs.runNumber==304715:
    mX1=1500.5
    mNeu2=1500
    MGproc = 1
    MSproc = 2
    avgtauNeu2 = 9000
elif runArgs.runNumber==304716:
    mX1=500.5
    mNeu2=500
    MGproc = 1
    MSproc = 3
    avgtauNeu2 = 6100
elif runArgs.runNumber==304717:
    mX1=1000.5
    mNeu2=1000
    MGproc = 1
    MSproc = 3
    avgtauNeu2 = 7800
elif runArgs.runNumber==304718:
    mX1=1500.5
    mNeu2=1500
    MGproc = 1
    MSproc = 3
    avgtauNeu2 = 9000
elif runArgs.runNumber==304719:
    mX1=500.5
    mNeu2=500
    MGproc = 1
    MSproc = 4
    avgtauNeu2 = 6100
elif runArgs.runNumber==304720:
    mX1=1000.5
    mNeu2=1000
    MGproc = 1
    MSproc = 4
    avgtauNeu2 = 7800
elif runArgs.runNumber==304721:
    mX1=1500.5
    mNeu2=1500
    MGproc = 1
    MSproc = 4
    avgtauNeu2 = 9000
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
 import model RPVMSSM_UFO
 define p = g u c d s u~ c~ d~ s~
 define l- = e- mu- ta-
 define l+ = e+ mu+ ta+
 define vl = ve vm vt
 define vl~ = ve~ vm~ vt~
 %s
 output -f
 """ % (MGprocess[MGproc]) )
    fcard.close()

# writing MadSpin card
    mcard = open('madspin_card_loc.dat','w')
    mcard.write("""
 set max_weight_ps_point 400  # number of PS to estimate the maximum for each event
 set spinmode none
 #
 # specify the decay for the final state particles
 %s
 # running the actual code
 launch
 """ % (MSprocess[MSproc]) )
    mcard.close()
    
    param_card = "MadGraph_param_card_RPVMSSM_Baryogenesis.dat"
    
    param_card_extras = { "MASS": {'Mch1': mX1 , 'Mneu2': mNeu2 } # chargino and neutralino mass
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
    generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name=rname,proc_dir=process_dir,grid_pack=False,gridpack_compile=False,cluster_type=None,cluster_queue=None,extlhapath=None,madspin_card_loc="madspin_card_loc.dat")
    
#replacing lifetime of scalar, manually
    os.remove(process_dir+'/Events/'+rname+'/unweighted_events.lhe')
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
            if pdgid == '1000023':
                part1 = line[:-22]
                part2 = "%.11E" % (lifetime(avgtauNeu2))
                part3 = line[-12:]
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
evgenConfig.description = 'Displaced hadronic jets process WIMP (wino model) with mChi=%sGeV' % (mNeu2)
evgenConfig.keywords = ["exotic", "WIMP", "BSM","longLived"]
evgenConfig.contact  = ['Cristiano.Alpigiani@cern.ch']
evgenConfig.process=" --> LLPs"
evgenConfig.inputfilecheck = rname
runArgs.inputGeneratorFile=rname+'._00001.events.tar.gz'
