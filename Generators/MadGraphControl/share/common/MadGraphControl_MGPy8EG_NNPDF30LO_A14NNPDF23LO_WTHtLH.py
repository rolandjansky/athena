from MadGraphControl.MadGraphUtils import *
import re
include("MC15JobOptions/VLQCouplingCalculator.py")
include("MC15JobOptions/lhe_hacker.py")
import subprocess
import sys
# import lheparser

reMass = int(re.findall(r'\d+',re.findall(r'\d+LH',runArgs.jobConfig[0])[0])[0])
reKappa = int(re.findall(r'\d+', runArgs.jobConfig[0])[-1])
runArgs.mass = reMass
runArgs.kappa = reKappa*0.01


def rewtcardmaker(m, K):
    #print m
    #print K_grid
    launch_line = "launch --rwgt_name="
    tagname = ('M' + str(int(m/100)) + 'K{0:03d}').format(int(K*100))
    if float(m) == float(reMass) and float(K) == float(int(runArgs.kappa*100)/100.0):
        return [False, tagname]
    f = open("reweight_card.dat", "w")
    print tagname
    c = VLQCouplingCalculator(float(m), 'T')
    c.setKappaxi(K, 0.5, 0.25)
    gamma = c.getGamma()
    print K, gamma
    f.write(launch_line + tagname + '\n')
    f.write('\tset KT ' + str(K) + '\n')
    f.write('\tset MTP ' + str(m) + '\n')
    f.write('\tset WTP ' + str(gamma) + '\n')
    f.flush()
    f.close()
    return [True, tagname]


M_grid = [reMass-100., reMass]
K_grid = [0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6]
rewtcardmaker(reMass,0.5)

MAX_TRIAL = 50

##Figure out what kind of process we have (from file name)
runArgs.proc = "add process p p > j VLQ bb / tp tp~ p t t~ y y~ bp bp~ x x~ z h a, (VLQ > h tt, tt > ferm ferm bb)"
#runArgs.proc = "add process p p > j VLQ bb / tp tp~ p t t~ y y~ bp bp~ x x~ z h a, (VLQ > z tt)"

print ("Mass:  ", runArgs.mass)
print ("Kappa: ", runArgs.kappa)
print ("Proc:  ", runArgs.proc)


fcard = open('proc_card_mg5_fullDecay.dat','w')
fcard.write("""
import model /cvmfs/atlas.cern.ch/repo/sw/Generators/madgraph/models/latest/VLQ_UFO

define p = g u c d s u~ c~ d~ s~ 
define j = g u c d s u~ c~ d~ s~
define bb = b b~
define WW = w+ w-
define tt = t t~
define ferm = ve vm vt ve~ vm~ vt~ mu- ta- e- mu+ ta+ e+ u c d s u~ c~ d~ s~
define VLQ = tp tp~
%s 
output -f
"""%(runArgs.proc))
fcard.close()

fcard = open('proc_card_mg5_minDecay.dat','w')
fcard.write("""
import model /cvmfs/atlas.cern.ch/repo/sw/Generators/madgraph/models/latest/VLQ_UFO

define p = g u c d s u~ c~ d~ s~ 
define j = g u c d s u~ c~ d~ s~
define bb = b b~
define WW = w+ w-
define tt = t t~
define ferm = ve vm vt ve~ vm~ vt~ mu- ta- e- mu+ ta+ e+ u c d s u~ c~ d~ s~
define VLQ = tp tp~
%s 
output -f
"""%("add process p p > j VLQ bb / tp tp~ p t t~ y y~ bp bp~ x x~ z h a, (VLQ > h tt)"))

fcard.close()

process_dir_fullDecay = new_process(card_loc='proc_card_mg5_fullDecay.dat')

if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    beamEnergy = 6500.

# allow usage of all PDF sets
os.environ['LHAPATH']=os.environ["LHAPDF_DATA_PATH"]=(os.environ['LHAPATH'].split(':')[0])+":/cvmfs/sft.cern.ch/lcg/external/lhapdfsets/current/"
# especially 263400 = NNPDF30_lo_as_0130_nf_4

