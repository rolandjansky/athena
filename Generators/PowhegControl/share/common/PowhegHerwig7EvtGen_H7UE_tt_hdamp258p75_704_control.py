#---------------------------------------------------------------------------
# Title: PowhegHerwig7EvtGen_H7UE_tt_hdamp258p75_704_control.py
#-----------------------------------------------------------------------------

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = 'POWHEG+Herwig704 ttbar production with Powheg hdamp equal 1.5*top mass, H7UE tune, MET/HT filter, ME NNPDF30 NLO, H7UE MMHT2014 LO'
evgenConfig.keywords    = [ 'SM', 'top', 'ttbar', 'lepton']
evgenConfig.tune        = "MMHT2014"
evgenConfig.contact     = [ 'keisuke.yoshihara@cern.ch', 'khoo@cern.ch']
evgenConfig.generators += ["Powheg", "Herwig7", "EvtGen"]


#--------------------------------------------------------------
# Herwig7 (H7UE) showering
#--------------------------------------------------------------
# initialize Herwig7 generator configuration for showering of LHE files
include("Herwig7_i/Herwig7_LHEF.py")

# configure Herwig7
Herwig7Config.me_pdf_commands(order="NLO", name="NNPDF30_nlo_as_0118")
Herwig7Config.tune_commands()
Herwig7Config.lhef_powhegbox_commands(lhe_filename=runArgs.inputGeneratorFile, me_pdf_order="NLO")

# add EvtGen
include("Herwig7_i/Herwig7_EvtGen.py")

Herwig7Config.add_commands("""
set /Herwig/Shower/LtoLGammaSudakov:pTmin 0.000001
set /Herwig/Shower/QtoGammaQSudakov:Alpha /Herwig/Shower/AlphaQED 
""")

# run Herwig7
Herwig7Config.run()

#--------------------------------------------------------------
# Event filter
#--------------------------------------------------------------
# Semi-leptonic decay filter
include('GeneratorFilters/TTbarWToLeptonFilter.py')
filtSeq.TTbarWToLeptonFilter.NumLeptons = -1
filtSeq.TTbarWToLeptonFilter.Ptcut = 0.

# Depending on the DSID, set up MET/HT filters
thisDSID = runArgs.runNumber
MET200_filter   = [407357]
MET300_filter   = [407358]
MET400_filter   = [407359]

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

HTcuts = {
    407354: (1500, 9000),
    407355: (1000, 1500),
    407356: ( 600, 1000),
    410432: ( 500, 1000),
    410433: (1000, 1500),
    410446: (1500, 9000),
}

# Provide the HT cuts in GeV
def setupStandardHTFilter(HTrange):
    HTmin,HTmax = HTrange
    include('GeneratorFilters/HTFilter.py')
    htfilt = filtSeq.HTFilter
    htfilt.MinJetPt = 35.*GeV # Min pT to consider jet in HT
    htfilt.MaxJetEta = 2.5 # Max eta to consider jet in HT
    htfilt.UseLeptonsFromWZTau = True # Include e/mu from the MC event in the HT
    htfilt.MinLeptonPt = 25.*GeV # Min pT to consider muon in HT
    htfilt.MaxLeptonEta = 2.5 # Max eta to consider muon in HT
    filtSeq.HTFilter.MinHT = HTmin*GeV # Min HT to keep event
    filtSeq.HTFilter.MaxHT = HTmax*GeV # Max HT to keep event

if thisDSID in HTcuts.keys():
    setupStandardHTFilter(HTcuts[thisDSID])

isAllHad = thisDSID in [410432,410433,410446]
if isAllHad:
    filtSeq.Expression = "((not TTbarWToLeptonFilter) and HTFilter)"
