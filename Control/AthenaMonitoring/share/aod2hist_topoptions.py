from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as af
af.FilesInput = [
#    "/afs/cern.ch/user/j/jboyd/gencomm/ForEric/data11_7TeV.00186156.express_express.recon.AOD.f391._lb0535._SFO-ALL._0001.1"
#    "/afs/cern.ch/atlas/maxidisk/d36/lumiJuly2010/data10_7TeV.00152166.physics_MinBias.merge.AOD.r1239_p134_tid129143_00/AOD.129143._000120.pool.root.1"
#    "root://eosatlas.cern.ch//eos/atlas/atlasdatadisk/data12_8TeV/AOD/r4065/data12_8TeV.00204955.express_express.recon.AOD.r4065_tid01034439_00/AOD.01034439._000460.pool.root.1"
    "AOD.01034439._000460.pool.root.1"
]

af.EvtMax=-1 # number of event to process
#af.EvtMax=100 # number of event to process

#======================================================================================

from RecExConfig.RecFlags import rec
rec.AutoConfiguration = ['everything']
rec.readRDO = False
rec.readESD = False
rec.readAOD = True
rec.doCBNT = False
rec.doWriteESD = False
rec.doWriteAOD = False
rec.doWriteTAG = False
rec.doDPD = False
#rec.doFileMetaData = False
rec.doHist = False
rec.doMonitoring = True

# Output log setting
OutputLevel = INFO

# main jobOption - must always be included
include ("RecExCommon/RecExCommon_topOptions.py")



