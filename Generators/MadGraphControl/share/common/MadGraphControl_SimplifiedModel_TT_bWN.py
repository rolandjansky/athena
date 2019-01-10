include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

masses['1000006'] = float(runArgs.jobConfig[0].split('_')[4].split('.')[0].replace('d','.'))
masses['1000022'] = float(runArgs.jobConfig[0].split('_')[5].split('.')[0].replace('d','.'))

if masses['1000022']<0.5: masses['1000022']=0.5

gentype = str(runArgs.jobConfig[0].split('_')[2])
decaytype = str(runArgs.jobConfig[0].split('_')[3])

process = '''
generate p p > t1 t1~ $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @1
add process p p > t1 t1~ j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @2
add process p p > t1 t1~ j j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @3
'''
njets = 2
evgenLog.info('Registered generation of stop pair production, stop to t+LSP;grid point ' + str(runArgs.runNumber) + ' decoded into mass point ' + str(masses['1000006']))

#bug fix for syst calc issue in AGENE-1542
extras["use_syst"]='F'
extras["event_norm"]='sum'

##
## -- Some extra parameters to keep the run card consistent with the run_card from MG 2.4.3
##
#extras['drjl']="0.5"
#extras['asrwgtflavor']="5"
#extras['auto_ptj_mjj']="F"

#MadGraph 2.6.X uses 5-flav for merging
#force 4-flav merging as use a 4-flav scheme
extras['pdgs_for_merging_cut']='1, 2, 3, 4, 21, 1000001, 1000002, 1000003, 1000004, 1000021, 2000001, 2000002, 2000003, 2000004'


evgenConfig.contact  = [ "daniel.joseph.antrim@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel', 'stop']
evgenConfig.description = 'stop direct pair production, st->b+W+LSP in simplified model, m_stop = %s GeV, m_N1 = %s GeV'%(masses['1000006'],masses['1000022'])

if 'MadSpin' in runArgs.jobConfig[0].split('_'):
    evgenLog.info('Running w/ MadSpin option')
    madspin_card='madspin_card_test.dat'

    mscard = open(madspin_card,'w')

    mscard.write("""#************************************************************
    #*                        MadSpin                           *
    #*                                                          *
    #*    P. Artoisenet, R. Frederix, R. Rietkerk, O. Mattelaer *
    #*                                                          *
    #*    Part of the MadGraph5_aMC@NLO Framework:              *
    #*    The MadGraph5_aMC@NLO Development Team - Find us at   *
    #*    https://server06.fynu.ucl.ac.be/projects/madgraph     *
    #*                                                          *
    #************************************************************
    #Some options (uncomment to apply)
    set max_weight_ps_point 400  # number of PS to estimate the maximum for each event
    #
    set seed %i
    set spinmode none
    # specify the decay for the final state particles
    decay t1 > n1 fu fd~ b
    decay t1~ > n1 fu~ fd b~
    # running the actual code
    launch"""%runArgs.randomSeed)

    mscard.close()

if '1L15' in runArgs.jobConfig[0].split('_')[-1]:
    evgenLog.info('1Lepton15 filter is applied')
    include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
    filtSeq.MultiElecMuTauFilter.MinPt  = 15000.
    filtSeq.MultiElecMuTauFilter.MaxEta = 2.8
    filtSeq.MultiElecMuTauFilter.NLeptons = 1
    filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0
    filtSeq.Expression = "MultiElecMuTauFilter"
    evt_multiplier = 5

if '2L15' in runArgs.jobConfig[0].split('_')[-1]:
    evgenLog.info('2Lepton15 filter is applied')
    include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
    filtSeq.MultiElecMuTauFilter.MinPt  = 15000.
    filtSeq.MultiElecMuTauFilter.MaxEta = 2.8
    filtSeq.MultiElecMuTauFilter.NLeptons = 2
    filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0
    filtSeq.Expression = "MultiElecMuTauFilter"
    evt_multiplier = 50

elif '1L20orMET60' in runArgs.jobConfig[0].split('_')[-1]:
    evgenLog.info('1Lepton or MET60 filter is applied')
    include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
    filtSeq.MultiElecMuTauFilter.MinPt  = 20000.
    filtSeq.MultiElecMuTauFilter.MaxEta = 2.8
    filtSeq.MultiElecMuTauFilter.NLeptons = 1
    filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0
    include ( 'GeneratorFilters/MissingEtFilter.py' )
    filtSeq.MissingEtFilter.METCut = 60000.
    filtSeq.Expression = "MultiElecMuTauFilter or MissingEtFilter"
    evt_multiplier = 3

elif 'm1001L20' in runArgs.jobConfig[0].split('_')[-1]:
    evgenLog.info('1Lepton or MET100 filter is applied')
    include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
    filtSeq.MultiElecMuTauFilter.MinPt  = 20000.
    filtSeq.MultiElecMuTauFilter.MaxEta = 2.8
    filtSeq.MultiElecMuTauFilter.NLeptons = 1
    filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0
    include ( 'GeneratorFilters/MissingEtFilter.py' )
    filtSeq.MissingEtFilter.METCut = 100000.
    filtSeq.Expression = "MultiElecMuTauFilter or MissingEtFilter"
    evt_multiplier = 5

elif 'm100' in runArgs.jobConfig[0].split('_')[-1]:
    evgenLog.info('MET100 filter is applied')
    include ( 'GeneratorFilters/MissingEtFilter.py' )
    filtSeq.MissingEtFilter.METCut = 100*GeV
    filtSeq.Expression = "MissingEtFilter"
    evt_multiplier = 10.

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{t1,1000006}{t1~,-1000006}"]
