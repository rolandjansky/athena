from MadGraphControl.MadGraphUtils import *
include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

print "Stop Mass", runArgs.jobConfig[0].split('_')[4]
print "ctau",      runArgs.jobConfig[0].split('_')[6].split('.')[0].replace('p','.')

gentype = str(runArgs.jobConfig[0].split('_')[2])
decaytype = str(runArgs.jobConfig[0].split('_')[3])

masses['1000006'] = float(runArgs.jobConfig[0].split('_')[4])
masses['1000005'] = 3.00000000E+05 
masses['1000022'] = 100000.

lifetimeString = str(runArgs.jobConfig[0].split('_')[6])
stopLifetime = lifetimeString.replace("ns","").replace(".py","").replace("p",".")
hbar = 6.582119514e-16
stopWidth = hbar/float(stopLifetime)

decays = {'1000006':"""
DECAY   1000006   %s  #stop1 decay
#          BR         NDA      ID1       ID2
     1.0000000000    2          -13         5   # BR(~t_1 -> b mu )
# 

DECAY 1  0.000000e+00
DECAY 2  0.000000e+00
DECAY 3  0.000000e+00
DECAY 4  0.000000e+00
DECAY 5  0.000000e+00
DECAY 11 0.000000e+00
DECAY 12 0.000000e+00
DECAY 13 0.000000e+00
DECAY 14 0.000000e+00
DECAY 15 0.000000e+00
DECAY 16 0.000000e+00
DECAY 21 0.000000e+00
DECAY 22 0.000000e+00

"""%(stopWidth),
}
# Specify the process here
process = '''
generate p p > t1 t1~ $ go susylq susylq~ b2 t1 t2 b2~ t1~ t2~ @1
add process p p > t1 t1~ j $ go susylq susylq~ b2 t1 t2 b2~ t1~ t2~ @2
add process p p > t1 t1~ j j $ go susylq susylq~ b2 t1 t2 b2~ t1~ t2~ @3
'''
njets = 2


# Specify evgenConfig
build_param_card(param_card_old='param_card.SM.TT.RPVdirectBL.dat', param_card_new='param_card.SM.TT.RPVdirect.BL.'+str(stopWidth)+'.dat',masses=masses,decays=decays)

evgenLog.info('Registered generation of stop pair production, sbottom to b+L; grid point '+str(runArgs.runNumber))
evgenConfig.contact  = [ "karri.folan.di.petrillo@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel']
evgenConfig.description = 'stop direct pair production, st->b+L in simplified model, m_stop = %s GeV'%(masses['1000006'])
extras['time_of_flight']='1E-2' # in mm

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{t1,1000006}{t1~,-1000006}"]

testSeq.TestHepMC.MaxVtxDisp=100000.# in mm

