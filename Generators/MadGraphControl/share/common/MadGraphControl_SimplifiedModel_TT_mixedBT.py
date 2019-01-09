include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

masses['1000006'] = float(runArgs.jobConfig[0].split('_')[4])
masses['1000024'] = float(runArgs.jobConfig[0].split('_')[5])
masses['1000022'] = float(runArgs.jobConfig[0].split('_')[6].split('.')[0])
if masses['1000022']<0.5: masses['1000022']=0.5
gentype = str(runArgs.jobConfig[0].split('_')[2])
decaytype = str(runArgs.jobConfig[0].split('_')[3])
process = '''
generate p p > t1 t1~ $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @1
add process p p > t1 t1~ j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @2
add process p p > t1 t1~ j j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @3
'''

njets = 2
evt_multiplier = 4
evgenLog.info('Registered generation of stop pair production, stop to b+chargino; grid point '+str(runArgs.runNumber)+' decoded into mass point ' + str(masses['1000006']))

evgenConfig.contact  = [ "bnachman@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel']
evgenConfig.description = 'stop direct pair production, st->b+chargino and st->t+neutralino (asymmetric) in simplified model, m_stop = %s GeV, m_C1 = %s GeV, m_N1 = %s GeV'%(masses['1000006'],masses['1000024'],masses['1000022'])

genSeq.Pythia8.Commands += ["24:mMin = 0.2"]

include( "GeneratorFilters/DecayModeFilter.py" )

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{t1,1000006}{t1~,-1000006}"]
