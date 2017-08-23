from AthenaCommon.CfgGetter import getPrivateTool
ckvptool = getPrivateTool("CrabKissingVertexPositioner")
# all lengths are in mm
ckvptool.BunchShape = "GAUSS" # GAUSS or FLAT
ckvptool.BunchLength = 81.
ckvptool.Epsilon = 2.5e-3 # normalized emittance
ckvptool.BetaStar = 150. #
ckvptool.AlfaParallel = 3.3e-3 # kissing angle
ckvptool.AlfaX = 6e-3 #rad, crabbing angle
ckvptool.ThetaX = 0. #rad, half crossing angle
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
job.BeamEffectsAlg.GenEventManipulators["GenEventVertexPositioner"].VertexShifters=[ckvptool]
