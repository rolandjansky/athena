###############################################################
#
# GRAVADD JOB OPTIONS FILE
#==============================================================
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
NumEvts = 100
##########################################################
theApp.EvtMax = NumEvts


from AthenaCommon.AlgSequence import AlgSequence
job=AlgSequence()
from PythiaExo_i.PythiaGS_iConf import PythiaGS
job +=PythiaGS()
job.PythiaGS.PythiaCommand =[
    "pydat1 mstu 21 1",
    "pysubs ckin 3 35.",
    "gravADD 2 7000.",
    "gravADD 4 1000.",
    "gravADD 3 4.",
    "pyinit user GRAVADD",
    "pypars mstp 52 2",     #  proton external PDF library
    "pypars mstp 54 2",     #  proton external PDF library
    "pypars mstp 56 2",     #  proton external PDF library
    "pypars mstp 51 10042", #  CTEQ6L
    "pypars mstp 53 10042", #  
    "pypars mstp 55 10042", #   
    "pydat2 pmas 6 1 172.5", # PDG2007 TOP mass
    "pydat2 pmas 24 1 80.403", # PDG2007 W mass
    "pydat2 pmas 24 2 2.141", # PDG2007 W width
    "pydat2 pmas 23 1 91.1876", # PDG2007 Z0 mass
    "pydat2 pmas 23 2 2.4952", # PDG2007 Z0 width 
    "pypars mstp 81 21", # MI new model
    "pypars mstp 82 4", # MI
    "pypars mstp 70 0", # regularization scheme for ISR
    "pypars mstp 72 1", # maximum scale for radiation off FSR dipoles
    "pypars mstp 90 0", # strategy to compensate the prim kt 
    "pypars parp 78 0.3", # parameter controlling the amount of colour reconnection in FS
    "pypars parp 80 0.1",
    "pypars parp 82 2.3", # regularization scale pt0 for spectrum MI
    "pypars parp 83 0.8", # parameters of the assumed matter overlap between the two colliding hadrons.
    "pypars parp 84 0.7",
    "pypars mstp 88 1" , # account for an assumed dominance of valence quarks at low transverse momentum scales
    "pypars parp 90 0.25",
    "pydat1 parj 81 0.29",
    "pypars mstp 95 6",
    "pydat1 mstj 11 4",
    "pydat1 mstj 22 2",
    "pydat1 parj 41 0.3",
    "pydat1 parj 42 0.58",
    "pydat1 parj 46 0.75",
    "pypars mstp 84 1",
    "pypars mstp 85 1",
    "pypars mstp 86 2",
    "pypars mstp 87 4",
    "pypars mstp 89 1",
    "pypars parp 89 1800"
    ] 

from EvgenJobOptions.PythiaGSEvgenConfig import evgenConfig

#
# End of job options file
#
###############################################################
