#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------

evgenConfig.description = 'POWHEG+Pythia8+EvtGen Wt production, HT/MET Filter, DR/DS scheme, inclusive, with Powheg hdamp equal 1.5*top mass, A14 tune, ME NNPDF30 NLO, A14 NNPDF23 LO'
evgenConfig.keywords    = [ 'SM', 'top', 'singleTop', 'Wt', 'inclusive']
evgenConfig.contact     = [ 'hass.abouzeid@cern.ch' ]

#--------------------------------------------------------------
# Pythia8 showering
#--------------------------------------------------------------
include('Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py')
include("Pythia8_i/Pythia8_Powheg_Main31.py")

genSeq.Pythia8.Commands += [ 'Powheg:pTHard = 0' ]
genSeq.Pythia8.Commands += [ 'Powheg:NFinal = 2' ]
genSeq.Pythia8.Commands += [ 'Powheg:pTdef = 2' ]
genSeq.Pythia8.Commands += [ 'Powheg:veto = 1' ]
genSeq.Pythia8.Commands += [ 'Powheg:vetoCount = 3' ]
genSeq.Pythia8.Commands += [ 'Powheg:pTemt  = 0' ]
genSeq.Pythia8.Commands += [ 'Powheg:emitted = 0' ]
genSeq.Pythia8.Commands += [ 'Powheg:MPIveto = 0' ]

#--------------------------------------------------------------
# Event filter
#--------------------------------------------------------------
# Depending on the DSID, set up MET/HT filters
thisDSID = runArgs.runNumber
HT1k5_filter    = [411183,411186,411189,411192]
HT1k_1k5_filter = [411182,411185,411188,411191]
HT6c_1k_filter  = [411181,411184,411187,411190]
MET200_filter   = [411193,411196,411199,411202]
MET300_filter   = [411194,411197,411200,411203]
MET400_filter   = [411195,411198,411201,411204]

if thisDSID in MET200_filter:
    include('GeneratorFilters/MissingEtFilter.py')
    filtSeq.MissingEtFilter.METCut = 200*GeV
    filtSeq.MissingEtFilterUpperCut.METCut = 300*GeV

if thisDSID in MET300_filter:
    include('GeneratorFilters/MissingEtFilter.py')
    filtSeq.MissingEtFilter.METCut = 300*GeV
    filtSeq.MissingEtFilterUpperCut.METCut = 400*GeV

if thisDSID in MET400_filter:
    include('GeneratorFilters/MissingEtFilter.py')
    filtSeq.MissingEtFilter.METCut = 400*GeV

if thisDSID in HT6c_1k_filter:
    include('GeneratorFilters/HTFilter.py')
    filtSeq.HTFilter.MinJetPt = 35.*GeV # Min pT to consider jet in HT
    filtSeq.HTFilter.MaxJetEta = 2.5 # Max eta to consider jet in HT
    filtSeq.HTFilter.MinHT = 600.*GeV # Min HT to keep event
    filtSeq.HTFilter.MaxHT = 1000.*GeV # Max HT to keep event
    filtSeq.HTFilter.UseLeptonsFromWZTau = True # Include e/mu from the MC event in the HT
    filtSeq.HTFilter.MinLeptonPt = 25.*GeV # Min pT to consider muon in HT
    filtSeq.HTFilter.MaxLeptonEta = 2.5 # Max eta to consider muon in HT

if thisDSID in HT1k_1k5_filter:
    include('GeneratorFilters/HTFilter.py')
    filtSeq.HTFilter.MinJetPt = 35.*GeV # Min pT to consider jet in HT
    filtSeq.HTFilter.MaxJetEta = 2.5 # Max eta to consider jet in HT
    filtSeq.HTFilter.MinHT = 1000.*GeV # Min HT to keep event
    filtSeq.HTFilter.MaxHT = 1500.*GeV # Max HT to keep event
    filtSeq.HTFilter.UseLeptonsFromWZTau = True # Include e/mu from the MC event in the HT
    filtSeq.HTFilter.MinLeptonPt = 25.*GeV # Min pT to consider muon in HT
    filtSeq.HTFilter.MaxLeptonEta = 2.5 # Max eta to consider muon in HT  

if thisDSID in HT1k5_filter:
    include('GeneratorFilters/HTFilter.py')
    filtSeq.HTFilter.MinJetPt = 35.*GeV # Min pT to consider jet in HT
    filtSeq.HTFilter.MaxJetEta = 2.5 # Max eta to consider jet in HT
    filtSeq.HTFilter.MinHT = 1500.*GeV # Min HT to keep event
    filtSeq.HTFilter.MaxHT = 20000.*GeV # Max HT to keep event
    filtSeq.HTFilter.UseLeptonsFromWZTau = True # Include e/mu from the MC event in the HT
    filtSeq.HTFilter.MinLeptonPt = 25.*GeV # Min pT to consider muon in HT
    filtSeq.HTFilter.MaxLeptonEta = 2.5 # Max eta to consider muon in HT
