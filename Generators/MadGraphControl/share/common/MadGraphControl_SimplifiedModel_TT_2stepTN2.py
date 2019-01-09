include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

evt_multiplier = 25

fields = runArgs.jobConfig[0].replace(".py","").split("_")

masses['1000006'] = float(fields[4])
masses['1000022'] = float(fields[6])
if masses['1000022']<0.5: masses['1000022']=0.5
masses['1000023'] = float(fields[5])
masses['1000024'] = masses['1000022'] + 0.5 

gentype = fields[2]
decaytype = fields[3]
nbody = fields[7]

if nbody == '3body':
    decays = {'1000023':'''DECAY   1000023     1.35149386E-03   # neutralino2 decays
#          BR         NDA      ID1       ID2
     1.874999185E-01    3     1000024        -2         1   # BR(~chi_20 -> ~chi_1+ ub      d)
     1.874999185E-01    3    -1000024        -1         2   # BR(~chi_20 -> ~chi_1- db      u)
     1.874999185E-01    3     1000024        -4         3   # BR(~chi_20 -> ~chi_1+ cb      s)
     1.874999185E-01    3    -1000024        -3         4   # BR(~chi_20 -> ~chi_1- sb      c)
     6.25000815E-02    3     1000024       -12        11   # BR(~chi_20 -> ~chi_1+ nu_eb   e-)
     6.25000815E-02    3    -1000024        12       -11   # BR(~chi_20 -> ~chi_1- nu_e    e+)
     6.25000815E-02    3     1000024       -14        13   # BR(~chi_20 -> ~chi_1+ nu_mub  mu-)
     6.25000815E-02    3    -1000024        14       -13   # BR(~chi_20 -> ~chi_1- nu_mu   mu+)
'''}

njets = 2
process = '\n'.join([
    'generate p p > t1 t1~ $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @1',
    'add process p p > t1 t1~ j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @2',
    'add process p p > t1 t1~ j j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @3'
])

evgenLog.info('Registered generation of stop pair production, stop to t+N2, N2 to W+C1, C1 to W+N1; grid point '+str(runArgs.runNumber)
    +' decoded into mass point (T1=%.0f, N2=%.0f, C1=%.0f, N1=%.0f)'%(masses['1000006'],masses['1000023'],masses['1000024'],masses['1000022']))

evgenConfig.contact  = [ "othmane.rifki@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel','stop'] 
evgenConfig.description = 'stop direct pair production, m_T1 = %s GeV, m_N2 = %s GeV, m_C1 = %s GeV, m_N1 = %s GeV'%(masses['1000006'],masses['1000023'],masses['1000024'],masses['1000022'])

evgenLog.info('### Setting use_syst to False ###')
extras['event_norm']='sum'
extras['use_syst']='F'
extras['pdgs_for_merging_cut']='1, 2, 3, 4, 21, 1000001, 1000002, 1000003, 1000004, 1000021, 2000001, 2000002, 2000003, 2000004'

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )
if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{t1,1000006}{t1~,-1000006}"]

if  len(fields) == 9 and 'LSS' in fields[8]:
    evgenLog.info('Using LeptonPairFilter accepting SS lepton pairs with massive parents')
    from GeneratorFilters.GeneratorFiltersConf import LeptonPairFilter
    filtSeq += LeptonPairFilter('SS3LFilter')
    myFilter = filtSeq.SS3LFilter
    myFilter.NLeptons_Min = 0
    if fields[8] == '2LSS':
        myFilter.NLeptons_Max = 2 # NB: filter is always passed if nLeptons > nLeptons_Max
    myFilter.Ptcut = 10000
    myFilter.Etacut = 2.8
    myFilter.NSFOS_Min = -1
    myFilter.NSFOS_Max = -1
    myFilter.NOFOS_Min = -1
    myFilter.NOFOS_Max = -1
    myFilter.NSFSS_Min = -1
    myFilter.NSFSS_Max = -1
    myFilter.NOFSS_Min = -1
    myFilter.NOFSS_Max = -1
    myFilter.NPairSum_Max = -1
    myFilter.NPairSum_Min = 1
    myFilter.UseSFOSInSum = False
    myFilter.UseSFSSInSum = True
    myFilter.UseOFOSInSum = False
    myFilter.UseOFSSInSum = True
    myFilter.OnlyMassiveParents = True
