include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

# These job options are designed to run MadGraph to make an LHE file and not to run a showering generator afterwards. 
# Because Generate_tf requires an output file, we need to fake it a bit, which is why we run Pythia8 on the first LHE event.
evgenConfig.contact  = [ "emma.sian.kuwertz@cern.ch" ]
njets = 2
evt_multiplier = 2400 

# This is to fake the system into not spending all its time running Pythia8, since we intend
# to throw out the EVNT file
evgenConfig.minevents = 1

gentype=runArgs.jobConfig[0].split('SM')[1].split('_')[1]
mass_string = runArgs.jobConfig[0].replace('.py','').split('_')[-1]
decaytype = 'direct'

if gentype=='GG':
    masses['1000021'] = float( mass_string )
    masses['1000022'] = 0.5 # just set to 0.5 here, will be overwritten when used for showering
    process = '''
    generate p p > go go $ susysq susysq~ @1
    add process p p > go go j $ susysq susysq~ @2
    add process p p > go go j j $ susysq susysq~ @3
    '''
    evgenConfig.keywords += ['simplifiedModel','gluino']
    evgenConfig.description = 'gluino production, m_glu = %s GeV'%(masses['1000021'])


elif gentype=='SS':
    masses['1000001'] = float( mass_string )
    masses['1000002'] = float( mass_string )
    masses['1000003'] = float( mass_string )
    masses['1000004'] = float( mass_string )
    masses['2000001'] = float( mass_string )
    masses['2000002'] = float( mass_string )
    masses['2000003'] = float( mass_string )
    masses['2000004'] = float( mass_string )
    masses['1000022'] = 0.5 # just set to 0.5 here, will be overwritten when used for showering
    process = '''
    generate p p > susysq susysq~ $ go @1
    add process p p > susysq susysq~ j $ go @2
    add process p p > susysq susysq~ j j $ go @3
    '''
    evgenConfig.keywords += ['simplifiedModel','squark']
    evgenConfig.description = 'squark production, m_squ = %s GeV'%(masses['1000001'])


include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if gentype=='GG':
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{go,1000021}{go,1000021}"]
else:
    genSeq.Pythia8.Commands += ["Merging:Process = guess"]
    if "UserHooks" in genSeq.Pythia8.__slots__.keys():
      genSeq.Pythia8.UserHooks += ['JetMergingaMCatNLO']
    else:
      genSeq.Pythia8.UserHook = 'JetMergingaMCatNLO'

evgenConfig.generators = ["MadGraph"]
