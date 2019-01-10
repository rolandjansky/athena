include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

splitConfig = runArgs.jobConfig[0].rstrip('.py').split('_')

masses['1000024'] = float(splitConfig[4]) # C1 mass

gentype = str(splitConfig[2])
decaytype = str(splitConfig[3])

process = '''
   generate p p > x1+ x1-     / susystrong @1
add process p p > x1+ x1- j   / susystrong @2
add process p p > x1+ x1- j j / susystrong @3
'''
njets = 2
evgenLog.info('Registered generation of ~chi1+/~chi1- production, decay via Zl,Hl,or Wv; grid point '+str(runArgs.runNumber))

evgenConfig.contact  = [ "leigh.schaefer@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel','SUSY','chargino','RPV']
evgenConfig.description = '~chi1+/~chi1- production, decay via Zl,Hl, or Wv in simplified model, m_C1 = %s GeV'%(masses['1000024'])

from GeneratorFilters.GeneratorFiltersConf import ZtoLeptonFilter
filtSeq += ZtoLeptonFilter("ZtoLeptonFilter")

filtSeq.Expression = "ZtoLeptonFilter"
evt_multiplier = 20.
if masses['1000024']<300:
    evt_multiplier = 50.

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{x1+,1000024}{x1-,-1000024}",
                                "1000024:spinType = 1"]
