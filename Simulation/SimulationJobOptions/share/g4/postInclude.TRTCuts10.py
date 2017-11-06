#########################################################
#
# SimulationJobOptions/preInclude.TRTCuts10.py
#
# For a special production to evaluate the effect of low
# energy deltas on TRT threshold tuning.
#
#########################################################

atlasG4log.info("G4 TRT Config: Setting TRT cut")
rom AthenaCommon.CfgGetter import getService
getService('DetectorGeometrySvc').RegionCreators['TRTPhysicsRegionTool'].ElectronCut=10. #FIXME Units?!
# FIXME: Is it correct to alter only the e- cut and leave the e+ and
# photon cuts unchanged?
