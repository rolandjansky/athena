""" Post include for BeamSpot to override various flags """


#from IOVDbSvc.CondDB import conddb
#conddb.addOverride("/Indet/Align", "InDetAlign_Collision_Robust_2009_04" )

if BeamSpotFlags.setIndetAlignTag():
    if BeamSpotFlags.setIndetAlignTag() != "NONE":
        print "Set IndetAlignTag to:", BeamSpotFlags.setIndetAlignTag()
        from IOVDbSvc.CondDB import conddb
        conddb.addOverride("/Indet/Align", BeamSpotFlags.setIndetAlignTag())

if BeamSpotFlags.setTRTAlignTag():
    if BeamSpotFlags.setTRTAlignTag() != "NONE":
        print "Set TRTAlignTag to:", BeamSpotFlags.setTRTAlignTag()
        from IOVDbSvc.CondDB import conddb
        conddb.addOverride("/TRT/Align", BeamSpotFlags.setTRTAlignTag())

if BeamSpotFlags.setTrkErrorScalingTag():
    if BeamSpotFlags.setTrkErrorScalingTag() != "NONE":
        print "Set TrkErrorScalingTag to:", BeamSpotFlags.setTrkErrorScalingTag()
        from IOVDbSvc.CondDB import conddb
        conddb.addOverride("/Indet/TrkErrorScaling", BeamSpotFlags.setTrkErrorScalingTag())


if BeamSpotFlags.setBeamposTag():
    if BeamSpotFlags.setBeamposTag() != "NONE":
        print "Set BeamposTag to:", BeamSpotFlags.setBeamposTag()
        from IOVDbSvc.CondDB import conddb
        conddb.addOverride("/Indet/Beampos", BeamSpotFlags.setBeamposTag())

