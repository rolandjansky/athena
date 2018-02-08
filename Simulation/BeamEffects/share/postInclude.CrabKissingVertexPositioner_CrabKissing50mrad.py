from AthenaCommon.CfgGetter import getPrivateTool
ckvptool = getPrivateTool("CrabKissingVertexPositioner")
# all lengths are in mm
ckvptool.BunchShape = "GAUSS" # GAUSS or FLAT
ckvptool.BunchLength = 75.
ckvptool.Epsilon = 2.5e-3 # normalized emittance
ckvptool.BetaStar = 150. #
ckvptool.AlfaParallel = 50e-3 #40mrad, kissing angle
ckvptool.AlfaX = 295e-6 #rad, crabbing angle, we assume crossing angle is fully compensated
ckvptool.ThetaX = 295e-6 #rad, half crossing angle
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
job.BeamEffectsAlg.GenEventManipulators["GenEventVertexPositioner"].VertexShifters=[ckvptool]
