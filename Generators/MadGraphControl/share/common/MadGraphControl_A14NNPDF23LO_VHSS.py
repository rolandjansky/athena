from MadGraphControl.MadGraphUtils import *

# Variables that depend on run number: which process and which Higgs mass
process = {
    #WH
    0:  'generate p p > w+ h , ( w+ > l+ vl ) , ( h > h2 h2, h2 > f f )    \n add process p p >  w- h , ( w- > l- vl~ ) , ( h > h2 h2, h2 > f f )',
    1:  'generate p p > w+ h , ( w+ > l+ vl ) , ( h > h2 h2, h2 > b b~)    \n add process p p >  w- h , ( w- > l- vl~ ) , ( h > h2 h2, h2 > b b~ )',
    2:  'generate p p > w+ h , ( w+ > l+ vl ) , ( h > h2 h2, h2 > ta+ ta-) \n add process p p >  w- h , ( w- > l- vl~ ) , ( h > h2 h2, h2 > ta+ ta-)',
    3:  'generate p p > w+ h , ( w+ > l+ vl ) , ( h > h2 h2, h2 > j j)     \n add process p p >  w- h , ( w- > l- vl~ ) , ( h > h2 h2, h2 > j j )', # here j is defined as a light jet (u,d,s)
    #ZH
    4:  'generate p p > z h , ( z > l+ l- ) , ( h > h2 h2, h2 > f f )',
    5:  'generate p p > z h , ( z > l+ l- ) , ( h > h2 h2, h2 > b b~)',
    6:  'generate p p > z h , ( z > l+ l- ) , ( h > h2 h2, h2 > ta+ ta-)',
    7:  'generate p p > z h , ( z > l+ l- ) , ( h > h2 h2, h2 > j j)', # here j is defined as a light jet (u,d,s)
}
modelcode='HAHM_variableMW_v3_UFO'

##########
#WH->bb
##########
if runArgs.runNumber==345728:
    mH = 125
    mhS = 20
    nProcess = 1
    avgtau = 0.1
elif runArgs.runNumber==345729:
    mH = 125
    mhS = 20
    nProcess = 1
    avgtau = 1
elif runArgs.runNumber==345730:
    mH = 125
    mhS = 20
    nProcess = 1
    avgtau = 10
elif runArgs.runNumber==345731:
    mH = 125
    mhS = 30
    nProcess = 1
    avgtau = 0.1
elif runArgs.runNumber==345732:
    mH = 125
    mhS = 30
    nProcess = 1
    avgtau = 1
elif runArgs.runNumber==345733:
    mH = 125
    mhS = 30
    nProcess = 1
    avgtau = 10
elif runArgs.runNumber==345734:
    mH = 125
    mhS = 60
    nProcess = 1
    avgtau = 0.1
elif runArgs.runNumber==345735:
    mH = 125
    mhS = 60
    nProcess = 1
    avgtau = 1
elif runArgs.runNumber==345736:
    mH = 125
    mhS = 60
    nProcess = 1
    avgtau = 10
##########
#ZH->bb
##########
elif runArgs.runNumber==345737:
    mH = 125
    mhS = 20
    nProcess = 5
    avgtau = 0.1
elif runArgs.runNumber==345738:
    mH = 125
    mhS = 20
    nProcess = 5
    avgtau = 1
elif runArgs.runNumber==345739:
    mH = 125
    mhS = 20
    nProcess = 5
    avgtau = 10
elif runArgs.runNumber==345740:
    mH = 125
    mhS = 30
    nProcess = 5
    avgtau = 0.1
elif runArgs.runNumber==345741:
    mH = 125
    mhS = 30
    nProcess = 5
    avgtau = 1
elif runArgs.runNumber==345742:
    mH = 125
    mhS = 30
    nProcess = 5
    avgtau = 10
elif runArgs.runNumber==345743:
    mH = 125
    mhS = 60
    nProcess = 5
    avgtau = 0.1
elif runArgs.runNumber==345744:
    mH = 125
    mhS = 60
    nProcess = 5
    avgtau = 1
elif runArgs.runNumber==345745:
    mH = 125
    mhS = 60
    nProcess = 5
    avgtau = 10
else:
    raise RuntimeError('Bad runNumber: ',runArgs.runNumber)

# initialise random number generator/sequence
import random
random.seed(runArgs.randomSeed)
# lifetime function
def lifetime(avgtau = 21):
    import math
    t = random.random()
    return -1.0 * avgtau * math.log(t)

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'): beamEnergy=runArgs.ecmEnergy/2.
else: raise RuntimeError('No center of mass energy found.')

# basename for madgraph LHEF file
rname = 'run_'+str(runArgs.runNumber)

# do not run MadGraph if config only is requested
if not opts.config_only:
    
    # writing proc card for MG
    fcard = open('proc_card_mg5.dat','w')
    fcard.write("""
        import model HAHM_variableMW_v3_UFO
        define p = g u c d s u~ c~ d~ s~
        define j = d u s d~ u~ s~ 
        define f = u c d s u~ c~ d~ s~ b b~ e+ e- mu+ mu- ta+ ta-
        define l+ = e+ mu+ ta+
        define l- = e- mu- ta-
        define vl = ve vm vt
        define vl~ = ve~ vm~ vt~
        %s
        output -f
        """ % (process[nProcess]))
    fcard.close()

    if mH <= 125: 
      param_card_extras = { "HIDDEN": { 'epsilon': '1e-10', #kinetic mixing parameter
      'kap': '1e-1',            #higgs mixing parameter
      'mhsinput':mhS},
      "HIGGS": { 'mhinput':mH}, #higgs mass
      "DECAY": { 'wzp':'Auto', 'wh':'Auto', 'wt':'Auto', 'whs':'Auto'} #auto-calculate decay widths and BR of Zp, H, t, hs
      }
    elif mH > 125:
      param_card_extras = { "HIDDEN": { 'epsilon': '1e-10', #kinetic mixing parameter
      'kap': '1e-1',            #higgs mixing parameter
      'mhsinput':mhS},
      "HIGGS": { 'mhinput':mH}, #higgs mass
      "DECAY": { 'wzp':'Auto', 'wh':'5', 'wt':'Auto', 'whs':'Auto'} #auto-calculate decay widths and BR of Zp, H, t, hs
      }
    run_card_extras = { 'lhe_version':'2.0',
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
          if 'vent' in line or line[0]=='<':
            newlhe.write(line)
            continue
          newline = line.rstrip('\n')
          columns = (' '.join(newline.split())).split()
          pdgid = int(columns[0])
          if pdgid == 35:
                part1 = ' '.join(columns[:-2])
                part2 = " %.11e " % (lifetime(avgtau))
                part3 = columns[-1]+'\n'
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
#genSeq.Pythia8.Commands += ["Main:timesAllowErrors = 60000"]

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
