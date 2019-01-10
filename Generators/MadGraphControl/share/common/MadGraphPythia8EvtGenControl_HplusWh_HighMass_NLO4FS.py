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
            newcard.write('      parameter (frac_upp=%fd0) \n' % (highfrac))
        else:
            newcard.write(line)
	

    oldcard.close()
    newcard.close()
    return newcard		

from MadGraphControl.MadGraphUtils import *
#
nevents=10000
mode=0

# this flag (with =1) is added from Aug 2015. "=1" will be used for any H+ production (from Aug 2015)
do_newtheory=1

# DSID lists (extensions can include systematics samples)
# H->Wh: 344878-344896
decay_Wh=[]
for x in xrange(344878,344897): decay_Wh.append(x)


fcard = open('proc_card_mg5.dat','w')
if runArgs.runNumber in decay_Wh:
    fcard.write("""
    set group_subprocesses Auto
    set ignore_six_quark_processes False   
    set loop_optimized_output True
    set complex_mass_scheme False
    import model 2HDMtypeII                     
    define p = g u c d s u~ c~ d~ s~
    define j = g u c d s u~ c~ d~ s~
    define l+ = e+ mu+
    define l- = e- mu-
    define vl = ve vm vt
    define vl~ = ve~ vm~ vt~
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
# Charge Higgs, and all other masses in GeV
#--------------------------------------------------------------
###
mhc_str = str(runArgs.jobConfig[0])  # JOB OPTION NAME MUST CONTAIN THE MASS WE WANT TO SIMULATE IN FORMAT LIKE: *_H400_*
mhc=0
int(s) 
for s in mhc_str.split("_"):
    ss=s.replace("H","")  
    if ss.isdigit():
        mhc = int(ss)        
if mhc==0:
   raise RuntimeError("Charged Higgs mass not set, mhc=0, check joOption name %s"%mhc_str)
###
import math
mh1=1.250e+02                 
mh2=math.sqrt(math.pow(mhc,2)+math.pow(8.0399e+01,2)) 
mh3=mh2

#--------------------------------------------------------------
# Scale -> fixed at (mt+mhc)/3
#--------------------------------------------------------------
if do_newtheory:
    scale=(mhc/3)
else:
    scale=(mhc+1.725e+02)/3

#Fetch default NLO run_card.dat and set parameters. using nn23nlo instead of cteq6l1

extras = { 'lhe_version':'3.0',                        # These two are not present in NLO cards, 
           'cut_decays':'F',                           # lhe version fixed by arrange_output function   
           'pdlabel':"'nn23nlo'",
           'parton_shower':'PYTHIA8',
           'fixed_ren_scale':'F',
           'fixed_fac_scale':'F',
           'muR_ref_fixed':str(-1), 
           'muF1_ref_fixed':str(-1), 
           'muF2_ref_fixed':str(-1), 
           'dynamical_scale_choice':str(-1),
           }

process_dir = new_process()

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)




#Build param_card.dat from UFO, and set Higgs masses
#write_param_card is part of the 2HDMTypeII model
import os
import sys
sys.path.append( os.environ['MADPATH']+'/models/2HDMtypeII/' )
from write_param_card import ParamCardWriter
ParamCardWriter('param_card.TMP.dat')
masses = {'25':str(mh1)+'  #  mh1',
          '35':str(mh2)+'  #  mh2',
          '36':str(mh2)+'  #  mh2',
          '37':str(mhc)+'  #  mhc'
          }
build_param_card(param_card_old='param_card.TMP.dat',param_card_new='param_card.dat',masses=masses)

print_cards()

runName='run_01'     

if do_newtheory:
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

# the following 4 lines are added (Aug 2015)
os.symlink(os.environ['FASTJETPATH']+'/include/fastjet',os.environ['PWD']+'/'+process_dir+'/SubProcesses/P0_gg_hpbtx/fastjet')
os.symlink(os.environ['FASTJETPATH']+'/include/fastjet',os.environ['PWD']+'/'+process_dir+'/SubProcesses/P0_uux_hpbtx/fastjet')
os.symlink(os.environ['FASTJETPATH']+'/include/fastjet',os.environ['PWD']+'/'+process_dir+'/SubProcesses/P0_uxu_hpbtx/fastjet')
os.system("/bin/ln -s " + os.environ['FASTJETPATH'] + "/lib/* "+os.environ['PWD']+'/'+process_dir+'/lib')

import sys,os
os.system('sed -i -e "s,tmp=tmp/2d0,tmp=tmp/3d0,g" -e "s,H_T/2,H_T/3,g" -e "s,mT(i)/2,mT(i)/3,g" `find . -name "*scales*" ` ')


generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=mode,proc_dir=process_dir,run_name=runName)

arrange_output(run_name=runName,lhe_version=3,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz')

# Shower
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_aMcAtNlo.py")

evgenConfig.description = 'aMcAtNlo High mass charged Higgs NLO4FS'
evgenConfig.keywords+=['Higgs','MSSM','BSMHiggs','chargedHiggs']
evgenConfig.contact = ['G. Carrillo-Montoya']

evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'

# JOB OPTION NAME MUST CONTAIN THE DECAY MODE WE WANT TO SIMULATE IN FORMAT LIKE: *_Wh* or *_tb*
# Also double check the RunNumber associated with that decay is in the appropianted list
doWh=False
for s in mhc_str.split("_"):
    ss=s.replace(".py","")  
    print ss
    if ss=='Wh' and runArgs.runNumber in decay_Wh:
        doWh=True

if doWh==False:
    raise RuntimeError("No decay mode was identified, check jobOption name %s, and/or runNumber %i."%(runArgs.jobConfig[0],runArgs.runNumber))

if doWh:
    genSeq.Pythia8.Commands += ["Higgs:useBSM = on",
                                "37:onMode = off",                   # turn off all mhc decays
                                "37:onIfMatch = 24 25"]              # switch on H+ to Wh
    evgenConfig.keywords+=['W','Higgs']

    genSeq.Pythia8.Commands += ["Higgs:useBSM = on",
                                "25:onMode = off",                   # turn off all mhc decays
                                "25:onIfMatch = 5 -5"]
    evgenConfig.keywords+=['bottom','bottom']
    

include('GeneratorFilters/LeptonFilter.py')
