include ('MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py')

import re
plist = re.sub(r'.+MGPy8EG_A14N23LO_', '', runArgs.jobConfig[0].rstrip('.py')).split('_')
#masses['1000021'] = float(plist[2])  # gluino mass
#masses['1000022'] = float(plist[3])  # LPS (wino) mass
masses['1000021'] = float(plist[3])  # gluino mass
masses['1000022'] = float(plist[4])  # LPS (wino) mass
deltaM = 0.16  # m_C1 - m_N1 in GeV
masses['1000024'] = masses['1000022'] + deltaM  # C1 mass
gentype = str(plist[0])
decaytype = str(plist[1])
lifetime = 1.0  # ns
#lifetime = 4.0  # ns

process = '''
generate p p > go go $ susysq susysq~ @1
add process p p > go go j $ susysq susysq~ @2
add process p p > go go j j $ susysq susysq~ @3
'''
#process = '''
#generate p p > go go $ susysq susysq~, (go > x1+ d u~ ), (go > n1 u u~) @1
#add process p p > go go $ susysq susysq~, (go > x1- u d~ ), (go > n1 u u~) @2
#add process p p > go go $ susysq susysq~, (go > x1+ d u~ ), (go > x1- u d~) @3
#add process p p > go go j $ susysq susysq~, (go > x1+ d u~ ), (go > n1 u u~) @4
#add process p p > go go j $ susysq susysq~, (go > x1- u d~ ), (go > n1 u u~) @5
#add process p p > go go j $ susysq susysq~, (go > x1+ d u~ ), (go > x1- u d~) @6
#add process p p > go go j j $ susysq susysq~, (go > x1+ d u~ ), (go > n1 u u~) @7
#add process p p > go go j j $ susysq susysq~, (go > x1- u d~ ), (go > n1 u u~) @8
#add process p p > go go j j $ susysq susysq~, (go > x1+ d u~ ), (go > x1- u d~) @9
#'''
njets = 2
evt_multiplier = 4
evgenLog.info('Registered generation of gluino grid '+str(runArgs.runNumber))

#evgenConfig.contact  = ["shimmpei.yamamoto@cern.ch"]
evgenConfig.contact  = ["chihiro.kozakai@cern.ch"]
evgenConfig.keywords += ['simplifiedModel','gluino', 'chargino', 'longLived']
evgenConfig.description = 'gluino production, glu->qq+wino in simplified model, m_glu = %s GeV, m_wino = %s GeV, lifetime = %s ns' % (masses['1000021'], masses['1000022'], lifetime)

# AGENE-1542: disable syscalc and set event_norm explicitly
extras['event_norm']='sum'
extras['use_syst']='F'

include ('MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py')

print "---"
print runArgs.gentype
print runArgs.decaytype
print runArgs.qcut
print "---"

if lifetime != 0:
    #include('MadGraphControl/SUSYMetadata.py')
    ## remove chargino1 decay from slha file
    #remove_decay('param_card.dat', '1000024')
    evgenConfig.specialConfig = 'AMSBC1Mass=%s*GeV;AMSBN1Mass=%s*GeV;AMSBC1Lifetime=%s*ns;preInclude=SimulationJobOptions/preInclude.AMSB.py' % (masses['1000024'], masses['1000022'], lifetime)

genSeq.Pythia8.Commands += ["Init:showAllParticleData = on",
                            "Next:numberShowLHA = 10",
                            "Next:numberShowEvent = 10",
                            "1000022:all = Chi Chi~ 1 0 0 %s" % (masses['1000022'])]


if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = guess"]
    if "UserHooks" in genSeq.Pythia8.__slots__.keys():
        genSeq.Pythia8.UserHooks += ['JetMergingaMCatNLO']
    else:
        genSeq.Pythia8.UserHook = 'JetMergingaMCatNLO'
#    genSeq.Pythia8.Commands += ["Merging:Process = pp>{go,1000021}{go,1000021}"]

bonus_file = open('pdg_extras.dat','w')
bonus_file.write( '1000024 Chargino 100.0 (MeV/c) boson Chargino 1\n')
bonus_file.write( '-1000024 Anti-chargino 100.0 (MeV/c) boson Chargino -1\n')
bonus_file.close()
 
testSeq.TestHepMC.G4ExtraWhiteFile='pdg_extras.dat'

import os
os.system("get_files %s" % testSeq.TestHepMC.G4ExtraWhiteFile)


