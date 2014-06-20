#general
from GaudiKernel.GaudiHandles import *
from GaudiKernel.Proxy.Configurable import *
import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AppMgr import ServiceMgr
#from AthenaCommon.AppMgr import ToolSvc

MessageSvc = Service("MessageSvc")
MessageSvc.OutputLevel = INFO  #DEBUG, INFO, WARNING, ERROR, FATAL

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#--------------------
#stear options
doRunOnGrid=False
theApp.EvtMax = -1
tuplename = 'myTree'
outFileName = 'run161562_newjO_newBcmFiller.file.root'
#--------------------


from RecExConfig.RecFlags import rec
rec.AutoConfiguration = ['everything']

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
import glob
#--------------------------------------------------------------------------------------------------
if not doRunOnGrid:
    athenaCommonFlags.FilesInput = ['/home/jansen/run161562/data10_7TeV.00161562.physics_MinBias.recon.ESD.f283/data10_7TeV.00161562.physics_MinBias.recon.ESD.f283._lb0210._0001.1.root']

else:
    athenaCommonFlags.FilesInput = glob.glob("*ESD*")

ServiceMgr.EventSelector.InputCollections=athenaCommonFlags.FilesInput()
#--------------------------------------------------------------------------------------------------


from RecExConfig.InputFilePeeker import inputFileSummary
from AthenaCommon.GlobalFlags import globalflags

if inputFileSummary['evt_type'][0] == 'IS_DATA' :
    globalflags.DataSource  = 'data'
if inputFileSummary['evt_type'][0] == 'IS_SIMULATION' :
    globalflags.DataSource  = 'geant4'

globalflags.DetDescrVersion = inputFileSummary['geometry']
print globalflags.DetDescrVersion



#set up all detector description description
include('RecExCond/AllDet_detDescr.py')
include('TrkDetDescrSvc/AtlasTrackingGeometrySvc.py')

from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg = TriggerConfigGetter() 


import D3PDMakerCoreComps
import EventCommonD3PDMaker
import MinBiasD3PDMaker
#import TrigMbD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject

## Evnt Info
from EventCommonD3PDMaker.EventInfoD3PDObject		import makeEventInfoD3PDObject
EventInfoD3PDObject = D3PDObject (makeEventInfoD3PDObject, 'EventInfo_')
EventInfoD3PDObject.defineBlock (0, 'eventID', EventCommonD3PDMaker.EventIDFillerTool)
# BCM RDO
from TrigMbD3PDMaker.BcmRdoD3PDObject			import BcmRdoD3PDObject
from TrigMbD3PDMaker.CtpDecisionD3PDObject		import CtpDecisionD3PDObject

#from TriggerD3PDMaker.TrigDecisionD3PDObject		import TrigDecisionD3PDObject
#from TriggerD3PDMaker.CTPD3PDObject			import CTPD3PDObject



## D3PD Maker
alg = D3PDMakerCoreComps.MakerAlg(tuplename, 
				  topSequence,
				  outFileName)

alg += EventInfoD3PDObject(10)
alg += BcmRdoD3PDObject(10)
alg += CtpDecisionD3PDObject(10)
#alg += TriggerDecisionD3PDObject(10)
#alg += TrigDecisionD3PDObject(10)
#alg += CTPD3PDObject(10)



