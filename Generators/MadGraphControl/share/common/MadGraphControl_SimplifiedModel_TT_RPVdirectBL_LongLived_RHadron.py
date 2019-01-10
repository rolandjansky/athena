from MadGraphControl.MadGraphUtils import *
include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

infoStrings = runArgs.jobConfig[0].split("/")[-1].split(".")[2].split("_")

evgenLog.info( "  jobConfig: %s  ", runArgs.jobConfig[0] )
evgenLog.info( "  stop mass: %s  ", infoStrings[4] )
evgenLog.info( "  stop ctau: %s  ", infoStrings[6].replace('p','.') )

gentype   = str(infoStrings[2])
decaytype = str(infoStrings[3])

## Setting masses from filename parsing

masses['1000006'] = float(infoStrings[4])
masses['1000005'] = 3.00000000E+05
masses['1000022'] = 100000.

## Converting lifetime from filename to width

lifetimeString = str(infoStrings[6])
stopLifetime = lifetimeString.replace("ns","").replace(".py","").replace("p",".")
hbar = 6.582119514e-16
stopWidth = hbar/float(stopLifetime)

evgenLog.info( "  stop lifetime, width: %f, %f  ", float(stopLifetime), stopWidth )

## Optional custom gluinoball probability in the filename

if len(infoStrings)>7:
	gluinoBallProbabilityString = str(infoStrings[7])
else:
	gluinoBallProbabilityString = "gball10"

gluinoBallProbability = float(gluinoBallProbabilityString.replace("gball",""))/100.
evgenLog.info( "  gluino-ball probability: %f  ", gluinoBallProbability )

## Defining the narrow width stop and its decay modes to b+mu
## ... Also need to define widths for SM particles for MG's add_lifetimes to work properly

decays = {'1000006':"""
DECAY   1000006   %s  #stop1 decay
    1.0000000000    2          -13         5   # BR(~t_1 -> b mu )
"""%(stopWidth)}

# Specify the process here

process = '''
generate p p > t1 t1~ $ go susylq susylq~ b2 t1 t2 b2~ t1~ t2~ @1
add process p p > t1 t1~ j $ go susylq susylq~ b2 t1 t2 b2~ t1~ t2~ @2
add process p p > t1 t1~ j j $ go susylq susylq~ b2 t1 t2 b2~ t1~ t2~ @3
'''
njets = 2

# Specify evgenConfig

build_param_card(param_card_old='param_card.SM.TT.RPVdirectBL.dat', param_card_new='param_card.SM.TT.RPVdirect.BL.'+str(stopWidth)+'.dat',masses=masses,decays=decays)

evgenLog.info('Registered generation of stop pair production to b+mu; grid point '+str(runArgs.runNumber))
evgenConfig.contact  = [ "karri.folan.di.petrillo@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel']
evgenConfig.description = 'stop direct pair production, st->b+mu in simplified model, m_stop = %s GeV, Long-lived with R-Hadron treatment.'%(masses['1000006'])
evgenConfig.minevents = 2000

## Specifies the distance over which things are special and "long-lived"

extras['time_of_flight']='1E-2' # in mm

## Forcing this to not use syscalc

extras['event_norm']='sum'
extras['use_syst']='F'

# Helper for the simulation step

if hasattr(runArgs,'inputEVNTFile'):
    SLHAonly=True

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

# Pythia8 R-Hadron Module Config

genSeq.Pythia8.Commands += ["Init:showChangedSettings = on"]
genSeq.Pythia8.Commands += ["Rhadrons:allow = on"]
genSeq.Pythia8.Commands += ["RHadrons:allowDecay = off"]
genSeq.Pythia8.Commands += ["RHadrons:probGluinoball = %f"%gluinoBallProbability]
genSeq.Pythia8.Commands += ["Next:showScaleAndVertex = on"]
genSeq.Pythia8.Commands += ["Check:nErrList = 2"]

# Merging configuration in case of extra jets in ME

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{t1,1000006}{t1~,-1000006}"]

# Make sure that the RHadrons are allowed by TestHepMC

if 'testSeq' in dir():
    extra_pdgids_f = open('extra_pdgids.txt','w')
    listOfPDGIDs = [1000512,1000522,1000612,1000622,1000632,1000642,1000652,1000991,1000993,1005211,1006113,1006211,1006213,1006223,1006311,1006313,1006321,1006323,1006333,1009111,1009113,1009211,1009213,1009223,1009311,1009313,1009321,1009323,1009333,1091114,1092112,1092114,1092212,1092214,1092224,1093114,1093122,1093214,1093224,1093314,1093324,1093334,-1000512,-1000522,-1000612,-1000622,-1000632,-1000642,-1000652,-1000991,-1000993,-1005211,-1006113,-1006211,-1006213,-1006223,-1006311,-1006313,-1006321,-1006323,-1006333,-1009111,-1009113,-1009211,-1009213,-1009223,-1009311,-1009313,-1009321,-1009323,-1009333,-1091114,-1092112,-1092114,-1092212,-1092214,-1092224,-1093114,-1093122,-1093214,-1093224,-1093314,-1093324,-1093334]
    extra_pdgids_f.write( "\n ".join([str(x) for x in listOfPDGIDs]) )
    extra_pdgids_f.close()
    testSeq.TestHepMC.G4ExtraWhiteFile='extra_pdgids.txt'
    testSeq.TestHepMC.MaxVtxDisp=1000000.# in mm

## Adding special config to metadata for use in sim step

evgenConfig.specialConfig = "CASE=stop;MASS={mass};LIFETIME={lifetime};MASSX={massx};SimulationJobOptions/preInclude.Rhadrons.py;".format(mass = masses['1000006'], lifetime = stopLifetime, massx = masses['1000022'])

