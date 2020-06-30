# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#-- set flags ------------------------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags

globalflags.InputFormat.set_Value_and_Lock('bytestream')
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.ConditionsTag.set_Value_and_Lock("COMCOND-ES1C-001-01")

from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.FilesInput = [InputFile]

from RecExConfig.RecFlags import rec
rec.doTruth.set_Value_and_Lock(False)

from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth.set_Value_and_Lock(False)

#-- set up job ------------------------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#-- set up BS reading ------------------------------------------------------------------------------

from ByteStreamCnvSvc import ReadByteStream
svcMgr.EventSelector.Input = [InputFile]

#-- set up output histogram file ------------------------------------------------------------------------------

from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
svcMgr.THistSvc.Output += ["GLOBAL DATAFILE='" + OutputFile + "' OPT='RECREATE'"]

#-- configure field and detector geometry ------------------------------------------------------------------------------

autokeys = ["FieldAndGeo","BeamType"]
from RecExConfig.AutoConfiguration import ConfigureFromListOfKeys
ConfigureFromListOfKeys(autokeys)

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag(globalflags.ConditionsTag())

include("RecExCond/AllDet_detDescr.py")

#-- import BS reading JO ------------------------------------------------------------------------------

include("RecExCommon/BSRead_config.py")

#-- get Trigger Configuration ------------------------------------------------------------------------------

from TriggerJobOpts.T0TriggerGetter import T0TriggerGetter
gettrigger = T0TriggerGetter()

#-- set up TrigDecision Tool ------------------------------------------------------------------------------

from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
monTrigDecTool = Trig__TrigDecisionTool("monTrigDecTool")
ToolSvc += monTrigDecTool

#-- run reconstruction ------------------------------------------------------------------------------
    # bug https://savannah.cern.ch/bugs/?54123

if not conddb.folderRequested('PIXEL/PixReco'):
   conddb.addFolder('PIXEL_OFL','/PIXEL/PixReco');

#protectedInclude( "InDetRecExample/InDetRec_jobOptions.py" )
#protectedInclude ("CaloRec/CaloRec_jobOptions.py")
#include ("MuonRecExample/MuonRec_jobOptions.py")
#
#protectedInclude( "egammaRec/egammaRec_jobOptions.py" )
#include ("MuonCombinedRecExample/MuonCombinedRec_config.py")
#include( "JetRec/JetRec_jobOptions.py" )
#protectedInclude ("tauRec/tauRec_config.py")
#include( "MissingET/MissingET_jobOptions.py" )

#-- include HLTMonitoring ------------------------------------------------------------------------------
#include("TrigHLTMonitoring/HLTMonitoring_topOptions.py")
include("TrigHLTOfflineMon/HLTOfflineMon_topOptions.py")
