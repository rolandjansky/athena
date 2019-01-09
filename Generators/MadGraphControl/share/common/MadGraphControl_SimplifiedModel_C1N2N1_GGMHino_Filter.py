include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' ) 
#### done by Arka, second time

def StringToFloat(s):
  if "p" in s:
    return float(s.replace("p", "."))
  return float(s)

# MC15.900001.MGPy8EG_A14N23LO_C1N2N1_GGMHino_95_noFilter.py
# C1/N2/N1 roughly mass denegenerate  mC1=mN2=mN1+1 GeV
masses['1000022'] = float(runArgs.jobConfig[0].split('_')[4])
masses['1000023'] = -(float(runArgs.jobConfig[0].split('_')[4])+1.0)
masses['1000024'] = float(runArgs.jobConfig[0].split('_')[4])+1.0

# Set Gen and Decay types 
gentype = str(runArgs.jobConfig[0].split('_')[2])
decaytype = str(runArgs.jobConfig[0].split('_')[3])

# x1+x1- + 2 partons inclusive
process = '''
generate p p > x1+ x1- / susystrong @1
add process p p > x1+ x1- j / susystrong @2
add process p p > x1+ x1- j j / susystrong @3
add process p p > x1+ n1 / susystrong @1
add process p p > x1+ n1 j / susystrong @2
add process p p > x1+ n1 j j / susystrong @3
add process p p > x1- n1 / susystrong @1
add process p p > x1- n1 j / susystrong @2
add process p p > x1- n1 j j / susystrong @3
add process p p > x1+ n2 / susystrong @1
add process p p > x1+ n2 j / susystrong @2
add process p p > x1+ n2 j j / susystrong @3
add process p p > x1- n2 / susystrong @1
add process p p > x1- n2 j / susystrong @2
add process p p > x1- n2 j j / susystrong @3
add process p p > n1 n2 / susystrong @1
add process p p > n1 n2 j / susystrong @2
add process p p > n1 n2 j j / susystrong @3
'''
njets = 1
evgenLog.info('Registered generation of ~hino pair production, decay to gravitino; grid point '+str(runArgs.runNumber)+' decoded into mass point ' + str(masses['1000024']) + ' ' + str(masses['1000022']))

evgenConfig.contact += [ "santra.arka@gmail.com" ]
evgenConfig.keywords += ['SUSY', 'chargino', 'neutralino', 'gravitino']
evgenConfig.description = '~hino pair production, decay to gravitino simplified model, m_C1N2 = %s GeV, m_N1 = %s GeV'%(masses['1000023'],masses['1000022'])

# Filter and event multiplier 
evt_multiplier = 3
### this is added to explicitly turn off systematic variations + push in the proper event_norm setting, this is to check the bug

extras['event_norm']='sum'
extras['use_syst']='F'

if '4L4' in runArgs.jobConfig[0].split('_')[-1]:
  evgenLog.info('4lepton4 filter is applied')
  
  include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
  filtSeq.MultiElecMuTauFilter.NLeptons  = 4
  filtSeq.MultiElecMuTauFilter.MinPt = 4000.         # pt-cut on the lepton
  filtSeq.MultiElecMuTauFilter.MaxEta = 2.8          # stay away from MS 2.7 just in case
  filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0    # don't include hadronic taus
  filtSeq.Expression = "MultiElecMuTauFilter"
  
  if masses['1000022'] >= 600 :
    evt_multiplier = 100
  elif masses['1000022'] >= 200 :
    evt_multiplier = 170
  elif masses['1000022'] >= 130 :
    evt_multiplier = 210
  else:
    evt_multiplier = 250

    
elif '2L8' in runArgs.jobConfig[0].split('_')[-1]:
  evgenLog.info('2lepton8 filter is applied')
  
  include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
  filtSeq.MultiElecMuTauFilter.NLeptons  = 2
  filtSeq.MultiElecMuTauFilter.MinPt = 8000.         # pt-cut on the lepton
  #filtSeq.MultiElecMuTauFilter.MinVisPtHadTau = 15000.   # pt-cut on the hadronic taus
  filtSeq.MultiElecMuTauFilter.MaxEta = 2.8          # stay away from MS 2.7 just in case
  filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0    # don't include hadronic taus
  filtSeq.Expression = "MultiElecMuTauFilter"
  
  evt_multiplier = 50

