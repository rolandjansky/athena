include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

masses['1000024'] = float(runArgs.jobConfig[0].split('_')[4])
masses['1000023'] = float(runArgs.jobConfig[0].split('_')[4])
masses['1000022'] = float(runArgs.jobConfig[0].split('_')[5])
if masses['1000022']<0.5: masses['1000022']=0.5
gentype = str(runArgs.jobConfig[0].split('_')[2])
decaytype = str(runArgs.jobConfig[0].split('_')[3])
process = '''
generate p p > x1+ n2 $ susystrong @1
add process p p > x1- n2 $ susystrong @1
add process p p > x1+ n2 j $ susystrong @2
add process p p > x1- n2 j $ susystrong @2
add process p p > x1+ n2 j j $ susystrong @3
add process p p > x1- n2 j j $ susystrong @3
'''
njets = 2
evgenLog.info('Registered generation of ~chi1+/- ~chi20 production, decay via WZ; grid point '+str(runArgs.runNumber)+' decoded into mass point ' + str(masses['1000024']) + ' ' + str(masses['1000022']))

evgenConfig.contact  = [ "keisuke.kouda@cern.ch, hidetoshi.otono@cern.ch, keisuke.yoshihara@cern.ch" ]
evgenConfig.keywords += ['SUSY', 'gaugino', 'chargino', 'neutralino']
evgenConfig.description = '~chi1+/- ~chi20 production, decay via W and Z with displaced vertex'

keepOutput=True

if 'p01ns' in runArgs.jobConfig[0].split('_')[6]:
	evgenLog.info('lifetime of 1000023 is set as 0.01ns, decay via Higgs')
	decays ={'1000023':'''DECAY 1000023 6.58211928E-14 #neutralino2 lifetime ~ 0.01ns
#	BR		NDA	ID1	  ID2
     1.00000000E+00    2     1000022        25   #BR(~chi_20 -> ~chi_10 h)
     0.00000000E+00    3     1000022        11     -11   # BR(~chi_20 -> ~chi_10 e+ e-)
#
     '''}

elif 'p03ns' in runArgs.jobConfig[0].split('_')[6]:
	evgenLog.info('lifetime of 1000023 is set as 0.03ns, decay via Higgs')
	decays ={'1000023':'''DECAY 1000023 2.19403976E-14 #neutralino2 lifetime ~ 0.03ns
#	BR		NDA	ID1	  ID2
     1.00000000E+00    2     1000022        25   #BR(~chi_20 -> ~chi_10 h)
     0.00000000E+00    3     1000022        11     -11   # BR(~chi_20 -> ~chi_10 e+ e-)
#
     '''}

elif 'p1ns' in runArgs.jobConfig[0].split('_')[6]:
	evgenLog.info('lifetime of 1000023 is set as 0.1ns, decay via Higgs')
	decays ={'1000023':'''DECAY 1000023 6.58211928E-15 #neutralino2 lifetime ~ 0.1ns
#	BR		NDA	ID1	  ID2
     1.00000000E+00    2     1000022        25   #BR(~chi_20 -> ~chi_10 h)
     0.00000000E+00    3     1000022        11     -11   # BR(~chi_20 -> ~chi_10 e+ e-)
#
     '''}

elif 'p3ns' in runArgs.jobConfig[0].split('_')[6]:
	evgenLog.info('lifetime of 1000023 is set as 0.3ns, decay via Higgs')
	decays ={'1000023':'''DECAY 1000023 2.19403976E-15 #neutralino2 lifetime ~ 0.3ns
#	BR		NDA	ID1	  ID2
     1.00000000E+00    2     1000022        25   #BR(~chi_20 -> ~chi_10 h)
     0.00000000E+00    3     1000022        11     -11   # BR(~chi_20 -> ~chi_10 e+ e-)
#
     '''}

elif '1ns' in runArgs.jobConfig[0].split('_')[6]:
	evgenLog.info('lifetime of 1000023 is set as 1ns, decay via Higgs')
	decays ={'1000023':'''DECAY 1000023 6.58211928E-16 #neutralino2 lifetime ~ 1ns
#	BR		NDA	ID1	  ID2
     1.00000000E+00    2     1000022        25   #BR(~chi_20 -> ~chi_10 h)
     0.00000000E+00    3     1000022        11     -11   # BR(~chi_20 -> ~chi_10 e+ e-)
#
     '''}

elif '3ns' in runArgs.jobConfig[0].split('_')[6]:
	evgenLog.info('lifetime of 1000023 is set as 3ns, decay via Higgs')
	decays ={'1000023':'''DECAY 1000023 2.19403976E-16 #neutralino2 lifetime ~ 3ns
#	BR		NDA	ID1	  ID2
     1.00000000E+00    2     1000022        25   #BR(~chi_20 -> ~chi_10 h)
     0.00000000E+00    3     1000022        11     -11   # BR(~chi_20 -> ~chi_10 e+ e-)
#
     '''}

elif '10ns' in runArgs.jobConfig[0].split('_')[6]:
	evgenLog.info('lifetime of 1000023 is set as 10ns, decay via Higgs')
	decays ={'1000023':'''DECAY 1000023 6.58211928E-17 #neutralino2 lifetime ~ 10ns
#	BR		NDA	ID1	  ID2
     1.00000000E+00    2     1000022        25   #BR(~chi_20 -> ~chi_10 h)
     0.00000000E+00    3     1000022        11     -11   # BR(~chi_20 -> ~chi_10 e+ e-)
#
     '''}

genSeq.Pythia8.Commands += ["23:mMin = 0.2"]
genSeq.Pythia8.Commands += ["24:mMin = 0.2"]
genSeq.Pythia8.Commands += ["25:mMin = 0.2"]

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += [ "Merging:Process = pp>{x1+,1000024}{x1-,-1000024}{n2,1000023}",
                                 "1000024:spinType = 1",
                                 "1000023:spinType = 1" ]

from MadGraphControl.MadGraphUtils import *
from glob import glob
a_proc_dir = glob('PROC*')[0]
add_lifetimes(process_dir=a_proc_dir,threshold=1E-25)
the_spot = arrange_output(run_name='Test',proc_dir= a_proc_dir,outputDS='madgraph_OTF._00001.events.tar.gz')
runArgs.inputGeneratorFile = the_spot

testSeq.TestHepMC.MaxVtxDisp = 1000*1000 #In mm                                                     
testSeq.TestHepMC.MaxTransVtxDisp = 1000*1000
