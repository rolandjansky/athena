# To register the correction constants with cool.

# This sets up writing the constants to a pool file.
include ("CaloClusterCorrection/CaloTopoEMCorrectionsToPool.py")

include ("RegistrationServices/IOVRegistrationSvc_jobOptions.py")

# Now to handle the cool registration.
# Set flag to register and run interval Run1/LB1 to Run2/LB2
#   Usually, only need to set Run1, others go to default
OutputConditionsAlg.WriteIOV   = True
OutputConditionsAlg.Run1       = IOVRunNumberMin 
OutputConditionsAlg.LB1        = IOVLBNumberMin
OutputConditionsAlg.Run2       = IOVRunNumberMax
OutputConditionsAlg.LB2        = IOVLBNumberMax

# Tags need to be globally unique!
OutputConditionsAlg.IOVTagList = tag_list

# Set the connection string
include ( "IOVDbSvc/IOVDbSvc_jobOptions.py" )
IOVDbSvc = Service( "IOVDbSvc" )
#IOVDbSvc.OutputLevel=DEBUG

from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
IOVRegistrationSvc().writeKeyInfo = False

#IOVRegSvc.RecreateFolders = True
IOVDbSvc.dbConnection = "sqlite://;schema=topoemcool.db;dbname=OFLP200"
