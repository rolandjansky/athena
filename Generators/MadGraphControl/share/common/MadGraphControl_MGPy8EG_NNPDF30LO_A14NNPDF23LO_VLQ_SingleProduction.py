from MadGraphControl.MadGraphUtils import *
import re

RealKT=0.1
RealKB=0.1
mode=re.split(r'_+',runArgs.jobConfig[0])[3][0:4]
if mode=="WTZt" or mode=="WTht" or mode=="WTWb" or mode=="ZTZt" or mode=="ZTht" or mode=="ZTWb":
	myKT=int(re.findall(r'\d+',re.findall(r'\d+.py',runArgs.jobConfig[0])[0])[0])
	RealKT = myKT/float(10)
elif  mode=="ZBZb" or mode=="ZBhb" or mode=="ZBWt" or mode=="WBZb" or mode=="WBhb" or mode=="WBWt": 
	myKB=int(re.findall(r'\d+',re.findall(r'\d+.py',runArgs.jobConfig[0])[0])[0])
	RealKB = myKB/float(10)
else: print "NO MODE"

mass=int(re.findall(r'\d+',re.findall(r'\d+LH',runArgs.jobConfig[0])[0])[0])

# Some printouts to check variables are correct or not
#print "RUNARGS" ,runArgs.jobConfig[0].split("_")[3]
#print mass
#print myKT
#print RealKT
#print mode

fcard = open('proc_card_mg5.dat','w')
if mode=="WTZt":
	fcard.write("""
	import model VLQ_UFO
	define p = g u c d s u~ c~ d~ s~
	define j = g u c d s u~ c~ d~ s~
	define bb = b b~
	define WW = w+ w-
	define tt = t t~
	define ferm = ve vm vt ve~ vm~ vt~ u c d s b u~ c~ d~ s~ b~ e- mu- ta- e+ mu+ ta+
	define VLQ = tp tp~
	generate  p p > j bb VLQ /h Z , (VLQ > Z tt, tt > ferm ferm bb, Z > ferm ferm)
	output -f
	""")
elif mode=="WTht":
	fcard.write("""
	import model VLQ_UFO
	define p = g u c d s u~ c~ d~ s~
	define j = g u c d s u~ c~ d~ s~
	define bb = b b~
	define WW = w+ w-
	define tt = t t~
	define ferm = ve vm vt ve~ vm~ vt~ u c d s b u~ c~ d~ s~ b~ e- mu- ta- e+ mu+ ta+
	define VLQ = tp tp~
	generate  p p > j bb VLQ /h Z , (VLQ > h tt, tt > ferm ferm bb)
	output -f
	""")
elif mode=="WTWb":
	fcard.write("""
	import model VLQ_UFO
	define p = g u c d s u~ c~ d~ s~
	define j = g u c d s u~ c~ d~ s~
	define bb = b b~
	define WW = w+ w-
	define tt = t t~
	define ferm = ve vm vt ve~ vm~ vt~ u c d s b u~ c~ d~ s~ b~ e- mu- ta- e+ mu+ ta+
	define VLQ = tp tp~
	generate  p p > j bb VLQ /h Z , (VLQ > WW bb, WW > ferm ferm)
	output -f
	""")
elif mode=="ZTZt": 
	fcard.write("""
	import model VLQ_UFO
	define p = g u c d s u~ c~ d~ s~
	define j = g u c d s u~ c~ d~ s~
	define bb = b b~
	define WW = w+ w-
	define tt = t t~
	define ferm = ve vm vt ve~ vm~ vt~ u c d s b u~ c~ d~ s~ b~ e- mu- ta- e+ mu+ ta+
	define VLQ = tp tp~
	generate  p p > j tt VLQ /W+ W- h , (VLQ > Z tt, tt > ferm ferm bb, Z > ferm ferm)
	output -f
	""")
elif mode=="ZTht":
	fcard.write("""
	import model VLQ_UFO
	define p = g u c d s u~ c~ d~ s~
	define j = g u c d s u~ c~ d~ s~
	define bb = b b~
	define WW = w+ w-
	define tt = t t~
	define ferm = ve vm vt ve~ vm~ vt~ u c d s b u~ c~ d~ s~ b~ e- mu- ta- e+ mu+ ta+
	define VLQ = tp tp~
	generate  p p > j tt VLQ /W+ W- h , (VLQ > h tt, tt > ferm ferm bb)
	output -f
	""")