extras = { 'lhe_version'   : '3.0',
           'cut_decays'    : 'F',
           'pdlabel'       : "'lhapdf'",
           'lhaid'         : '263400',
           'bwcutoff'      : '10000',
           'event_norm'    : 'average',
           'use_syst'      : 'False',
           'drjj'          :  -1.0,
           'drll'          :  -1.0,
           'draa'          :  -1.0,
           'draj'          :  -1.0,
           'drjl'          :  -1.0,
           'dral'          :  -1.0,
           'etal'          :  -1.0,
           'etaj'          :  -1.0,
           'etaa'          :  -1.0,
}

safe_factor = 1.1
build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir_fullDecay), run_card_new='run_card.dat', nevts=runArgs.maxEvents * safe_factor, rand_seed=runArgs.randomSeed, beamEnergy=beamEnergy, xqcut=0., extras=extras)


if not os.access(process_dir_fullDecay+'/Cards/param_card.dat',os.R_OK):
    print 'ERROR: Could not get param card'
elif os.access('param_card.dat',os.R_OK):
    print 'ERROR: Old run card in the current directory.  Dont want to clobber it.  Please move it first.'
else:
    oldcard = open(process_dir_fullDecay+'/Cards/param_card.dat','r')
    newcard = open('param_card.dat','w')
    for line in oldcard:
        if ' # MX ' in line:
            newcard.write(' 6000005 %i # MX \n'%(runArgs.mass))
        elif ' # MTP ' in line:
            newcard.write(' 6000006 %i # MTP \n'%(runArgs.mass))
        elif ' # MBP ' in line:
            newcard.write(' 6000007 %i # MBP \n'%(runArgs.mass))
        elif ' # MY ' in line:
            newcard.write(' 6000008 %i # MY \n'%(runArgs.mass))
        elif ' # KX ' in line:
            newcard.write(' 1 %e # KX \n'%(runArgs.kappa))
        elif ' # KT ' in line:
            newcard.write(' 2 %e # KT \n'%(runArgs.kappa))
        elif ' # KB ' in line:
            newcard.write(' 3 %e # KB \n'%(runArgs.kappa))
        elif ' # KY ' in line:
            newcard.write(' 4 %e # KY \n'%(runArgs.kappa))
        elif ' # WX ' in line:
            newcard.write('DECAY 6000005 auto # WX \n')
        elif ' # WTP ' in line:
            newcard.write('DECAY 6000006 auto # WTP \n')
        elif ' # WBP ' in line:
            newcard.write('DECAY 6000007 auto # WBP \n')
        elif ' # WY ' in line:
            newcard.write('DECAY 6000008 auto # WY \n')
        elif ' # zetaXuL ' in line:
            newcard.write('    1 0.000000e-01 # zetaXuL \n')
        elif ' # zetaXcL ' in line:
            newcard.write('    2 0.000000e-01 # zetaXcL \n')
        elif ' # zetaXtL ' in line:
            newcard.write('    3 1.000000e+00 # zetaXtL \n')
        elif ' # zetaTuL ' in line:
            newcard.write('    4 0.000000e-01 # zetaTuL \n')
        elif ' # zetaTcL ' in line:
            newcard.write('    5 0.000000e-01 # zetaTcL \n')
        elif ' # zetaTtL ' in line:
            newcard.write('    6 1.000000e+00 # zetaTtL \n')
        elif ' # zetaBdL ' in line:
            newcard.write('    7 0.000000e-01 # zetaBdL \n')
        elif ' # zetaBsL ' in line:
            newcard.write('    8 0.000000e-01 # zetaBsL \n')
        elif ' # zetaBbL ' in line:
            newcard.write('    9 1.000000e+00 # zetaBdL \n')
        elif ' # zetaYdL ' in line:
            newcard.write('   10 0.000000e+00# zetaYdL \n')
        elif ' # zetaYsL ' in line:
            newcard.write('   11 0.000000e+00 # zetaYsL \n')
        elif ' # zetaYbL ' in line:
            newcard.write('   12 0.000000e+00 # zetaYbL \n')
        elif ' # zetaXuR ' in line:
            newcard.write('   13 0.000000e+00 # zetaXuR \n')
        elif ' # zetaXcR ' in line:
            newcard.write('   14 0.000000e+00 # zetaXcR \n')
        elif ' # zetaXtR ' in line:
            newcard.write('   15 0.000000e+00 # zetaXtR \n')
        elif ' # zetaTuR ' in line:
            newcard.write('   16 0.000000e+00 # zetaTuR \n')
        elif ' # zetaTcR ' in line:
            newcard.write('   17 0.000000e+00 # zetaTcR \n')
        elif ' # zetaTtR ' in line:
            newcard.write('   18 0.000000e+00 # zetaTtR \n')
        elif ' # zetaBdR ' in line:
            newcard.write('   19 0.000000e+00 # zetaBdR \n')
        elif ' # zetaBsR ' in line:
            newcard.write('   20 0.000000e+00 # zetaBsR \n')
        elif ' # zetaBbR ' in line:
            newcard.write('   21 0.000000e+00 # zetaBdR \n')
        elif ' # zetaYdR ' in line:
            newcard.write('   22 0.000000e+00 # zetaYdR \n')
        elif ' # zetaYsR ' in line:
            newcard.write('   23 0.000000e+00 # zetaYsR \n')
        elif ' # zetaYbR ' in line:
            newcard.write('   24 0.000000e+00 # zetaYbR \n')
        elif '# xitpw' in line:
            newcard.write('   1 5.000000e-01 # xitpw \n')
        elif '# xitpz' in line:
            newcard.write('   2 2.500000e-01 # xitpz \n')
        elif '# xitph' in line:
            newcard.write('   3 2.500000e-01 # xitph \n') 
