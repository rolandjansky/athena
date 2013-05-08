###############################################################
# File: MergeOldAOD.py                                        #
# Author: Thomas Burgess                                      #
# Date: 2009-10-24                                            #
#                                                             #
# Description:                                                #
#    Read rel 14 AOD files with old tau EDM                   # 
#    Tau(Rec/1P3P)Details, the clean invalid/extra details    #
#    and finally merge into rel 15 style TauCommonDetails     #
###############################################################

theApp.EvtMax = 10
svcMgr.MessageSvc.OutputLevel = WARNING

### Ignore geo tag (needed for some old files...)
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc.IgnoreTagDifference = True

import AthenaPoolCnvSvc.ReadAthenaPool
include( "RecExCond/AllDet_detDescr.py" )

### Specify input file
svcMgr.EventSelector.InputCollections = [
    "data/oldEDM_AOD.pool.root"]

# Get algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Add tau common details merger to the algorithm sequence
from tauRec.tauRecConf import TauAODDetailsCleaner
topSequence += TauAODDetailsCleaner("TauAODDetailsCleaner", OutputLevel=VERBOSE)

# Add tau common details merger to the algorithm sequence
from TauTools.TauToolsConf import TauCommonDetailsMerger
topSequence += TauCommonDetailsMerger("TauCommonDetailsMerger", OutputLevel=VERBOSE)

### Write output file
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1" )
Stream1.OutputFile = "Merged_old_aod.root"
Stream1.TakeItemsFromInput=TRUE;
Stream1.ForceRead=TRUE;  #force read of output data objs
# The next line is an example on how to exclude clid's if they are causing a  problem
#Stream1.ExcludeList += [ "TrackCollection#*" ]
svcMgr.AthenaPoolCnvSvc.TopLevelContainerName = "<type>/<key>"
svcMgr.AthenaPoolCnvSvc.SubLevelBranchName = ""
