from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
#This needs another file for Forward Detectors !
#athenaCommonFlags.FilesInput=['root://castoratlas//castor/cern.ch/grid/atlas/atlasgroupdisk/phys-hi/dq2/mc09_7TeV/RDO/e511_s780_s767_d313/mc09_7TeV.208801.Hijing_PbPb_2p75TeV_MinBias.digit.RDO.e511_s780_s767_d313_tid144736_00/RDO.144736._000001.pool.root.1']
athenaCommonFlags.FilesInput=['root://castoratlas//castor/cern.ch/grid/atlas/atlasgroupdisk/phys-hi/dq2/group10/phys-hi/mc09_7TeV/group10.phys-hi.mc09_7TeV.208801.Hijing_PbPb_2p75TeV_MinBias_tid144736_00_r15.6.9.13p3_r1/group10.phys-hi.mc09_7TeV.208801.Hijing_PbPb_2p75TeV_MinBias_tid144736_00_r15.6.9.13p3_r1.EXT0._00001.ESD.pool.root']



athenaCommonFlags.EvtMax=2

outfile="test"

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel =5

include("ForwardDetectorsD3PDMaker/ForwardDetectorsD3PDMaker_configure.py")

include("RecExCommon/RecExCommon_topOptions.py")

include("ForwardDetectorsD3PDMaker/ForwardDetectorsD3PDMaker_jobOptions.py")
