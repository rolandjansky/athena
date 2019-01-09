include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

masses['1000005'] = float(runArgs.jobConfig[0].split('_')[4])
masses['1000022'] = float(runArgs.jobConfig[0].split('_')[5].split('.')[0])
if masses['1000022']<0.5: masses['1000022']=0.5
gentype = str(runArgs.jobConfig[0].split('_')[2])
decaytype = str(runArgs.jobConfig[0].split('_')[3])
process = '''
generate p p > b1 b1~ $ go susylq susylq~ b2 t1 t2 b2~ t1~ t2~ @1
add process p p > b1 b1~ j $ go susylq susylq~ b2 t1 t2 b2~ t1~ t2~ @2
add process p p > b1 b1~ j j $ go susylq susylq~ b2 t1 t2 b2~ t1~ t2~ @3
'''
njets = 2
evgenLog.info('Registered generation of sbottom pair production, sbottom to b+LSP; grid point '+str(runArgs.runNumber)+' decoded into mass point ' + str(masses['1000005']))

if 'MET100' in runArgs.jobConfig[0].split('_')[-1]:
    include ( 'GeneratorFilters/MissingEtFilter.py' )
    print "Adding MET100 filter"
    filtSeq.MissingEtFilter.METCut = 100*GeV
    evt_multiplier = 10.

if 'MET200' in runArgs.jobConfig[0].split('_')[-1]:
    print "Adding MET200 filter"
    include ( 'GeneratorFilters/MissingEtFilter.py' )
    filtSeq.MissingEtFilter.METCut = 200*GeV
    evt_multiplier = 30.


evgenConfig.contact  = [ "takashi.yamanaka@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel']
evgenConfig.description = 'sbottom direct pair production, sb->b+LSP in simplified model, m_sbottom = %s GeV, m_N1 = %s GeV'%(masses['1000005'],masses['1000022'])

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{b1,1000005}{b1~,-1000005}"]
