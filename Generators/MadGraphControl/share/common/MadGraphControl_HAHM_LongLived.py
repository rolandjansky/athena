from MadGraphControl.MadGraphUtils import *

# 305018 run number for zp -> mu mu, zp -> f f  (mZd20 ctau 500)
# 305019 run number for zp -> mu mu, zp -> mu mu  (mZd20 ctau 500)

# 305020 run number for zp -> mu mu, zp -> f f  (mZd40 ctau 500)
# 305021 run number for zp -> mu mu, zp -> mu mu  (mZd40 ctau 500)

# 305022 run number for zp -> mu mu, zp -> f f  (mZd40 ctau 5000)
# 305023 run number for zp -> mu mu, zp -> mu mu  (mZd40 ctau 5000)

# 305024 run number for zp -> mu mu, zp -> f f  (mZd60 ctau 500)
# 305025 run number for zp -> mu mu, zp -> mu mu  (mZd60 ctau 500)

# 305026 run number for zp -> mu mu, zp -> f f  (mZd60 ctau 5000)
# 305027 run number for zp -> mu mu, zp -> mu mu  (mZd60 ctau 5000)

# MadGraph process
MGprocess = {
    1:  'generate g g > h HIG=1 HIW=0 QED=0 QCD=0, (h > zp zp, (zp > mu+ mu-),(zp > f f))',
    2:  'generate g g > h HIG=1 HIW=0 QED=0 QCD=0, (h > zp zp, (zp > mu+ mu-))'
}

if runArgs.runNumber==305018:
    mZd=20
    MGproc = 1
    avgtauZd = 500
elif runArgs.runNumber==305019:
    mZd=20
    MGproc = 2
    avgtauZd = 500
elif runArgs.runNumber==305020:
    mZd=40
    MGproc = 1
    avgtauZd = 500
elif runArgs.runNumber==305021:
    mZd=40
    MGproc = 2
    avgtauZd = 500
elif runArgs.runNumber==305022:
    mZd=40
    MGproc = 1
    avgtauZd = 5000
elif runArgs.runNumber==305023:
    mZd=40
    MGproc = 2
    avgtauZd = 5000
elif runArgs.runNumber==305024:
    mZd=60
    MGproc = 1
    avgtauZd = 500
elif runArgs.runNumber==305025:
    mZd=60
    MGproc = 2
    avgtauZd = 500
elif runArgs.runNumber==305026:
    mZd=60
    MGproc = 1
    avgtauZd = 5000
elif runArgs.runNumber==305027:
    mZd=60
    MGproc = 2
    avgtauZd = 5000
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
 define f = u d s u~ d~ s~ c c~ b b~ ve vm vt ve~ vm~ vt~ e- mu- e+ mu+ ta- ta+
 %s
 output -f
 """ % (MGprocess[MGproc]) )
    fcard.close()

    param_card_extras = { "HIDDEN" : { 'epsilon': '1e-4', #kinetic mixing parameter
                                      'kap' : '1e-4', #higgs mixing parameter
                                      'mzdinput' : mZd, #Zd mass
                                      'mhsinput' : '200.0' }, #dark higgs mass
                          "HIGGS": { 'mhinput' : '125.0'}, #higgs mass
                          "DECAY": { 'wzp' : 'Auto', 'wh':'Auto' } #auto-calculate decay widths
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
    generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name=rname,proc_dir=process_dir,grid_pack=False,gridpack_compile=False,cluster_type=None,cluster_queue=None,extlhapath=None)

    #madspin_card_loc='madspin_card_loc.dat"
    
#replacing lifetime of scalar, manually
    #os.remove(process_dir+'/Events/'+rname+'/unweighted_events.lhe')
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
            if pdgid == '32':
                part1 = line[:-7]
                part2 = "%.11E" % (lifetime(avgtauZd))
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
evgenConfig.description = 'Displaced Dark Photon with Zd=%sGeV' % (mZd)
evgenConfig.keywords = ["exotic", "BSM", "darkPhoton","longLived"]
evgenConfig.contact  = ['Nathan.Rogers.Bernard@cern.ch']
evgenConfig.process=" --> LLPs"
evgenConfig.inputfilecheck = rname
runArgs.inputGeneratorFile=rname+'._00001.events.tar.gz'
