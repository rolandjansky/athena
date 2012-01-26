include("TrigNtInDet/jobOfragment_TrigNtInDet.py")

# lines below are needed to avoid crash from extrapolator if there is no magnetic field
Algorithm("ToolSvc.TrigInDetTrackFitter").UseAthenaFieldService=False
Algorithm("ToolSvc.TrigInDetTrackExtrapolator").UseAthenaFieldService=False
Algorithm("ToolSvc.TrigTRT_TrackExtensionTool.TrigMagneticFieldTool").UseAthenaFieldService=False
Algorithm("ToolSvc.TrigMagneticFieldTool").UseAthenaFieldService=False

CBNT_AthenaAware.Members += [ 'CBNTAA_TrigCosmic' ]
CBNT_AthenaAware.Members += [ 'CBNTAA_MdtHit' ]
CBNT_AthenaAware.Members += [ 'CBNTAA_TgcHit' ]
