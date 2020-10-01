#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = 'POWHEG+Pythia8 ttbar production with Powheg hdamp equal 1.5*top mass, A14 tune, at least one lepton filter, ME NNPDF30 NLO, A14 NNPDF23 LO with shower weights and kinematic filtering'
evgenConfig.keywords    = [ 'SM', 'top', 'ttbar', 'lepton']
# The contact for the slice configuration
evgenConfig.contact     = [ 'ZLMarshall@lbl.gov' ]

#--------------------------------------------------------------
# Pythia8 showering - identical to 410470
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
# Semi-leptonic decay filter
include('GeneratorFilters/TTbarWToLeptonFilter.py')
filtSeq.TTbarWToLeptonFilter.NumLeptons = -1
filtSeq.TTbarWToLeptonFilter.Ptcut = 0.

# Depending on the DSID, set up MET/HT filters
thisDSID = runArgs.runNumber
HT1k5_filter    = [407342]
HT1k_1k5_filter = [407343]
HT6c_1k_filter  = [407344]
MET100_filter   = [345935]
MET200_filter   = [407345]
MET300_filter   = [407346]
MET400_filter   = [407347]
LJet_Vpt_100200 = [345951]
LJet_Vpt_200    = [346031]

if thisDSID in LJet_Vpt_100200:
    include('GeneratorFilters/TTbarLJetsVptFilter.py')
    filtSeq.TTbarLJetsVptFilter.Ptcut =  22000.
    filtSeq.TTbarLJetsVptFilter.Etacut =  3.0
    filtSeq.TTbarLJetsVptFilter.VptMin = 100000
    filtSeq.TTbarLJetsVptFilter.VptMax = 200000
    filtSeq.TTbarLJetsVptFilter.FollowTaus=True

if thisDSID in LJet_Vpt_200:
    include('GeneratorFilters/TTbarLJetsVptFilter.py')
    filtSeq.TTbarLJetsVptFilter.Ptcut =  22000.
    filtSeq.TTbarLJetsVptFilter.Etacut =  3.0
    filtSeq.TTbarLJetsVptFilter.VptMin = 200000
    filtSeq.TTbarLJetsVptFilter.FollowTaus=True

if thisDSID in MET100_filter:
    include('GeneratorFilters/MissingEtFilter.py')
    filtSeq.MissingEtFilter.METCut = 100*GeV
    filtSeq.MissingEtFilterUpperCut.METCut = 200*GeV

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
    filtSeq.HTFilter.MaxHT = 9000.*GeV # Max HT to keep event
    filtSeq.HTFilter.UseLeptonsFromWZTau = True # Include e/mu from the MC event in the HT
    filtSeq.HTFilter.MinLeptonPt = 25.*GeV # Min pT to consider muon in HT
    filtSeq.HTFilter.MaxLeptonEta = 2.5 # Max eta to consider muon in HT