elif mode=="ZTWb":
	fcard.write("""
	import model VLQ_UFO
	define p = g u c d s u~ c~ d~ s~
	define j = g u c d s u~ c~ d~ s~
	define bb = b b~
	define WW = w+ w-
	define tt = t t~
	define ferm = ve vm vt ve~ vm~ vt~ u c d s b u~ c~ d~ s~ b~ e- mu- ta- e+ mu+ ta+
	define VLQ = tp tp~
	generate  p p > j tt VLQ /W+ W- h , (VLQ > WW bb, WW > ferm ferm)
	output -f
	""")
elif mode=="ZBZb":
	fcard.write("""
	import model VLQ_UFO
	define p = g u c d s u~ c~ d~ s~
	define j = g u c d s u~ c~ d~ s~
	define bb = b b~
	define WW = w+ w-
	define tt = t t~
	define ferm = ve vm vt ve~ vm~ vt~ u c d s b u~ c~ d~ s~ b~ e- mu- ta- e+ mu+ ta+
	define VLQ = bp bp~
	generate p p > j bb VLQ /W+ W- h , (VLQ > Z bb, Z > ferm ferm)
	output -f
	""")
elif mode=="ZBhb":
	fcard.write("""
	import model VLQ_UFO
	define p = g u c d s u~ c~ d~ s~
	define j = g u c d s u~ c~ d~ s~
	define bb = b b~
	define WW = w+ w-
	define tt = t t~
	define ferm = ve vm vt ve~ vm~ vt~ u c d s b u~ c~ d~ s~ b~ e- mu- ta- e+ mu+ ta+
	define VLQ = bp bp~
	generate  p p > j bb VLQ /W+ W- h , VLQ > h bb
	output -f
	""")
elif mode=="ZBWt":
	fcard.write("""
	import model VLQ_UFO
	define p = g u c d s u~ c~ d~ s~
	define j = g u c d s u~ c~ d~ s~
	define bb = b b~
	define WW = w+ w-
	define tt = t t~
	define ferm = ve vm vt ve~ vm~ vt~ u c d s b u~ c~ d~ s~ b~ e- mu- ta- e+ mu+ ta+
	define VLQ = bp bp~
	generate  p p > j bb VLQ /W+ W- h , (VLQ > WW tt, tt > ferm ferm bb, WW > ferm ferm)
	output -f
	""")
elif mode=="WBZb":
	fcard.write("""
	import model VLQ_UFO
	define p = g u c d s u~ c~ d~ s~
	define j = g u c d s u~ c~ d~ s~
	define bb = b b~
	define WW = w+ w-
	define tt = t t~
	define ferm = ve vm vt ve~ vm~ vt~ u c d s b u~ c~ d~ s~ b~ e- mu- ta- e+ mu+ ta+
	define VLQ = bp bp~
	generate  p p > j tt VLQ /h Z , (VLQ > Z bb, Z > ferm ferm)
	output -f
	""")
elif mode=="WBhb":
	fcard.write("""
	import model VLQ_UFO
	define p = g u c d s u~ c~ d~ s~
	define j = g u c d s u~ c~ d~ s~
	define bb = b b~
	define WW = w+ w-
	define tt = t t~
	define ferm = ve vm vt ve~ vm~ vt~ u c d s b u~ c~ d~ s~ b~ e- mu- ta- e+ mu+ ta+
	define VLQ = bp bp~
	generate  p p > j tt VLQ /h Z , VLQ > h bb
	output -f
	""")
else: #mode=WBWt
	fcard.write("""
	import model VLQ_UFO
	define p = g u c d s u~ c~ d~ s~
	define j = g u c d s u~ c~ d~ s~
	define bb = b b~
	define WW = w+ w-
	define tt = t t~
	define ferm = ve vm vt ve~ vm~ vt~ u c d s b u~ c~ d~ s~ b~ e- mu- ta- e+ mu+ ta+
	define VLQ = bp bp~
	generate  p p > j tt VLQ /h Z , (VLQ > WW tt, tt > ferm ferm bb, WW > ferm ferm)
	output -f
	""")
