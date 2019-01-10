include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

masses['1000005'] = float(runArgs.jobConfig[0].split('_')[4])
masses['1000024'] = (float(runArgs.jobConfig[0].split('_')[5]))
masses['1000022'] = float(runArgs.jobConfig[0].split('_')[6].split('.')[0])

if masses['1000022']<0.5: masses['1000022']=0.5
gentype = str(runArgs.jobConfig[0].split('_')[2])
decaytype = str(runArgs.jobConfig[0].split('_')[3])
process = '''
generate p p > b1 b1~ $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @1
add process p p > b1 b1~ j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @2
add process p p > b1 b1~ j j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @3
'''

njets = 2
evt_multiplier = 4
evgenLog.info('Registered generation of sbottom pair production, sbottom to t+chargino, b+neutralino; grid point '+str(runArgs.runNumber)+' decoded into mass point ' + str(masses['1000005']))

evgenConfig.contact  = [ "john.kenneth.anders@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel']
evgenConfig.description = 'sbottom direct pair production, sb->t+chargino and sb->b+neutralino (asymmetric) in simplified model, m_sbottom = %s GeV, m_C1 = %s GeV, m_N1 = %s GeV'%(masses['1000005'],masses['1000024'],masses['1000022'])


jobConfigParts = runArgs.jobConfig[0].split("_")
print jobConfigParts[-2]
print jobConfigParts[-1]

include("GeneratorFilters/DecayModeFilter.py")


if '1L20orMET100' in "%s_%s" % (jobConfigParts[-2], jobConfigParts[-1]):

    include("GeneratorFilters/LeptonFilter.py")
    filtSeq.LeptonFilter.Ptcut  = 20*GeV
    filtSeq.LeptonFilter.Etacut = 2.8
    include('GeneratorFilters/MissingEtFilter.py')
    filtSeq.MissingEtFilter.METCut = 100*GeV
    filtSeq.Expression = "(LeptonFilter or MissingEtFilter) and DecayModeFilter"
    evt_multiplier = 8
else:
    filtSeq.Expression = "DecayModeFilter"

genSeq.Pythia8.Commands += ["24:mMin = 0.2"]

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )
    
if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{b1,1000005}{b1~,-1000005}"]
