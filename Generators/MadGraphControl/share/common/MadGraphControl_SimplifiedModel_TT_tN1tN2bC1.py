include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

masses['1000006'] = float(runArgs.jobConfig[0].split('_')[4])
masses['1000024'] = float(runArgs.jobConfig[0].split('_')[5])
masses['1000022'] = float(runArgs.jobConfig[0].split('_')[6].split('.')[0])
if masses['1000022']<0.5: masses['1000022']=0.5
masses['1000023'] = 2 * masses['1000024'] - masses['1000022'] # dM(N2,N1) = 2 * dM(C1,N1)
gentype = str(runArgs.jobConfig[0].split('_')[2])
decaytype = str(runArgs.jobConfig[0].split('_')[3])
process = '''
generate p p > t1 t1~ $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @1
add process p p > t1 t1~ j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @2
add process p p > t1 t1~ j j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @3
'''

# For off-shel W, use BRs obtained from SUSY-HIT 1.5a
if 'DM2' in runArgs.jobConfig[0].strip('.py').split('_'):
    decays = {'1000024':'''DECAY   1000024     1.25982028E-08   # chargino1+ decays
#           BR         NDA      ID1       ID2       ID3
     5.82417245E-01    3     1000022         2        -1   # BR(~chi_1+ -> ~chi_10 u    db)
     3.13660408E-02    3     1000022         4        -3   # BR(~chi_1+ -> ~chi_10 c    sb)
     1.94139144E-01    3     1000022       -11        12   # BR(~chi_1+ -> ~chi_10 e+   nu_e)
     1.91494275E-01    3     1000022       -13        14   # BR(~chi_1+ -> ~chi_10 mu+  nu_mu)
     5.83295368E-04    3     1000022       -15        16   # BR(~chi_1+ -> ~chi_10 tau+ nu_tau)
''','1000023':'''DECAY   1000023     9.23629247E-10   # neutralino2 decays
#           BR         NDA      ID1       ID2       ID3
     1.26460568E-01    3     1000022        -2         2   # BR(~chi_20 -> ~chi_10 ub      u)
     1.64023060E-01    3     1000022        -1         1   # BR(~chi_20 -> ~chi_10 db      d)
     3.88928110E-02    3     1000022        -4         4   # BR(~chi_20 -> ~chi_10 cb      c)
     1.61045338E-01    3     1000022        -3         3   # BR(~chi_20 -> ~chi_10 sb      s)
     3.76028132E-02    3     1000022       -11        11   # BR(~chi_20 -> ~chi_10 e+      e-)
     3.74707231E-02    3     1000022       -13        13   # BR(~chi_20 -> ~chi_10 mu+     mu-)
     2.39188407E-03    3     1000022       -15        15   # BR(~chi_20 -> ~chi_10 tau+    tau-)
     7.51656120E-02    3     1000022       -12        12   # BR(~chi_20 -> ~chi_10 nu_eb   nu_e)
     7.51656120E-02    3     1000022       -14        14   # BR(~chi_20 -> ~chi_10 nu_mub  nu_mu)
     7.51656120E-02    3     1000022       -16        16   # BR(~chi_20 -> ~chi_10 nu_taub nu_tau)
     2.13412953E-02    3     1000024        -2         1   # BR(~chi_20 -> ~chi_1+ ub      d)
     2.13412953E-02    3    -1000024        -1         2   # BR(~chi_20 -> ~chi_1- db      u)
     2.17077450E-03    3     1000024        -4         3   # BR(~chi_20 -> ~chi_1+ cb      s)
     2.17077450E-03    3    -1000024        -3         4   # BR(~chi_20 -> ~chi_1- sb      c)
     7.11376703E-03    3     1000024       -12        11   # BR(~chi_20 -> ~chi_1+ nu_eb   e-)
     7.11376703E-03    3    -1000024        12       -11   # BR(~chi_20 -> ~chi_1- nu_e    e+)
     7.03241095E-03    3     1000024       -14        13   # BR(~chi_20 -> ~chi_1+ nu_mub  mu-)
     7.03241095E-03    3    -1000024        14       -13   # BR(~chi_20 -> ~chi_1- nu_mu   mu+)
     1.11665525E-04    3     1000024       -16        15   # BR(~chi_20 -> ~chi_1+ nu_taub tau-)
     1.11665525E-04    3    -1000024        16       -15   # BR(~chi_20 -> ~chi_1- nu_tau  tau+)
'''}

