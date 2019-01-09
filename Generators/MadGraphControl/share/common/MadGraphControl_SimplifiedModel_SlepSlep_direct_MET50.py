include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

def StringToFloat(s):
  if "p" in s:
    return float(s.replace("p", "."))
  return float(s)

gentype    = str(runArgs.jobConfig[0].split('_')[2])
decaytype  = str(runArgs.jobConfig[0].split('_')[3])
mslep      = StringToFloat(runArgs.jobConfig[0].split('_')[4]) 
mn1        = StringToFloat(runArgs.jobConfig[0].split('_')[5].split('.')[0])

# Set masses in the parameter card
masses['1000011'] = mslep # selectron_L
masses['2000011'] = mslep # selectron_R

masses['1000013'] = mslep # smuon_L
masses['2000013'] = mslep # smuon_1

masses['1000015'] = mslep # stau_1
masses['2000015'] = mslep # stau_2

masses['1000022'] = mn1   # N1_LSP

# Specify MadGraph process
process = '''
define slep = el- el+ er- er+ mul- mul+ mur- mur+ 
generate p p > slep slep $ susystrong @1
add process p p > slep slep j $ susystrong @2
add process p p > slep slep j j $ susystrong @3
''' 

njets = 2
pythia = genSeq.Pythia8

#--------------------------------------------------------------
# Metadata for log
#--------------------------------------------------------------
evgenLog.info('p p > slep slep production. ')
evgenConfig.contact  = [ "jesse.liu@cern.ch" ]
evgenConfig.keywords += ['SUSY','slepton']
evgenConfig.description = 'Direct slepton pair production (selectrons & smuons, LH & RH) in simplified model with direct decays, m_sleptonLR = %s GeV, m_N1 = %s GeV'%(mslep,mn1)

#--------------------------------------------------------------
# Use lower xqcut for compressed points more reliant on ISR
#--------------------------------------------------------------
deltaM = mslep - mn1
xqcut = max( 15, float(deltaM) / 4. )
evgenLog.info('Signal has mass splitting of {0} GeV, use xqcut {1}'.format( deltaM, xqcut ) )

#--------------------------------------------------------------
# MET filters
#--------------------------------------------------------------

# default evt_multiplier is 2
# if filter efficiency is 1/2, set evt_multiplier = 4

if 'MET' in runArgs.jobConfig[0]:
  from GeneratorFilters.GeneratorFiltersConf import MissingEtFilter 
  filtSeq += MissingEtFilter("MissingEtFilter")
  filtSeq.Expression = 'MissingEtFilter'
if 'MET50' in runArgs.jobConfig[0]:
  evt_multiplier = 25
  evgenLog.info('MET50 filter is applied')
  include ( 'GeneratorFilters/MissingEtFilter.py' )
  filtSeq.MissingEtFilter.METCut = 50*GeV
if 'MET75' in runArgs.jobConfig[0]:
  evt_multiplier = 40
  evgenLog.info('MET75 filter is applied')
  include ( 'GeneratorFilters/MissingEtFilter.py' )
  filtSeq.MissingEtFilter.METCut = 75*GeV

#--------------------------------------------------------------
# Lepton filters
#--------------------------------------------------------------
if '2L2' in runArgs.jobConfig[0]:
  evgenLog.info('2L2 filter is applied')
  from GeneratorFilters.GeneratorFiltersConf import MultiElecMuTauFilter
  filtSeq += MultiElecMuTauFilter("MultiElecMuTauFilter2L2")
  ElecMuTauFilter2L2 = filtSeq.MultiElecMuTauFilter2L2
  ElecMuTauFilter2L2.MinPt = 2000.
  ElecMuTauFilter2L2.MaxEta = 2.8
  ElecMuTauFilter2L2.NLeptons = 2
  ElecMuTauFilter2L2.IncludeHadTaus = 0    # don't include hadronic taus

  # Raise the event multipliers for very compressed splittings
  if deltaM <= 5:
    evt_multiplier = 100
  if deltaM <= 1:
    evt_multiplier = 600
    #evgenConfig.minevents = 200

  filtSeq.Expression += ' and MultiElecMuTauFilter2L2'

if '2L8' in runArgs.jobConfig[0]:
  from GeneratorFilters.GeneratorFiltersConf import MultiElecMuTauFilter
  filtSeq += MultiElecMuTauFilter("MultiElecMuTauFilter2L8")
  ElecMuTauFilter2L8 = filtSeq.MultiElecMuTauFilter2L8
  ElecMuTauFilter2L8.MinPt = 8000.
  ElecMuTauFilter2L8.MaxEta = 2.8
  ElecMuTauFilter2L8.NLeptons = 2
  ElecMuTauFilter2L8.IncludeHadTaus = 0    # don't include hadronic taus
  if '2L8OR2L2MET75' in runArgs.jobConfig[0]:
  # Do logical or of (MET75 and 2L2) or (2L8)
    filtSeq.Expression = '( MissingEtFilter and MultiElecMuTauFilter2L2 ) or MultiElecMuTauFilter2L8'
    evgenLog.info('OR of 2L8 filter is applied')
  else:
    filtSeq.Expression = 'MultiElecMuTauFilter2L8'
    evgenLog.info('2L8 filter is applied')
    evt_multiplier = 15

evgenLog.info('Event multiplier applied: {0}'.format(evt_multiplier) )

include('MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py')

# ------------------------------------------------------------------
# Ensure event weights behave as expected
# See https://its.cern.ch/jira/browse/AGENE-1542
evgenLog.info('Setting event_nom to sum, use_syst to F')
extras['event_norm']='sum'
extras['use_syst']='F'
# ------------------------------------------------------------------

# ------------------------------------------------------------------
# Manual interventions to generate < 10000 events
if mslep >= 100. and deltaM <= 1:
  evgenConfig.minevents = 2000
if mslep >= 150. and deltaM <= 5:
  evgenConfig.minevents = 2000
if mslep >= 175. and deltaM <= 30:
  evgenConfig.minevents = 2000
if mslep >= 200. and deltaM <= 40:
  evgenConfig.minevents = 2000
if mslep >= 225.:
  evgenConfig.minevents = 2000
if deltaM < 1:
  evgenConfig.minevents = 500

if evgenConfig.minevents:
  evgenLog.info('evgenConfig.minevents: {0}'.format(evgenConfig.minevents) )
# ------------------------------------------------------------------

#--------------------------------------------------------------
# Jet-parton matching in Pythia8
#--------------------------------------------------------------
if njets>0:
  evgenLog.info('Performing the Pythia8 guess CKKW-L merging feature https://its.cern.ch/jira/browse/AGENE-1511')
  genSeq.Pythia8.Commands += ["Merging:Process = guess"]
  if "UserHooks" in genSeq.Pythia8.__slots__.keys():
    genSeq.Pythia8.UserHooks += ['JetMergingaMCatNLO']
  else:
    genSeq.Pythia8.UserHook = 'JetMergingaMCatNLO'
