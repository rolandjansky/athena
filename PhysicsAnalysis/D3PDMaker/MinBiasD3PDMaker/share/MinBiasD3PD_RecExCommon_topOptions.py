from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.FilesInput=['ESD_2.pool.root']
athenaCommonFlags.EvtMax=10

from MinBiasD3PDMaker.MinBiasD3PDMakerFlags import minbiasD3PDflags
minbiasD3PDflags.FileName = '/tmp/zaidan/MinBiasD3PD.root'

include("MinBiasD3PDMaker/MinBiasD3PD_configure.py")

include( "RecExCommon/RecExCommon_topOptions.py" )

include("MinBiasD3PDMaker/MinBiasD3PD_jobOptions.py")

