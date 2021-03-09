include.block("HIRecExample/hip_flagsESD.py")

# put here HeavyIon-proton specific ESD configuration
if rec.doESD :
     print "Applying HIP ESD flags mods"

     from RecExConfig.RecFlags import rec
     rec.Commissioning.set_Value_and_Lock(True)

     from InDetRecExample.InDetJobProperties import InDetFlags
     InDetFlags.doMinBias.set_Value_and_Lock(True)
     InDetFlags.doRefitInvalidCov.set_Value_and_Lock(False)
     InDetFlags.doRejectInvalidCov.set_Value_and_Lock(True)
     InDetFlags.perigeeExpression.set_Value_and_Lock('Vertex')
     InDetFlags.doSplitVertexFindingForMonitoring.set_Value_and_Lock(False)
     InDetFlags.writeRDOs.set_Value_and_Lock(True)

     from CaloTools.CaloNoiseFlags import jobproperties
     jobproperties.CaloNoiseFlags.FixedLuminosity.set_Value_and_Lock(0)

     from CaloRec.CaloCellFlags import jobproperties
     jobproperties.CaloCellFlags.doPileupOffsetBCIDCorr.set_Value_and_Lock(False)

     #these jet tools clash w/ HIJetRec
     from JetRec.JetRecFlags import jetFlags
     jetFlags.skipTools+=["jetisol","run1jetisol"]
