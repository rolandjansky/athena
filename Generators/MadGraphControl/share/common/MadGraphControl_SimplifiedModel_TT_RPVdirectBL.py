include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

masses['1000006'] = float(runArgs.jobConfig[0].split('_')[4].split('.')[0])
masses['1000022'] = 100000.

gentype = str(runArgs.jobConfig[0].split('_')[2])
decaytype = str(runArgs.jobConfig[0].split('_')[3])
process = '''
generate p p > t1 t1~ $ go susylq susylq~ b2 t1 t2 b2~ t1~ t2~ @1
add process p p > t1 t1~ j $ go susylq susylq~ b2 t1 t2 b2~ t1~ t2~ @2
add process p p > t1 t1~ j j $ go susylq susylq~ b2 t1 t2 b2~ t1~ t2~ @3
'''
njets = 2
evgenLog.info('Registered generation of stop pair production, sbottom to b+L; grid point '+str(runArgs.runNumber))
evgenConfig.contact  = [ "giuseppe.lerner@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel']
evgenConfig.description = 'stop direct pair production, st->b+L in simplified model, m_stop = %s GeV'%(masses['1000006'])

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{t1,1000006}{t1~,-1000006}"]
