# #######################################################################
# TopOptions to process cosmics /single beam data 
# M. Woudstra, R. Nikolaidou 
#
# By default we run on 10 events of the 87863 run, full reconstruction
# Muonboy and Moore and we produce as
# output : CBNT, ESD, Monitoring, ascii file with the configuration of the job  
# #######################################################################

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BFieldFlags import jobproperties
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.AppMgr import theApp


# from RecExConfig.RecFlags import rec
# from RecExConfig.RecConfFlags import recConfFlags

# in Standalone mode, don't allow any configuration errors
athenaCommonFlags.AllowIgnoreConfigError = False

#Just Muons
DetFlags.Muon_setOn()
# print DetFlag
#
# command line parameters (-c)
#
if 'EvtMax' not in dir():
    EvtMax = 10 # maximum number of events to process

if 'SkipEvents' not in dir():
    SkipEvents = 0

theApp.evtMax=EvtMax

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.MessageSvc.OutputLevel = VERBOSE

# Set properly beam type, DataSource, InputFormat, Detector Description, Conditions Tag
from IOVDbSvc.CondDB import conddb
globalflags.DataSource.set_Value_and_Lock('data')
if globalflags.DataSource() == 'data':
    jobproperties.Beam.beamType.set_Value_and_Lock("cosmics")
    globalflags.InputFormat.set_Value_and_Lock('bytestream')
    globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEONF-04-00-00')
else :
    globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEONF-02-01-00')
    globalflags.ConditionsTag.set_Value_and_Lock('OFLCOND-SIM-00-00-00')

include ("RecExCond/AllDet_detDescr.py")

# for running over data with field on need this to be set to true
jobproperties.BField.solenoidOn.set_Value_and_Lock(False)
jobproperties.BField.barrelToroidOn.set_Value_and_Lock(False)
jobproperties.BField.endcapToroidOn.set_Value_and_Lock(False)


# Max number of events to run, number of events to skip
athenaCommonFlags.EvtMax.set_Value_and_Lock(EvtMax)
athenaCommonFlags.SkipEvents.set_Value_and_Lock(SkipEvents)

# setup environment 
athenaCommonFlags.BSRDOInput.set_Value_and_Lock(['/castor/cern.ch/grid/atlas/atlasdatadisk/data08_cos/RAW/data08_cos.00091060.physics_RPCwBeam.daq.RAW.o4/daq.ATLAS.0091060.physics.RPCwBeam.LB0001.SFO-1._0001.data'])
#        '/castor/cern.ch/grid/atlas/DAQ/2008/87863/physics_BPTX/daq.NoTag.0087863.physics.BPTX.LB0000.SFO-1._0001.data'])
globalflags.ConditionsTag.set_Value_and_Lock('COMCOND-REPC-001-03')

include ("MuonCnvExample/MuonReadBS_jobOptions.py")

include ("MuonTestEDM/MuonTestEDM_jobOptions.py")
MyEDMTester.DoDumpPRDs=False
MyEDMTester.DoDumpTracks=False
MyEDMTester.DoDumpRDOs=True
MyEDMTester.DoDumpSegments=False
print MyEDMTester
 
if MyEDMTester.DoDumpRDOs:
  from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
  StreamESD = AthenaPoolOutputStream( "StreamESD" )
  # Add RDOs to output, if we're comparing these too
  StreamESD.ItemList+=["MdtCsmContainer#*"]
  StreamESD.ItemList+=["CscRawDataContainer#*"]
  StreamESD.ItemList+=["RpcPadContainer#*"]
  StreamESD.ItemList+=["TgcRdoContainer#*"]

from TrkEventCnvTools.TrkEventCnvToolsConf import Trk__EventCnvSuperTool
TrkEventCnvSuperTool = Trk__EventCnvSuperTool(name="EventCnvSuperTool", DoID=False)
ToolSvc += TrkEventCnvSuperTool
print TrkEventCnvSuperTool
