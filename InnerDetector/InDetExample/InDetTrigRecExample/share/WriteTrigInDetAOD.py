#+++++++++++++++++ Beginning of WriteInDetAOD.py
StreamAOD.ItemList+=["VxContainer#*"]
StreamAOD.ItemList+=["Rec::TrackParticleContainer#*"]

if doTruth:
   StreamAOD.ItemList+=["TrackParticleTruthCollection#*"]
#+++++++++++++++++ End of WriteInDetAOD.py
