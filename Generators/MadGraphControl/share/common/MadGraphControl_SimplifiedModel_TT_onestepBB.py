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

# For off-shel W, use BRs obtained from SUSY-HIT 1.5a
if 'DM5' in runArgs.jobConfig[0].split('_')[-1] or 'DM5' in runArgs.jobConfig[0].split('_')[-2]:
    evgenConfig.minevents = 1000
    decays = {'1000024':'''DECAY   1000024     1.25982028E-08   # chargino1+ decays
#           BR         NDA      ID1       ID2       ID3
     3.95250711E-01    3     1000022         2        -1   # BR(~chi_1+ -> ~chi_10 u    db)
     2.69429321E-01    3     1000022         4        -3   # BR(~chi_1+ -> ~chi_10 c    sb)
     1.31750510E-01    3     1000022       -11        12   # BR(~chi_1+ -> ~chi_10 e+   nu_e)
     1.31457940E-01    3     1000022       -13        14   # BR(~chi_1+ -> ~chi_10 mu+  nu_mu)
     7.21115175E-02    3     1000022       -15        16   # BR(~chi_1+ -> ~chi_10 tau+ nu_tau)
'''}


njets = 2
evgenLog.info('Registered generation of stop pair production, stop to b+chargino; grid point '+str(runArgs.runNumber)+' decoded into mass point ' + str(masses['1000006']))

if '1Lep20orMET60' in runArgs.jobConfig[0].split('_')[-1]:

    evgenLog.info('1lepton or MET60 filter is applied')
    include ( 'GeneratorFilters/LeptonFilter.py' )
    filtSeq.LeptonFilter.Ptcut  = 20000.
    filtSeq.LeptonFilter.Etacut = 2.8 
    include('GeneratorFilters/MissingEtFilter.py')
    filtSeq.MissingEtFilter.METCut = 60000.
    filtSeq.Expression = "LeptonFilter or MissingEtFilter"

    evt_multiplier = 3.0

elif 'MET60' in runArgs.jobConfig[0].split('_')[-1]:
    evgenLog.info('MET60 filter is applied')
    include ( 'GeneratorFilters/MissingEtFilter.py' )

    filtSeq.MissingEtFilter.METCut = 60*GeV

    evt_multiplier = 10.

elif 'MET150' in runArgs.jobConfig[0].split('_')[-1]:
    evgenLog.info('MET150 filter is applied')
    include ( 'GeneratorFilters/MissingEtFilter.py' )

    filtSeq.MissingEtFilter.METCut = 150*GeV

    evt_multiplier = 100.

elif 'MET200' in runArgs.jobConfig[0].split('_')[-1]:
    evgenLog.info('MET200 filter is applied')
    include ( 'GeneratorFilters/MissingEtFilter.py' )

    filtSeq.MissingEtFilter.METCut = 200*GeV

    evt_multiplier = 100.


elif '2Lep18' in runArgs.jobConfig[0].split('_')[-1]:
    
    evgenLog.info('2lepton filter is applied')
    include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
    filtSeq.MultiElecMuTauFilter.MinPt  = 18000.
    filtSeq.MultiElecMuTauFilter.MaxEta = 2.8
    filtSeq.MultiElecMuTauFilter.NLeptons = 2
    filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0
    
    filtSeq.Expression = "MultiElecMuTauFilter"

    evt_multiplier = 50.0
    
evgenConfig.contact  = [ "takashi.yamanaka@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel']
evgenConfig.description = 'stop direct pair production, st->b+chargino in simplified model, m_stop = %s GeV, m_C1 = %s GeV, m_N1 = %s GeV'%(masses['1000006'],masses['1000024'],masses['1000022'])

genSeq.Pythia8.Commands += ["24:mMin = 0.2"]

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{t1,1000006}{t1~,-1000006}"]
