include.block ('ForwardRec/ForwardRec_jobOptions.py')

from AthenaCommon.Resilience import treatException   
from AthenaCommon.GlobalFlags  import globalflags
from AthenaCommon.DetFlags import DetFlags
from RecExConfig.RecFlags import rec

if (rec.doLucid):
  include ( "ForwardRec/LUCID_Rec_jobOptions.py" )

#if (rec.doForwardDet and rec.doZdc):
#  include ( "ZdcRec/ZdcRec_jobOptions.py" )

if DetFlags.makeRIO.ZDC_on() and not rec.doWriteBS() : 
  try:
      #from ZdcRec.ZdcRawChannelGetter import ZdcRawChannelGetter
      #ZdcRawChannelGetter()
    from ZdcRec.ZdcModuleGetter import ZdcModuleGetter
    ZdcModuleGetter()
  except Exception:
    treatException("Problem with ZdcModuleGetter. Switched off.")
    DetFlags.makeRIO.ZDC_setOff()

if rec.doAlfa() and rec.doESD():
    if DetFlags.readRDOBS.ALFA_on():
        #Needed for real-data reconstruction:
        from ALFA_RawDataByteStreamCnv.ALFA_RawDataByteStreamCnvConf import ALFA_RawDataProvider
        topSequence+=ALFA_RawDataProvider()

        from ALFA_Raw2Digit.ALFA_Raw2DigitConf import ALFA_Raw2Digit
        
        topSequence+=ALFA_Raw2Digit(MeasuredDataType = "tunnel")
        pass

    #For both, real and simulated data:
    #from IOVDbSvc.CondDB import conddb
    #conddb.addFolder("FWD","/FWD/ALFA/position_calibration<tag>FWDALFAposition_calibration-run373-v2</tag>")

    include("ALFA_LocRec/ALFA_LocRec_joboption.py")
    include("ALFA_LocRecCorr/ALFA_LocRecCorr_joboption.py")
    include("ALFA_CLinkAlg/ALFA_CLinkAlg_joboption.py")        

if rec.doAFP() and rec.doESD():
  
  # Real-data reconstruction:
  if DetFlags.readRDOBS.AFP_on():
    from AFP_ByteStream2RawCnv.AFP_ByteStream2RawCnvConf import AFP_RawDataProvider
    topSequence+=AFP_RawDataProvider()

    from AFP_Raw2Digi.AFP_Raw2DigiConf import AFP_Raw2Digi
    topSequence+=AFP_Raw2Digi()
  
  #cluster reconstruction
  from AFP_SiClusterTools.AFP_SiClusterTools import AFP_SiClusterTools_Cfg
  topSequence+=AFP_SiClusterTools_Cfg()
  
  # tracks reconstruction
  from AFP_LocReco.AFP_LocReco import AFP_LocReco_SiD_Cfg, AFP_LocReco_TD_Cfg
  topSequence+=AFP_LocReco_SiD_Cfg()
  topSequence+=AFP_LocReco_TD_Cfg()
   
  # protons reconstruction
  from AFP_GlobReco.AFP_GlobReco import AFP_GlobReco_Cfg
  topSequence+=AFP_GlobReco_Cfg()
  
  # vertex reconstruction
  from AFP_VertexReco.AFP_VertexReco import AFP_VertexReco_Cfg
  topSequence+=AFP_VertexReco_Cfg()
  
