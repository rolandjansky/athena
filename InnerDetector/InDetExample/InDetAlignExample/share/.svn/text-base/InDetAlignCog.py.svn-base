# --- setup flags with default values
##############################################################
#
# Algorithm to calculate centre-of-gravity of ID
#
# Sergio Gonzalez-Sevilla
# segonzal@ific.uv.es
# version II:
# Pawel Bruckman de Renstrom
# bruckman@cern.ch
###############################################################

if "ReadAlignmentConstants" not in dir():
	ReadAlignmentConstants = True

#------------------------------------------------
# input file with Si and TRT constants 
#------------------------------------------------
if "inputfilename" not in dir():
	inputfilename = 'TestConstants.root'
#------------------------------------------------
# output files with Si and TRT constants after cog shift
#------------------------------------------------
if "outFilename" not in dir():
	outFilename    = 'TestConstantsOut.root'
	outdbFilename  = 'TestConstantsOut.db'
	outTxtFilename = 'TestConstantsOut.txt'

if "onlySilicon" not in dir():
	onlySilicon = False

# To extract constants from the database select the appropriate database tag.
from AthenaCommon.GlobalFlags import globalflags
#globalflags.DataSource='geant4'
globalflags.DataSource='data'

# Select the geometry version. 
globalflags.DetDescrVersion = 'ATLAS-GEO-03-00-00'      
print globalflags.DetDescrVersion()

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag("COMCOND-REPC-002-13")
#conddb.setGlobalTag("OFLCOND-CSC-00-02-00")
#conddb.setGlobalTag("COMCOND-HLTC-000-00")

# Setup geometry
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

if onlySilicon:
	DetFlags.TRT_setOff()
	DetFlags.detdescr.TRT_setOn()
#------------------------------------------------
# GeoModel stuff
#------------------------------------------------
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion

from IOVDbSvc.CondDB import conddb


if ReadAlignmentConstants:
	conddb.blockFolder("/Indet/Align")
	conddb.blockFolder("/TRT/Align")
	from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import CondProxyProvider
	from AthenaCommon.AppMgr import ServiceMgr
	ServiceMgr += CondProxyProvider()
	ServiceMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
	print 'Loading initial alignment File',inputfilename
	ServiceMgr.CondProxyProvider.InputCollections = [ inputfilename ]
	ServiceMgr.PoolSvc.AttemptCatalogPatch = True
	ServiceMgr.CondProxyProvider.OutputLevel=INFO
	print ServiceMgr.CondProxyProvider
	ServiceMgr.IOVSvc.preLoadData = True
else:
	conddb.addOverride("/Indet/Align","InDet_Cosmic_2008_05_03" )
	conddb.addOverride("/TRT/Align","TRT_Cosmic_2008_10_03" )
	
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.AppMgr import ToolSvc	

#------------------------------------------------
# Alignment tools
#------------------------------------------------
from InDetAlignGenTools.InDetAlignGenToolsConf import InDetAlignDBTool
ToolSvc += InDetAlignDBTool()

from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_AlignDbSvc
ServiceMgr += TRT_AlignDbSvc()

# Needed for database-, Pool- and AlignableTransforms-Handling
include ( "DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )
from OutputStreamAthenaPool.OutputStreamAthenaPoolConf import AthenaPoolOutputStreamTool
InDetCondStream = AthenaPoolOutputStreamTool( name="CondStream1",OutputFile=outFilename )
ToolSvc += InDetCondStream
print InDetCondStream

# To produced a local SQLite DB with new Alignment Parameters
include( "RegistrationServices/IOVRegistrationSvc_jobOptions.py" )
regSvc = Service( "IOVRegistrationSvc" )
regSvc.RecreateFolders = False
IOVDbSvc = Service( "IOVDbSvc" )
IOVDbSvc.dbConnection = "sqlite://;schema=%s;dbname=OFLP200" % outdbFilename

#------------------------------------------------
# InDetAlignCog algorithm
#------------------------------------------------
from InDetAlignGenAlgs.InDetAlignGenAlgsConf import InDetAlignCog
InDetAlignCog = InDetAlignCog(name = 'InDetAlignCog',
#                              Det              = 2,
#                              SiBec            = 0,
#                              SiLayer          = 1,
#                              TRT_Bec          = -2,
#                              TRT_Layer        = 99,
#                              PrintFullMatrix  = True,
#                              PrintDB          = True,
                              ErrorTranslation = 1,
                              ErrorRotation    = 10,
                              SiTextOutput     = True,
                              SiTextFile       = outTxtFilename,
#                              SQLiteTag        = "cog_tag", 
                              TRT_TextOutput   = True,
			      DoCoG            = True,
			      DoL1             = False,
			      TraX             = 0.0,
                              TraY             = 0.0,
                              TraZ             = 0.0,
                              RotX             = 0.0,
			      RotY             = 0.0,
			      RotZ             = 0.0,
			      OutputLevel      = 3)

topSequence += InDetAlignCog
print topSequence.InDetAlignCog
if onlySilicon:
    InDetAlignCog.Det = 12    # no TRT
#    InDetAlignCog.SiBec = 0
    
#------------------------------------------------
# output level
#------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel  = 3
ServiceMgr.MessageSvc.Format       = "% F%30W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.defaultLimit = 9999999
ServiceMgr.IOVDbSvc.OutputLevel=INFO
ServiceMgr.IOVSvc.OutputLevel=INFO
ServiceMgr.IOVDbSvc.forceRunNumber = 52280
theApp.EvtMax = 1


