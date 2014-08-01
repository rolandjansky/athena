# W -> tau nu production with tau->mu mu mu deacy

from AthenaCommon.AlgSequence import AlgSequence 
topAlg = AlgSequence("TopAlg") 
# include ( "EvgenJobOptions/MC8_Pythia_Common.py" ) 

#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
from PythiaExo_i.PythiaTau3Mu_iConf import PythiaTau3Mu
topAlg += PythiaTau3Mu()
Pythia = topAlg.PythiaTau3Mu

Pythia.PythiaCommand       =  ["pysubs msel 0",          # Users decay choice.
                               "pydat1 mstj 28 2",       # use pytaud 
                               "pydat1 parj 90 20000",   # Turn off FSR.
                               "pysubs msub 2 1",        # Create W bosons.
                               "pydat3 mdme 190 1 0",
                               "pydat3 mdme 191 1 0",
                               "pydat3 mdme 192 1 0",
                               "pydat3 mdme 194 1 0",
                               "pydat3 mdme 195 1 0",
                               "pydat3 mdme 196 1 0",
                               "pydat3 mdme 198 1 0",
                               "pydat3 mdme 199 1 0",
                               "pydat3 mdme 200 1 0",
                               "pydat3 mdme 206 1 0",    # Switch for W->enu.
                               "pydat3 mdme 207 1 0",    # Switch for W->munu.
                               "pydat3 mdme 208 1 1",    # Switch for W->taunu.
                               "pydat3 mdme 209 1 0"]
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

from EvgenJobOptions.PythiaEvgenConfig import evgenConfig
evgenConfig.efficiency = 0.9
#from TruthExamples.TruthExamplesConf import TestHepMC
#topAlg +=  TestHepMC(CmEnergy=10000000.)

