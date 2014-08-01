# example MC11 jO file that shows how to use PythiaResMod class
# - for W',Z',f + fbar -> G* [ISUB 141,142,391,392]:
# -- take out Breit-Wigner dependence +
# -- suppress low mass events from parton luminosities
# -- PythiaModified/pysgex.F modif. introduced by Oliver Stelzer-Chilton & Daniel Hayden, Exotics group

from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence("TopAlg")

from PythiaExo_i.PythiaResMod_iConf import PythiaResMod
topAlg += PythiaResMod()
PythiaResMod = topAlg.PythiaResMod

theApp.EvtMax = 100

#----------------------------------------------------------------------------------------------------------------------
# enable using the modified pysgex.F
#(default: UseResMod=0; do not use modified pysgex.F)
#(UseResMod=1; use modified pysgex.F for W',Z',f + fbar -> G*)
PythiaResMod.UseResMod=1


# Graviton resonance mass (in GeV)
GravitonMass = 1000

#other commands for PythiaResMod are as they are for Pythia alg.
# Z prime specific parameters for pythia :
PythiaResMod.PythiaCommand += [
                        "pysubs msel 0",
# 391 and 392 are the graviton subprocesses qqbar/gg
                        "pysubs msub 391 1",
                        "pysubs msub 392 1",
                        "pydat3 mdcy 347 1 1",
# I  5000039  Graviton*    (m =  1000.000 GeV)  -->
                        "pydat3 mdme 4084 1 0",
                        "pydat3 mdme 4085 1 0",
                        "pydat3 mdme 4086 1 0",
                        "pydat3 mdme 4087 1 0",
                        "pydat3 mdme 4088 1 0",
                        "pydat3 mdme 4089 1 0",
                        "pydat3 mdme 4090 1 0",
                        "pydat3 mdme 4091 1 0",
# select decay to e-e+:
                        "pydat3 mdme 4092 1 1",
                        "pydat3 mdme 4093 1 0",
                        "pydat3 mdme 4094 1 0",
                        "pydat3 mdme 4095 1 0",
                        "pydat3 mdme 4096 1 0",
                        "pydat3 mdme 4097 1 0",
                        "pydat3 mdme 4098 1 0",
                        "pydat3 mdme 4099 1 0",
                        "pydat3 mdme 4100 1 0",
                        "pydat3 mdme 4101 1 0",
                        "pydat3 mdme 4102 1 0",
                        "pydat3 mdme 4103 1 0",
# PARP(50) : (D=0.054) dimensionless coupling, which enters quadratically
#                     in all partial widths of the excited graviton G* resonance,
#                     is kappa_mG* = sqrt(2)*x1*k/MPl~, where x1~3.83 is the first zero
#                     of the J1 Bessel function and MPl~ is the modified
#                     Planck mass scale [Ran99, Bij01].
#                     default corresponds to k/Mpl~=0.01 in hep-ph/0006114
                        "pypars parp 50 0.54",
# cutoff for QED FSR in Pythia to 20000 GeV (='infinity', photos takes care of it).
                        "pydat2 pmas 5000039 1 "+str(GravitonMass)
                        ]

PythiaResMod.PythiaCommand +=  ["pydat1 parj 90 200000",
                                "pydat3 mdcy 15 1 0"] 

include ( "MC11JobOptions/MC11_Tauola_Fragment.py" )
include ( "MC11JobOptions/MC11_Photos_Fragment.py" )
