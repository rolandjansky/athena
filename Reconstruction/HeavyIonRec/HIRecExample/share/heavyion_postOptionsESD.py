# Andrzej Olszewski
####################################################

if rec.doESD :
    # trk vs PV  (20.1.9) version    
    from TrackToVertex.TrackToVertexConf import Reco__TrackToVertex
    ttvx = Reco__TrackToVertex("TrackToVertex")
    ToolSvc+=ttvx;topSequence.InDetVxLinkSetter.TrackToVertex=ttvx

#=========================================================
#
# End of job options file
#
###############################################################
