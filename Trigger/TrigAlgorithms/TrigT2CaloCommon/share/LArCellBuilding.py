include.block("TrigT2CaloCommon/LArCellBuilding.py")

if jobproperties.Global.DataSource.get_Value() == 'data':

 from AthenaCommon.AppMgr import ToolSvc
 from LArByteStream.LArByteStreamConf import LArRodDecoder

 from LArConditionsCommon.LArCondFlags import larCondFlags

 ToolSvc += LArRodDecoder()
 ToolSvc.LArRodDecoder.UseCellMakerTool = True
 ToolSvc.LArRodDecoder.OutputLevel = FATAL
 ToolSvc.LArRodDecoder.MultiDSPMode=True

 dbon=( larCondFlags.LoadElecCalib() and larCondFlags.SingleVersion() )

 from LArROD.LArRODConf import LArCellBuilderDriver,LArRawChannelBuilderToolCubic,LArRawChannelBuilderToolAverage,LArRawChannelBuilderPedestalSampleZero,LArRawChannelBuilderADC2EConstants
 if dbon:
  from LArRecUtils.LArADC2MeVToolDefault import LArADC2MeVToolDefault
  from LArROD.LArRODConf import LArRawChannelBuilderToolOFCIter, LArRawChannelBuilderPedestalDataBase, LArRawChannelBuilderADC2EDataBase
  theLArRawChannelBuilderToolOFCIter=LArRawChannelBuilderToolOFCIter()
  theLArRawChannelBuilderToolOFCIter.minSample = 2
  theLArRawChannelBuilderToolOFCIter.maxSample = 12
  theLArRawChannelBuilderToolOFCIter.minADCforIterInSigma=4 # ADCmax at least 4 sigma above noise for iteration
  theLArRawChannelBuilderToolOFCIter.minADCforIter=15 # min adc for iteration (only if no pedestalRMS found)
  theLArRawChannelBuilderToolOFCIter.defaultPhase=0    # starting delay, also the fixed delay for ADC below min.
  theLArRawChannelBuilderToolOFCIter.ECut=250.         # Energy to save quality
 
  # Pedestal
  # main method from database
  theLArRawChannelBuilderPedestalDataBase=LArRawChannelBuilderPedestalDataBase()
  theLArRawChannelBuilderPedestalDataBase.LArPedestalKey = "Pedestal"
  # ADC to energy
  # Some other tool before
  from LArRecUtils.LArADC2MeVToolDefault import LArADC2MeVToolDefault
  theADC2MeVTool = LArADC2MeVToolDefault()
  ToolSvc += theADC2MeVTool

  # main method from database
  from LArROD.LArRODConf import LArRawChannelBuilderADC2EDataBase
  theLArRawChannelBuilderADC2EDataBase=LArRawChannelBuilderADC2EDataBase()
  theLArRawChannelBuilderADC2EDataBase.ADC2MeVTool = theADC2MeVTool
  #more tools to be configured
  from LArRecUtils.LArRecUtilsConf import LArOFPeakRecoTool
  theLArOFPeakRecoTool=LArOFPeakRecoTool()
  from LArConditionsCommon.LArCondFlags import larCondFlags
  theLArOFPeakRecoTool.UseShape=larCondFlags.useShape()

 ToolSvc+=LArCellBuilderDriver()
 t2=LArRawChannelBuilderToolCubic()
 t3=LArRawChannelBuilderToolAverage()
 t5=LArRawChannelBuilderADC2EConstants()
 t6=LArRawChannelBuilderPedestalSampleZero()

 if dbon:
  TheToolList=[theLArRawChannelBuilderToolOFCIter,t2,t3]
  ToolSvc += theLArRawChannelBuilderPedestalDataBase
  ToolSvc += theLArRawChannelBuilderADC2EDataBase
  ToolSvc.LArCellBuilderDriver.DefaultShiftTimeSample=1
  ToolSvc.LArCellBuilderDriver.DataLocation    = "FREE"
 else:
  TheToolList=[t2,t3]

 ToolSvc += TheToolList
 ToolSvc += [t5,t6]

 ToolSvc.LArCellBuilderDriver.BuilderTools = TheToolList
 ToolSvc.LArCellBuilderDriver.DefaultShiftTimeSample=1

 ToolSvc.LArRawChannelBuilderToolCubic.minADCforCubic=30 
 ToolSvc.LArRawChannelBuilderToolAverage.AverageSamplesEM=4
 ToolSvc.LArRawChannelBuilderToolAverage.NScan=0

 if dbon:
  ToolSvc.LArCellBuilderDriver.ADCtoEnergyTools = [theLArRawChannelBuilderADC2EDataBase,t5]
  ToolSvc.LArCellBuilderDriver.PedestalTools = [theLArRawChannelBuilderPedestalDataBase,t6]
 else:
  ToolSvc.LArCellBuilderDriver.ADCtoEnergyTools = [t5]
  ToolSvc.LArCellBuilderDriver.PedestalTools = [t6]

 ToolSvc.LArCellBuilderDriver.OutputLevel = ERROR
 ToolSvc.LArCellBuilderDriver.properties()

