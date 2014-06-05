#==============================================================
#
# Job options file
#
# @file NtupleFragment.py
#
# @brief Includes CTP and MuCTPI ntuple algorithms.
#
# @author David Berge <berge@cern.ch>
#
#==============================================================
    
include( "TrigT1CTMonitoring/CTPFlags.py" )
 
## to decode the MuCTPI bytestream
from TrigT1ResultByteStream.TrigT1ResultByteStreamConf import MuCTPIByteStreamTool,RecMuCTPIByteStreamTool
## to decode the CTP bytestream
from TrigT1ResultByteStream.TrigT1ResultByteStreamConf import CTPByteStreamTool,RecCTPByteStreamTool


if hasattr(svcMgr,"LVL1ConfigSvc"):
    log.info("From TrigT1CTMonitoring NtupleFragment, ServiceMgr already includes LVL1ConfigSvc")
else:
    log.info("From TrigT1CTMonitoring NtupleFragment, setup LVL1ConfigSvc and add instance to ServiceMgr")
    from TrigConfigSvc.TrigConfigSvcConfig import LVL1ConfigSvc
    LVL1ConfigSvc = LVL1ConfigSvc('LVL1ConfigSvc')
    svcMgr += LVL1ConfigSvc

svcMgr.LVL1ConfigSvc.ConfigSource = "XML"
from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
tf.inputLVL1configFile = "LVL1config_SingleBeam_v1_7-bit_trigger_types_20080905.xml"
svcMgr.LVL1ConfigSvc.XMLFile = tf.inputLVL1configFile()
svcMgr.LVL1ConfigSvc.CreateLegacyObjects = True
svcMgr.LVL1ConfigSvc.DumpTTVmap = False

if CBNTAthenaAware :

    if CTPFlags.doMuRDO:
        if not "CBNTAA_ReadMuCTPI_RDO/CBNTAA_ReadMuCTPI_RDO" in CBNT_AthenaAware.Members :
            from TrigT1Muctpi.TrigT1MuctpiConf import CBNTAA_ReadMuCTPI_RDO
            CBNTAA_ReadMuCTPI_RDO = CBNTAA_ReadMuCTPI_RDO()
            CBNTAA_ReadMuCTPI_RDO.StoreRawData = False
            CBNT_AthenaAware += CBNTAA_ReadMuCTPI_RDO
        
    if CTPFlags.doMuRIO:
        if not "CBNTAA_ReadMuCTPI_RIO/CBNTAA_ReadMuCTPI_RIO" in CBNT_AthenaAware.Members :
            from TrigT1Muctpi.TrigT1MuctpiConf import CBNTAA_ReadMuCTPI_RIO
            CBNTAA_ReadMuCTPI_RIO = CBNTAA_ReadMuCTPI_RIO()
            CBNT_AthenaAware += CBNTAA_ReadMuCTPI_RIO

    if rec.doMuon:
        if not "CBNTAA_MuctpiRoI/CBNTAA_MuctpiRoI" in CBNT_AthenaAware.Members :
            from TrigT1Muctpi.TrigT1MuctpiConf import CBNTAA_MuctpiRoI
            CBNTAA_MuctpiRoI = CBNTAA_MuctpiRoI()
            #      CBNTAA_MuctpiRoI.RoISource = "roi"
            CBNT_AthenaAware += CBNTAA_MuctpiRoI
                           
    if CTPFlags.doCTRDO:
        if not "LVL1CTP::CBNTAA_CTP_RDO/LVL1CTP::CBNTAA_CTP_RDO" in CBNT_AthenaAware.Members :
            from TrigT1CTP.TrigT1CTPConf import LVL1CTP__CBNTAA_CTP_RDO
            CBNTAA_CTP_RDO = LVL1CTP__CBNTAA_CTP_RDO("CBNTAA_CTP_RDO")
            CBNT_AthenaAware += CBNTAA_CTP_RDO

    if CTPFlags.doCTRIO:
        if not "LVL1CTP::CBNTAA_ReadCTP_RIO/LVL1CTP::CBNTAA_ReadCTP_RIO" in CBNT_AthenaAware.Members :
            from TrigT1CTP.TrigT1CTPConf import LVL1CTP__CBNTAA_ReadCTP_RIO
            LVL1CTP__CBNTAA_ReadCTP_RIO = LVL1CTP__CBNTAA_ReadCTP_RIO()
            CBNT_AthenaAware += LVL1CTP__CBNTAA_ReadCTP_RIO
