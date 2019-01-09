include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' ) 

def StringToFloat(s):
  ss=s.replace("ns","")
  if "p" in ss:
    return float(ss.replace("p", "."))
  return float(ss)

# Extract job settings/masses etc.
gentype    = str(runArgs.jobConfig[0].split('_')[2])
decaytype  = str(runArgs.jobConfig[0].split('_')[3])
mstau      = StringToFloat(runArgs.jobConfig[0].split('_')[4]) 
lifetime = str(runArgs.jobConfig[0].split('_')[6].split('.py')[0])
lifetime        = StringToFloat(lifetime)
mn1        = StringToFloat(runArgs.jobConfig[0].split('_')[5])

masses['1000011'] = mstau
masses['1000013'] = mstau
masses['1000015'] = mstau
masses['2000011'] = mstau
masses['2000013'] = mstau
masses['2000015'] = mstau

# slepton pairs + up to 2 extra partons
process = '''
generate p p > ta1- ta1+ $ susystrong @1
add process p p > ta2- ta2+ $ susystrong @1
add process p p > ta1- ta1+ j $ susystrong @2
add process p p > ta2- ta2+ j $ susystrong @2
add process p p > ta1- ta1+ j j $ susystrong @3
add process p p > ta2- ta2+ j j $ susystrong @3
'''
njets = 2
evgenLog.info('Registered generation of stau-pair production via direct decays; grid point ' + str(runArgs.runNumber) + ' decoded into mass point ' + str(mstau) + ' with lifetime ' + str(lifetime))

evgenConfig.contact = [ "emma.sian.kuwertz@cern.ch" ]
evgenConfig.keywords += ['SUSY','stau','longLived','gravitino','simplifiedModel']
evgenConfig.description = 'Direct stau-pair production in simplified model with non-prompt decays, m_stauLR = %s GeV, lifetime = %s'%(mstau,lifetime)

if lifetime != 0:
  # Gravitino mass set to 10**-7 GeV
  evgenConfig.specialConfig = 'GMSBStau=%s*GeV;GMSBGravitino=%s*GeV;GMSBStauTime=%s*ns;preInclude=SimulationJobOptions/preInclude.SleptonsLLP.py' % (mstau,0.0000001,lifetime)

# Filter and event multiplier 
evt_multiplier = 3

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = guess"]
    if "UserHooks" in genSeq.Pythia8.__slots__.keys(): 
      genSeq.Pythia8.UserHooks += ['JetMergingaMCatNLO'] 
    else: 
      genSeq.Pythia8.UserHook = 'JetMergingaMCatNLO'

bonus_file = open('pdg_extras.dat','w')
bonus_file.write( '1000015 Stau1 %s (MeV/c) fermion Stau -1\n'%(str(mstau)))
bonus_file.write( '2000015 Stau2 %s (MeV/c) fermion Stau -1\n'%(str(mstau)))
bonus_file.write( '-1000015 Anti-stau1 %s (MeV/c) fermion Stau 1\n'%(str(mstau)))
bonus_file.write( '-2000015 Anti-stau2 %s (MeV/c) fermion Stau 1\n'%(str(mstau)))
bonus_file.close()

testSeq.TestHepMC.G4ExtraWhiteFile='pdg_extras.dat'

import os
os.system("get_files %s" % testSeq.TestHepMC.G4ExtraWhiteFile)
