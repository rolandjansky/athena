# File for feeding Reco_tf.py vertexing options for the searches 
# looking for displaced vertices in the SUSY and Exotics groups.
# The options listed in this file explicitly are needed only when 
# running on ESD input, and the common options needed for both
# RAW and ESD input are included at the bottom.
# Must run after the large-radius tracking to use large-d0 tracks.

# configure 
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
StoreGateSvc=svcMgr.StoreGateSvc
StoreGateSvc.AllowOverwrite=True

# (JM) In release 22 ForceRerun is effectively always true. The configuration
#      should add the alg if it needs to run, not if it doesn't
# # force the truth conversion to rerun, discussed in https://its.cern.ch/jira/browse/ATLASRECTS-2062
# if not globalflags.DataSource() == 'data':
#     topSequence.GEN_AOD2xAOD.ForceRerun=True

# include options 
include("VrtSecInclusive/VrtSecInclusive_DV_postInclude.py")
