include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )
masses['1000006'] = float(runArgs.jobConfig[0].split('_')[4])
masses['1000022'] = float(runArgs.jobConfig[0].split('_')[5].split('.')[0])
 
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

#remember to turn this one off!!
#keepOutput=False


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


evgenConfig.contact  = [ "primaver@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel', 'stop']
evgenConfig.description = 'stop direct pair production, st->b+ff+LSP in simplified model, m_stop = %s GeV, m_N1 = %s GeV'%(masses['1000006'],masses['1000022'])
 
if 'MadSpin' in runArgs.jobConfig[0].split('_'):
  evgenLog.info('Running w/ MadSpin option')
  madspin_card='madspin_card_test.dat'

  mscard = open(madspin_card,'w')

  decay_chains = [ "t1 > n1 fu fd~ b" , "t1~ > n1 fu~ fd b~" ]
  if '2L15' in runArgs.jobConfig[0].split('_')[-1] or 'LepBRonly' in runArgs.jobConfig[0].split('_'):
      evgenLog.info('Setting only leptonic decay chains')
      decay_chains = [ "t1 > n1 ll+ vl b" , "t1~ > n1 ll- vl~ b~" ]

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
  define ll- = e- mu- ta-
  define ll+ = e+ mu+ ta+
  decay %s
  decay %s
  # running the actual code
  launch"""%(runArgs.randomSeed,decay_chains[0],decay_chains[1]))

  mscard.close()
 
if 'm1002L3' in runArgs.jobConfig[0].split('_')[-1]:
  evgenLog.info('met1002Lep3GeV filter is applied')
  include ( 'GeneratorFilters/MissingEtFilter.py' )
  filtSeq.MissingEtFilter.METCut =100000.

  include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
  filtSeq.MultiElecMuTauFilter.MinPt  = 3000.
  filtSeq.MultiElecMuTauFilter.MaxEta = 2.8
  filtSeq.MultiElecMuTauFilter.NLeptons = 2
  filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0

  filtSeq.Expression = "(MultiElecMuTauFilter and MissingEtFilter)"

  evt_multiplier = 50
  if masses['1000006']-masses['1000022']<20: evt_multiplier=200
  elif masses['1000006']-masses['1000022']<30: evt_multiplier=100
 
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
 
elif '2L3' in runArgs.jobConfig[0].split('_')[-1]:
  evgenLog.info('2lepton3 filter is applied')
 
  include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
  filtSeq.MultiElecMuTauFilter.MinPt  = 3000.
  filtSeq.MultiElecMuTauFilter.MaxEta = 2.8
  filtSeq.MultiElecMuTauFilter.NLeptons = 2
  filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0

  filtSeq.Expression = "MultiElecMuTauFilter"

  evt_multiplier = 20
 
elif '2L15' in runArgs.jobConfig[0].split('_')[-1]:
  evgenLog.info('2lepton15 filter is applied')
 
  include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
  filtSeq.MultiElecMuTauFilter.MinPt  = 15000.
  filtSeq.MultiElecMuTauFilter.MaxEta = 2.8
  filtSeq.MultiElecMuTauFilter.NLeptons = 2
  filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0

  filtSeq.Expression = "MultiElecMuTauFilter"

  evt_multiplier = 50
  if masses['1000006']-masses['1000022']<40: evt_multiplier=150
  elif masses['1000006']-masses['1000022']<70: evt_multiplier=75
 
elif 'm100' in runArgs.jobConfig[0].split('_')[-1]:
  evgenLog.info('met100 filter is applied')
 
  include ( 'GeneratorFilters/MissingEtFilter.py' )
  filtSeq.MissingEtFilter.METCut =100000.
  filtSeq.Expression = "MissingEtFilter"

  evt_multiplier = 13
 
include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )
 
if njets>0:
  genSeq.Pythia8.Commands += ["Merging:Process = pp>{t1,1000006}{t1~,-1000006}"]
