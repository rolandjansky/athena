## @file MuonRDO_to_PRD_jobOptions.py
#
# @brief jobOptions to setup data conversion from RDO to PRD for offline (i.e. decode full event)
#
# Used when reading Pool RDO files or bytestream raw data files.
# Adds the algorithms that convert all RDO collections to PRD collections.
# There is one algorithm per detector technology and a specific algorithm is added or not depending
# on the flags<br>
# <code>DetFlags.makeRIO.MDT_on(), RPC_on(), TGC_on() and CSC_on()</code><br>
# See also @ref MuonRecExampleConverters "the section on configuring the converters"

include.block("MuonRecExample/MuonRDO_to_PRD_jobOptions.py")

from AthenaCommon.AppMgr import ToolSvc

from AthenaCommon.BeamFlags import jobproperties
beamFlags = jobproperties.Beam

from AthenaCommon.CfgGetter import getAlgorithm

if muonRecFlags.doCSCs() and DetFlags.makeRIO.CSC_on() and (DetFlags.haveRDO.CSC_on() or DetFlags.digitize.CSC_on()):
    topSequence += getAlgorithm("CscRdoToCscPrepData")

if muonRecFlags.doMDTs() and DetFlags.makeRIO.MDT_on() and (DetFlags.haveRDO.MDT_on() or DetFlags.digitize.MDT_on()):
    topSequence += getAlgorithm("MdtRdoToMdtPrepData", tryDefaultConfigurable=True)

if muonRecFlags.doRPCs() and DetFlags.makeRIO.RPC_on() and (DetFlags.haveRDO.RPC_on() or DetFlags.digitize.RPC_on()):
    topSequence += getAlgorithm("RpcRdoToRpcPrepData", tryDefaultConfigurable=True)

if muonRecFlags.doTGCs() and DetFlags.makeRIO.TGC_on() and (DetFlags.haveRDO.TGC_on() or DetFlags.digitize.TGC_on()):
    topSequence += getAlgorithm("TgcRdoToTgcPrepData", tryDefaultConfigurable=True)

if muonRecFlags.dosTGCs() and DetFlags.makeRIO.sTGC_on() and (DetFlags.haveRDO.sTGC_on() or DetFlags.digitize.sTGC_on()):
    topSequence += getAlgorithm("StgcRdoToStgcPrepData", tryDefaultConfigurable=True)


#
# Remove hits from part of the detector to mimic dead channels
#
if muonRecFlags.doPrdSelect():
    include("MuonPrdSelector/MuonPrdSelector_jobOptions.py")

if muonRecFlags.doCSCs() and DetFlags.makeRIO.CSC_on():
    topSequence += getAlgorithm("CscThresholdClusterBuilder")


if muonRecFlags.doCreateClusters():
    from AthenaCommon.CfgGetter import getPublicTool
    from AthenaCommon import CfgMgr

#    TGCCol = ""
#    RPCCol = ""
#    RPCClusCol = ""
#    TGCClusCol = ""
#    if muonRecFlags.doRPCs() and DetFlags.makeRIO.RPC_on() and (DetFlags.haveRDO.RPC_on() or DetFlags.digitize.RPC_on()):
#        RPCCol = "RPC_MeasurementsUnClustered"
#        RPCClusCol = "RPC_Measurements"
#        #ToolSvc.RpcPrepDataProviderTool.OutputCollection = RPCCol

#    if muonRecFlags.doTGCs() and DetFlags.makeRIO.TGC_on() and (DetFlags.haveRDO.TGC_on() or DetFlags.digitize.TGC_on()):
#        TGCCol = "TGC_MeasurementsUnClustered"
#        TGCClusCol = "TGC_Measurements"
    #    #ToolSvc.TgcPrepDataProviderTool.OutputCollection = TGCCol

    getPublicTool("MuonClusterizationTool")
    topSequence += CfgMgr.MuonClusterizationAlg("MuonClusterizationAlg",TgcPrepDataContainer="TGC_MeasurementsAllBCs" )

#                                               TgcPrepDataContainer = TGCCol, TgcPrepDataContainerOutput = TGCClusCol,
#                                               RpcPrepDataContainer = RPCCol, RpcPrepDataContainerOutput = RPCClusCol
#                                               )
