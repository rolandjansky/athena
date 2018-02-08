#########################################################
#
# SimulationJobOptions/postInclude.TRTCuts_Ar15mm_Xe25um.py
#
# For a special production to evaluate the effect of high
# energy deltas on HIP trigger efficiency.
#########################################################

atlasG4log.info("G4 TRT Config: Setting TRT cut")
from AthenaCommon.CfgGetter import getService
import AthenaCommon.SystemOfUnits as Units
getService('DetectorGeometrySvc').RegionCreators['TRTPhysicsRegionTool'].ElectronCut=0.025*Units.mm
getService('DetectorGeometrySvc').RegionCreators['TRTPhysicsRegionTool'].PositronCut=0.025*Units.mm
getService('DetectorGeometrySvc').RegionCreators['TRT_ArPhysicsRegionTool'].ElectronCut=15.*Units.mm
getService('DetectorGeometrySvc').RegionCreators['TRT_ArPhysicsRegionTool'].PositronCut=15.*Units.mm
