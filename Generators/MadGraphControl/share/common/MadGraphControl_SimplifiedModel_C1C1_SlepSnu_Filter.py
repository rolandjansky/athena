include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' ) 

def StringToFloat(s):
  if "p" in s:
    return float(s.replace("p", "."))
  return float(s)

# C1/N2 denegenerate  
masses['1000023'] = StringToFloat(runArgs.jobConfig[0].split('_')[5])
masses['1000024'] = StringToFloat(runArgs.jobConfig[0].split('_')[5])
masses['1000022'] = StringToFloat(runArgs.jobConfig[0].split('_')[6])

# Slepton/Sneutrino degenerate and between C1/N2 and N1
xvalue = StringToFloat(runArgs.jobConfig[0].split('_')[4].split('x')[1])
mslep = xvalue*masses['1000024'] + (1-xvalue)*masses['1000022']
masses['1000011'] = mslep
masses['1000012'] = mslep
masses['1000013'] = mslep
masses['1000014'] = mslep
masses['1000015'] = mslep
masses['1000016'] = mslep

# Set Gen and Decay types 
gentype = str(runArgs.jobConfig[0].split('_')[2])
decaytype = str(runArgs.jobConfig[0].split('_')[3])

# Lines to prevent inconsistent weights
extras['event_norm']='sum'
extras['use_syst']='F'

# x1+x1- + 2 partons inclusive
process = '''
generate p p > x1+ x1- $ susystrong @1
add process p p > x1+ x1- j $ susystrong @2
add process p p > x1+ x1- j j $ susystrong @3
'''
njets = 2
evgenLog.info('Registered generation of ~chi1+/~chi1- production, decay via slepton/sneutrino; grid point '+str(runArgs.runNumber)+' decoded into mass point ' + str(masses['1000024']) + ' ' + str(masses['1000022']))

evgenConfig.contact = [ "francesco.giuseppe.gravili@cern.ch" ]
evgenConfig.keywords += ['SUSY', 'gaugino', 'chargino', 'slepton', 'sneutrino']
evgenConfig.description = '~chi1+/~chi1- production, decay via slepton/sneutrino in simplified model, m_C1N2 = %s GeV, m_N1 = %s GeV, x = %s'%(masses['1000023'],masses['1000022'],xvalue)

# Filter and event multiplier 
evt_multiplier = 5 

if '2L8and1J80' in runArgs.jobConfig[0].split('_')[-1]:
    evgenLog.info('2lepton8 and Jet80 filter is applied')

    include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
    filtSeq.MultiElecMuTauFilter.NLeptons  = 2
    filtSeq.MultiElecMuTauFilter.MinPt = 8000.         # pt-cut on the lepton
    filtSeq.MultiElecMuTauFilter.MaxEta = 2.8          # stay away from MS 2.7 just in case
    filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0    # don't include hadronic taus

    include( 'GeneratorFilters/AntiKt4TruthWZJets.py')
    include( 'GeneratorFilters/JetFilter_Fragment.py')
    filtSeq.QCDTruthJetFilter.MinPt  = 80000.          # pt-cut on the jet
    filtSeq.QCDTruthJetFilter.MaxEta = 5.0             # eta-cut on the jet
    filtSeq.QCDTruthJetFilter.TruthJetContainer = "AntiKt4TruthWZJets"

    filtSeq.Expression = "MultiElecMuTauFilter and QCDTruthJetFilter"

    evt_multiplier = 25

elif '2L8' in runArgs.jobConfig[0].split('_')[-1]:
    evgenLog.info('2lepton8 filter is applied')

    include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
    filtSeq.MultiElecMuTauFilter.NLeptons  = 2
    filtSeq.MultiElecMuTauFilter.MinPt = 8000.         # pt-cut on the lepton
    filtSeq.MultiElecMuTauFilter.MaxEta = 2.8          # stay away from MS 2.7 just in case
    filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0    # don't include hadronic taus

    filtSeq.Expression = "MultiElecMuTauFilter"

    if abs(masses['1000024'] - masses['1000022']) <= 25:
        evt_multiplier = 10
    elif abs(masses['1000024'] - masses['1000022']) <= 100:
        evt_multiplier = 5

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += [ "Merging:Process = pp>{x1+,1000024}{x1-,-1000024}",
                                 "1000024:spinType = 1" ]
