#====================================================================
# DAPR0.py 
# reductionConf flag DAPR0 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
streamName = derivationFlags.WriteDAOD_DAPR0Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_DAPR0Stream )
from DerivationFrameworkDataPrep.DataPrepJobProperties import DataPrepFlags
pileupFileName = DataPrepFlags.outputFile()
DAPR0Stream = MSMgr.NewPoolRootStream( streamName, fileName )


##use pileupreweighting provider to generate a df.metadata.root prw config file
##only to run if the input file is post-digitization (i.e. dont run this on EVNT)
from RecExConfig.InputFilePeeker import inputFileSummary

if 'metadata' in inputFileSummary and '/Digitization/Parameters' in inputFileSummary['metadata']:
    ToolSvc += CfgMgr.CP__PileupReweightingTool("auto",ConfigFiles=[],LumiCalcFiles=[])
    DerivationFrameworkJob += CfgMgr.CP__PileupReweightingProvider(ConfigOutputStream="DFMETADATA",Tool=ToolSvc.auto,RunSystematics=False)
    if not hasattr(svcMgr,'THistSvc'):
        svcMgr += CfgMgr.THistSvc()
    histString = "DFMETADATA DATAFILE=\'"+pileupFileName+"\' OPT=\'RECREATE\'"
    svcMgr.THistSvc.Output += [histString]