# set some sm parameters to atlas defaults
        elif ' # MB ' in line:
            newcard.write('    5 4.950000e+00 # MB \n')
        elif ' # MT ' in line:
            newcard.write('    6 1.725000e+02 # MT \n')
        elif ' # WT ' in line:
            newcard.write('DECAY   6 1.350000e+00 # WT \n')
        elif ' # WZ ' in line:
            newcard.write('DECAY  23 2.495200e+00 # WZ \n')
        elif ' # WW ' in line:
            newcard.write('DECAY  24 2.085000e+00 # WW \n')
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()


runName='run_01'
#generate(run_card_loc='./run_card.dat',param_card_loc='./param_card.dat',run_name=runName+'_fullDecay',proc_dir=process_dir_fullDecay)
trial_count = 0

while os.path.exists(process_dir_fullDecay + '/Events/' + runName + '_fullDecay/unweighted_events.lhe.gz') == False and os.path.exists(process_dir_fullDecay + '/Events/' + runName + '_fullDecay/unweighted_events.lhe') == False and trial_count < MAX_TRIAL:
    generate(run_card_loc='./run_card.dat',param_card_loc='./param_card.dat',run_name=runName+'_fullDecay',proc_dir=process_dir_fullDecay)
    trial_count += 1

arrange_output(run_name=runName+'_fullDecay',proc_dir=process_dir_fullDecay,outputDS=runName+'_fullDecay._00001.events.tar.gz',saveProcDir=True)


process_dir_minDecay = new_process(card_loc='proc_card_mg5_minDecay.dat')
#generate(run_card_loc='./run_card.dat',param_card_loc='./param_card.dat',run_name=runName+'_minDecay',proc_dir=process_dir_minDecay)
trial_count = 0

while os.path.exists(process_dir_minDecay + '/Events/' + runName + '_minDecay/unweighted_events.lhe') == False and os.path.exists(process_dir_minDecay + '/Events/' + runName + '_minDecay/unweighted_events.lhe.gz') == False and trial_count < MAX_TRIAL:
    generate(run_card_loc='./run_card.dat',param_card_loc='./param_card.dat',reweight_card_loc='./reweight_card.dat',run_name=runName+'_minDecay',proc_dir=process_dir_minDecay)
    trial_count += 1

