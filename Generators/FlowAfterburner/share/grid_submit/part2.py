
#--------------------------------------------------------------
# AthenaRoot Persistency
#--------------------------------------------------------------
include( 'GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py' )
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1" )
# 2101   == EventInfo
# 133273 == MC Truth (HepMC)
# 54790518 == HijigEventParams
Stream1.ItemList += [ "2101#*" ]
Stream1.ItemList += [ "133273#*" ]
Stream1.ItemList += [ "54790518#*" ]
Stream1.OutputFile = "hijing.flow.p_pb.pool.root"

#==============================================================
#
# End of job options file
#
###############################################################
