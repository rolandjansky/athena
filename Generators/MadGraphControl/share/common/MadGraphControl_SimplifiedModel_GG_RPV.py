include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )
gentype = str(runArgs.jobConfig[0].split('_')[2])
decaytype = str(runArgs.jobConfig[0].split('_')[3])
njets=2
masses['1000021'] = float(runArgs.jobConfig[0].split('_')[4].split('.')[0])
if decaytype == 'RPV10':
    masses['1000022'] = float(runArgs.jobConfig[0].split('_')[5].split('.')[0])
    if masses['1000022'] > 180:
        decays['1000022'] = '''DECAY   1000022  1.0000000000E+00   # neutralino1 decays
#          BR          NDA       ID1       ID2       ID3       ID4		
      0.05555556	3    2	1  3
      0.05555556	3    2	1  5
      0.05555556	3    2	3  5
      0.05555556	3    4	1  3
      0.05555556	3    4	1  5
      0.05555556	3    4	3  5
      0.05555556	3    6	1  3
      0.05555556	3    6	1  5
      0.05555556	3    6	3  5
      0.05555556	3   -2 -1 -3
      0.05555556	3   -2 -1 -5
      0.05555556	3   -2 -3 -5
      0.05555556	3   -4 -1 -3
      0.05555556	3   -4 -1 -5
      0.05555556	3   -4 -3 -5
      0.05555556	3   -6 -1 -3
      0.05555556	3   -6 -1 -5
      0.05555556	3   -6 -3 -5
#'''
process = '''
generate p p > go go @1
add process p p > go go j @2
add process p p > go go j j @3
'''
evgenLog.info('Registered generation of gluino grid '+str(runArgs.runNumber))
evgenConfig.contact  = [ "brian.thomas.amadio@cern.ch" ]
evgenConfig.keywords += ['gluino','SUSY','simplifiedModel','RPV']
if decaytype == 'RPV10':
    evgenConfig.description = 'gluino pair production with gluino -> qq+neutralino, neutralino->qqq via RPV UDD coupling. m_glu = %s GeV, m_N1 = %s GeV'%(masses['1000021'],masses['1000022'])
if decaytype == 'RPV6':
    evgenConfig.description = 'gluino pair production with gluino -> qqq via RPV UDD coupling. m_glu = %s GeV' % masses['1000021']
include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )
genSeq.Pythia8.Commands += ["Merging:Process = pp>{go,1000021}{go,1000021}"]
