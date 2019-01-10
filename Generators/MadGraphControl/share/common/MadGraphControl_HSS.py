from MadGraphControl.MadGraphUtils import *

# 305588 run number for mH125 mS5   (lab-frame lifetime = 5 m)
# 304794 run number for mH125 mS8   (lab-frame lifetime = 5 m)
# 303680 run number for mH125 mS15  (lab-frame lifetime = 5 m)
# 304795 run number for mH125 mS25  (lab-frame lifetime = 5 m)
# 303681 run number for mH125 mS40  (lab-frame lifetime = 5 m)
# 304796 run number for mH125 mS55  (lab-frame lifetime = 5 m)
# 305589 run number for mH125 mS5   (lab-frame lifetime = 9 m)
# 304797 run number for mH125 mS8   (lab-frame lifetime = 9 m)
# 304798 run number for mH125 mS15  (lab-frame lifetime = 9 m)
# 304799 run number for mH125 mS25  (lab-frame lifetime = 9 m)
# 304800 run number for mH125 mS40  (lab-frame lifetime = 9 m)
# 304801 run number for mH125 mS55  (lab-frame lifetime = 9 m)
#
# 304802 run number for mH100 mS8   (lab-frame lifetime = 5 m)
# 304803 run number for mH100 mS25  (lab-frame lifetime = 5 m)
#
# 304804 run number for mH200 mS8   (lab-frame lifetime = 5 m)
# 304805 run number for mH200 mS25  (lab-frame lifetime = 5 m)
# 304806 run number for mH200 mS50  (lab-frame lifetime = 5 m)
# 304807 run number for mH200 mS8   (lab-frame lifetime = 9 m)
# 304808 run number for mH200 mS25  (lab-frame lifetime = 9 m)
# 304809 run number for mH200 mS50  (lab-frame lifetime = 9 m)
#
# 304810 run number for mH400 mS50  (lab-frame lifetime = 5 m)
# 304811 run number for mH400 mS100 (lab-frame lifetime = 5 m)
# 304812 run number for mH400 mS50  (lab-frame lifetime = 9 m)
# 304813 run number for mH400 mS100 (lab-frame lifetime = 9 m)
#
# 304814 run number for mH600 mS50  (lab-frame lifetime = 5 m)
# 304815 run number for mH600 mS150 (lab-frame lifetime = 5 m)
# 304816 run number for mH600 mS50  (lab-frame lifetime = 9 m)
# 304817 run number for mH600 mS150 (lab-frame lifetime = 9 m)
#
# 304818 run number for mH1000 mS50  (lab-frame lifetime = 5 m)
# 304819 run number for mH1000 mS150 (lab-frame lifetime = 5 m)
# 304820 run number for mH1000 mS400 (lab-frame lifetime = 5 m)
# 304821 run number for mH1000 mS50  (lab-frame lifetime = 9 m)
# 304822 run number for mH1000 mS150 (lab-frame lifetime = 9 m)
# 304823 run number for mH1000 mS400 (lab-frame lifetime = 9 m)
#
# 305812 run number for mH125 mS40  (lab-frame lifetime = 5 m)
# 305813 run number for mH600 mS50  (lab-frame lifetime = 5 m)
#
# 308368 run number for mH125 mS40  (lab-frame lifetime = 1 m)
# 308369 run number for mH600 mS50  (lab-frame lifetime = 0.5 m)

# Variables that depend on run number: which process and which Higgs mass
process = {
    1:  'generate g g > h HIG=1 HIW=0 QED=0 QCD=0, (h > h2 h2, h2 > f f)',
    2:  'generate g g > h HIG=1 HIW=0 QED=0 QCD=0, (h > h2 h2, h2 > b b~)'
}
modelcode='HAHM_variableMW_v3_UFO'


if runArgs.runNumber==305588:  ## mH=125 (lab-frame lifetime = 5 m)
    mH=125
    mhS=5
    nProcess=1
    avgtau = 127
elif runArgs.runNumber==304794:
    mH=125
    mhS=8
    nProcess=1
    avgtau = 200
elif runArgs.runNumber==303680:
    mH=125
    mhS=15
    nProcess=1
    avgtau = 580
elif runArgs.runNumber==304795:
    mH=125
    mhS=25
    nProcess=1
    avgtau = 760
elif runArgs.runNumber==303681:
    mH=125
    mhS=40
    nProcess=1
    avgtau = 1180
elif runArgs.runNumber==304796:
    mH=125
    mhS=55
    nProcess=1
    avgtau = 1540
elif runArgs.runNumber==305589:  ## mH=125 (lab-frame lifetime = 9 m)
    mH=125
    mhS=5
    nProcess=1
    avgtau = 228
elif runArgs.runNumber==304797:
    mH=125
    mhS=8
    nProcess=1
    avgtau = 375
elif runArgs.runNumber==304798:
    mH=125
    mhS=15
    nProcess=1
    avgtau = 710
elif runArgs.runNumber==304799:
    mH=125
    mhS=25
    nProcess=1
    avgtau = 1210
elif runArgs.runNumber==304800:
    mH=125
    mhS=40
    nProcess=1
    avgtau = 1900
elif runArgs.runNumber==304801:
    mH=125
    mhS=55
    nProcess=1
    avgtau = 2730
elif runArgs.runNumber==304802: ## mH=100 (lab-frame lifetime = 5 m)
    mH=100
    mhS=8
    nProcess=1
    avgtau = 240
elif runArgs.runNumber==304803:
    mH=100
    mhS=25
    nProcess=1
    avgtau = 740
elif runArgs.runNumber==304804: ## mH=200 (lab-frame lifetime = 5 m)
    mH=200
    mhS=8
    nProcess=1
    avgtau = 170
