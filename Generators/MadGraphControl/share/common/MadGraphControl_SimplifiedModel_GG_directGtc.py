include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

gentype = str(runArgs.jobConfig[0].split('_')[2])
decaytype = str(runArgs.jobConfig[0].split('_')[3])
deltaM = 2
if decaytype.startswith('directGtc'):
    deltaM = float(decaytype.replace('directGtc',''))
    decaytype = 'directGtc'

masses['1000021'] = float(runArgs.jobConfig[0].split('_')[4])
masses['1000022'] = float(runArgs.jobConfig[0].split('_')[5].split('.')[0])
masses['1000006'] = masses['1000022'] + deltaM
if masses['1000022']<0.5: masses['1000022']=0.5
process = '''
generate p p > go go @1
add process p p > go go j @2
add process p p > go go j j @3
'''
njets = 2
evt_multiplier = 4
evgenLog.info('Registered generation of gluino grid '+str(runArgs.runNumber))

evgenConfig.contact  = [ "bnachman@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel','gluino']
evgenConfig.description = 'gluino production in simplified model with decay to top and charm, m_glu = %s GeV, m_N1 = %s GeV, deltaM = %s Gev'%(masses['1000021'],masses['1000022'],deltaM)

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{go,1000021}{go,1000021}"]
