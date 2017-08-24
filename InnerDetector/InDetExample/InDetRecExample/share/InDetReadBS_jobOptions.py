from InDetRecExample.InDetJobProperties import InDetFlags
from InDetRecExample.InDetKeys import InDetKeys
# --- load cabling setup
include ("InDetRecExample/InDetRecCabling.py")

if DetFlags.readRDOBS.pixel_on():
  from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConf import PixelRodDecoder
  InDetPixelRodDecoder = PixelRodDecoder(name = "InDetPixelRodDecoder")
  ToolSvc += InDetPixelRodDecoder
  if (InDetFlags.doPrintConfigurables()):
    print      InDetPixelRodDecoder
  #InDetPixelRodDecoder.OutputLevel = VERBOSE
  
  from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConf import PixelRawDataProviderTool
  InDetPixelRawDataProviderTool = PixelRawDataProviderTool(name    = "InDetPixelRawDataProviderTool",
                                                           Decoder = InDetPixelRodDecoder)
  ToolSvc += InDetPixelRawDataProviderTool
  if (InDetFlags.doPrintConfigurables()):
    print      InDetPixelRawDataProviderTool
  #InDetPixelRawDataProviderTool.OutputLevel = VERBOSE
  
  # load the PixelRawDataProvider
  from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConf import PixelRawDataProvider
  InDetPixelRawDataProvider = PixelRawDataProvider(name         = "InDetPixelRawDataProvider",
                                                   RDOKey       = InDetKeys.PixelRDOs(),
                                                   ProviderTool = InDetPixelRawDataProviderTool)
  topSequence += InDetPixelRawDataProvider
  if (InDetFlags.doPrintConfigurables()):
    print          InDetPixelRawDataProvider
  #InDetPixelRawDataProvider.OutputLevel = VERBOSE

  #ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "PixelRDO_Container/"+InDetKeys.PixelRDOs() ]

if DetFlags.readRDOBS.SCT_on():
  from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConf import SCT_RodDecoder
  InDetSCTRodDecoder = SCT_RodDecoder(name        = "InDetSCTRodDecoder",
                                      TriggerMode = False)
  ToolSvc += InDetSCTRodDecoder
  if (InDetFlags.doPrintConfigurables()):
    print      InDetSCTRodDecoder
  #InDetSCTRodDecoder.OutputLevel = VERBOSE
  
  from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConf import SCTRawDataProviderTool
  InDetSCTRawDataProviderTool = SCTRawDataProviderTool(name    = "InDetSCTRawDataProviderTool",
                                                      Decoder = InDetSCTRodDecoder)
  ToolSvc += InDetSCTRawDataProviderTool
  if (InDetFlags.doPrintConfigurables()):
    print      InDetSCTRawDataProviderTool
  #InDetSCTRawDataProviderTool.OutputLevel = VERBOSE
  
  # load the SCTRawDataProvider
  from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConf import SCTRawDataProvider
  InDetSCTRawDataProvider = SCTRawDataProvider(name         = "InDetSCTRawDataProvider",
                                              RDOKey       = InDetKeys.SCT_RDOs(),
                                              ProviderTool = InDetSCTRawDataProviderTool)
  if (InDetFlags.doPrintConfigurables()):
    print          InDetSCTRawDataProvider
  topSequence += InDetSCTRawDataProvider
  #InDetSCTRawDataProvider.OutputLevel = VERBOSE

  #ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "SCT_RDO_Container/"+InDetKeys.SCT_RDOs()]

if DetFlags.readRDOBS.TRT_on():
  from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConf import TRT_RodDecoder
  InDetTRTRodDecoder = TRT_RodDecoder(name = "InDetTRTRodDecoder",
                                      LoadCompressTableDB = (globalflags.DataSource() != 'geant4'))
  
  ToolSvc += InDetTRTRodDecoder
  if (InDetFlags.doPrintConfigurables()):
    print      InDetTRTRodDecoder
  #InDetTRTRodDecoder.OutputLevel = VERBOSE
  
  from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConf import TRTRawDataProviderTool
  InDetTRTRawDataProviderTool = TRTRawDataProviderTool(name    = "InDetTRTRawDataProviderTool",
                                                      Decoder = InDetTRTRodDecoder)
  ToolSvc += InDetTRTRawDataProviderTool
  if (InDetFlags.doPrintConfigurables()):
    print      InDetTRTRawDataProviderTool
  #InDetTRTRawDataProviderTool.OutputLevel = VERBOSE
  
  # load the TRTRawDataProvider
  from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConf import TRTRawDataProvider
  InDetTRTRawDataProvider = TRTRawDataProvider(name         = "InDetTRTRawDataProvider",
                                              RDOKey       = InDetKeys.TRT_RDOs(),
                                              ProviderTool = InDetTRTRawDataProviderTool)
  topSequence += InDetTRTRawDataProvider
  if (InDetFlags.doPrintConfigurables()):
    print          InDetTRTRawDataProvider
  #InDetTRTRawDataProvider.OutputLevel = VERBOSE

  #ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "TRT_RDO_Container/"+InDetKeys.TRT_RDOs() ]

if DetFlags.readRDOBS.BCM_on():
  from BCM_RawDataByteStreamCnv.BCM_RawDataByteStreamCnvConf import BCM_RodDecoder
  InDetBCMRodDecoder = BCM_RodDecoder(name = "InDetBCMRodDecoder")
  ToolSvc += InDetBCMRodDecoder
  if (InDetFlags.doPrintConfigurables()):
    print      InDetBCMRodDecoder
  #InDetBCMRodDecoder.OutputLevel = VERBOSE
  
  from BCM_RawDataByteStreamCnv.BCM_RawDataByteStreamCnvConf import BCM_RawDataProviderTool
  InDetBCMRawDataProviderTool = BCM_RawDataProviderTool(name    = "InDetBCMRawDataProviderTool",
                                                        Decoder = InDetBCMRodDecoder)
  ToolSvc += InDetBCMRawDataProviderTool
  if (InDetFlags.doPrintConfigurables()):
    print      InDetBCMRawDataProviderTool
  #InDetBCMRawDataProviderTool.OutputLevel = VERBOSE
  
  # load the BCM_RawDataProvider
  from BCM_RawDataByteStreamCnv.BCM_RawDataByteStreamCnvConf import BCM_RawDataProvider
  InDetBCMRawDataProvider = BCM_RawDataProvider(name         = "InDetBCMRawDataProvider",
                                                RDOKey       = InDetKeys.BCM_RDOs(),
                                                ProviderTool = InDetBCMRawDataProviderTool)
  topSequence += InDetBCMRawDataProvider
  if (InDetFlags.doPrintConfigurables()):
    print          InDetBCMRawDataProvider
  #InDetBCMRawDataProvider.OutputLevel = VERBOSE

  #ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "BCM_RDO_Container/"+InDetKeys.BCM_RDOs() ]