elif runArgs.runNumber==304805:
    mH=200
    mhS=25
    nProcess=1
    avgtau = 540
elif runArgs.runNumber==304806:
    mH=200
    mhS=50
    nProcess=1
    avgtau = 1070
elif runArgs.runNumber==304807: ## mH=200 (lab-frame lifetime = 9 m)
    mH=200
    mhS=8
    nProcess=1
    avgtau = 290
elif runArgs.runNumber==304808:
    mH=200
    mhS=25
    nProcess=1
    avgtau = 950
elif runArgs.runNumber==304809:
    mH=200
    mhS=50
    nProcess=1
    avgtau = 1900
elif runArgs.runNumber==304810: ## mH=400 (lab-frame lifetime = 5 m)
    mH=400
    mhS=50
    nProcess=1
    avgtau = 700
elif runArgs.runNumber==304811:
    mH=400
    mhS=100
    nProcess=1
    avgtau = 1460
elif runArgs.runNumber==304812: ## mH=400 (lab-frame lifetime = 9 m)
    mH=400
    mhS=50
    nProcess=1
    avgtau = 1260
elif runArgs.runNumber==304813:
    mH=400
    mhS=100
    nProcess=1
    avgtau = 2640
elif runArgs.runNumber==304814: ## mH=600 (lab-frame lifetime = 5 m)
    mH=600
    mhS=50
    nProcess=1
    avgtau = 520
elif runArgs.runNumber==304815:
    mH=600
    mhS=150
    nProcess=1
    avgtau = 1720
elif runArgs.runNumber==304816: ## mH=600 (lab-frame lifetime = 9 m)
    mH=600
    mhS=50
    nProcess=1
    avgtau = 960
elif runArgs.runNumber==304817:
    mH=600
    mhS=150
    nProcess=1
    avgtau = 3140
elif runArgs.runNumber==304818: ## mH=1000 (lab-frame lifetime = 5 m)
    mH=1000
    mhS=50
    nProcess=1
    avgtau = 380
elif runArgs.runNumber==304819:
    mH=1000
    mhS=150
    nProcess=1
    avgtau = 1170
elif runArgs.runNumber==304820:
    mH=1000
    mhS=400
    nProcess=1
    avgtau = 3960
elif runArgs.runNumber==304821: ## mH=1000 (lab-frame lifetime = 9 m)
    mH=1000
    mhS=50
    nProcess=1
    avgtau = 670
elif runArgs.runNumber==304822:
    mH=1000
    mhS=150
    nProcess=1
    avgtau = 2110
elif runArgs.runNumber==304823:
    mH=1000
    mhS=400
    nProcess=1
    avgtau = 7200
elif runArgs.runNumber==305812: ## Sample for upgrade studies (lab-frame lifetime = 5 m)
    mH=125
    mhS=40
    nProcess=1
    avgtau = 1180
elif runArgs.runNumber==305813: ## Sample for upgrade studies (lab-frame lifetime = 5 m)
    mH=600
    mhS=50
    nProcess=1
    avgtau = 520
elif runArgs.runNumber==308368: ## Sample for upgrade studies (lab-frame lifetime = 1 m) 
    mH=125
    mhS=40
    nProcess=1
    avgtau = 215
elif runArgs.runNumber==308369: ## Sample for upgrade studies (lab-frame lifetime = 0.5 m)
    mH=600
    mhS=50
    nProcess=1
    avgtau = 55
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
        import model HAHM_variableMW_v3_UFO
        define p = g u c d s u~ c~ d~ s~
        define f = u c d s u~ c~ d~ s~ b b~ e+ e- mu+ mu- ta+ ta-
        %s
        output -f
        """ % (process[1]))
    fcard.close()

    if mH <= 125: 
      param_card_extras = { "HIDDEN": { 'epsilon': '1e-10', #kinetic mixing parameter
      'kap': '1e-4',            #higgs mixing parameter
      'mhsinput':mhS },         #dark higgs mass
      "HIGGS": { 'mhinput':mH}, #higgs mass
      "DECAY": { 'wzp':'Auto', 'wh':'Auto', 'wt':'Auto', 'whs':'Auto'} #auto-calculate decay widths and BR of Zp, H, t, hs
      }
    elif mH > 125:
      param_card_extras = { "HIDDEN": { 'epsilon': '1e-10', #kinetic mixing parameter
      'kap': '1e-4',            #higgs mixing parameter
      'mhsinput':mhS },         #dark higgs mass
      "HIGGS": { 'mhinput':mH}, #higgs mass
      "DECAY": { 'wzp':'Auto', 'wh':'5', 'wt':'Auto', 'whs':'Auto'} #auto-calculate decay widths and BR of Zp, H, t, hs
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

   #create the param_card 
    if os.access('param_card.dat',os.R_OK):
      print("Deleting old param_card.dat")
      os.remove('param_card.dat')
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
    if run_card == "": run_card = get_default_runcard(proc_dir = process_dir)
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
          columns = (' '.join(newline.split())).split()
          pdgid = int(columns[0])
          if pdgid == 35:
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
evgenConfig.description = 'Displaced hadronic jets process Higgs -> hs hs with mH=%sGeV, mhs=%sGeV' % (mH, mhS)
evgenConfig.keywords = ["exotic", "BSM","BSMHiggs","longLived"]
evgenConfig.contact  = ['hrussell@cern.ch','Cristiano.Alpigiani@cern.ch']
evgenConfig.process="Higgs --> LLPs"
evgenConfig.inputfilecheck = rname
runArgs.inputGeneratorFile=rname+'._00001.events.tar.gz'
