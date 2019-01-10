include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

for q in squarks: masses[q] = float(runArgs.jobConfig[0].split('_')[4])
masses['1000022'] = float(runArgs.jobConfig[0].split('_')[5].split('.')[0])
if masses['1000022']<0.5: masses['1000022']=0.5
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


######## Filter Section #######                                                                                                       
if 'MET100' in runArgs.jobConfig[0].split('_')[-1]:

    evgenLog.info('MET 100 GeV filter is applied')

    include ( 'GeneratorFilters/MissingEtFilter.py' )

    filtSeq.MissingEtFilter.METCut = 100*GeV
 
    evt_multiplier = 5.

evgenConfig.contact  = [ "takashi.yamanaka@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel', 'squark']
evgenConfig.description = 'squark production, sq->q+LSP in simplified model, m_sq = %s GeV, m_N1 = %s GeV'%(masses[squarks[0]],masses['1000022'])
evgenConfig.inputfilecheck = "_".join(runArgs.jobConfig[0].split("_")[2:5])

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = guess"]
    if "UserHooks" in genSeq.Pythia8.__slots__.keys():
        genSeq.Pythia8.UserHooks += ['JetMergingaMCatNLO']
    else:
        genSeq.Pythia8.UserHook = 'JetMergingaMCatNLO'
    #genSeq.Pythia8.Commands += ["Merging:Process = pp>{ul,1000002}{ul~,-1000002}{ur,2000002}{ur~,-2000002}{dl,1000001}{dl~,-1000001}{dr,2000001}{dr~,-2000001}{sl,1000003}{sl~,-1000003}{sr,2000003}{sr~,-2000003}{cl,1000004}{cl~,-1000004}{cr,2000004}{cr~,-2000004}"]
