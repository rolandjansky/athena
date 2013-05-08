###############################################################
# File: MergeOldESD.py                                        #
# Author: Thomas Burgess                                      #
# Date: 2009-10-24                                            #
#                                                             #
# Description:                                                #
#    Read rel 14 ESD files with old tau EDM                   #
#    Tau(Rec/1P3P)(Extra)Details and merge them into rel 15   #
#    style TauCommonExtraDetails                              # 
###############################################################

theApp.EvtMax = 10
svcMgr.MessageSvc.OutputLevel = WARNING

### Ignore geo tag (needed for some old files...)
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc.IgnoreTagDifference = True

import AthenaPoolCnvSvc.ReadAthenaPool
include( "RecExCond/AllDet_detDescr.py" )

# Set input file
svcMgr.EventSelector.InputCollections = [
    "data/oldEDM_ESD.pool.root"]

# Get algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Add tau common details merger to the algorithm sequence
from TauTools.TauToolsConf import TauCommonDetailsMerger
topSequence += TauCommonDetailsMerger("TauCommonDetailsMerger", OutputLevel=VERBOSE)

#Produce output
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1" )
Stream1.OutputFile = "Merged_old_esd.root"
Stream1.TakeItemsFromInput=TRUE;
Stream1.ForceRead=TRUE;  #force read of output data objs
# The next line is an example on how to exclude clid's if they are causing a  problem
#Stream1.ExcludeList += [ "TrackCollection#*" ]
svcMgr.AthenaPoolCnvSvc.TopLevelContainerName = "<type>/<key>"
svcMgr.AthenaPoolCnvSvc.SubLevelBranchName = ""
