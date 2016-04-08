evgenConfig.description = "Heavy ion electromagnetic dissociation (EDM) with neutrons in the final state"
evgenConfig.keywords    = ["heavy ion", "EDM"] 
evgenConfig.contact     = ["Antonello Sbrizzi"]
evgenConfig.generators  = ["Reldis"]

from Reldis_i.Reldis_iConf import Reldis_i
topAlg += Reldis_i()
topAlg.Reldis_i.ReldisRootFileName = "/afs/cern.ch/atlas/offline/external/reldis/v0.0/test/inter/root/_cwn.root"
topAlg.Reldis_i.ReldisTreeName     = "h2032"

evgenConfig.generators += ["EvtGen"]
evgenConfig.auxfiles   += ['inclusiveP8.dec', 'inclusiveP8.pdt']

from EvtGen_i.EvtGen_iConf import EvtInclusiveDecay
topAlg += EvtInclusiveDecay()
topAlg.EvtInclusiveDecay.OutputLevel = INFO
topAlg.EvtInclusiveDecay.decayFile   = "inclusiveP8.dec"
topAlg.EvtInclusiveDecay.pdtFile     = "inclusiveP8.pdt"

from TruthExamples.TruthExamplesConf import TestHepMC
topAlg += TestHepMC()
topAlg.TestHepMC.EnergyDifference = 1e10
