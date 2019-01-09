include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

masses['1000006'] = float(runArgs.jobConfig[0].split('_')[4].split('.')[0])
gentype = str(runArgs.jobConfig[0].split('_')[2])
decaytype = str(runArgs.jobConfig[0].split('_')[3])
process = '''
generate p p > t1 t1~ $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @0
generate p p > t1 t1~ j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @1
generate p p > t1 t1~ j j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @2
'''

njets = 0
evgenLog.info('Registered generation of RPV stop pair production with UDD coupling, stop to two b-quarks; grid point '+str(runArgs.runNumber)+' decoded into mass point ' + str(masses['1000006']))

evgenConfig.contact  = [ "simone.amoroso@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel', 'stop','RPV']
evgenConfig.description = 'stop direct pair production, RPV decay stop->b+b in simplified model, mstop = '+str(runArgs.jobConfig[0].split('_')[4].split('.')[0])
evgenConfig.minevents = 5000
include ('MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py')

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{t1,1000006}{t1~,-1000006}"]
