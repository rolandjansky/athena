###############################################################
# PRODUCTION SYSTEM FRAGMENT
#       jobOptions for quarkonium production in the
#               NRQCD colour-octet framework
# Author:   Darren D Price ( Darren.Price@cern.ch )
# Date:     Jun 2006
# Modified: Oct 2006
#==============================================================

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence 
topAlg = AlgSequence("TopAlg") 

from Pythia_i.Pythia_iConf import Pythia
topAlg += Pythia()


from GeneratorFilters.GeneratorFiltersConf import BSignalFilter
topAlg += BSignalFilter()


# AtRndmGenSvc.ReadFromFile = true;
Pythia = topAlg.Pythia

Pythia.PythiaCommand += [
                   "pypars mstp 70 2",
                   "pypars mstp 72 0",
                   "pypars mstp 81 21",
                   "pypars mstp 82 4",
                   "pypars mstp 84 1",
                   "pypars mstp 85 1",
                   "pypars mstp 86 2",
                   "pypars mstp 87 4",
                   "pypars mstp 88 0",
                   "pypars mstp 89 1",
                   "pypars mstp 90 1",
                   "pypars mstp 95 1",
                   "pypars parp 78 0.2",
                   "pypars parp 80 0.01",
                   "pypars parp 82 1.9",
                   "pypars parp 83 0.3",
                   "pypars parp 84 0.5",
                   "pypars parp 89 1800",
                   "pypars parp 90 0.22",
                   "pydat1 parj 81 0.14"]

Pythia.PythiaCommand += [ # quarkonium processes
    
    #"pysubs msel 0",   # user chooses decay mode(s)
    "pysubs msel 61", # colour octet charmonium production (421-439)
    #"pysubs msel 62", # colour octet bottomonium production (461-479)
    #"pysubs msel 63", # colour octet onium production (msel 61+62)
    
    # ----- J/Psi production -----
    # --- 3S1(1)                        
    #"pysubs msub 421 1",  # gg->cc~[3S1(1)]+g
    # --- 3S1(8)
    #"pysubs msub 422 1",  # gg->cc~[3S1(8)]+g
    #"pysubs msub 425 1",  # gq->q+cc~[3S1(8)]
    #"pysubs msub 428 1",  # qq~->g+cc~[3S1(8)]
    # --- 1S0(8)                        
    #"pysubs msub 423 1",  # gg->cc~[1S0(8)]+g
    #"pysubs msub 426 1",  # gq->q+cc~[1S0(8)]
    #"pysubs msub 429 1",  # qq~->g+cc~[1S0(8)]
    # --- 3PJ(8)
    #"pysubs msub 424 1",  # gg->cc~[3PJ(8)]+g
    #"pysubs msub 427 1",  # gq->q+cc~[3PJ(8)]
    #"pysubs msub 430 1",  # qq~->g+cc~[3PJ(8)]

    # ----- Chi's -----
    #"pysubs msub 431 1",  # gg->cc~[3P0(1)]+g
    #"pysubs msub 432 1",  # gg->cc~[3P1(1)]+g
    #"pysubs msub 433 1",  # gg->cc~[3P2(1)]+g
    #"pysubs msub 434 1",  # qg->q+cc~[3P0(1)]
    #"pysubs msub 435 1",  # qg->q+cc~[3P1(1)]
    #"pysubs msub 436 1",  # qg->q+cc~[3P2(1)]
    #"pysubs msub 437 1",  # qq~->cc~[3P0(1)]+g
    #"pysubs msub 438 1",  # qq~->cc~[3P1(1)]+g
    #"pysubs msub 439 1",  # qq~->cc~[3P2(1)]+g
                        
    # ----- Upsilon production -----
    # --- 3S1(1)
    #"pysubs msub 461 1",  # gg->bb~[3S1(1)]+g
    # --- 3S1(8)
    #"pysubs msub 462 1",  # gg->bb~[3S1(8)]+g
    #"pysubs msub 465 1",  # gq->q+bb~[3S1(8)]
    #"pysubs msub 468 1",  # qq~->g+bb~[3S1(8)]
    # --- 1S0(8)
    #"pysubs msub 463 1",  # gg->bb~[1S0(8)]+g
    #"pysubs msub 466 1",  # gq->q+bb~[1S0(8)]
    #"pysubs msub 469 1",  # qq~->g+bb~[1S0(8)]
    # --- 3PJ(8)
    #"pysubs msub 464 1",  # gg->bb~[3PJ(8)]+g
    #"pysubs msub 467 1",  # gq->q+bb~[3PJ(8)]
    #"pysubs msub 470 1",  # qq~->g+bb~[3PJ(8)]

    # ----- Chi's -----
    #"pysubs msub 471 1",  # gg->bb~[3P0(1)]+g
    #"pysubs msub 472 1",  # gg->bb~[3P1(1)]+g
    #"pysubs msub 473 1",  # gg->bb~[3P2(1)]+g
    #"pysubs msub 474 1",  # qg->q+bb~[3P0(1)]
    #"pysubs msub 475 1",  # qg->q+bb~[3P1(1)]
    #"pysubs msub 476 1",  # qg->q+bb~[3P2(1)]
    #"pysubs msub 477 1",  # qq~->bb~[3P0(1)]+g
    #"pysubs msub 478 1",  # qq~->bb~[3P1(1)]+g
    #"pysubs msub 479 1",  # qq~->bb~[3P2(1)]+g
    
    ]                        

