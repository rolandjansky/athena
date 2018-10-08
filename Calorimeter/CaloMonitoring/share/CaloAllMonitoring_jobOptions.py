include.block("CaloMonitoring/CaloAllMonitoring_jobOptions.py")

#################################################################
#
# Steers LAr monitoring based on beam type and Commissioning switches.  
# Rob McPherson, first version 30 Sept 2008
#
# 20 Oct: move LAr monitoring to LArMonTools/LArAllMonitoring_jobOptions.py 
#  Note: the LArMon and CaloMon assignments to LArmonMan are slightly
#        ugly, but mean we can keep the head version compatible
#        with old Athena releases.
#
#################################################################
if DQMonFlags.monManEnvironment() == 'tier0Raw':
     caloESDMon=False
else: # it should be 'tier0'
     caloESDMon=True

if caloESDMon:
  try:
    include("CaloMonitoring/RecBackgroundForCaloMon_jobOptions.py")
  except Exception:
    treatException("CaloAllMonitoring_jobOptions.py: exception when setting up RecBackgroundForCaloMon")

if 'doCaloCellVecMon' not in dir():
   doCaloCellVecMon=True

if doCaloCellVecMon:
   try:
     include ("LArCellRec/LArCollisionTime_jobOptions.py")
   except Exception:
     treatException("CaloAllMonitoring_jobOptions.py: exception when setting up LArCollisionTime")

try:
    CaloMon = AthenaMonManager(name                = "CaloMonManager",
                               FileKey             = DQMonFlags.monManFileKey(),
                               Environment         = DQMonFlags.monManEnvironment(),
                               ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                               DataType            = DQMonFlags.monManDataType())
    topSequence += CaloMon

    from AthenaCommon.BeamFlags import jobproperties
    from RecExConfig.RecFlags import rec
    from AthenaMonitoring.DQMonFlags import DQMonFlags

    if jobproperties.Beam.beamType() == 'cosmics':
       if caloESDMon:
         if doCaloCellVecMon:
              include ("CaloMonitoring/CaloCellVecMonCosmics_jobOpt.py")
         include("CaloMonitoring/CaloClusterVecMonCosmics_jobOpt.py")
         include("CaloMonitoring/EMClusterVecMonCosmics_jobOpt.py")
         if rec.triggerStream()=='CosmicCalo' or rec.triggerStream()=='ZeroBias':
              include("CaloMonitoring/CaloBaselineMon_jobOpt.py")

    else:  # should be jobproperties.Beam.beamType() =='collisions' or 'singlebeam'
       if caloESDMon:
         if doCaloCellVecMon:
              include("CaloMonitoring/CaloCellVecMonCollisions_jobOpt.py")
         include("CaloMonitoring/CaloClusterVecMonCollisions_jobOpt.py")
         include("CaloMonitoring/EMClusterVecMonCollisions_jobOpt.py")
         if rec.triggerStream()=='CosmicCalo' or rec.triggerStream()=='ZeroBias':
              include("CaloMonitoring/CaloBaselineMon_jobOpt.py")

except Exception:
         treatException("CaloAllMonitoring_jobOptions.py: exception when setting up Calo monitoring")
