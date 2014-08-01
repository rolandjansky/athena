# example MC11 jO file that shows how to use PythiaTopMdiff class
# PythiaTopMdiff class:
# In pyofsh.F and pyscat.F:
# - for  qqbar (gg) -> QQbar [ISUB 81 (82)]:     
# -- pyofsh.F : ISUB 81,82: set top partner to 3000006, spectrum to be filled via SLHA
# -- pyscat.F : account for qqbar (gg) -> QQbar vs qqbar (gg) -> QbarQ 

from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence("TopAlg")

from PythiaExo_i.PythiaTopMdiff_iConf import PythiaTopMdiff
topAlg += PythiaTopMdiff()
PythiaTopMdiff = topAlg.PythiaTopMdiff

theApp.EvtMax = 10

#----------------------------------------------------------------------------------------------------------------------
# enable using the modifications in 
#(default: UseTopMdiff=0; do not use TopMdiff modifications in pyofsh.F and pyscat.F)
#(UseTopMdiff=1; use modified )
PythiaTopMdiff.UseTopMdiff=1

PythiaTopMdiff.PythiaCommand += [ "pyinit win 7000" ]

# relevant proc. setup
PythiaTopMdiff.PythiaCommand += [ "pypars mstp 7 6",    # Heavy Flavour production top 
                                  "pymssm imss 1 11",   # Turn on use of SLHA
                                  # "pymssm imss 21 63",: done by default by Pythia_i  
                                  # "pymssm imss 22 63", : done by default by Pythia_i  
                                  #"pyinit dumpr 1 5",
                                  #"pyinit pylistf 1"]
                                  ]

PythiaTopMdiff.SusyInputFile = "t_tb162_5_test.slha"

PythiaTopMdiff.PythiaCommand +=  ["pydat1 parj 90 200000",
                                  "pydat3 mdcy 15 1 0"]
                                  
include ( "MC11JobOptions/MC11_Tauola_Fragment.py" )
include ( "MC11JobOptions/MC11_Photos_Fragment.py" )