Pythia.PythiaCommand += [ # force decays

    "pydat3 mdme 858 1 0", # J/psi->e+e-
    "pydat3 mdme 859 1 1", # J/psi->mumu (br 0.06)
    "pydat3 mdme 860 1 0",  # J/psi->rndmflavpairs

    "pydat3 mdme 1501 1 1", # chi0c->J/psi gamma (br 0.007)
    "pydat3 mdme 1502 1 0", # chi0c->rfp 
    
    "pydat3 mdme 1555 1 1", # chi1c->J/psi gamma (br 0.273)
    "pydat3 mdme 1556 1 0", # chi1c->rfp
    
    "pydat3 mdme 861 1 1", # chi2c->J/psi gamma (br 0.135)
    "pydat3 mdme 862 1 0", # chi2c->rfp

    "pydat3 mdme 1034 1 0", # Upsilon->e+e- (br 0.0254)
    "pydat3 mdme 1035 1 1", # Upsilon->mu+mu- (br 0.0248)
    "pydat3 mdme 1036 1 0", # Upsilon->tau+tau- (br 0.0267)
    "pydat3 mdme 1037 1 0", # Upsilon->ddbar
    "pydat3 mdme 1038 1 0", # Upsilon->uubar
    "pydat3 mdme 1039 1 0", # Upsilon->ssbar
    "pydat3 mdme 1040 1 0", # Upsilon->ccbar
    "pydat3 mdme 1041 1 0", # Upsilon->ggg
    "pydat3 mdme 1042 1 0", # Upsilon->gamma gg
    
    "pydat3 mdme 1520 1 1", # chi0b->Upsilon gamma (br 0.02)
    "pydat3 mdme 1521 1 0", # chi0b->gg
    
    "pydat3 mdme 1565 1 1", # chi1b->Upsilon gamma (br 0.35)
    "pydat3 mdme 1566 1 0", # chi1b->gg
    
    "pydat3 mdme 1043 1 1", # chi2b->Upsilon gamma (br 0.22)
    "pydat3 mdme 1044 1 0", # chi2b->gg
    
    ]

Pythia.PythiaCommand += [ # NRQCD matrix elements
    
    "pypars parp 141 1.16",   # Jpsi-3S1(1) NRQCD ME
    "pypars parp 142 0.0119", # Jpsi-3S1(8) NRQCD ME
    "pypars parp 143 0.01",   # Jpsi-1S0(8) NRQCD ME
    "pypars parp 144 0.01",   # Jpsi-3P0(8) NRQCD ME / m_c^2
    "pypars parp 145 0.05",   # chi_c0-3P0(1) NRQCD ME / m_c^2

    "pypars parp 146 9.28",   # Upsilon-3S1(1) NRQCD ME
    "pypars parp 147 0.15",   # Upsilon-3S1(8) NRQCD ME
    "pypars parp 148 0.02",   # Upsilon-1S0(8) NRQCD ME
    "pypars parp 149 0.02",   # Upsilon-3P0(8) NRQCD ME / m_b^2 
    "pypars parp 150 0.085",  # chi_b0-3P0(1) NRQCD ME / m_b^2
        
    ]

#Pythia.PythiaCommand += [ ### Moraes tunings (here for reference only)

#        "pypars mstp 70 2",
#        "pypars mstp 72 0",

#            "pypars mstp 81 1",
#            "pypars mstp 82 4",

#            "pypars mstp 84 1",
#            "pypars mstp 85 1",
#            "pypars mstp 86 2",
#            "pypars mstp 87 4",
#            "pypars mstp 88 0",
#            "pypars mstp 89 1",
#            "pypars mstp 90 1",

#            "pypars mstp 95 1",

#            "pypars parp 78 0.2",
#            "pypars parp 80 0.01",
#            "pypars parp 82 1.9",
#            "pypars parp 83 0.3",
#            "pypars parp 84 0.5",
#            "pypars parp 89 1800",
#            "pypars parp 90 0.22",

#            "pydat1 parj 81 0.14",

#            ]


Pythia.PythiaCommand += [

    "pysubs ckin 3 4.5",   # lower pT cut on hard process in GeV

    #"pypars mstp 61 0", # inital state showers off
    #"pypars mstp 71 0", # final state showers off
    #"pypars mstp 111 0" # fragmentation/hadronisation off
    
    ]

Pythia.PythiaCommand += ["pyinit pylistf 1",
                        "pystat mstat 1", 
			"pyinit dumpr 0 3",#dump this event range to screen
                         ]


#------- Muon Trigger Cuts --------
BSignalFilter = topAlg.BSignalFilter
#-------------- Level 1 Muon Cuts --------------------- 
BSignalFilter.LVL1MuonCutOn = True
BSignalFilter.LVL1MuonCutPT = 6000.0 
BSignalFilter.LVL1MuonCutEta = 2.5 
#-------------- Level 2 lepton cuts -------------------
# These will only function if LVL1 trigger used. 
BSignalFilter.LVL2MuonCutOn = True 
BSignalFilter.LVL2MuonCutPT = 4000.0 
BSignalFilter.LVL2MuonCutEta = 2.5
try:
     Stream1.RequireAlgs += ["BSignalFilter"]
except Exception, e:
     pass

#==============================================================
#
# End of job options file
#
###############################################################