arrange_output(run_name=runName+'_minDecay',proc_dir=process_dir_minDecay,outputDS=runName+'_minDecay._00001.events.tar.gz',saveProcDir=True)  


#runName='run_01'
#process_dir_fullDecay = 'PROC_VLQ_UFO_0'
#process_dir_minDecay = 'PROC_VLQ_UFO_1'
#Refurbishing

status = lhe_hacker(lhe_minDecay=process_dir_minDecay+'/Events/run_01_minDecay/unweighted_events.lhe', lhe_fullDecay=process_dir_fullDecay+'/Events/run_01_fullDecay/unweighted_events.lhe',vlq='T',decay='H')
status_2 = False
ME_script = open('script.txt','w')
ME_script.write('''
reweight run_RWT -f
done
''')
#for ii in range(31):
#    ME_script.write('done\n')

ME_script.flush()
ME_script.close()
if status:
    subprocess.call('mkdir -p ' + process_dir_minDecay+'/Events/run_RWT/', shell=True)
    subprocess.call('cp unweighted_events.lhe ' + process_dir_minDecay+'/Events/run_RWT/', shell=True)
    for m in M_grid:
        for K in K_grid:
            [is_ok, tagname] = rewtcardmaker(m,K)
            if is_ok:
                subprocess.call('cp reweight_card.dat ' + process_dir_minDecay+'/Cards/', shell=True)
                did_it_work = False
                trial_count = 0
                while not did_it_work and trial_count < MAX_TRIAL:
                    trial_count += 1
                    subprocess.call('cp reweight_card.dat ' + process_dir_minDecay+'/Cards/', shell=True)
                    subprocess.call('export CC=gcc && ' + process_dir_minDecay + '/bin/madevent script.txt -f', shell=True)
                    sys.stdout.flush()
                    try: 
                        subprocess.call('gunzip ' + process_dir_minDecay + '/Events/run_RWT/unweighted_events.lhe.gz', shell=True)
                        print "found gzipped file in run_RWT and unzipped it"
                    except:
                        print "did not find gzipped file. Already unzipped?"
                        pass
                    thisfile = open(process_dir_minDecay + '/Events/run_RWT/unweighted_events.lhe' , 'r')
                    for line in thisfile:
                        if "<weight id='" + tagname +"'" in line:
                            print tagname, " reweighting worked"
                            did_it_work = True
                            break
                        else:
                            continue
                    if not did_it_work:
                        print tagname, " reweighting did not work. Retrying!"
                    sys.stdout.flush()
                        
    status_2 = placeback(lhe_fullDecay=process_dir_fullDecay+'/Events/run_01_fullDecay/unweighted_events.lhe',lhe_reweighted=process_dir_minDecay+'/Events/run_RWT/unweighted_events.lhe')
    if status_2:
        subprocess.call('tar -czf tmp_final_events.events.tar.gz tmp_final_events.events', shell=True)

if status and status_2:
    runArgs.inputGeneratorFile='final_events.events.tar.gz'
else:
    runArgs.inputGeneratorFile=runName+'_fullDecay._00001.events.tar.gz'

#runName = 'run_01'
#evgenConfig.inputfilecheck = runName                                                                                                                                    
#runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz' 

include("MC15JobOptions/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("MC15JobOptions/Pythia8_MadGraph.py")


evgenConfig.description = "MadGraph+Pythia8 production JO with NNPDF30LN and A15NNPDF23LO for VLQ single T to Ht with T produced via W"

evgenConfig.keywords = ["BSM", "BSMtop", "exotic"]

evgenConfig.process = "T_Ht"

evgenConfig.contact =  ['fschenck@cern.ch,avik.roy@cern.ch']

evgenConfig.minevents = 5000

