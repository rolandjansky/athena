# Flags for Heavy Ion AOD production
# ----------------------------------
include.block ('HIRecExample/heavyion_flagsAOD.py')

from HIRecExample.HIRecExampleFlags import jobproperties
from RecExConfig.RecFlags import rec

if not jobproperties.HIRecExampleFlags.ppMode:
  print ("Applying HI AOD flags mods")

  rec.doTau = False

  # AO (standard JetRec turned off)
  from JetRec.JetRecFlags import jetFlags
  jetFlags.Enabled = False

  # AO InDet 2015
  InDetFlags.cutLevel = 3
  InDetFlags.doSGDeletion = False

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

  # almost always (check AODFlags.py)
  AODFlags.FastSimulation = False
  AODFlags.MissingEtTruth = False
  AODFlags.FastTrackParticle = False

  AODFlags.ThinNegativeEnergyNeutralPFOs.set_Value_and_Lock(False); 
  AODFlags.ThinInDetForwardTrackParticles.set_Value_and_Lock(False); 
  AODFlags.AddEgammaMuonTracksInAOD.set_Value_and_Lock(False);
  AODFlags.ThinNegativeEnergyCaloClusters.set_Value_and_Lock(False);
