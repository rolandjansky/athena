# Creation: David Cote, 19 May 2008
# This temporary file replaces the former include( "TriggerRelease/jobOfragment_ReadBS.py" ) 
# Note that this requires Event/ByteStreamCnvSvcBase/share/BSAddProvSvc_RIO_jobOptions.py revision 1.9
#avoid doing BS read configuration twice

#Modification: Walter Lampl, 16 July 2009
# Merge this file with RecExCommission/RecExCommission_BSRead_config_hack.py and RecExCommission/RecExCommissionBSCnv_jobOptions.py


from AthenaCommon.Logging import logging
logBSRead = logging.getLogger( 'BSRead_config' )


svcMgr = theApp.serviceMgr()
if not hasattr( svcMgr, "ByteStreamAddressProviderSvc" ):
    logBSRead.error("Can't find ByteStreamAddressProviderSvc!")

if not athenaCommonFlags.isOnline() and not rec.readTAG():
    try:
        #svcMgr.ByteStreamInputSvc.ProcessBadEvent=True
        svcMgr.EventSelector.ProcessBadEvent=True
    except Exception:
        treatException ("could not set svcMgr.EventSelector.ProcessBadEvent=True")



from RecExConfig.RecFlags import rec
from AthenaCommon.DetFlags import DetFlags
if rec.doInDet():
    include ("InDetRecExample/InDetReadBS_jobOptions.py")

if rec.doMuon():
    include("MuonCnvExample/MuonReadBS_jobOptions.py")

if DetFlags.readRDOBS.LAr_on():
    if globalflags.DataSource()=='data':
        svcMgr.ByteStreamAddressProviderSvc.TypeNames += ["LArFebHeaderContainer/LArFebHeader"]
    else: # Bytestream from MC
        svcMgr.ByteStreamAddressProviderSvc.TypeNames += ["LArRawChannelContainer/LArRawChannels"]

    #everything else is take care of by LArRawChanneGetter.py
        
##         from LArROD.LArRODFlags import larRODFlags
##         if larRODFlags.readDigits() or larRODFlags.doDSP():
##             svcMgr.ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/FREE"]
##         #if larRODFlags.readRawChannels() or not larRODFlags.doDSP():
##         #    svcMgr.ByteStreamAddressProviderSvc.TypeNames += ["LArRawChannelContainer/LArRawChannels"]
##     else: # Bytestream from MC
##         svcMgr.ByteStreamAddressProviderSvc.TypeNames += ["LArRawChannelContainer/LArRawChannels"]
        
if DetFlags.readRDOBS.Tile_on():
    svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
        "TileRawChannelContainer/TileRawChannelCnt",
        "TileRawChannelContainer/MuRcvRawChCnt",
        "TileDigitsContainer/MuRcvDigitsCnt",
        "TileL2Container/TileL2Cnt"
      ]

if DetFlags.readRDOBS.LVL1_on():
    svcMgr.ByteStreamAddressProviderSvc.TypeNames += [                
        "ROIB::RoIBResult/RoIBResult",
        "MuCTPI_RDO/MUCTPI_RDO",        
        "CTP_RDO/CTP_RDO",
        "L1TopoRDOCollection/L1TopoRDOCollection"
        ] 

#if ATLASCosmicFlags.doLVL1Calo:
if rec.doTrigger() :
    include ("TrigT1CaloByteStream/ReadLVL1CaloBS_jobOptions.py")

if rec.doTrigger() :
    if DetFlags.readRIOBS.Tile_on():
        svcMgr.ByteStreamAddressProviderSvc.TypeNames += [      
            "TileCellIDC/TileCellIDC",
            ]

    if DetFlags.readRIOBS.pixel_on():
        svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
            "Trk::PixelClusterContainer/PixelOnlineClusters",
            ]

    if DetFlags.readRIOBS.SCT_on() :
        svcMgr.ByteStreamAddressProviderSvc.TypeNames += [          
            "Trk::SCT_ClusterContainer/SCT_OnlineClusters",
            ]

    if DetFlags.readRIOBS.TRT_on() :
        svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
            "Trk::TRT_DriftCircleContainer/TRT_DriftCircle",
            ]

    svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
        "HLT::HLTResult/HLTResult_L2",
        "HLT::HLTResult/HLTResult_EF"
        ]
    
if DetFlags.readRDOBS.Lucid_on():
    protectedInclude ("LUCID_RawDataByteStreamCnv/LUCID_ByteStreamRawDataCnv.py")

if DetFlags.readRDOBS.ZDC_on() and rec.doTrigger():
    protectedInclude ("ZdcByteStream/ZdcByteStreamRawDataCnvV2.py")
    svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::TriggerTowerContainer/ZdcTriggerTowers"]	
    svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::TriggerTowerAuxContainer/ZdcTriggerTowersAux."]