fcard.flush()

process_dir = new_process()

if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    beamEnergy = 6500.
#mass=int(re.findall(r'\d+',re.findall(r'\d+RH',runArgs.jobConfig[0])[0])[0])
#print "RUNARGS" ,runArgs.jobConfig
#print mass

# allow usage of all PDF sets
os.environ['LHAPATH']=os.environ["LHAPDF_DATA_PATH"]=(os.environ['LHAPATH'].split(':')[0])+":/cvmfs/sft.cern.ch/lcg/external/lhapdfsets/current/"
# especially 263400 = NNPDF30_lo_as_0130_nf_4

extras = { 'lhe_version'   : '2.0',
           'cut_decays'    : 'F',
           'pdlabel'       : "'lhapdf'",
           'lhaid'         : '263400',
           'bwcutoff'      : '10000'
}
build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir), run_card_new='run_card.dat', nevts=runArgs.maxEvents * 1.1, rand_seed=runArgs.randomSeed, beamEnergy=beamEnergy, xqcut=0., extras=extras)


if not os.access(process_dir+'/Cards/param_card.dat',os.R_OK):
    print 'ERROR: Could not get param card'
elif os.access('param_card.dat',os.R_OK):
    print 'ERROR: Old run card in the current directory.  Dont want to clobber it.  Please move it first.'
else:
    oldcard = open(process_dir+'/Cards/param_card.dat','r')
    newcard = open('param_card.dat','w')
    for line in oldcard:
        if ' # MX ' in line:
            newcard.write(' 6000005 %i # MX \n'%(mass))
        elif ' # MTP ' in line:
            newcard.write(' 6000006 %i # MTP \n'%(mass))
        elif ' # MBP ' in line:
            newcard.write(' 6000007 %i # MBP \n'%(mass))
        elif ' # MY ' in line:
            newcard.write(' 6000008 %i # MY \n'%(mass))
        elif ' # KX ' in line:
            newcard.write(' 1 0.1 # KX \n')
        elif ' # KT ' in line:
            newcard.write(' 2 %f # KT \n'%(RealKT))
        elif ' # KB ' in line:
            newcard.write(' 3 %f # KB \n'%(RealKB))
        elif ' # KY ' in line:
            newcard.write(' 4 0.1 # KY \n')
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
            newcard.write('    9 1.000000e+00 # zetaBbL \n')
        elif ' # zetaYdL ' in line:
            newcard.write('   10 0.000000e-01 # zetaYdL \n')
        elif ' # zetaYsL ' in line:
            newcard.write('   11 0.000000e-01 # zetaYsL \n')
        elif ' # zetaYbL ' in line:
            newcard.write('   12 1.000000e+00 # zetaYbL \n')
        elif ' # zetaXuR ' in line:
            newcard.write('   13 0.000000e-01 # zetaXuR \n')
        elif ' # zetaXcR ' in line:
            newcard.write('   14 0.000000e-01 # zetaXcR \n')
        elif ' # zetaXtR ' in line:
            newcard.write('   15 0.000000e-01 # zetaXtR \n')
        elif ' # zetaTuR ' in line:
            newcard.write('   16 0.000000e-01 # zetaTuR \n')
        elif ' # zetaTcR ' in line:
            newcard.write('   17 0.000000e-01 # zetaTcR \n')
        elif ' # zetaTtR ' in line:
            newcard.write('   18 0.000000e-01 # zetaTtR \n')
        elif ' # zetaBdR ' in line:
            newcard.write('   19 0.000000e-01 # zetaBdR \n')
        elif ' # zetaBsR ' in line:
            newcard.write('   20 0.000000e-01 # zetaBsR \n')
        elif ' # zetaBbR ' in line:
            newcard.write('   21 0.000000e-01 # zetaBbR \n')
        elif ' # zetaYdR ' in line:
            newcard.write('   22 0.000000e-01 # zetaYdR \n')
        elif ' # zetaYsR ' in line:
            newcard.write('   23 0.000000e-01 # zetaYsR \n')
        elif ' # zetaYbR ' in line:
            newcard.write('   24 0.000000e-01 # zetaYbR \n')
        # set some sm parameters to atlas defaults
        elif ' # MB ' in line:
            newcard.write('    5 4.950000e+00 # MB \n')
        elif ' # MT ' in line:
            newcard.write('    6 1.725000e+02 # MT \n')
        elif ' # WT ' in line:
            newcard.write('DECAY   6 auto # WT \n')
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

