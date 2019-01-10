#--------------------------------------------------------------                                                                                                                                                          
# EVGEN configuration                                                                                                                                                                                                    
#--------------------------------------------------------------                                                                                                                                                          
evgenConfig.description = 'MG5_aMC@NLO+Pythia8 ttbar production A14 tune NNPDF23LO EvtGen from DSID 410440 LHE files with Shower Weights and MET/HT filter added '
evgenConfig.keywords    = [ 'SM', 'top', 'ttbar', 'lepton']
evgenConfig.contact     = [ 'keisuke.yoshihara@cern.ch' ]
evgenConfig.generators += ["aMcAtNlo","Pythia8"]

#if runArgs.trfSubstepName == 'generate' :
#  evgenConfig.inputfilecheck = "aMcAtNloPythia8EvtGen_MEN30NLO_A14N23LO_ttbar_incl_LHE"

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_aMcAtNlo.py")
#include("Pythia8_i/Pythia8_ShowerWeights.py")

#--------------------------------------------------------------
# Event filter
#--------------------------------------------------------------

include("GeneratorFilters/TTbarWToLeptonFilter.py")
filtSeq.TTbarWToLeptonFilter.NumLeptons = -1 #(-1: non-all had, 0: all had, 1: l+jets, 2: dilepton)
filtSeq.TTbarWToLeptonFilter.Ptcut = 0.0

# Depending on the DSID, set up MET/HT filters
thisDSID = runArgs.runNumber
HT1k5_filter    = [407348]
HT1k_1k5_filter = [407349]
HT6c_1k_filter  = [407350]
MET200_filter   = [407351]
MET300_filter   = [407352]
MET400_filter   = [407353]

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

