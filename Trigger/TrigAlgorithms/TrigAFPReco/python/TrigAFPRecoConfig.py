from TrigAFPReco.TrigAFPRecoConf  import Trig_AFPSiTrkReco

class AFPSiTrkReco(Trig_AFPSiTrkReco):
    def __init__(self,
                 name = "AFPSiTrkReco",
                 raw_data_key = "AFP_RawData"):
        super (Trig_AFPSiTrkReco, self).__init__(name)
        from AthenaCommon.AppMgr import ToolSvc
        # set name of the collection with raw data
        self.AFP_RawDataCollectionKey = "AFP_RawData"
        # set name of the container with AFP tracks
        self.AFP_SiHitContainerName = "AFPSiHitContainer"
        # set name of the container with AFP tracks
        self.AFP_TrackContainer = "AFPTrackContainer"
        # configure AFP raw data provider tool
        from AFP_ByteStream2RawCnv.AFP_ByteStream2RawCnvConf import AFP_RawDataProviderTool 
        rawTool = AFP_RawDataProviderTool("AFP_RawDataProviderTool")
        ToolSvc += rawTool
        self.RawDataProviderTool = rawTool
        # configure raw to digi tool
        from AFP_Raw2Digi.AFP_Raw2DigiConf import AFP_Raw2DigiTool
        raw2DigiTool = AFP_Raw2DigiTool("AFP_Raw2DigiTool")
        raw2DigiTool.AFPSiHitsContainerName = self.AFP_SiHitContainerName
        ToolSvc += raw2DigiTool
        self.DigiTool = raw2DigiTool
        # configure track reconstruction tool
        from AFP_LocReco.AFP_LocRecoConf import AFPSiDBasicKalmanTool
        siDTool = AFPSiDBasicKalmanTool("AFPSiDBasicKalmanTool")
        siDTool.tracksContainerName = self.AFP_TrackContainer
        ToolSvc += siDTool
        self.SiDTool = siDTool
