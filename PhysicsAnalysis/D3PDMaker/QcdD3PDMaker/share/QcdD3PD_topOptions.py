###########################
# QcdD3PDMaker topOptions
###########################
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

# Input file
athenaCommonFlags.FilesInput = [ 'AOD.pool.root' ]

#output file
from D3PDMakerConfig.D3PDProdFlags import prodFlags
prodFlags.WriteQcdD3PD.set_Value_and_Lock(True)
prodFlags.WriteQcdD3PD.FileName="qcd.root"
prodFlags.WriteQcdD3PD.lock()

# Number of events
athenaCommonFlags.EvtMax = -1

from RecExConfig.RecFlags import rec

rec.UserAlgs+=['QcdD3PDMaker/QcdD3PD_preSetup.py']
rec.DPDMakerScripts.append( "QcdD3PDMaker/QcdD3PD_prodJobOFragment.py" )

# Final JO
include ("RecExCommon/RecExCommon_topOptions.py")
