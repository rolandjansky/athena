# $Id$
#
# @file MuonD3PDMaker/share/AODToMuonD3PD.py
# @author srivas prasad <srivas.prasad@cern.ch>
# @date Nov, 2010
# @brief Example for building a muon D3PD from an AOD.

# Define the input file here.
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.FilesInput= ["AOD.pool.root"]
athenaCommonFlags.FilesInput= [
"/tmp/srivas/data11_7TeV.00183412.physics_Muons.merge.AOD.f385_m884/data11_7TeV.00183412.physics_Muons.merge.AOD.f385_m884._lb0420-lb0421._0001.1"
]

athenaCommonFlags.EvtMax = 2000

# Define the output file here.
if not globals().get('inputFileType'):
    inputFileType = 'AOD'
if not globals().get('tupleFileOutput'):
    tupleFileOutput = 'muon.root'

###################################################################
# Configure RecExCommon.
from RecExConfig.RecFlags import rec
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doESD.set_Value_and_Lock(False)
rec.doDPD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doForwardDet.set_Value_and_Lock(False)
include ("RecExCommon/RecExCommon_topOptions.py")

# Load the conditions stuff for TRT and Tile timing
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()

if not ('conddb' in dir()):
 IOVDbSvc = Service("IOVDbSvc")
 from IOVDbSvc.CondDB import conddb
conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/RT","/TRT/Calib/RT")
conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/T0","/TRT/Calib/T0")
from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbSvc
InDetTRTCalDbSvc = TRT_CalDbSvc()
ServiceMgr += InDetTRTCalDbSvc

####### Reduce output
#turn off perfmon
from PerfMonComps.PerfMonFlags import jobproperties as jp
jp.PerfMonFlags.doMonitoring = False
jp.PerfMonFlags.doFastMon = False
###################################################################

from MuonD3PDMaker.muonD3PD import muonD3PD
alg = muonD3PD (tupleFileOutput)
