###############################################################
#
# Job options 
#
#==============================================================

#--------------------------------------------------------------
# ATLAS default Application Configuration options
#--------------------------------------------------------------

# Use McEventSelector so we can run with AthenaMP
import AthenaCommon.AtlasUnixGeneratorJob
#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

# the global detflags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

#
DetDescrVersion = 'ATLAS-R2-2015-02-01-00'
# configuration of the job 
InputMagicTag  = 'AtlasLayerMat_v20_ATLAS-R2-2015_type0'
OutputMagicTag = 'AtlasLayerMat_v20_ATLAS-R2-2015' 
OutputMapName  = '/GLOBAL/TrackingGeo/LayerMaterialV2'

# Tracking specifications ####################################
from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
# now say where the file is
TrkDetFlags.MaterialStoreGateKey        = '/GLOBAL/TrackingGeo/BinnedLayerMaterial'
TrkDetFlags.MaterialDatabaseLocalPath    = '' 
TrkDetFlags.MaterialDatabaseLocalName    = 'AtlasLayerMaterial-'+DetDescrVersion+'.db'
TrkDetFlags.MaterialMagicTag             = InputMagicTag

import logging
logger = logging.getLogger( 'MaterialManipulation_jobOptions' )
logger.info(TrkDetFlags)

# we need the conditions interface
from IOVDbSvc.CondDB import conddb
# specify the local database
DataBasePath  = TrkDetFlags.MaterialDatabaseLocalPath() 
DataBaseName  = TrkDetFlags.MaterialDatabaseLocalName()
DataBaseConnection = '<dbConnection>sqlite://X;schema='+DataBasePath+DataBaseName+';dbname=OFLP200</dbConnection>'
conddb.blockFolder('/GLOBAL/TrackingGeo/LayerMaterialV2')
conddb.addFolderWithTag('',DataBaseConnection+TrkDetFlags.MaterialStoreGateKey(),TrkDetFlags.MaterialMagicTag(),force=True)

# Number of events to be processed
theApp.EvtMax = 1

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# set up the Material Mapping
from TrkDetDescrAlgs.TrkDetDescrAlgsConf import Trk__MaterialManipulation
MaterialManipulation = Trk__MaterialManipulation(name ='MaterialManipulation')
# input-output handling
MaterialManipulation.LayerMaterialMapNameInput    = TrkDetFlags.MaterialStoreGateKey()
MaterialManipulation.LayerMaterialMapNameOutput   = OutputMapName
# screen output 
MaterialManipulation.OutputLevel                  = DEBUG
# screen output level [ Default : 0 |  0, 1, 2 )
job += MaterialManipulation

#--------------------------------------------------------------
# Write Stream 
#--------------------------------------------------------------

OutputDataBaseName = 'AtlasLayerMaterial-'+DetDescrVersion+'-output.db'

# the output conditions alg
from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
  
################################################################
# create the files with the appropriate name
MaterialFileName = 'AtlasLayerMaterial-'+DetDescrVersion+'-output.pool.root'

OutputObjects = [ 'Trk::LayerMaterialMap#'+OutputMapName ]
IOVTagEntries = [ OutputMagicTag ]

# create the conditions alg
myOCA=OutputConditionsAlg("CondAlg_Material",outputFile=MaterialFileName, ObjectList=OutputObjects, WriteIOV=True,IOVTagList=IOVTagEntries)      
# and create the database connection
ServiceMgr.IOVDbSvc.dbConnection = 'sqlite://schema='+OutputDataBaseName+';dbname=OFLP200'
