#Test job options to see how things work.                                                                                                        
include("VBFNLOControl/MC12_VBFNLO.py")

VbfnloConfig.lo_points = 21
VbfnloConfig.launchOnTheFly()

#--------------------------------------------------------------                                                                                  
# General MC12 configuration                                                                                                                     
#--------------------------------------------------------------                                                                                  
#to be changed

from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence("TopAlg")

evgenConfig.description = "VBFNLO VBF X->gamma gamma with UEEE3,CTEQ6L1 SpinCP 0p SM"
evgenConfig.keywords = ["vbf", "higgs", "gamma", "SpinCP"]

include("MC12JobOptions/Herwigpp_UEEE3_CTEQ6L1_LHEF_Common.py")
evgenConfig.generators += ["Lhef"]

cmds = """
create ThePEG::ParticleData graviton
setup graviton 39 G 125.0 0.0 0.0 0.0 3 0 1 0

decaymode graviton->gamma,gamma; 1.0 1 /Herwig/Decays/Mambo
"""

topAlg.Herwigpp.Commands += cmds.splitlines()
