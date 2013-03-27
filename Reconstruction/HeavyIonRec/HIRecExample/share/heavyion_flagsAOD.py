# Flags for Heavy Ion AOD production
# ----------------------------------
include.block ('HIRecExample/heavyion_flagsAOD.py')

from HIRecExample.HIRecExampleFlags import jobproperties

if not jobproperties.HIRecExampleFlags.ppMode:
  print "Applying HI AOD flags mods"

  # AO (standard JetRec turned off)
  from JetRec.JetRecFlags import jetFlags
  jetFlags.Enabled = False

  # running CaloTopoClusters for monitoring purposes
  if rec.doMonitoring() :
    from CaloRec.CaloRecFlags import jobproperties
    jobproperties.CaloRecFlags.Enabled.set_Value_and_Lock(True) 
    jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(True)
    jobproperties.CaloRecFlags.doCaloEMTopoCluster.set_Value_and_Lock(True)

  from ParticleBuilderOptions.AODFlags import AODFlags

  AODFlags.Photon = True
  AODFlags.Electron = True
  AODFlags.Muon = True
  AODFlags.Tau = False
  AODFlags.ParticleJet = False
  AODFlags.JetTag = False
  #AODFlags.SpclMC = True
  AODFlags.TruthParticleJet = True
  AODFlags.Trigger = False
  AODFlags.Streaming = False

  AODFlags.TauRec = False
  AODFlags.Tau1p3p = False
  AODFlags.TauTrackSlimmer = False

  # almost always (check AODFlags.py)
  AODFlags.FastSimulation = False
  AODFlags.MissingEtTruth = False
  AODFlags.FastTrackParticle = False
