include.block("TrigAFPReco/TrigAFPRecoDefault.py")

# hits reconstruction tool
from AFP_Raw2Digi.AFP_Raw2DigiConf import AFP_Raw2DigiTool
raw2DigiTool=AFP_Raw2DigiTool("AFP_Raw2DigiTool")
ToolSvc+=raw2DigiTool

# track reconstruction tool
from AFP_LocReco.AFP_LocRecoConf import AFPSiDBasicKalmanTool
basicKalman=AFPSiDBasicKalmanTool("AFPSiDBasicKalmanTool")
ToolSvc+=basicKalman


from TrigAFPReco.TrigAFPRecoConf import AFPSiTrkReco
afpTrkAlg = AFPSiTrkReco()
topSequence += afpTrkAlg

afpTrkAlg.DigiTool = raw2DigiTool
afpTrkAlg.SIDTool = basicKalman
