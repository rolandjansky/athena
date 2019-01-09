include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

gentype=runArgs.jobConfig[0].split('SM')[1].split('_')[1]
if 'bb' in runArgs.jobConfig[0]: decaytype='onestepbbN2'
else: decaytype='onestepN2'

mass_string = runArgs.jobConfig[0].replace('.py','').split('N2_')[1]

if gentype=='GG':
# Direct gluino decay to LSP (0-lepton, grid 1 last year)
    masses['1000021'] = float( mass_string.split('_')[0] )
    masses['1000022'] = float( mass_string.split('_')[2] )
    masses['1000023'] = float( mass_string.split('_')[1] )
    process = '''
    generate p p > go go $ susysq susysq~ @1
    add process p p > go go jb $ susysq susysq~ @2
    '''
elif gentype=='SS':
    masses['1000001'] = float( mass_string.split('_')[0] )
    masses['1000002'] = float( mass_string.split('_')[0] )
    masses['1000003'] = float( mass_string.split('_')[0] )
    masses['1000004'] = float( mass_string.split('_')[0] )
    masses['2000001'] = float( mass_string.split('_')[0] )
    masses['2000002'] = float( mass_string.split('_')[0] )
    masses['2000003'] = float( mass_string.split('_')[0] )
    masses['2000004'] = float( mass_string.split('_')[0] )
    masses['1000022'] = float( mass_string.split('_')[2] )
    masses['1000023'] = float( mass_string.split('_')[1] )
    process = '''
    generate p p > susysq susysq~ $ go @1
    add process p p > susysq susysq~ jb $ go @2
    '''

elif gentype=='BB':
    masses['1000005'] = float( mass_string.split('_')[0] )
    masses['1000022'] = float( mass_string.split('_')[2] )
    masses['1000023'] = float( mass_string.split('_')[1] )
    process = '''
    generate p p > b1 b1~ $ go susysq susysq~ @1
    add process p p > b1 b1~ jb $ go susysq susysq~ @2
    '''

# This comes after all Simplified Model setup files
evgenLog.info('Will use Pythia8...')

#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
pythia = genSeq.Pythia8



evgenConfig.contact  = [ "emma.sian.kuwertz@cern.ch" ]
if 'GG' in gentype:
    evgenConfig.keywords += ['simplifiedModel','gluino','Z']
    if 'bb' in runArgs.jobConfig[0]:
        evgenConfig.description = 'gluino production, glu->bb+~chi20, chi20->Z+LSP in simplified model, m_glu = %s GeV, m_N2 = %s GeV, m_N1 = %s GeV'%(masses['1000021'],masses['1000023'],masses['1000022'])
    else:
        evgenConfig.description = 'gluino production, glu->qq+~chi20, chi20->Z+LSP in simplified model, m_glu = %s GeV, m_N2 = %s GeV, m_N1 = %s GeV'%(masses['1000021'],masses['1000023'],masses['1000022'])
elif 'SS' in gentype:
    evgenConfig.keywords += ['simplifiedModel','squark','Z']
    evgenConfig.description = 'squark production, sq->q+~chi20, chi20->Z+LSP in simplified model, m_sq = %s GeV, m_N2 = %s GeV, m_N1 = %s GeV'%(masses['1000001'],masses['1000023'],masses['1000022'])
elif 'BB' in gentype:
    evgenConfig.keywords += ['simplifiedModel','sbottom','Z']
    evgenConfig.description = 'sbottom production, sb->b+~chi20, chi20->Z+LSP in simplified model, m_sbottom = %s GeV, m_N2 = %s GeV, m_N1 = %s GeV'%(masses['1000005'],masses['1000023'],masses['1000022'])

njets = 1
# Two-lepton filter
if '2L' in runArgs.jobConfig[0]:
    evt_multiplier = 15
    include('GeneratorFilters/MultiLeptonFilter.py')
    MultiLeptonFilter = filtSeq.MultiLeptonFilter
    MultiLeptonFilter.Ptcut = 8000.
    MultiLeptonFilter.Etacut = 2.8
    MultiLeptonFilter.NLeptons = 2

include('MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py')

if gentype=='SS':
    pythia.Commands += ["Merging:Process = pp>{ul,1000002}{ul~,-1000002}{ur,2000002}{ur~,-2000002}{dl,1000001}{dl~,-1000001}{dr,2000001}{dr~,-2000001}{sl,1000003}{sl~,-1000003}{sr,2000003}{sr~,-2000003}{cl,1000004}{cl~,-1000004}{cr,2000004}{cr~,-2000004}"]
elif gentype=='GG':
    pythia.Commands += ["Merging:Process = pp>{go,1000021}{go,1000021}"]
elif gentype=='BB':
    pythia.Commands += ["Merging:Process = pp>{b1,1000005}{b1~,-1000005}"]


