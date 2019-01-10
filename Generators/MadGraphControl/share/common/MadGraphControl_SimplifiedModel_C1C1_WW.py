include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' ) 

def StringToFloat(s):
  if "p" in s:
    return float(s.replace("p", "."))
  return float(s)

# C1/N2 denegenerate  
masses['1000023'] = StringToFloat(runArgs.jobConfig[0].split('_')[4])
masses['1000024'] = StringToFloat(runArgs.jobConfig[0].split('_')[4])
masses['1000022'] = StringToFloat(runArgs.jobConfig[0].split('_')[5])
extras['event_norm']='sum'
extras['use_syst']='F'


# Set Gen and Decay types 
gentype = str(runArgs.jobConfig[0].split('_')[2])
decaytype = str(runArgs.jobConfig[0].split('_')[3])

# x1+x1- + 2 partons inclusive
process = '''
generate p p > x1+ x1- $ susystrong @1
add process p p > x1+ x1- j $ susystrong @2
add process p p > x1+ x1- j j $ susystrong @3
'''
njets = 2
evgenLog.info('Registered generation of ~chi1+/~chi1- production, decay via WW; grid point '+str(runArgs.runNumber)+' decoded into mass point ' + str(masses['1000024']) + ' ' + str(masses['1000022']))

evgenConfig.contact = [ "Alaettin.Serhan.Mete@cern.ch", "Valentina.Tudorache@cern.ch"]
evgenConfig.keywords += ['SUSY', 'gaugino', 'chargino', 'slepton', 'sneutrino']
evgenConfig.description = '~chi1+/~chi1- production, decay via WW in simplified model, m_C1N2 = %s GeV, m_N1 = %s GeV'%(masses['1000023'],masses['1000022'])

# Filter and event multiplier 
evt_multiplier = 50

if '2L8' in runArgs.jobConfig[0].split('_')[-1]:
    evgenLog.info('2lepton8 filter is applied')

    include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
    filtSeq.MultiElecMuTauFilter.NLeptons  = 2
    filtSeq.MultiElecMuTauFilter.MinPt = 8000.         # pt-cut on the lepton
    filtSeq.MultiElecMuTauFilter.MaxEta = 2.8          # stay away from MS 2.7 just in case
    filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0    # don't include hadronic taus

    filtSeq.Expression = "MultiElecMuTauFilter"

    if abs(masses['1000024'] - masses['1000022']) <= 25:
        evgenLog.info('changing event multiplier to 600')
        evt_multiplier = 600
    elif abs(masses['1000024'] - masses['1000022']) <= 100:
        evt_multiplier = 80
        evgenLog.info('changing event multiplier to 80')

    


elif '1L' in runArgs.jobConfig[0].split('_')[-1]:
    evgenLog.info('1lepton filter is applied')   
    
    include ( 'GeneratorFilters/DecaysFinalStateFilter.py' ) # use to pick semileptonic WW decays
    
    DecaysFinalStateFilter.PDGAllowedParents = [ 24, -24 ]     
    DecaysFinalStateFilter.NQuarks = 2
    DecaysFinalStateFilter.NChargedLeptons= 1
    DecaysFinalStateFilter.NNeutrinos=1
    DecaysFinalStateFilter.Expression = "DecaysFinalStateFilter"
    
    if masses['1000022'] <0.5:masses['1000022']=0.5 
    
    if abs(masses['1000024'] - masses['1000022']) > 100:
        evt_multiplier = 10
        evgenLog.info('changing event multiplier to 10')
    
    



	

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = guess"] # New feature in Pythia 8.230+
    if "UserHooks" in genSeq.Pythia8.__slots__.keys():
      genSeq.Pythia8.UserHooks += ['JetMergingaMCatNLO']
    else:
      genSeq.Pythia8.UserHook = 'JetMergingaMCatNLO'
#    genSeq.Pythia8.Commands += [ "Merging:Process = pp>{x1+,1000024}{x1-,-1000024}",
#                                 "1000024:spinType = 1" ]
