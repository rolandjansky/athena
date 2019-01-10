include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

masses['1000005'] = float(runArgs.jobConfig[0].split('_')[4])
masses['1000023'] = float(runArgs.jobConfig[0].split('_')[5])
masses['1000022'] = float(runArgs.jobConfig[0].split('_')[6].split('.')[0])



if masses['1000022']<0.5: masses['1000022']=0.5
gentype = str(runArgs.jobConfig[0].split('_')[2])
decaytype = str(runArgs.jobConfig[0].split('_')[3])
process = '''
generate p p > b1 b1~ $ go susylq susylq~ b2 t1 t2 b2~ t1~ t2~ @1
add process p p > b1 b1~ j $ go susylq susylq~ b2 t1 t2 b2~ t1~ t2~ @2
add process p p > b1 b1~ j j $ go susylq susylq~ b2 t1 t2 b2~ t1~ t2~ @3
'''
njets = 2
evt_multiplier = 4
evgenLog.info('Generation of sbottom pair production, sbottom to b+N2, N2 to h+LSP; grid point '+str(runArgs.runNumber)+' decoded into mass point ' + str(masses['1000005']) + ' N2 mass '  + str(masses['1000023']) + ' N1 mass '  + str(masses['1000022']))

if 'MET100' in runArgs.jobConfig[0].split('_')[-1]:
    evgenLog.info('MET100 filter is applied')
    include ( 'GeneratorFilters/MissingEtFilter.py' )
    filtSeq.MissingEtFilter.METCut = 100*GeV
    evt_multiplier = 40
if 'HT100' in runArgs.jobConfig[0].split('_')[-1]:
    evgenLog.info('HT100 filter is applied')
    include ( 'GeneratorFilters/HTFilter.py' )
    filtSeq.HTFilter.MinHT = 100*GeV
    filtSeq.HTFilter.MaxHT = 6000*GeV

if ('1tau' in runArgs.jobConfig[0].split('_')[-1] or '2tau' in runArgs.jobConfig[0].split('_')[-1]):
    include("GeneratorFilters/AntiKt4TruthJets.py")
    from GeneratorFilters.GeneratorFiltersConf import TruthJetFilter
    filtSeq += TruthJetFilter("TruthJetFilter")
    filtSeq.TruthJetFilter.jet_pt1 = 100000.
    filtSeq.TruthJetFilter.NjetMinPt = 0.
    filtSeq.TruthJetFilter.NjetMaxEta = 2.8
    filtSeq.TruthJetFilter.Njet = 1

    ntaus=1
    if '1tau' in runArgs.jobConfig[0].split('_')[-1]:
        ntaus=1
        evt_multiplier=10
        evgenLog.info('JetPt100 AND 1tau filters applied')
    if '2tau' in runArgs.jobConfig[0].split('_')[-1]:
        ntaus=2
        evt_multiplier=80
        evgenLog.info('JetPt100 AND 2tau filters applied')

    include('GeneratorFilters/MultiElecMuTauFilter.py')
    MultiElecMuTauFilter = filtSeq.MultiElecMuTauFilter
    MultiElecMuTauFilter.NLeptons = ntaus
    MultiElecMuTauFilter.MinPt = 1e10
    MultiElecMuTauFilter.MaxEta = 2.8
    MultiElecMuTauFilter.MinVisPtHadTau =  12000
    MultiElecMuTauFilter.IncludeHadTaus = 1

    filtSeq.Expression = "MultiElecMuTauFilter and TruthJetFilter"

evgenConfig.contact  = [ "john.kenneth.anders@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel']
evgenConfig.description = 'sbottom direct pair production, sb->b+N2, N2->h+N1 in simplified model, m_sbottom = %s GeV, m_N2 = %s GeV, m_N1 = %s GeV'%(masses['1000005'],masses['1000023'],masses['1000022'])

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{b1,1000005}{b1~,-1000005}"]
