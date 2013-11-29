#--------------------------------------------------------------------------
#
# writeTopOptions.py
#
# Edward.Moyse@cern.ch
# 
#--------------------------------------------------------------------------
import MuonRecExample.MuonRecStandaloneOnlySetup
from MuonRecExample.MuonRecFlags import muonRecFlags
from RecExConfig.RecFlags import rec as recFlags
recFlags.doWriteESD             = True
recFlags.doCBNT                 = False

from MuonRecExample import MuonRecUtils
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

# if not MuonRecUtils.hasJobPropertyBeenSet(athenaCommonFlags.FilesInput):
#     athenaCommonFlags.FilesInput = MuonRecUtils.FileList.readDirectory(
#         "root://eosatlas//eos/atlas/atlascerngroupdisk/det-muon/ReferenceDatasets/Digitization/SingleMu_20GeV_e1717_s1609_s1611_r4076")
athenaCommonFlags.FilesInput =["root://eosatlas//eos/atlas/atlascerngroupdisk/det-muon/ReferenceDatasets/Digitization/SingleMu_20GeV_e1717_s1609_s1611_r4076/RDO.01138972._000001.pool.root.1"]
include ("RecExCond/RecExCommon_flags.py")
# switch off ID, calo, or muons
DetFlags.ID_setOff()
DetFlags.Calo_setOff()

# Want to use this for position checks
# recFlags.doCBNT = True
# doCBNT=True

# Run Moore
muonRecFlags.doMoore = True
# Run Muonboy
muonRecFlags.doMuonboy = True

include("MuonRecExample/MuonRec_topOptions.py")

# ------------------------------------------------------------
# TEST EDM
# ------------------------------------------------------------

if not 'DumpFileName' in dir():
    DumpFileName="WriteESDfromRDO"

include ("MuonTestEDM/MuonTestEDM_jobOptions.py")
#MyEDMTester.OutputLevel=1
print MyEDMTester

if MyEDMTester.DoDumpRDOs:
  # Add RDOs to output, if we're comparing these too
  StreamESD.ItemList+=["MdtCsmContainer#*"]
  StreamESD.ItemList+=["CscRawDataContainer#*"]
  StreamESD.ItemList+=["RpcPadContainer#*"]
  StreamESD.ItemList+=["TgcRdoContainer#*"]


#from TrkEventCnvTools.TrkEventCnvToolsConf import Trk__EventCnvSuperTool
#TrkEventCnvSuperTool = Trk__EventCnvSuperTool(name="EventCnvSuperTool", DoID=False)
#ToolSvc += TrkEventCnvSuperTool
#print TrkEventCnvSuperTool
