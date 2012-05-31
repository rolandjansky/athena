#---------------------
from InDetRecExample.InDetJobProperties import InDetFlags

from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags
TrackD3PDFlags.trackParametersAtGlobalPerigeeLevelOfDetails.set_Value_and_Lock(2)
TrackD3PDFlags.trackParametersAtBeamSpotLevelOfDetails.set_Value_and_Lock(2)

TrackD3PDFlags.storeDiagonalCovarianceAsErrors.set_Value_and_Lock(True)

InDetFlags.doTrkD3PD = True
InDetFlags.doVtxD3PD = True
#---------------------
