include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

masses['2000006'] = float(runArgs.jobConfig[0].split('_')[4])
masses['1000006'] = float(runArgs.jobConfig[0].split('_')[5])
masses['1000022'] = float(runArgs.jobConfig[0].split('_')[6].split('.')[0])
if masses['1000022']<0.5: masses['1000022']=0.5
gentype = str(runArgs.jobConfig[0].split('_')[2])
decaytype = str(runArgs.jobConfig[0].split('_')[3])
process = '''
generate p p > t2 t2~ $ go susylq susylq~ b1 b2 t1 b1~ b2~ t1~ @1
add process p p > t2 t2~ j $ go susylq susylq~ b1 b2 t1 b1~ b2~ t1~ @2
add process p p > t2 t2~ j j $ go susylq susylq~ b1 b2 t1 b1~ b2~ t1~ @3
'''
njets = 2
evt_multiplier = 10

evgenLog.info('Registered generation of stop2 pair production, stop2 to Z+stop1, stop1->t+LSP; grid point '+str(runArgs.runNumber)+' decoded into mass point ' + str(masses['2000006']))

include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
filtSeq.MultiElecMuTauFilter.MinPt  = 10000.
filtSeq.MultiElecMuTauFilter.MaxEta = 2.8
filtSeq.MultiElecMuTauFilter.NLeptons = 2
filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0
filtSeq.Expression = "MultiElecMuTauFilter"

evgenConfig.contact  = [ "federico.meloni@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel', 'stop']
evgenConfig.description = 'stop2 direct pair production, st2->Z+st1, st1->t+LSP in simplified model, m_stop2 = %s GeV, m_stop1 = %s GeV, m_N1 = %s GeV'%(masses['2000006'],masses['1000006'],masses['1000022'])

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{t2,2000006}{t2~,-2000006}"]
