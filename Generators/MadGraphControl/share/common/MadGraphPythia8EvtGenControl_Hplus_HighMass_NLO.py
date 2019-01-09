#--------------------------------------------------------------
# on-the-fly generation of H+ MG5 events, montoya@cern.ch
# mc16: pawel.bruckman.de.renstrom@cern.ch, mir@ifae.es
#--------------------------------------------------------------
def build_madfks_inc(madfks_card_old=None,madfks_card_new='madfks_mcatnlo.inc.new'):
    """Build a new madfks_card.dat from an existing one.
    Params should be a dictionary of dictionaries. The first key is the block name, and the second in the param name.
    Eventually madfkss will replace the other arguments, but we need to keep them for backward compatibility for now."""
    # Grab the old madfks card and move it into place
    madfkscard = subprocess.Popen(['get_files','-data',madfks_card_old])
    madfkscard.wait()
    if not os.access(madfks_card_old,os.R_OK):
        mglog.info('Could not get madfks card '+madfks_card_old)
    if os.access(madfks_card_new,os.R_OK):
        mglog.error('Old madfks card at'+str(madfks_card_new)+' in the current directory. Dont want to clobber it. Please move it first.')
        return -1

    oldcard = open(madfks_card_old,'r')
    newcard = open(madfks_card_new,'w')
    lowfrac=0.025
    highfrac=0.25
    for line in oldcard:
        print "line ",line
        print "T/F ",line.find("frac_low=")
        if line.find("frac_low=")!= -1 :
            newcard.write('      parameter (frac_low=%1.3fd0) \n' % (lowfrac))
        elif line.find("frac_upp=")!= -1 :
            newcard.write('      parameter (frac_upp=%1.2fd0) \n' % (highfrac))
        else:
            newcard.write(line)


    oldcard.close()
    newcard.close()
    return newcard		

def fix_dynamic_scale(run_card_old=None,run_card_new='run_card.dat'):
    oldcard = open(run_card_old,'r')
    newcard = open(run_card_new,'w')
    for line in oldcard:
        print "line", line
        if line.find("dynamical_scale_choice") != -1 : 
            newcard.write(' 2 = dynamical_scale_choice ! dynamical: HT\n')
        elif line.find("muR_over_ref") != -1 :
            newcard.write(' 0.333  = muR_over_ref  ! HT/3 when used with scale = 2 (HT)\n')
        elif line.find("muF_over_ref") != -1 :
            newcard.write(' 0.333  = muF_over_ref  ! HT/3 when used with scale = 2 (HT)\n')
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()
    return newcard
	
from MadGraphControl.MadGraphUtils import *

# Define number of generated LHE events
nevents=10*(runArgs.maxEvents)

mode=0


dotaunu=False
dotb=False
dowz=False
dodil=False
dowh=False  
mhc_str = str(runArgs.jobConfig[0])  # get the main JO name string
for s in mhc_str.split("_"):
    ss=s.replace(".py","")
    print ss
    if ss=='taunu':
        dotaunu=True
    if ss=='tb':
        dotb=True
    if ss=='wz':
        dowz=True
    if ss=='dil':
        dodil=True
    if ss=='wh':
        dowh=True



fcard = open('proc_card_mg5.dat','w')
if dotaunu or dotb or dowz or dowh:  
    fcard.write("""
    set group_subprocesses Auto
    set ignore_six_quark_processes False   
    set loop_optimized_output True
    set complex_mass_scheme False
    import model sm
    define p = g u c d s u~ c~ d~ s~
    define j = g u c d s u~ c~ d~ s~
    define l+ = e+ mu+
    define l- = e- mu-
    define vl = ve vm vt
    define vl~ = ve~ vm~ vt~
    import model 2HDMtypeII                     
    generate p p > t~ h+ b [QCD]               
    output -f""")
    fcard.close()
else: 
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")

#--------------------------------------------------------------
# Charged Higgs and all other masses in GeV
# JOB OPTION NAME MUST CONTAIN THE MASS WE WANT TO SIMULATE IN FORMAT LIKE: *_H400_*
#--------------------------------------------------------------
mhc=0 
for s in mhc_str.split("_"):
    ss=s.replace("H","")  
    if ss.isdigit():
        mhc = int(ss)        
if mhc==0:
    raise RuntimeError("Charged Higgs mass not set, mhc=0, check joOption name %s"%mhc_str)


# Define masses for h0, H0 and A0:
import math
mh1=1.250e+02                 
mh2=math.sqrt(math.pow(mhc,2)+math.pow(8.0399e+01,2)) 
mh3=mh2


#--------------------------------------------------------------
# now use Dynamic scale HT/3
#--------------------------------------------------------------

#Fetch default NLO run_card.dat and set parameters. 

