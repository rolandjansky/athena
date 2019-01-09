include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

masses['1000006'] = float(runArgs.jobConfig[0].split('_')[4])
masses['1000023'] = float(runArgs.jobConfig[0].split('_')[5])
gentype = str(runArgs.jobConfig[0].split('_')[2])
decaytype = str(runArgs.jobConfig[0].split('_')[3])

extras['event_norm']='sum'
extras['use_syst']='F'
extras['pdgs_for_merging_cut']='1, 2, 3, 4, 21, 1000001, 1000002, 1000003, 1000004, 1000021, 2000001, 2000002, 2000003, 2000004'


process = '''
generate p p > t1 t1~ $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @1
add process p p > t1 t1~ j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @2
add process p p > t1 t1~ j j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @3
'''
njets = 2
evt_multiplier = 5

evgenLog.info('Registered generation of stop1 pair production, stop1 to t chi02, chi02 ->Z+LSP and chi02 ->h+LSP; grid point '+str(runArgs.runNumber)+' decoded into mass point ' + str(masses['1000006']))

if "1L20" in runArgs.jobConfig[0] :
	include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
	filtSeq.MultiElecMuTauFilter.MinPt  = 20000.
	filtSeq.MultiElecMuTauFilter.MaxEta = 2.8
	filtSeq.MultiElecMuTauFilter.NLeptons = 1
	filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0
	filtSeq.Expression = "MultiElecMuTauFilter"

evgenConfig.contact  = [ "federico.meloni@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel', 'stop']
evgenConfig.description = 'stop2 direct pair production, st1->t chi02, chi02->Z+LSP, chi02->h+LSP in simplified model, m_stop1 = %s GeV, m_N2 = %s GeV'%(masses['1000006'],masses['1000023'])

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{t1,1000006}{t1~,-1000006}"]