elif '3L4' in runArgs.jobConfig[0].split('_')[-1]:
  evgenLog.info('3lepton4 filter is applied')
  
  include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
  filtSeq.MultiElecMuTauFilter.NLeptons  = 3
  filtSeq.MultiElecMuTauFilter.MinPt = 4000.         # pt-cut on the lepton
  filtSeq.MultiElecMuTauFilter.MinVisPtHadTau = 15000.   # pt-cut on the hadronic taus
  filtSeq.MultiElecMuTauFilter.MaxEta = 2.8          # stay away from MS 2.7 just in case
  filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 1    # include hadronic taus
  filtSeq.Expression = "MultiElecMuTauFilter"
  
  evt_multiplier = 30

elif '4LT4' in runArgs.jobConfig[0].split('_')[-1]:
  evgenLog.info('4leptontau4 filter is applied')
  
  include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
  filtSeq.MultiElecMuTauFilter.NLeptons  = 4
  filtSeq.MultiElecMuTauFilter.MinPt = 4000.         # pt-cut on the lepton
  filtSeq.MultiElecMuTauFilter.MinVisPtHadTau = 15000.   # pt-cut on the hadronic taus
  filtSeq.MultiElecMuTauFilter.MaxEta = 2.8          # stay away from MS 2.7 just in case
  filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 1    # include hadronic taus
  filtSeq.Expression = "MultiElecMuTauFilter"

  if masses['1000022'] >= 500 :
    evt_multiplier = 40
  elif masses['1000022'] >= 400 :
    evt_multiplier = 45
  elif masses['1000022'] >= 300 :
    evt_multiplier = 50
  elif masses['1000022'] >= 200 :
    evt_multiplier = 80
  elif masses['1000022'] >= 130 :
    evt_multiplier = 120
  else:
    evt_multiplier = 100  

  minevents=2000     # only do 2k events at once in prod system
  
  
#### the code snippet needed for GMSB Higgsino NLSP signal point ####

elif '2L3' in runArgs.jobConfig[0].split('_')[-1]:
  
  evgenLog.info('2leptonMET50 filter is applied')
  
  include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
  filtSeq.MultiElecMuTauFilter.NLeptons  = 2         # need 2 leptons
  filtSeq.MultiElecMuTauFilter.MinPt = 3000.         # pt-cut on the lepton
  filtSeq.MultiElecMuTauFilter.MaxEta = 2.8          # stay away from MS 2.7 just in case
  filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0    # do not include hadronic taus
  
  include("GeneratorFilters/MissingEtFilter.py")
  filtSeq.MissingEtFilter.METCut = 50*GeV            # MET > 50 GeV
  
  filtSeq.Expression = "(MultiElecMuTauFilter) and (MissingEtFilter)"

  
  if masses['1000022'] >= 600 :
    evt_multiplier = 12
  elif masses['1000022'] >= 300 :
    evt_multiplier = 16
  elif masses['1000022'] >= 150 :
    evt_multiplier = 20
  else:
    evt_multiplier = 20
  
    
elif '2G15' in runArgs.jobConfig[0].split('_')[-1]:
  evgenLog.info('2photon15 filter is applied')
  
  include ( 'GeneratorFilters/DirectPhotonFilter.py' )
  filtSeq.DirectPhotonFilter.NPhotons  = 2
  filtSeq.DirectPhotonFilter.Ptmin = 15000.         # pt-cut on the photon
  filtSeq.DirectPhotonFilter.Etacut = 2.8          # eta cut on the photon
  filtSeq.Expression = "DirectPhotonFilter"
  
  evt_multiplier = 50


include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += [ "Merging:Process = guess" ]
    
    ### needed to use "guess" option of Pythia.
    if "UserHooks" in genSeq.Pythia8.__slots__.keys():
        genSeq.Pythia8.UserHooks += ['JetMergingaMCatNLO']
    else:
        genSeq.Pythia8.UserHook = 'JetMergingaMCatNLO'
                                 

