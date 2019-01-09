include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

for q in squarks: masses[q] = float(runArgs.jobConfig[0].split('_')[4])
masses['1000024'] = float(runArgs.jobConfig[0].split('_')[5])
masses['1000023'] = float(runArgs.jobConfig[0].split('_')[5])
masses['1000022'] = float(runArgs.jobConfig[0].split('_')[6].split('.')[0])
if masses['1000022']<0.5: masses['1000022']=0.5
genSeq.Pythia8.Commands += ["25:mMin = 0.2"]
genSeq.Pythia8.Commands += ["24:mMin = 0.2"]
genSeq.Pythia8.Commands += ["23:mMin = 0.2"]


gentype = str(runArgs.jobConfig[0].split('_')[2])
decaytype = str(runArgs.jobConfig[0].split('_')[3])
process = '''
generate p p > susylq susylq~ $ go susyweak @1
add process p p > susylq susylq~ j $ go susyweak @2
add process p p > susylq susylq~ j j $ go susyweak @3
'''

njets = 2
evt_multiplier = 4
evgenLog.info('Registered generation of squark grid '+str(runArgs.runNumber))



evgenConfig.contact  = [ "antonia.struebig@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel', 'squark']
evgenConfig.description = 'squark production in simplified model, one-step decays through chargino or neutralino2, m_sq = %s GeV, m_C1 = m_N2 = %s GeV, m_N1 = %s GeV'%(masses[squarks[0]],masses['1000024'],masses['1000022'])
evgenConfig.inputconfcheck = "SS_direct_"+runArgs.jobConfig[0].split("_")[4]

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{ul,1000002}{ul~,-1000002}{dl,1000001}{dl~,-1000001}{sl,1000003}{sl~,-1000003}{cl,1000004}{cl~,-1000004}"]


