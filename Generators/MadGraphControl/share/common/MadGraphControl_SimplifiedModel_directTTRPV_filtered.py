include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

masses['1000006'] = float(runArgs.jobConfig[0].split('_')[4].split('.')[0])
gentype = str(runArgs.jobConfig[0].split('_')[2])
decaytype = str(runArgs.jobConfig[0].split('_')[3])
process = '''
generate p p > t1 t1~ $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @1
add process p p > t1 t1~ j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @2
add process p p > t1 t1~ j j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @3
'''

njets = 2

if masses['1000006'] < 250:
    evt_multiplier=50
    evgenConfig.minevents = 2000
if masses['1000006'] < 175: 
    evt_multiplier=200
    evgenConfig.minevents = 500
if masses['1000006'] < 150: 
    evt_multiplier=500
    evgenConfig.minevents = 100
if masses['1000006'] < 100:
    extras['ptj']=70 
    extras['auto_ptj_mjj']='F'
    evt_multiplier=500
    evgenConfig.minevents = 200
    
evgenLog.info('Registered generation of RPV stop pair production with UDD coupling, stop to b+s; grid point '+str(runArgs.runNumber)+' decoded into mass point ' + str(masses['1000006']))


evgenConfig.contact  = [ "simone.amoroso@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel', 'stop','RPV']
evgenConfig.description = 'stop direct pair production, RPV decay  st->b+s in simplified model'



if masses['1000006'] < 250:
    # Add phase space filters (4j80 OR ht800)
    include('GeneratorFilters/HTFilter.py')
    include('GeneratorFilters/QCDTruthMultiJetFilter.py')
    filtSeq.HTFilter.MinJetPt = 10.*GeV # Min pT to consider jet in HT
    filtSeq.HTFilter.MaxJetEta = 4.0 # Max eta to consider jet in HT
    filtSeq.HTFilter.UseLeptonsFromWZTau = True # Include e/mu from the MC event in the HT
    filtSeq.HTFilter.MinLeptonPt = 5.*GeV # Min pT to consider muon in HT
    filtSeq.HTFilter.MaxLeptonEta = 4.0 # Max eta to consider muon in HT
    filtSeq.HTFilter.MinHT = 800.*GeV # Min HT to keep event
    filtSeq.HTFilter.MaxHT = 13000.*GeV # Max HT to keep event
    
    filtSeq.QCDTruthMultiJetFilter.Njet = 4 # Min pT to consider jet in HT
    filtSeq.QCDTruthMultiJetFilter.NjetMinPt = 80*GeV # Max eta to consider jet in HT
    filtSeq.QCDTruthMultiJetFilter.MinLeadJetPt = 80*GeV  # Include e/mu from the MC event in the HT
    filtSeq.QCDTruthMultiJetFilter.MaxLeadJetPt = 13000*GeV # Min pT to consider muon in HT
    filtSeq.QCDTruthMultiJetFilter.MaxEta = 4.0 # Max eta to consider muon in HT
    filtSeq.QCDTruthMultiJetFilter.TruthJetContainer = "AntiKt4TruthJets" # Min HT to keep event
    
    filtSeq.Expression = "(QCDTruthMultiJetFilter or HTFilter)" 



include ('MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py')
if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{t1,1000006}{t1~,-1000006}"]



