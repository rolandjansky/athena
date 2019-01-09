include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

evt_multiplier = 25

masses['1000005'] = float(runArgs.jobConfig[0].split('_')[4].split('.')[0])
masses['1000022'] = float(runArgs.jobConfig[0].split('_')[6].split('.')[0])
if masses['1000022']<0.5: masses['1000022']=0.5
masses['1000024'] = float(runArgs.jobConfig[0].split('_')[5].split('.')[0])
masses['1000023'] = masses['1000024']
masses['1000025'] = 5e3
masses['1000035'] = 5e3
masses['1000037'] = 5e3

gentype = str(runArgs.jobConfig[0].split('_')[2])
decaytype = str(runArgs.jobConfig[0].split('_')[3])
njets = 2
process = '\n'.join([
    'generate p p > b1 b1~ $ go susylq susylq~ b2 t1 t2 b2~ t1~ t2~ @1',
    'add process p p > b1 b1~ j $ go susylq susylq~ b2 t1 t2 b2~ t1~ t2~ @2',
    'add process p p > b1 b1~ j j $ go susylq susylq~ b2 t1 t2 b2~ t1~ t2~ @3'
])

evgenLog.info('Registered generation of sbottom pair production, sbottom to t+C1; grid point '+str(runArgs.runNumber)
    +' decoded into mass point (B=%.0f, C=%.0f, N=%.0f)'%(masses['1000005'],masses['1000024'],masses['1000022']))

evgenConfig.contact  = [ "takashi.yamanaka@cern.ch", "julien.maurer@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel','sbottom']
evgenConfig.description = 'sbottom direct pair production, m_sbottom = %s GeV, m_C1N2 = %s GeV, m_N1 = %s GeV'%(masses['1000005'],masses['1000024'],masses['1000022'])

evgenLog.info('### Setting use_syst to False ###')
extras['event_norm']='sum'
extras['use_syst']='F'
extras['pdgs_for_merging_cut']='1, 2, 3, 4, 21, 1000001, 1000002, 1000003, 1000004, 1000021, 2000001, 2000002, 2000003, 2000004'

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )
if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{b1,1000005}{b1~,-1000005}"]

evgenLog.info('Using LeptonPairFilter accepting SS lepton pairs with massive parents')
from GeneratorFilters.GeneratorFiltersConf import LeptonPairFilter
filtSeq += LeptonPairFilter('SS3LFilter')
myFilter = filtSeq.SS3LFilter
myFilter.NLeptons_Min = 0
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

