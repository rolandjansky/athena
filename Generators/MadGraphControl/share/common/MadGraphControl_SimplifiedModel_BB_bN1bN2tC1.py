include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

masses['1000005'] = float(runArgs.jobConfig[0].split('_')[4])
masses['1000024'] = float(runArgs.jobConfig[0].split('_')[5])
masses['1000022'] = float(runArgs.jobConfig[0].split('_')[6].split('.')[0])
if masses['1000022']<0.5: masses['1000022']=0.5
masses['1000023'] = 2 * masses['1000024'] - masses['1000022'] # dM(N2,N1) = 2 * dM(C1,N1)
gentype = str(runArgs.jobConfig[0].split('_')[2])
decaytype = str(runArgs.jobConfig[0].split('_')[3])
process = '\n'.join([
    'generate p p > b1 b1~ $ go susylq susylq~ b2 t1 t2 b2~ t1~ t2~ @1',
    'add process p p > b1 b1~ j $ go susylq susylq~ b2 t1 t2 b2~ t1~ t2~ @2',
    'add process p p > b1 b1~ j j $ go susylq susylq~ b2 t1 t2 b2~ t1~ t2~ @3'
])

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
evgenLog.info('Registered generation of sbottom pair production, sbottom to t+chargino / b+neutralino; grid point '+str(runArgs.runNumber)+' decoded into mass point ' + str(masses['1000005']) + ', with mN1 = '+str(masses['1000022'])+' and mC1 = '+str(masses['1000024']))


if 'MET150' in runArgs.jobConfig[0].split('_')[-1]:
    evgenLog.info('MET150 filter is applied')
    include ( 'GeneratorFilters/MissingEtFilter.py' )

    filtSeq.MissingEtFilter.METCut = 150*GeV

    evt_multiplier = 100.

elif 'MET200' in runArgs.jobConfig[0].split('_')[-1]:
    evgenLog.info('MET200 filter is applied')
    include ( 'GeneratorFilters/MissingEtFilter.py' )

    filtSeq.MissingEtFilter.METCut = 200*GeV

    evt_multiplier = 100.

    
evgenConfig.contact  = [ "herwig@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel']
evgenConfig.description = 'sbottom direct pair production, sb->b+neutralino/t+chargino in simplified model, m_sbottom = %s GeV, m_C1 = %s GeV, m_N1 = %s GeV'%(masses['1000005'],masses['1000024'],masses['1000022'])

genSeq.Pythia8.Commands += ["24:mMin = 0.2"]

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{b1,1000005}{b1~,-1000005}"]
