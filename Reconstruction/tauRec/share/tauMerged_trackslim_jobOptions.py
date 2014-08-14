from RecExConfig.RecFlags import rec 
if rec.ScopingLevel()<=3:
  from ParticleBuilderOptions.AODFlags import AODFlags
  if AODFlags.TauTrackSlimmer:
    from tauRec.tauTrackSlimmer import tauTrackSlimmer
    tauTrackSlimmer()

