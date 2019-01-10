include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

def MassToFloat(s):
  if "p" in s:
    return float(s.replace("p", "."))
  return float(s)

splitConfig = runArgs.jobConfig[0].rstrip('.py').split('_')

#C1/N2 degenerate
masses['1000024'] = MassToFloat(splitConfig[4])
masses['1000023'] = MassToFloat(splitConfig[4])
masses['1000022'] = MassToFloat(splitConfig[5])

#sleptons mass half way between C1/N2 and N1
xsplit = 0.5

#get slepton mass splitting from name
try:
    xsplit = MassToFloat(splitConfig[6])
except ValueError:
    pass

if xsplit<0 or xsplit>1:
    raise ValueError('Mass spliting parameter should be within [0,1]. Current value: '+str(xsplit))

mslep = masses['1000024']*xsplit+masses['1000022']*(1-xsplit)

masses['1000011'] = mslep
masses['1000012'] = mslep
masses['1000013'] = mslep
masses['1000014'] = mslep
masses['1000015'] = mslep
masses['1000016'] = mslep
if masses['1000022']<0.5: masses['1000022']=0.5

#will be C1N2
gentype = splitConfig[2]
#will be Slep
decaytype = splitConfig[3]

process = '''
generate p p > x1+ n2 $ susystrong @1
add process p p > x1- n2 $ susystrong @1
add process p p > x1+ n2 j $ susystrong @2
add process p p > x1- n2 j $ susystrong @2
add process p p > x1+ n2 j j $ susystrong @3
add process p p > x1- n2 j j $ susystrong @3
'''
njets = 2
evgenLog.info('Registered generation of ~chi1+/- ~chi20 production, decay via slepton; grid point '+str(runArgs.runNumber)+' decoded into mass point ' + str(masses['1000024']) + ' ' + str(masses['1000022']) + ' slep mass '+str(mslep))

evgenConfig.contact  = [ "jmiguens@cern.ch" ]
evgenConfig.keywords += ['gaugino', 'chargino', 'neutralino', 'slepton']
evgenConfig.description = '~chi1+/- ~chi20 production, decay via slepton in simplified model, m_C1N2 = %s GeV, m_slep = %s GeV, m_N1 = %s GeV'%(masses['1000024'],masses['1000011'],masses['1000022'])

#--------------------------------------------------------------
# add some filter here
#--------------------------------------------------------------

# need more events from MG due to filter - this needs to be set before MadGraphControl_SimplifiedModelPostInclude.py is run (it's set at 2 there)
# 3 is only sufficient for large mass splittings
evt_multiplier = 3

if '2L5andJ20' in splitConfig[-1]:
    evgenLog.info('2leptons5 and jet20 filter')

    include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
    filtSeq.MultiElecMuTauFilter.NLeptons  = 2
    filtSeq.MultiElecMuTauFilter.MinPt = 5000.         # low pt-cut on the lepton
    filtSeq.MultiElecMuTauFilter.MaxEta = 2.8          # stay away from MS 2.7 just in case
    filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0    # don't include hadronic taus

    include( 'GeneratorFilters/AntiKt4TruthWZJets.py')
    include( 'GeneratorFilters/JetFilter_Fragment.py')
    filtSeq.QCDTruthJetFilter.MinPt  = 20000.          # low pt-cut on the jet
    filtSeq.QCDTruthJetFilter.MaxEta = 5.0             # eta-cut on the jet
    filtSeq.QCDTruthJetFilter.TruthJetContainer = "AntiKt4TruthWZJets"

    filtSeq.Expression = "MultiElecMuTauFilter and QCDTruthJetFilter"

    evt_multiplier = 15

elif '2L5' in splitConfig[-1]:
    evgenLog.info('2leptons5 filter')

    include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
    filtSeq.MultiElecMuTauFilter.NLeptons  = 2
    filtSeq.MultiElecMuTauFilter.MinPt = 5000.         # low pt-cut on the lepton
    filtSeq.MultiElecMuTauFilter.MaxEta = 2.8          # stay away from MS 2.7 just in case
    filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0    # don't include hadronic taus

    filtSeq.Expression = "MultiElecMuTauFilter"

    evt_multiplier = 6
    if xsplit>0.8 and masses['1000024'] - masses['1000022'] < 100:
        evt_multiplier *= 100./(masses['1000024'] - masses['1000022'])

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += [ "Merging:Process = pp>{x1+,1000024}{x1-,-1000024}{n2,1000023}",
                                 "1000024:spinType = 1",
                                 "1000023:spinType = 1" ]
