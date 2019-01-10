
include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

fields = runArgs.jobConfig[0].replace(".py","").split("_")

masses['1000021'] = float(fields[4])
masses['1000006'] = float(fields[5])
masses['1000022'] = float(fields[6])

gentype = fields[2]
decaytype = fields[3]

if len(fields) >= 8:
    nbody = fields[7]
    if nbody == '5body':
        decays = {'1000021':'''DECAY   1000021     7.40992706E-02   # gluino decays
#           BR         NDA  ID1        ID2   ID3    ID4   ID5
     1.00000000E+00    5    1000022    24    -24    5    -5   # 5 body decay\n
'''}
    if nbody == '4body':
        decays = {'1000021':'''DECAY   1000021     7.40992706E-02   # gluino decays
#           BR         NDA  ID1        ID2   ID3    ID4
0.50000000E+00    4    1000022    24    5      -6      # 4 body decay t -> b W+
0.50000000E+00    4     1000022         -24        -5 6  # 4 body decay tb -> bb W-\n
'''}

if  len(fields) == 9 and fields[8] == 'Np0':
    njets = 0
    process = '''
generate p p > go go $ susysq susysq~ @1
'''
else:
    njets = 2
    process = '''
generate p p > go go $ susysq susysq~ @1
add process p p > go go j $ susysq susysq~ @2
add process p p > go go j j $ susysq susysq~ @3
'''

evgenLog.info('Registered generation of Gtt grid '+str(runArgs.runNumber))

evgenConfig.contact  = ["othmane.rifki@cern.ch", "louis.guillaume.gagnon@cern.ch"]
evgenConfig.keywords += ['simplifiedModel', 'gluino', 'neutralino', 'SUSY', 'top']
evgenConfig.description = 'gluino pair production and decay via an off-shell stop, m_glu = %s GeV, m_stop = %s GeV, m_N1 = %s GeV'%(masses['1000021'],masses['1000006'],masses['1000022'])

runName = str(runArgs.jobConfig[0].split(".")[1])
evgenConfig.inputfilecheck = runName

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{go,1000021}{go,1000021}"]
    
