# @author Renaud Bruneliere <Renaud.Bruneliere@cern.ch>
# @date Apr, 2010
# @brief Example for building a D3PD from event generation file.

###################################################################
# Define the input file here.
#

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput= ["EVNT.pool.root"]

###################################################################
# Define the output file here.
#
from TruthD3PDMaker.TruthD3PDMakerFlags import TruthD3PDFlags
if not globals().get('tupleFileOutput'):
    tupleFileOutput = 'susy.root'
TruthD3PDFlags.TruthD3PDOutputFileName = tupleFileOutput


###################################################################
# Define other job options here.
#

athenaCommonFlags.EvtMax = 10

include("TopInputsD3PD.py")
#from TruthD3PDMaker.topInputsD3PD import topInputsD3PD
#alg = topInputsD3PD(tupleFileOutput)

#from HforTool.HforToolConf import HforTool
#ToolSvc += HforTool("HforTool");
#ToolSvc.HforTool.RemovalSchema = "angularbased" #"jetbased", "angularbased","mc08"
#ToolSvc.HforTool.SampleType = "isC"  #isBB isCC isC isLightFlavor

from RecExConfig.RecFlags import rec
rec.doHist = False
rec.doMonitoring = False
rec.doCBNT = False
rec.doWriteESD = False
rec.doWriteAOD = False
rec.doWriteTAG = False
rec.doApplyAODFix = False

include("TruthD3PDMaker/TruthD3PDfromEVGEN_preInclude.py")

include("RecExCommon/RecExCommon_topOptions.py")
