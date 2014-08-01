#
# CTEQ6L with UE tuning by Arthur Moraes
#
# author: B-Physics group, 2006-11-09
#
#--------------------------------------------------------------------------
PythiaB = topAlg.PythiaB
#PythiaB.PythiaCommand += [
# eta ranges 
#                          "pysubs ckin 9 -4.5",
#                          "pysubs ckin 10 4.5",
#                          "pysubs ckin 11 -4.5",
#                          "pysubs ckin 12 4.5",
# no B-B mixing
#                          "pydat1 mstj 26 0",
# parton decay length scheme:
# particle decayed if average proper lifetime
# c\tau < PARJ(71): default PARJ(71):= 10 mm
#                          "pydat1 mstj 22 2",
# Peterson / SLAC fragmentaton function for b-quark (def: -0.005)
#                          "pydat1 parj 55 -0.006",
# Excited B-meson parameters
#                          "pydat1 parj 13 0.65",
#                          "pydat1 parj 14 0.12",
#                          "pydat1 parj 15 0.04",
#                          "pydat1 parj 16 0.12",
#                          "pydat1 parj 17 0.2"
#                         ]

PythiaB.PythiaCommand += [
    "pysubs ckin 9 -4.5",
    "pysubs ckin 10 4.5",
    "pysubs ckin 11 -4.5",
    "pysubs ckin 12 4.5",
    "pydat1 mstj 26 0",
    "pydat1 mstj 22 2",
    "pydat1 parj 13 0.65",
    "pydat1 parj 14 0.12",
    "pydat1 parj 15 0.04",
    "pydat1 parj 16 0.12",
    "pydat1 parj 17 0.2",
    "pydat1 parj 55 -0.006",
    ]


#########################################################################################
####################### Arthur Moraes tuning (for reference) ############################

# lambda value for alpha_s
#                          "pydat1 parj 81 0.14",
# effects the amount of color reconnection
#                          "pypars parp 78 0.2",
# suppression probability for string pieces reattaching beam remnant 
# to hard scatter
#                          "pypars parp 80 0.01",
# effective pT_min for MI (pt0 scale)
#			  "pypars parp 82 1.9",
# matter distribution
#			  "pypars parp 83 0.3",
#			  "pypars parp 84 0.5",
# pT0 scale
#			  "pypars parp 89 1800",
#			  "pypars parp 90 0.22",
# ISR
#                          "pypars mstp 70 2",
# max scale for FSR dipoles
#                          "pypars mstp 72 0",
# new multiple interaction model and new parton shower selected
#                          "pypars mstp 81 1",
# MPI
#                          "pypars mstp 82 4",
# ISR
#                          "pypars mstp 84 1",
# FSR
#                          "pypars mstp 85 1",
# MPI
#                          "pypars mstp 86 2",
# sea quarks: large x behavior of assumed gluon distribution
#                          "pypars mstp 87 4",
# color reconnection 
#                          "pypars mstp 88 0",
#                          "pypars mstp 89 1",
#                          "pypars mstp 90 1",
#                          "pypars mstp 95 1",
#                          
#########################################################################################
#########################################################################################
