## @file OutputStreamAthenaPool_jobOptions.py
## @brief OutputStream job options file to illustrate available OutputStream properties.
## @author Peter van Gemmeren <gemmeren@bnl.gov>
###############################################################
#
# OutputStreamAthenaPool job options file
# Principally for documentation, to illustrate available OutputStreamAthenaPool properties
#
#==============================================================
# The next lines tell Athena to load the OutputStreamAthenaPool, and also where to find it.

from OutputStreamAthenaPool.OutputStreamAthenaPool import *

WritingTool = AthenaPoolOutputStreamTool( "StreamTool" )

# List of additional DataHeader, which point to subset of DataObjects only
# - "<SatelliteName>:<Type>[#<Key>],...",
# - <SatelliteName> represents the StoreGate key of the satellite DataHeader.
# - As a special character, a '/' is used at the end to mark this DataHeader as a satellite.
# - <Type>[#<Key>],. determines the references that are copied from the full DataHeader to the satellite.
# - They should always include EventInfo, as that is for any event processing.
WritingTool.DataHeaderSatellites = [ "basic/:EventInfo#*" ]
WritingTool.DataHeaderSatellites = [ ]

# Prefix for DataHeader POOL container, default is "POOLContainer_".
# Note: this needs to be "POOLContainer_" for EventData, so that EventSelector can open it.
WritingTool.OutputCollection = "POOLContainer_";

# The following properties are similar to the corresponding AthenaPoolCnvSvc properties,
# but affect this Stream only.
# Prefix for top level POOL container.
AthenaPoolCnvSvc.PoolContainerPrefix = "CollectionTree"
# Naming hint policy for top level POOL container. E.G.:
# - "", no hint will cause all objects to be written to the same tree (requires branches).
# - "<type>", use the data object type as container name (tree per type).
# - "<type>/<key>", use the data object type and key as container name (tree per type/key).
AthenaPoolCnvSvc.TopLevelContainerName = ""
# Naming hint policy for top level POOL branching. E.G.:
# - "", no hint will cause all objects of a tree to be written to the same branch.
# - "<type>", use the data object type as branch name (required if type is not used for tree name).
# - "<type>/<key>", use the data object type and key as branch name.
AthenaPoolCnvSvc.SubLevelBranchName = "<type>/<key>"
# Optional key for AttributeList to be written as part of the DataHeader
# - "", no AttributeList list is written to the payload file.
AthenaPoolCnvSvc.AttributeListKey = ""
