include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

print runArgs.jobConfig[0].split('_')[4]
print runArgs.jobConfig[0].split('_')[5]
print runArgs.jobConfig[0].split('_')[6]

masses['1000021'] = float(runArgs.jobConfig[0].split('_')[4])

masses['1000024'] = float(runArgs.jobConfig[0].split('_')[5])
masses['1000023'] = float(runArgs.jobConfig[0].split('_')[5])
masses['1000022'] = float(runArgs.jobConfig[0].split('_')[6].split('.')[0])
if masses['1000022']<0.5: masses['1000022']=0.5
genSeq.Pythia8.Commands += ["23:mMin = 0.2"]
genSeq.Pythia8.Commands += ["24:mMin = 0.2"]
genSeq.Pythia8.Commands += ["25:mMin = 0.2"]

gentype = str(runArgs.jobConfig[0].split('_')[2])
decaytype = str(runArgs.jobConfig[0].split('_')[3])
process = '''
generate p p > go go $ susysq susysq~ @1
add process p p > go go j $ susysq susysq~ @2
add process p p > go go j j $ susysq susysq~ @3
'''
njets = 2
evt_multiplier = 4
evgenLog.info('Registered generation of gluino grid '+str(runArgs.runNumber))

evgenConfig.contact  = [ "danijela.bogavac@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel','gluino']
evgenConfig.description = 'Gluino production in simplified model,one-step decays through chargino or neutralino2, m_sq = %s GeV, m_C1 = m_N2 = %s GeV, m_N1 = %s GeV'%(masses['1000021'],masses['1000024'],masses['1000022'])

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{go,1000021}{go,1000021}"]
