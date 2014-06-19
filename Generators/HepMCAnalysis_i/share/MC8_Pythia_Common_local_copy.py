###############################################################
# job option fragments for year 2008 10TeV production
# U.E. tuning parameters are for PYTHIA 6.417
# contact: Osamu Jinnouchi
###############################################################

#from Pythia_i.Pythia_iConf import Pythia
#topAlg += Pythia()
job.Pythia.PythiaCommand = [
    # initializations
    # "pyinit win 10000.",    # CM energy (comment out: defined in JobTransform)
    "pyinit pylisti 12",
    "pyinit pylistf 1",
    "pystat 1 3 4 5",
    "pyinit dumpr 1 5",
    # mass
    "pydat2 pmas 6 1 172.5",    # PDG2007 TOP mass 
    "pydat2 pmas 24 1 80.403",  # PDG2007 W mass
    "pydat2 pmas 24 2 2.141",   # PDG2007 W width
    "pydat2 pmas 23 1 91.1876", # PDG2007 Z0 mass
    "pydat2 pmas 23 2 2.4952",  # PDG2007 Z0 width
    # misc
    #sj#"pypars mstp 128 1", # fix junk output for documentary particles
    #sj# next two lines added by me, with them I got the top decays back
    "pypars mstp 128 2", # fix junk output for documentary particles
    "pypars mstp 125 2", # fix junk output for documentary particles

    "pydat1 mstu 21 1", # prevent Pythia from exiting when it exceeds its errors limit 
    # main switch
    "pypars mstp 81 21",  # treatment for MI, ISR, FSR and beam remnants: MI on, new model
    "pypars mstp 82 4",   # multiple interaction model: double Gaussian matter dist
    # U.E. main switches
    "pypars mstp 70 0",   # (was 2: def manual 1, def code 0) virtuality scale for ISR 
    "pypars mstp 72 1",   # (was 0: def 1) maximum scale for FSR
    "pypars mstp 88 1",   # (was 0: def 1) strategy for qq junction to di-quark or baryon in beam remnant
    "pypars mstp 90 0",   # (was 1: def 0) strategy of compensate the primordial kT
    # U.E. tuning parameters
    "pypars parp 78 0.3",  # the amount of color reconnection in the final state
    "pypars parp 80 0.1",  # probability of color partons kicked out from beam remnant
    "pypars parp 82 2.1",  # cut off scale
    "pypars parp 83 0.8",  # matter distribution
    "pypars parp 84 0.7",  # matter distribution
    "pypars parp 90 0.16", # exponent of energy dependence
    "pydat1 parj 81 0.29", # (was 0.14: def 0.29) Labmda value in running alpha_s for parton showers
    "pypars mstp 95 1",    # (was 1; def 1) minimize the total string length  
    # PDF
    "pypars mstp 52 2",     #  external PDF library
    "pypars mstp 54 2",     #  external PDF library
    "pypars mstp 56 2",     #  external PDF library
    "pypars mstp 51 10042", #  CTEQ6L1 (LO fit with LO alpha_s)
    "pypars mstp 53 10042", #  CTEQ6L1 (LO fit with LO alpha_s)
    "pypars mstp 55 10042", #  CTEQ6L1 (LO fit with LO alpha_s)
    # Fragmentation
    "pydat1 mstj 11 3",       #  hybrid: Peterson for c/b, symmetric Lund for lightq
    "pydat1 mstj 22 2",       #  Make K_s, Lambda stable, ctau up to 10mm
    "pydat1 parj 54 -0.07",   #  c hadronization
    "pydat1 parj 55 -0.006",  #  b hadronization 
    # Others (redundant: just overwriting hard coded
    #         parameters, anyway they are Pythia default)
    "pypars mstp 84 1",   # 
    "pypars mstp 85 1",   # 
    "pypars mstp 86 2",   # 
    "pypars mstp 87 4",   # 
    "pypars mstp 89 1",   # 
    "pypars parp 89 1800."# 
    ]

## #pdf Pythia CTEQ6L1
## job.Pythia.PythiaCommand = ["pypars mstp 52 2",
##                             "pypars mstp 54 2",
##                             "pypars mstp 56 2",
##                             "pypars mstp 51 10042",
##                             "pypars mstp 53 10042",
##                             "pypars mstp 55 10042",]
