# example MC11 jO file that shows how to use PythiaResMod class
# PythiaResMod class:
# * for W',Z' [ISUB 141,142]:
# * - take out Breit-Wigner dependence +
# * - supress low mass events from parton luminosities
# implementation: PythiaModified/pysgex.F

from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence("TopAlg")

from PythiaExo_i.PythiaResMod_iConf import PythiaResMod
topAlg += PythiaResMod()
PythiaResMod = topAlg.PythiaResMod

theApp.EvtMax = 10

#----------------------------------------------------------------------------------------------------------------------
# enable using the modified pysgex.F
#(default: UseResMod=0; do not use modified pysgex.F)
#(UseResMod=1; use modified pysgex.F)
PythiaResMod.UseResMod=1

# Zprime resonance mass (in GeV)
ZprimeMass = 1000

# Minimum mass for Drell-Yan production (in GeV)
ckin1 = 100

# relevant proc. setup
PythiaResMod.PythiaCommand += [
       "pysubs msel 0",
       "pysubs msub 141 1",   # Z',Z,g with interference
       # Z' decays - quarks
       "pydat3 mdme 289 1 0",
       "pydat3 mdme 290 1 0",
       "pydat3 mdme 291 1 0",
       "pydat3 mdme 292 1 0",
       "pydat3 mdme 293 1 0",
       "pydat3 mdme 294 1 0",
       # Z' decays - leptons
       "pydat3 mdme 297 1 0",
       "pydat3 mdme 298 1 0",
       "pydat3 mdme 299 1 1", #Z'-> mu+ mu-
       "pydat3 mdme 300 1 0",
       "pydat3 mdme 301 1 0", #Z'-> tau+ tau-
       "pydat3 mdme 302 1 0",
       # tau decays are left open
       "pysubs ckin 1 "+str(ckin1),  # sqrhat > 500
       #    "pysubs ckin 13 -3",  #
       #    "pysubs ckin 14 3",   # eta cuts
       #    "pysubs ckin 15 -3",  # |eta| < 3
       #    "pysubs ckin 16 3",   #
       "pydat1 mstu 1 0",
       "pydat1 mstu 2 0",
       "pydat2 pmas 32 1 "+str(ZprimeMass)
       ]

PythiaResMod.PythiaCommand +=  ["pydat1 parj 90 200000",
                                "pydat3 mdcy 15 1 0"]

include ( "MC11JobOptions/MC11_Tauola_Fragment.py" )
include ( "MC11JobOptions/MC11_Photos_Fragment.py" )
