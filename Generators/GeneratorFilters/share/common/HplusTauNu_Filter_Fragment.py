#--------------------------------------------------------------
# Filter on the H+->taunu process for either tau+jets (D1)
# or tau+lep (D2) channels. Can be used as an OR of the two
# if none of the above is specified.
# use:  JO name must contain "filter" field and optionally
# either "D1" or "D2"
# pawel.bruckman.de.renstrom@cern.ch
#--------------------------------------------------------------
#
# Setup Filters:
include("GeneratorFilters/AntiKt4TruthJets_pileup.py")
# if hasattr(runArgs,'filter'):
joboptparts = runArgs.jobConfig[0].strip('\.py').split('_')
# joboptparts = runArgs.jobConfig[0].split("_")
if 'filter' in joboptparts:
    
  from GeneratorFilters.GeneratorFiltersConf import TauFilter
  tauFilterD1 = TauFilter("tauFilterD1")
  filtSeq += tauFilterD1
  filtSeq.tauFilterD1.Ntaus = 1
  filtSeq.tauFilterD1.EtaMaxhad = 2.5
  filtSeq.tauFilterD1.Ptcuthad = 35000.0 #MeV
  filtSeq.tauFilterD1.Ptcute = 10000000.0 # turn off tau->el
  filtSeq.tauFilterD1.Ptcutmu = 10000000.0 # turn off tau->el

  tauFilterD2 = TauFilter("tauFilterD2")
  filtSeq += tauFilterD2
  filtSeq.tauFilterD2.Ntaus = 1
  filtSeq.tauFilterD2.EtaMaxhad = 2.5
  filtSeq.tauFilterD2.Ptcuthad = 25000.0 #MeV
  filtSeq.tauFilterD2.Ptcute = 10000000.0 # turn off tau->el
  filtSeq.tauFilterD2.Ptcutmu = 10000000.0 # turn off tau->el


  from GeneratorFilters.GeneratorFiltersConf import TruthJetFilter
  jetFilterD1 = TruthJetFilter("jetFilterD1")
  filtSeq += jetFilterD1
  filtSeq.jetFilterD1.TruthJetContainer = "AntiKt4TruthJets"
  filtSeq.jetFilterD1.NjetMaxEta = 2.6
  filtSeq.jetFilterD1.NjetMinPt = 18000.
  filtSeq.jetFilterD1.jet_pt1 = 18000.
  filtSeq.jetFilterD1.Njet = 4

  jetFilterD2e = TruthJetFilter("jetFilterD2e")
  filtSeq += jetFilterD2e
  filtSeq.jetFilterD2e.TruthJetContainer = "AntiKt4TruthJets"
  filtSeq.jetFilterD2e.NjetMaxEta = 2.6
  filtSeq.jetFilterD2e.NjetMinPt = 18000.
  filtSeq.jetFilterD2e.jet_pt1 = 18000.
  filtSeq.jetFilterD2e.Njet = 3

  jetFilterD2m = TruthJetFilter("jetFilterD2m")
  filtSeq += jetFilterD2m
  filtSeq.jetFilterD2m.TruthJetContainer = "AntiKt4TruthJets"
  filtSeq.jetFilterD2m.NjetMaxEta = 2.6
  filtSeq.jetFilterD2m.NjetMinPt = 18000.
  filtSeq.jetFilterD2m.jet_pt1 = 18000.
  filtSeq.jetFilterD2m.Njet = 2

  from GeneratorFilters.GeneratorFiltersConf import ParticleFilter
  bFilter = ParticleFilter("bFilter")
  filtSeq += bFilter
  filtSeq.bFilter.Ptcut = 18000.
  filtSeq.bFilter.Etacut = 2.6
  filtSeq.bFilter.PDG = 5
  filtSeq.bFilter.StatusReq =23
  filtSeq.bFilter.MinParts = 1

  from GeneratorFilters.GeneratorFiltersConf import MissingEtFilter
  METFilter = MissingEtFilter("METFilter")
  filtSeq += METFilter
  filtSeq.METFilter.METCut = 100000.
  filtSeq.METFilter.UseNeutrinosFromHadrons = True

  from GeneratorFilters.GeneratorFiltersConf import ElectronFilter
  elFilter = ElectronFilter("elFilter")
  filtSeq += elFilter
  filtSeq.elFilter.Ptcut = 25000.
  filtSeq.elFilter.Etacut = 2.6

  from GeneratorFilters.GeneratorFiltersConf import MuonFilter
  muFilter = MuonFilter("muFilter")
  filtSeq += muFilter
  filtSeq.muFilter.Ptcut = 25000.
  filtSeq.muFilter.Etacut = 2.6


  if 'D1' in joboptparts:
    filtSeq.Expression = "tauFilterD1 and bFilter and jetFilterD1 and METFilter"
  elif 'D2' in joboptparts:
    filtSeq.Expression = "tauFilterD2 and bFilter and ((jetFilterD2e and elFilter) or (jetFilterD2m and muFilter))"
  else:
    filtSeq.Expression = "bFilter and ((tauFilterD1 and jetFilterD1 and METFilter) or (tauFilterD2 and jetFilterD2e and elFilter) or (tauFilterD2 and jetFilterD2m and muFilter))"