extras = { 'lhe_version':'3.0',
           'pdlabel':"'lhapdf'",
           'lhaid':' 260400',
           'parton_shower':'PYTHIA8',
           'fixed_ren_scale':'F',
           'fixed_fac_scale':'F',
           'reweight_scale' :'T',
           'rw_rscale'      :'1.0 0.5 2.0',
           'rw_fscale'      :'1.0 0.5 2.0',
           'reweight_PDF'   :'T',
           'PDF_set_min'    :'260401',
           'PDF_set_max'    :'260500',
           'store_rwgt_info':'T'
           }

process_dir = new_process()


build_run_card(run_card_old=get_default_runcard(process_dir),run_card_new='run_card.tmp2.dat',nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)

fix_dynamic_scale(run_card_old='run_card.tmp2.dat',run_card_new='run_card.dat')

import os

masses = {'25':str(mh1)+'  #  mh1',
          '35':str(mh2)+'  #  mh2',
          '36':str(mh3)+'  #  mh3',
          '37':str(mhc)+'  #  mhc'}

build_param_card(param_card_old=process_dir+'/Cards/param_card.dat',param_card_new='param_card.dat',masses=masses)
print_cards()

runName='run_01'     

    
#copy file to running dir 
if os.access(os.environ['PWD']+'/'+process_dir+'/SubProcesses/madfks_mcatnlo.inc',os.R_OK):
    shutil.copy(os.environ['PWD']+'/'+process_dir+'/SubProcesses/madfks_mcatnlo.inc','madfks_mcatnow.SM.inc')
else:
    raise RuntimeError('Cannot find default NLO madfks_mcatnow.SM.inc!')
#modify file

build_madfks_inc(madfks_card_old='madfks_mcatnow.SM.inc', madfks_card_new='madfks_mcatnlo.inc.new')

#copy it back
if os.access(os.environ['PWD']+'/'+process_dir+'/SubProcesses/madfks_mcatnlo.inc',os.R_OK):
    shutil.copy('madfks_mcatnlo.inc.new',os.environ['PWD']+'/'+process_dir+'/SubProcesses/madfks_mcatnlo.inc')
else:
    raise RuntimeError('Cannot find default NLO madfks_mcatnow.inc.new!')


generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=mode,proc_dir=process_dir,run_name=runName)

outputDS=arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz',lhe_version=3,saveProcDir=True)

#### Shower     
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_aMcAtNlo.py")

evgenConfig.description = 'aMcAtNlo High mass charged Higgs NLO4FS'
evgenConfig.keywords+=['Higgs','MSSM','BSMHiggs','chargedHiggs']

evgenConfig.contact = ['Pawel Bruckman <pawel.bruckman.de.renstrom@cern.ch>','Lluisa-Maria Mir <mir@ifae.es>']

#evgenConfig.inputfilecheck = runName
#runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'
runArgs.inputGeneratorFile=outputDS


# JOB OPTION NAME MUST CONTAIN THE DECAY MODE WE WANT TO SIMULATE IN FORMAT LIKE: *_taunu* or *_tb or *_wh*

if dotaunu==False and dotb==False and dowh==False:  
    raise RuntimeError("No decay mode was identified, check jobOption name %s, and/or runNumber %i."%(runArgs.jobConfig[0],runArgs.runNumber))

if dotaunu:
    genSeq.Pythia8.Commands += ["Higgs:useBSM = on",
                                "37:onMode = off",                   # turn off all mhc decays
                                "37:onIfMatch = 15 16"]              # switch on H+ to taunu
    evgenConfig.keywords+=['tau','neutrino']
    include ('GeneratorFilters/HplusTauNu_Filter_Fragment.py')         # filter for tau, jets, leptons, MET

if dotb:
    genSeq.Pythia8.Commands += ["Higgs:useBSM = on",
                                "37:onMode = off",                   # turn off all mhc decays
                                "37:onIfMatch = 5 6"]                # switch on H+ to tb
    evgenConfig.keywords+=['top','bottom']
    include('GeneratorFilters/TTbarWToLeptonFilter.py')                # lep filter   
    filtSeq.TTbarWToLeptonFilter.NumLeptons = -1 
    filtSeq.TTbarWToLeptonFilter.Ptcut = 0.

if dowh:
    genSeq.Pythia8.Commands += ["Higgs:useBSM = on",
                                "37:onMode = off",                   # turn off all mhc decays
                                "37:onIfMatch = 24 25"]              # switch on H+ to Wh
    evgenConfig.keywords+=['W','Higgs']

    genSeq.Pythia8.Commands += ["Higgs:useBSM = on",
                                "25:onMode = off",                   # turn off all mhc decays
                                "25:onIfMatch = 5 -5"]
    evgenConfig.keywords+=['bottom','bottom']

if dowz:
    genSeq.Pythia8.Commands += ["Higgs:useBSM = on",
                                "37:onMode = off",                   # turn off all mhc decays
                                "37:onIfMatch = 23 24"]              # switch on H+ to wz

if dodil:
    include('GeneratorFilters/TTbarWToLeptonFilter.py')                # lep filter   
    filtSeq.TTbarWToLeptonFilter.NumLeptons = 2 
    filtSeq.TTbarWToLeptonFilter.Ptcut = 0.
    evgenConfig.minevents=2000
    
