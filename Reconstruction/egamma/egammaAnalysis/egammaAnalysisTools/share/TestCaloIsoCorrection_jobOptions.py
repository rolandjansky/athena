# $Id $
#

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput= ["AOD.pool.root"]

athenaCommonFlags.EvtMax = 10

tupleFileOutput = 'NTUP.root'

from D3PDMakerConfig.D3PDProdFlags import prodFlags
prodFlags.WriteEgammaD3PD = True
prodFlags.WriteEgammaD3PD.FileName = tupleFileOutput
prodFlags.WriteEgammaD3PD.lock()

from RecExConfig.RecFlags import rec
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doESD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doForwardDet.set_Value_and_Lock(False)
include ("RecExCommon/RecExCommon_topOptions.py")

from egammaAnalysisTools.CaloIsolationWrapperTool import CaloIsolationWrapperTool
caloIsoTool = CaloIsolationWrapperTool(DoSmearing=True,
                                       DoScaling=True)

# run a test algorithm to check that the corrections are working
from egammaAnalysisTools.egammaAnalysisToolsConf import TestCaloIsoCorrection
testIsoAlg=TestCaloIsoCorrection(OutputLevel=INFO,
                                 CaloIsolationWrapperTool=caloIsoTool,
                                 ConeSize = 30,
                                 IsMC = rec.doTruth())

from AthenaCommon.AlgSequence import AlgSequence,AthSequencer
mainJob = AlgSequence()
mainJob += testIsoAlg
