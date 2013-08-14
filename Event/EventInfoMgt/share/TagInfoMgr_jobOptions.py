###############################################################
#
## @file TagInfoMgr_jobOptions.py
## @brief Default properties for TagInfoMgr
# 
#
###############################################################

# block multiple inclusion
include.block ( "EventInfoMgt/TagInfoMgr_jobOptions.py" )

#TagInfoMgr = Service ( "TagInfoMgr" )

# Set the key for TagInfo (default = "")
#TagInfoMgr.TagInfoKey = "ESDtags"

# Turn off adding tags to TagInfo from EventInfo (default = true)
#TagInfoMgr.AddEventInfoTags      = False

# Turn off adding tags to TagInfo from GeoModel (default = true)
#TagInfoMgr.AddGeoModelTags       = False

# Turn off override of tags from EventInfo (default = true)
#TagInfoMgr.OverrideEventInfoTags = False

# Add extra value/tag pairs directly (default - nothing added)
# Note that EventInfoMgt_jobOptions.py add the release version
#TagInfoMgr.ExtraTagValuePairs = ["Release", "10.0.0" ]


