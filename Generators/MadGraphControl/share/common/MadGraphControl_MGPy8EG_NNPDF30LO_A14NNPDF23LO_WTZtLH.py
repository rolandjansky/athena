from MadGraphControl.MadGraphUtils import *
import re

reMass = int(re.findall(r'\d+',re.findall(r'\d+LH',runArgs.jobConfig[0])[0])[0])

reKappa = int(re.findall(r'\d+', runArgs.jobConfig[0])[-1])

runArgs.mass = reMass
runArgs.kappa = reKappa*0.01

#Figure out what kind of process we have (from file name)
runArgs.proc = "add process p p > j VLQ bb / tp tp~ p t t~ y y~ bp bp~ x x~ z h a, (VLQ > Z tt, Z > ferm ferm, tt > ferm ferm bb)"


print ("Mass:  ", runArgs.mass)
print ("Kappa: ", runArgs.kappa)
print ("Proc:  ", runArgs.proc)


fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model VLQ_UFO

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



fcard.flush()


process_dir = new_process()
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
build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir), run_card_new='run_card.dat', nevts=runArgs.maxEvents * safe_factor, rand_seed=runArgs.randomSeed, beamEnergy=beamEnergy, xqcut=0., extras=extras)


if not os.access(process_dir+'/Cards/param_card.dat',os.R_OK):
    print 'ERROR: Could not get param card'
elif os.access('param_card.dat',os.R_OK):
    print 'ERROR: Old run card in the current directory.  Dont want to clobber it.  Please move it first.'
else:
    oldcard = open(process_dir+'/Cards/param_card.dat','r')
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
        elif ' # xitpw   ' in line:
            newcard.write('   1 5.000000e-01 # xitpw \n')
        elif ' # xitpz   ' in line:
            newcard.write('   2 2.500000e-01 # xitpz \n')
        elif ' # xitpt   ' in line:
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
generate(run_card_loc='./run_card.dat',param_card_loc='./param_card.dat',run_name=runName,proc_dir=process_dir)

arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz')  


runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'


include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")


evgenConfig.description = "MadGraph+Pythia8 production JO with NNPDF30LN and A15NNPDF23LO for VLQ single Y to Wb with Y produced via W"

evgenConfig.keywords = ["BSM", "BSMtop", "exotic"]

evgenConfig.process = "Y_Wb"

evgenConfig.contact =  ['fschenck@cern.ch']

evgenConfig.minevents = 5000

