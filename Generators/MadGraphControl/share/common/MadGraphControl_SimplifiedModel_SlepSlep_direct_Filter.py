include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' ) 

def StringToFloat(s):
  if "p" in s:
    return float(s.replace("p", "."))
  return float(s)

# Extract job settings/masses etc.
gentype    = str(runArgs.jobConfig[0].split('_')[2])
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

# slepslept + 2 partons
process = '''
generate p p > el- el+ $ susystrong @1
add process p p > er- er+ $ susystrong @1
add process p p > mul- mul+ $ susystrong @1
add process p p > mur- mur+ $ susystrong @1
add process p p > ta1- ta1+ $ susystrong @1
add process p p > ta2- ta2+ $ susystrong @1
add process p p > el- el+ j $ susystrong @2
add process p p > er- er+ j $ susystrong @2
add process p p > mul- mul+ j $ susystrong @2
add process p p > mur- mur+ j $ susystrong @2
add process p p > ta1- ta1+ j $ susystrong @2
add process p p > ta2- ta2+ j $ susystrong @2
add process p p > el- el+ j j $ susystrong @3
add process p p > er- er+ j j $ susystrong @3
add process p p > mul- mul+ j j $ susystrong @3
add process p p > mur- mur+ j j $ susystrong @3
add process p p > ta1- ta1+ j j $ susystrong @3
add process p p > ta2- ta2+ j j $ susystrong @3
'''
njets = 2
evgenLog.info('Registered generation of slepton-pair production via direct decays; grid point ' + str(runArgs.runNumber) + ' decoded into mass point ' + str(mslep) + ' ' + str(mn1))

evgenConfig.contact = [ "Alaettin.Serhan.Mete@cern.ch" ]
evgenConfig.keywords += ['SUSY','slepton']
evgenConfig.description = 'Direct slepton-pair production in simplified model with direct decays, m_sleptonLR = %s GeV, m_N1 = %s GeV'%(mslep,mn1)

# Filter and event multiplier 
evt_multiplier = 3

if '2L8' in runArgs.jobConfig[0].split('_')[-1]:
    evgenLog.info('2lepton8 filter is applied')

    include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
    filtSeq.MultiElecMuTauFilter.NLeptons  = 2
    filtSeq.MultiElecMuTauFilter.MinPt = 8000.         # pt-cut on the lepton
    filtSeq.MultiElecMuTauFilter.MaxEta = 2.8          # stay away from MS 2.7 just in case
    filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0    # don't include hadronic taus

    filtSeq.Expression = "MultiElecMuTauFilter"

    # set evt_multiplier according to the mass splitting
    evt_multiplier = 5

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{el-,1000011}{el+,-1000011}"  ,
                                "Merging:Process = pp>{er-,2000011}{er+,-2000011}"  ,
                                "Merging:Process = pp>{mul-,1000013}{mul+,-1000013}",
                                "Merging:Process = pp>{mur-,2000013}{mur+,-2000013}",
                                "Merging:Process = pp>{ta1+,1000015}{ta1-,-1000015}",
                                "Merging:Process = pp>{ta2+,2000015}{ta2-,-2000015}"]
