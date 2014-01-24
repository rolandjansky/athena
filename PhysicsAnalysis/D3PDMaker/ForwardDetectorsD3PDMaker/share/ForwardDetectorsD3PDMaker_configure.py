from RecExConfig.RecFlags import rec
rec.doESD=True
rec.doAOD=False
rec.doCBNT=False
rec.doWriteESD=False
rec.doWriteAOD=False
rec.doWriteTAG=False

rec.doForwardDetectors = True

from ForwardDetectorsD3PDMaker.ForwardDetectorsD3PDMakerFlags import jobproperties
if jobproperties.ForwardDetectorsD3PDMakerFlags.reReco():
  rec.doESD.set_Value_and_Lock(True)
  rec.doWriteESD.set_Value_and_Lock(True)


from ForwardDetectorsD3PDMaker.ForwardDetectorsD3PDMakerFlags import jobproperties
if jobproperties.ForwardDetectorsD3PDMakerFlags.reReco():
  rec.doESD.set_Value_and_Lock(True)

  #from InDetRecExample.InDetJobProperties import InDetFlags
  #InDetFlags.disableInDetReco.set_Value_and_Lock(False)
  #InDetFlags.doHeavyIon.set_Value_and_Lock(True)
