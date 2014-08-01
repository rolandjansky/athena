if rec.doESD():
    from InDetRecExample.InDetJobProperties import InDetFlags
    InDetFlags.doLowPt.set_Value_and_Lock(True)
    InDetFlags.primaryVertexSetup.set_Value_and_Lock("DefaultFastFinding")
    InDetFlags.useBeamConstraint.set_Value_and_Lock(False)
    
