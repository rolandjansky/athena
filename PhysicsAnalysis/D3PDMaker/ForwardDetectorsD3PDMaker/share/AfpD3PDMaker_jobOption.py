InputRootCollection = 'rootfile_AfpData.root'
OutputRootFile      = 'rootfile_Afp.D3PD.root'

from GaudiKernel.GaudiHandles import *
from GaudiKernel.Proxy.Configurable import *
import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc

ServiceMgr.EventSelector.InputCollections += [InputRootCollection]
theApp.EvtMax = -1
ServiceMgr.EventSelector.SkipEvents = 0

IOVDbSvc = Service( "IOVDbSvc" )

from IOVDbSvc.CondDB import conddb
####for other possible servers see dbreplica.config in Athena installation directory
#IOVDbSvc.dbConnection="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200"

###################################################################################################
# Load Global Flags and set defaults (import the new jobProperty globalflags)
###################################################################################################
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BeamFlags import jobproperties
globalflags.DetGeo = 'atlas'
globalflags.InputFormat = 'pool'
#globalflags.DetDescrVersion = DetDescrVersion
#globalflags.DataSource  = 'data' # 'geant4'#

globalflags.print_JobProperties()

## Load algorithm to TopSequence
#
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

###################################################################################################
#D3PD maker setup
###################################################################################################
import D3PDMakerCoreComps
import EventCommonD3PDMaker
from   D3PDMakerCoreComps.D3PDObject import D3PDObject
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

from ForwardDetectorsD3PDMaker.AfpD3PDObjects import AfpSIDSimHitD3PDObject
from ForwardDetectorsD3PDMaker.AfpD3PDObjects import AfpTDSimHitD3PDObject
from ForwardDetectorsD3PDMaker.AfpD3PDObjects import AfpSIDDigiD3PDObject
from ForwardDetectorsD3PDMaker.AfpD3PDObjects import AfpTDDigiD3PDObject
from ForwardDetectorsD3PDMaker.AfpD3PDObjects import AfpSIDLocRecoEvD3PDObject
from ForwardDetectorsD3PDMaker.AfpD3PDObjects import AfpTDLocRecoEvD3PDObject
from ForwardDetectorsD3PDMaker.AfpD3PDObjects import AfpTruthInfoD3PDObject

alg = MSMgr.NewRootStream( "AfpD3PD", OutputRootFile, "AfpD3PDTree" )
alg += EventInfoD3PDObject(10)
alg += AfpSIDSimHitD3PDObject(0)
alg += AfpTDSimHitD3PDObject(0)
alg += AfpSIDDigiD3PDObject(0)
alg += AfpTDDigiD3PDObject(0)
alg += AfpSIDLocRecoEvD3PDObject(0)
alg += AfpTDLocRecoEvD3PDObject(0)
alg += AfpTruthInfoD3PDObject(0)

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 3
MessageSvc.defaultLimit = 9999999

ClassIDSvc = Service( "ClassIDSvc" )
ClassIDSvc.OutputLevel = 2

