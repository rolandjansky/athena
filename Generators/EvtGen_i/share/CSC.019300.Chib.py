
try:
     Stream1.RequireAlgs += ["BSignalFilter"]
except Exception, e:
     pass

StoreGateSvc = Service( "StoreGateSvc" )

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence 
topAlg = AlgSequence("TopAlg") 

from Pythia_i.Pythia_iConf import Pythia
topAlg += Pythia()

from GeneratorFilters.GeneratorFiltersConf import BSignalFilter
topAlg += BSignalFilter()

from GeneratorFilters.GeneratorFiltersConf import BSignalFilter
topAlg += BSignalFilter()


#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
Pythia = topAlg.Pythia

Pythia.PythiaCommand = [        "pysubs msel 0",              #user chooses decay
                                "pysubs msub 471 1",          #gg->bb~[3P0(1)]+g
                                "pysubs msub 474 1",          #qg->q+bb~[3P0(1)]
# Force chi_b ->J/psi J/psi
                                "pydat3 mdme 1521 1 0",       #chi_0b->gg
                                "pydat3 mdme 1520 1 1",       #chi_0b->Y+gamma
                                "pydat3 kfdp 1520 1 443",     #chi_0b->J/psi+gamma
                                "pydat3 kfdp 1520 2 443",     #chi_0b->J/psi+J/psi


# J/Psi Decays (close ee-pairs and random flavours production, and open only mumu pairs production.

                                "pydat3 mdme 860 1 0",
                                "pydat3 mdme 859 1 1",
                                "pydat3 mdme 858 1 0",

# Other stuff (pysubs ckin 3 3- mean that i demand pt > 3 GeV

#                               "pyinit pylistf 1",
                                "pyinit dumpr 0 3",
                                "pysubs ckin 3 3"]

TopSequence = topAlg.TopSequence
TopSequence.Members  += ["Sequencer/Filter"]

#------- Muon Trigger Cuts --------
BSignalFilter = topAlg.BSignalFilter
#-------------- Level 1 Muon Cuts ---------------------
BSignalFilter.LVL1MuonCutOn = "true"
BSignalFilter.LVL1MuonCutPT = 1000.0
BSignalFilter.LVL1MuonCutEta = 2.5
#-------------- Level 2 lepton cuts -------------------
# These will only function if LVL1 trigger used.
BSignalFilter.LVL2MuonCutOn = "true"
BSignalFilter.LVL2MuonCutPT = 1000.0
BSignalFilter.LVL2MuonCutEta = 2.5

#==============================================================
#
# End of job options file
#
###############################################################

