include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

masses['1000006'] = float(runArgs.jobConfig[0].split('_')[4])
masses['1000022'] = float(runArgs.jobConfig[0].split('_')[5].split('.')[0])
masses['1000024'] = masses['1000022'] + 5
masses['1000023'] = masses['1000022'] + 10 #  dM(N2,N1) = 2 * dM(C1,N1)

gentype = str(runArgs.jobConfig[0].split('_')[2])
decaytype = str(runArgs.jobConfig[0].split('_')[3])
extras['event_norm']='sum'
extras['use_syst']='F'


process = '''
generate p p > t~ t1 n1 $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @1
add process p p > t~ t1 n1 j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @2
add process p p > t~ t1 n1 j j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @3
'''
njets = 2 
evgenLog.info('Registered generation of antitop stop neutralino production, stop to bffN; grid point '+str(runArgs.runNumber)+' decoded into mass point ' + str(masses['1000006']))

if 'MadSpin' in runArgs.jobConfig[0].split('_')[-2]:
    evgenLog.info('Running w/ MadSpin option')
    madspin_card='madspin_card_test.dat'

    mscard = open(madspin_card,'w')

    decay_chains = [ "t1 > n1 fu fd~ b" , " t~ > b~ w-, w- > all"]

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
    decay %s
    decay %s
    # running the actual code
    launch"""%(runArgs.randomSeed,decay_chains[0],decay_chains[1]))

    mscard.close()




if 'MET100' in runArgs.jobConfig[0].split("_")[-1]:
    evgenLog.info('MET100 filter is applied')
    include ( 'GeneratorFilters/MissingEtFilter.py' )

    filtSeq.MissingEtFilter.METCut = 100*GeV

    evt_multiplier = 50.

elif 'MET150or1L20' in runArgs.jobConfig[0].split('_')[-1]:
    evgenLog.info('1Lepton or MET150 filter is applied')
    include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
    filtSeq.MultiElecMuTauFilter.MinPt  = 20000.
    filtSeq.MultiElecMuTauFilter.MaxEta = 2.8
    filtSeq.MultiElecMuTauFilter.NLeptons = 1
    filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0
    include ( 'GeneratorFilters/MissingEtFilter.py' )
    filtSeq.MissingEtFilter.METCut = 150*GeV
    filtSeq.Expression = "MultiElecMuTauFilter or MissingEtFilter"
    evt_multiplier = 10.


evgenConfig.contact  = [ "paola.arrubarrena@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel', 'stop']
evgenConfig.description = 'top stop neutralino production, st->bffN in simplified model, m_stop = %s GeV, m_neutralino = %s GeV'%(masses['1000006'],masses['1000022'])

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{t~,-6}{t1,1000006}{n1,1000022}"]