if 'DM5' in runArgs.jobConfig[0].strip('.py').split('_'):
    decays = {'1000024':'''DECAY   1000024     1.25982028E-08   # chargino1+ decays
#           BR         NDA      ID1       ID2       ID3
     3.95250711E-01    3     1000022         2        -1   # BR(~chi_1+ -> ~chi_10 u    db)
     2.69429321E-01    3     1000022         4        -3   # BR(~chi_1+ -> ~chi_10 c    sb)
     1.31750510E-01    3     1000022       -11        12   # BR(~chi_1+ -> ~chi_10 e+   nu_e)
     1.31457940E-01    3     1000022       -13        14   # BR(~chi_1+ -> ~chi_10 mu+  nu_mu)
     7.21115175E-02    3     1000022       -15        16   # BR(~chi_1+ -> ~chi_10 tau+ nu_tau)
''','1000023':'''DECAY   1000023     1.07306407E-11   # neutralino2 decays
#           BR         NDA      ID1       ID2       ID3
     1.18593262E-01    3     1000022        -2         2   # BR(~chi_20 -> ~chi_10 ub      u)
     1.53738112E-01    3     1000022        -1         1   # BR(~chi_20 -> ~chi_10 db      d)
     1.03933361E-01    3     1000022        -4         4   # BR(~chi_20 -> ~chi_10 cb      c)
     1.53283808E-01    3     1000022        -3         3   # BR(~chi_20 -> ~chi_10 sb      s)
     8.11680907E-04    3     1000022        -5         5   # BR(~chi_20 -> ~chi_10 bb      b)
     3.52008978E-02    3     1000022       -11        11   # BR(~chi_20 -> ~chi_10 e+      e-)
     3.51806811E-02    3     1000022       -13        13   # BR(~chi_20 -> ~chi_10 mu+     mu-)
     2.89732056E-02    3     1000022       -15        15   # BR(~chi_20 -> ~chi_10 tau+    tau-)
     7.03474532E-02    3     1000022       -12        12   # BR(~chi_20 -> ~chi_10 nu_eb   nu_e)
     7.03474532E-02    3     1000022       -14        14   # BR(~chi_20 -> ~chi_10 nu_mub  nu_mu)
     7.03474532E-02    3     1000022       -16        16   # BR(~chi_20 -> ~chi_10 nu_taub nu_tau)
     2.68305591E-02    3     1000024        -2         1   # BR(~chi_20 -> ~chi_1+ ub      d)
     2.68305591E-02    3    -1000024        -1         2   # BR(~chi_20 -> ~chi_1- db      u)
     2.00971277E-02    3     1000024        -4         3   # BR(~chi_20 -> ~chi_1+ cb      s)
     2.00971277E-02    3    -1000024        -3         4   # BR(~chi_20 -> ~chi_1- sb      c)
     8.94353442E-03    3     1000024       -12        11   # BR(~chi_20 -> ~chi_1+ nu_eb   e-)
     8.94353442E-03    3    -1000024        12       -11   # BR(~chi_20 -> ~chi_1- nu_e    e+)
     8.92856781E-03    3     1000024       -14        13   # BR(~chi_20 -> ~chi_1+ nu_mub  mu-)
     8.92856781E-03    3    -1000024        14       -13   # BR(~chi_20 -> ~chi_1- nu_mu   mu+)
     5.68364178E-03    3     1000024       -16        15   # BR(~chi_20 -> ~chi_1+ nu_taub tau-)
     5.68364178E-03    3    -1000024        16       -15   # BR(~chi_20 -> ~chi_1- nu_tau  tau+)
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

    #modify to 40/40/20 BR instead for filtered points
    decays['1000006'] = '''DECAY   1000006     6.85567609E-01   # stop1 decays
#          BR         NDA      ID1       ID2
     4.00000000E-01    2     1000022         6   # BR(~t_1 -> ~chi_10 t )
     4.00000000E-01    2     1000023         6   # BR(~t_1 -> ~chi_20 t )
     0.00000000E+00    2     1000025         6   # BR(~t_1 -> ~chi_30 t )
     0.00000000E+00    2     1000035         6   # BR(~t_1 -> ~chi_40 t )
     2.00000000E-01    2     1000024         5   # BR(~t_1 -> ~chi_1+ b )
     0.00000000E+00    2     1000037         5   # BR(~t_1 -> ~chi_2+ b )
     0.00000000E+00    2     1000021         6   # BR(~t_1 -> ~g      t )
     0.00000000E+00    2     1000005        37   # BR(~t_1 -> ~b_1    H+)
     0.00000000E+00    2     2000005        37   # BR(~t_1 -> ~b_2    H+)
     0.00000000E+00    2     1000005        24   # BR(~t_1 -> ~b_1    W+)
     0.00000000E+00    2     2000005        24   # BR(~t_1 -> ~b_2    W+)
'''


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
