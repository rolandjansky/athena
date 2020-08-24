
InputRootCollection = 'rootfile_AlfaData.root'
OutputRootFile      = 'rootfile_Alfa.D3PD.root'

from GaudiKernel.GaudiHandles import *
from GaudiKernel.Proxy.Configurable import *
import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.GlobalFlags import globalflags

ServiceMgr.EventSelector.InputCollections += [InputRootCollection]
theApp.EvtMax = -1
ServiceMgr.EventSelector.SkipEvents = 0

###################################################################################################
# Load Global Flags and set defaults (import the new jobProperty globalflags)
###################################################################################################
#from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BeamFlags import jobproperties
globalflags.DetGeo = 'atlas'
globalflags.InputFormat = 'pool'
#globalflags.DetDescrVersion = DetDescrVersion
globalflags.DataSource  = 'data' # 'geant4'#

globalflags.print_JobProperties()



if globalflags.DataSource == "data":
    #--------------------------------------------------------------
	IOVDbSvc = Service( "IOVDbSvc" )
	from IOVDbSvc.CondDB import conddb

	if not conddb.folderRequested('/RPO/DCS/BLM'):
		conddb.addFolder("DCS_OFL","/RPO/DCS/BLM")
	if not conddb.folderRequested('/RPO/DCS/FECONFIGURATION'):
		conddb.addFolder("DCS_OFL","/RPO/DCS/FECONFIGURATION")
	if not conddb.folderRequested('/RPO/DCS/HVCHANNEL'):
		conddb.addFolder("DCS_OFL","/RPO/DCS/HVCHANNEL")
	if not conddb.folderRequested('/RPO/DCS/LOCALMONITORING'):
		conddb.addFolder("DCS_OFL","/RPO/DCS/LOCALMONITORING")
	if not conddb.folderRequested('/RPO/DCS/MOVEMENT'):
		conddb.addFolder("DCS_OFL","/RPO/DCS/MOVEMENT")
	if not conddb.folderRequested('/RPO/DCS/RADMON'):
		conddb.addFolder("DCS_OFL","/RPO/DCS/RADMON")
	if not conddb.folderRequested('/RPO/DCS/TRIGGERRATES'):
		conddb.addFolder("DCS_OFL","/RPO/DCS/TRIGGERRATES")
	if not conddb.folderRequested('/RPO/DCS/TRIGGERSETTINGS'):
		conddb.addFolder("DCS_OFL","/RPO/DCS/TRIGGERSETTINGS")

    ####for other possible servers see dbreplica.config in Athena installation directory
	#IOVDbSvc.dbConnection="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200"
	#IOVDbSvc.Folders += ["/RPO/DCS/BLM"]
	#IOVDbSvc.Folders += ["/RPO/DCS/FECONFIGURATION"]
	#IOVDbSvc.Folders += ["/RPO/DCS/HVCHANNEL"]
	#IOVDbSvc.Folders += ["/RPO/DCS/LOCALMONITORING"]
	#IOVDbSvc.Folders += ["/RPO/DCS/MOVEMENT"]
	#IOVDbSvc.Folders += ["/RPO/DCS/RADMON"]
	#IOVDbSvc.Folders += ["/RPO/DCS/TRIGGERRATES"]
	#IOVDbSvc.Folders += ["/RPO/DCS/TRIGGERSETTINGS"]

## Load algorithm to TopSequence
#
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

###################################################################################################
#D3PD maker setup
###################################################################################################

## D3PD Maker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

from ForwardDetectorsD3PDMaker.AlfaD3PDObject import AlfaD3PDObject
alg = MSMgr.NewRootStream( "AlfaD3PD", OutputRootFile , "AlfaD3PDTree" )
alg += EventInfoD3PDObject(10)
alg += AlfaD3PDObject(0)

from ForwardDetectorsD3PDMaker import AlfaEventHeaderFillerTool
if globalflags.DataSource == "data":
	from ForwardDetectorsD3PDMaker.AlfaDCSMetadata import addAlfaDcsMetadata
	AlfaEventHeaderFillerTool.DataType = 1
	addAlfaDcsMetadata(alg)
elif globalflags.DataSource == "geant4":
	from ForwardDetectorsD3PDMaker.AlfaTruthInfoD3PDObject import AlfaTruthInfoD3PDObject
	AlfaEventHeaderFillerTool.DataType = 0
	alg += AlfaTruthInfoD3PDObject(0)


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 5
MessageSvc.defaultLimit = 9999999

ClassIDSvc = Service( "ClassIDSvc" )
ClassIDSvc.OutputLevel = 5

