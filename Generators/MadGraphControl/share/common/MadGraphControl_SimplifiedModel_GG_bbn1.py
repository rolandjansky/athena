include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

fields = runArgs.jobConfig[0].replace(".py","").split("_")

masses['1000021'] = float(fields[4])
masses['1000005'] = float(fields[5])
masses['1000022'] = float(fields[6])

gentype = fields[2]
decaytype = fields[3]

process = '''
generate p p > go go $ susysq susysq~ @1
add process p p > go go j $ susysq susysq~ @2
add process p p > go go j j $ susysq susysq~ @3
'''

njets = 2

evgenLog.info('Registered generation of Gbb grid '+str(runArgs.runNumber))

evgenConfig.contact  = ["louis.guillaume.gagnon@cern.ch"]
evgenConfig.keywords += ['simplifiedModel', 'gluino', 'neutralino', 'SUSY', 'bottom']
evgenConfig.description = 'gluino pair production and decay to bottom pair and LSP via off-shell sbottom, m_glu = %s GeV, m_sbottom = %s GeV, m_N1 = %s GeV'%(masses['1000021'],masses['1000005'],masses['1000022'])

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{go,1000021}{go,1000021}"]
