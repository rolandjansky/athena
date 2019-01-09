include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' ) 

def StringToFloat(s):
  ss=s.replace("ns","")
  if "p" in ss:
    return float(ss.replace("p", "."))
  return float(ss)

# Extract job settings/masses etc.
gentype    = str(runArgs.jobConfig[0].split('_')[2])
decaytype  = str(runArgs.jobConfig[0].split('_')[3])
mslep      = StringToFloat(runArgs.jobConfig[0].split('_')[4]) 
lifetime = str(runArgs.jobConfig[0].split('_')[6].split('.py')[0])
lifetime        = StringToFloat(lifetime)
print "LIFETIME  ",lifetime
mn1        = StringToFloat(runArgs.jobConfig[0].split('_')[5])

masses['1000011'] = mslep
masses['1000013'] = mslep
masses['1000015'] = mslep
masses['2000011'] = mslep
masses['2000013'] = mslep
masses['2000015'] = mslep

# slepton pairs + up to 2 extra partons
process = '''
generate p p > el- el+ $ susystrong @1
add process p p > er- er+ $ susystrong @1
add process p p > mul- mul+ $ susystrong @1
add process p p > mur- mur+ $ susystrong @1
add process p p > el- el+ j $ susystrong @2
add process p p > er- er+ j $ susystrong @2
add process p p > mul- mul+ j $ susystrong @2
add process p p > mur- mur+ j $ susystrong @2
add process p p > el- el+ j j $ susystrong @3
add process p p > er- er+ j j $ susystrong @3
add process p p > mul- mul+ j j $ susystrong @3
add process p p > mur- mur+ j j $ susystrong @3
'''
njets = 2
evgenLog.info('Registered generation of slepton-pair production via direct decays; grid point ' + str(runArgs.runNumber) + ' decoded into mass point ' + str(mslep) + ' with lifetime ' + str(lifetime))

evgenConfig.contact = [ "emma.sian.kuwertz@cern.ch" ]
evgenConfig.keywords += ['SUSY','slepton','longLived','gravitino','simplifiedModel']
evgenConfig.description = 'Direct slepton-pair production in simplified model with non-prompt decays, m_sleptonLR = %s GeV, lifetime = %s'%(mslep,lifetime)

if lifetime != 0:
  evgenConfig.specialConfig = 'GMSBSlepton=%s*GeV;GMSBGravitino=%s*GeV;GMSBSleptonTime=%s*ns;preInclude=SimulationJobOptions/preInclude.SleptonsLLP.py' % (mslep,0.0000001,lifetime)

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
bonus_file.write( '1000011 SelectronL %s (MeV/c) fermion Selectron -1\n'%(str(mslep)))
bonus_file.write( '2000011 SelectronR %s (MeV/c) fermion Selectron -1\n'%(str(mslep)))
bonus_file.write( '1000013 SmuonL %s (MeV/c) fermion Smuon -1\n'%(str(mslep)))
bonus_file.write( '2000013 SmuonR %s (MeV/c) fermion Smuon -1\n'%(str(mslep)))
bonus_file.write( '-1000011 Anti-selectronL %s (MeV/c) fermion Selectron 1\n'%(str(mslep)))
bonus_file.write( '-2000011 Anti-selectronR %s (MeV/c) fermion Selectron 1\n'%(str(mslep)))
bonus_file.write( '-1000013 Anti-smuonL %s (MeV/c) fermion Smuon 1\n'%(str(mslep)))
bonus_file.write( '-2000013 Anti-smuonR %s (MeV/c) fermion Smuon 1\n'%(str(mslep)))
bonus_file.close()

testSeq.TestHepMC.G4ExtraWhiteFile='pdg_extras.dat'

import os
os.system("get_files %s" % testSeq.TestHepMC.G4ExtraWhiteFile)
