
### Commands needed to run with calorimeters ###

from LArROD.LArRODFlags import larRODFlags
larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True)
larRODFlags.NumberOfCollisions.set_Value_and_Lock(60)

from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.bunchSpacing = 25

from CaloTools.CaloNoiseFlags import jobproperties
jobproperties.CaloNoiseFlags.FixedLuminosity.set_Value_and_Lock(60/2.9)
# was (60*1.45/8*50/25) until release 20.20.9

if rec.OutputFileNameForRecoStep() == 'RAWtoESD':
    from CaloRec.CaloCellFlags import jobproperties
    jobproperties.CaloCellFlags.doLArCellEmMisCalib = False
    from tauRec.tauRecFlags import tauFlags
    tauFlags.tauRecSeedMaxEta.set_Value_and_Lock(4.0)