import os
if 'ATHENA_PROC_NUMBER' in os.environ:
    evgenLog.info('Noticed that you have run with an athena MP-like whole-node setup.  Will re-configure now to make sure that the remainder of the job runs serially.')
    njobs = os.environ.pop('ATHENA_PROC_NUMBER')
    # Try to modify the opts underfoot
    if not hasattr(opts,'nprocs'): mglog.warning('Did not see option!')
    else: opts.nprocs = 0
    print opts

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

#WT mode
if mode=="WTZt": 
	evgenConfig.description = "MadGraph+Pythia8 production JO with NNPDF30LN and A15NNPDF23LO for VLQ single T to Zt with T produced via W"
	evgenConfig.process = "T_Zt"
elif mode=="WTht":
	evgenConfig.description = "MadGraph+Pythia8 production JO with NNPDF30LN and A15NNPDF23LO for VLQ single T to ht with T produced via W"
	evgenConfig.process = "T_ht"
elif mode=="WTWb":
	evgenConfig.description = "MadGraph+Pythia8 production JO with NNPDF30LN and A15NNPDF23LO for VLQ single T to Wb with T produced via W"
	evgenConfig.process = "T_Wb"
#ZT mode
elif mode=="ZTZt":
	evgenConfig.description = "MadGraph+Pythia8 production JO with NNPDF30LN and A15NNPDF23LO for VLQ single T to Zt with T produced via Z"
	evgenConfig.process = "T_Zt"
elif mode=="ZTht":
	evgenConfig.description = "MadGraph+Pythia8 production JO with NNPDF30LN and A15NNPDF23LO for VLQ single T to ht with T produced via Z"
	evgenConfig.process = "T_ht"
elif mode=="ZTWb":
	evgenConfig.description = "MadGraph+Pythia8 production JO with NNPDF30LN and A15NNPDF23LO for VLQ single T to Wb with T produced via Z"
	evgenConfig.process = "T_Wb"
#ZB mode
elif mode=="ZBZb":
	evgenConfig.description = "MadGraph+Pythia8 production JO with NNPDF30LN and A15NNPDF23LO for VLQ single B to Zb with B produced via Z"
	evgenConfig.process = "B_Zb"
elif mode=="ZBhb":
	evgenConfig.description = "MadGraph+Pythia8 production JO with NNPDF30LN and A15NNPDF23LO for VLQ single B to hb with B produced via Z"
	evgenConfig.process = "B_hb"
elif mode=="ZBWt":
	evgenConfig.description = "MadGraph+Pythia8 production JO with NNPDF30LN and A15NNPDF23LO for VLQ single B to Wt with B produced via Z"
	evgenConfig.process = "B_Wt"
#WB mode
elif mode=="WBZb":
	evgenConfig.description = "MadGraph+Pythia8 production JO with NNPDF30LN and A15NNPDF23LO for VLQ single B to Zb with B produced via W"
	evgenConfig.process = "B_Zb"
elif mode=="WBhb":
	evgenConfig.description = "MadGraph+Pythia8 production JO with NNPDF30LN and A15NNPDF23LO for VLQ single B to hb with B produced via W"
	evgenConfig.process = "B_hb"
else: #mode==WBWt:
	evgenConfig.description = "MadGraph+Pythia8 production JO with NNPDF30LN and A15NNPDF23LO for VLQ single B to Wt with B produced via W"
	evgenConfig.process = "B_Wt"

evgenConfig.keywords = ["BSM", "BSMtop", "exotic"]

#evgenConfig.process = "B_hb"

evgenConfig.contact =  ['mesut.unal@cern.ch']

evgenConfig.minevents = 5000

runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'

