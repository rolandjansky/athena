include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' ) 

def MassToFloat(s):
  if "p" in s:
    return float(s.replace("p", "."))
  return float(s)

splits = runArgs.jobConfig[0].split('_')

masses['1000024'] = MassToFloat(splits[4])
masses['1000023'] = MassToFloat(splits[4])
masses['1000022'] = MassToFloat(splits[5].split('.')[0])
splitting = 0.5
if len(splits) > 7:
  splitting = MassToFloat(splits[-2].split('.')[0])
  evgenLog.info("Using mass splitting of %f for mslep" % splitting)
mslep = ( masses['1000024'] - masses['1000022'] )*splitting + masses['1000022']
evgenLog.info("  Stau / sneutrino mass is %f GeV" % mslep)
masses['1000015'] = mslep
masses['1000016'] = mslep
# NOTE: stau2 is not used (decoupled as in Run-1)
if masses['1000022']<0.5: masses['1000022']=0.5
gentype = str(splits[2])
decaytype = str(splits[3])
process = '''
generate p p > x1+ x1- $ susystrong @1
add process p p > x1+ x1- j $ susystrong @2
add process p p > x1+ x1- j j $ susystrong @3
'''
njets = 2
evgenLog.info('Registered generation of ~chi1+/~chi1- production, decay via stau; grid point '+str(runArgs.runNumber)+' decoded into mass point ' + str(masses['1000024']) + ' ' + str(masses['1000022']))

evgenConfig.contact  = [ "mann@cern.ch" ]
evgenConfig.keywords += ['SUSY', 'gaugino', 'chargino', 'stau']
evgenConfig.description = '~chi1+/~chi1- production, decay via stau in simplified model, m_C1 = %s GeV, m_stau = %s GeV, m_N1 = %s GeV' % (masses['1000024'],masses['1000015'],masses['1000022'])

# need more events from MG due to filter - this needs to be set before MadGraphControl_SimplifiedModelPostInclude.py is run
# evt_multiplier = 3 is only sufficient for large mass splittings
evt_multiplier = 20

extras['event_norm']='sum'
extras['use_syst']='F'

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += [ "Merging:Process = pp>{x1+,1000024}{x1-,-1000024}",
                                 "1000024:spinType = 1" ]


#--------------------------------------------------------------
# Lepton filter
#--------------------------------------------------------------
if '2L1T' in runArgs.jobConfig[0].split("_")[-1]:
  evgenLog.info('2L1T filter is applied')
  from GeneratorFilters.GeneratorFiltersConf import MultiElecMuTauFilter
  filtSeq += MultiElecMuTauFilter("DileptonFilter")
  filtSeq += MultiElecMuTauFilter("TauFilter")
  
  MultiElecMuTauFilter1 = filtSeq.DileptonFilter
  MultiElecMuTauFilter1.NLeptons  = 2
  MultiElecMuTauFilter1.MinPt = 5000.
  MultiElecMuTauFilter1.MaxEta = 2.7
  MultiElecMuTauFilter1.MinVisPtHadTau = 15000. # pt-cut on the visible hadronic tau
  MultiElecMuTauFilter1.IncludeHadTaus = 1      # include hadronic taus
  
  MultiElecMuTauFilter2 = filtSeq.TauFilter
  MultiElecMuTauFilter2.NLeptons  = 1
  MultiElecMuTauFilter2.MinPt = 1e10
  MultiElecMuTauFilter2.MaxEta = 2.7
  MultiElecMuTauFilter2.MinVisPtHadTau = 15000. # pt-cut on the visible hadronic tau
  MultiElecMuTauFilter2.IncludeHadTaus = 1      # include hadronic taus
  filtSeq.Expression = "(DileptonFilter and TauFilter)"

