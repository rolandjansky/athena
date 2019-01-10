include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' ) 

def StringToFloat(s):
  if "p" in s:
    return float(s.replace("p", "."))
  return float(s)

# Extract job settings/masses etc.
gentype    = "SlepSlep" # determines xqcut via MadGraphUtils.py from masses["1000011"]
decaytype  = str(runArgs.jobConfig[0].split('_')[3])
mslep      = StringToFloat(runArgs.jobConfig[0].split('_')[4]) 
mn1        = StringToFloat(runArgs.jobConfig[0].split('_')[5].split('.')[0])
masses['1000011'] = mslep
masses['1000013'] = mslep
masses['1000015'] = mslep
masses['2000011'] = mslep
masses['2000013'] = mslep
masses['2000015'] = mslep
masses['1000022'] = mn1

# Stau-stau + 2 partons
process = '''
generate    p p > ta1- ta1+     $ susystrong @1
add process p p > ta2- ta2+     $ susystrong @1
add process p p > ta1- ta1+ j   $ susystrong @2
add process p p > ta2- ta2+ j   $ susystrong @2
add process p p > ta1- ta1+ j j $ susystrong @3
add process p p > ta2- ta2+ j j $ susystrong @3
'''
njets = 2

evgenLog.info('Registered generation of stau-pair production; grid point ' + str(runArgs.runNumber) + ' decoded into mass point ' + str(mslep) + ' ' + str(mn1))

evgenConfig.contact = [ "mann@cern.ch" ]
evgenConfig.keywords += ['SUSY', 'stau']
evgenConfig.description = 'Direct stau-pair production in simplified model, m_stauLR = %s GeV, m_N1 = %s GeV' % (mslep, mn1)

# Filter and event multiplier 
evt_multiplier = 2

if '2L8' in runArgs.jobConfig[0].split('_')[-1]:

    evgenLog.info('2lepton8 filter is applied')

    include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
    filtSeq.MultiElecMuTauFilter.NLeptons  = 2
    filtSeq.MultiElecMuTauFilter.MinPt = 8000.         # pt-cut on the lepton
    filtSeq.MultiElecMuTauFilter.MaxEta = 2.8          # stay away from MS 2.7 just in case
    filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 1    # include hadronic taus

    filtSeq.Expression = "MultiElecMuTauFilter"

    # set higher evt_multiplier when using filter
    evt_multiplier = 8

else:
    evgenLog.info('2lepton8 filter is NOT applied')

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{ta1+,1000015}{ta1-,-1000015}",
                                "Merging:Process = pp>{ta2+,2000015}{ta2-,-2000015}"]
