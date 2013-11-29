# default is trigger off
if not 'doTrigger' in dir():
    doTrigger=False 

# main jobOption
include ("RecExCommon/rdotobs.py")

from MuonTestEDM.MuonTestEDMConf import MuonTestEDM
MyEDMTester = MuonTestEDM(DoDumpPRDs=False, DoDumpTracks=False, DoDumpRDOs=True, DoDumpSegments=False)
topSequence += MyEDMTester

if not 'DumpFileName' in dir():
    DumpFileName="WriteBS"

MyEDMTester.RdoDumpFileName    = DumpFileName+".rdo.log"

print MyEDMTester
