include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

#Setting the masses

m_sq = float(runArgs.jobConfig[0].split('_')[6])
m_cn2 = float(runArgs.jobConfig[0].split('_')[7])
m_sl = float(runArgs.jobConfig[0].split('_')[8])
m_n1 = float(runArgs.jobConfig[0].split('_')[9].split('.')[0])

if m_sq == m_cn2: m_sq = m_sq+1.8
if m_cn2 == m_sl: m_cn2 = m_cn2+1.8
if m_sl == m_n1: m_sl = m_sl+1.8


#the squarks
for q in squarks: masses[q] = m_sq
#chargino 1 and neutralino 2
masses['1000024'] = m_cn2
masses['1000023'] = m_cn2
#sleptons
masses['1000011'] = m_sl
masses['1000012'] = m_sl
masses['1000013'] = m_sl
masses['1000014'] = m_sl
masses['1000015'] = m_sl
masses['1000016'] = m_sl
masses['2000011'] = m_sl
masses['2000013'] = m_sl
masses['2000015'] = m_sl
#neutralino 1
masses['1000022'] = m_n1
if masses['1000022']<0.5: masses['1000022']=0.5
genSeq.Pythia8.Commands += ["25:mMin = 0.2"]
genSeq.Pythia8.Commands += ["24:mMin = 0.2"]
genSeq.Pythia8.Commands += ["23:mMin = 0.2"]


gentype = str(runArgs.jobConfig[0].split('_')[3])
decaytype = str(runArgs.jobConfig[0].split('_')[4]+"_"+runArgs.jobConfig[0].split('_')[5])
process = '''
generate p p > susylq susylq~ $ go susyweak @1
add process p p > susylq susylq~ j $ go susyweak @2
add process p p > susylq susylq~ j j $ go susyweak @3
'''

njets = 2
evt_multiplier = 4
evgenLog.info('Registered generation of squark grid '+str(runArgs.runNumber))

evgenConfig.contact  = [ "federico.meloni@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel', 'squark']
evgenConfig.description = 'squark production in simplified model, two-step decays through chargino or neutralino2 followed by sleptons, m_sq = %s GeV, m_C1 = m_N2 = %s GeV, m_N1 = %s GeV'%(masses[squarks[0]],masses['1000024'],masses['1000022'])
evgenConfig.inputconfcheck = "SS_direct_"+runArgs.jobConfig[0].split("_")[4]+"_"+runArgs.jobConfig[0].split('_')[5]

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{ul,1000002}{ul~,-1000002}{dl,1000001}{dl~,-1000001}{sl,1000003}{sl~,-1000003}{cl,1000004}{cl~,-1000004}"]


