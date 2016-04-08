###########################
# JetSubstructureD3PDMaker topOptions
###########################
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

# Input file
athenaCommonFlags.FilesInput = []

athenaCommonFlags.FilesInput+= ["mc10_7TeV.105594.Pythia_Zprime_tt1600.merge.AOD.e574_s933_s946_r2302_r2300_tid322318_00/AOD.322318._000006.pool.root.1"]

from D3PDMakerConfig.D3PDProdFlags import prodFlags
prodFlags.WriteTopBoostJSD3PD.set_Value_and_Lock(True)
prodFlags.WriteTopBoostJSD3PD.FileName="boostedtop.root"

# Number of events
athenaCommonFlags.EvtMax = 10

#OutputLevel=DEBUG

from RecExConfig.RecFlags import rec

rec.doDPD.set_Value_and_Lock(True)
rec.DPDMakerScripts.append("BoostedTopD3PDMaker/TopBoostJSD3PD_prodJobOFragment.py")

rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doESD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doHist.set_Value_and_Lock(False)

# Final JO
include ("RecExCommon/RecExCommon_topOptions.py")


