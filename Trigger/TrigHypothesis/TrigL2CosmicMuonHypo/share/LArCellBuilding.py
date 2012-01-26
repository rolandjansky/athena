
print "Denis"
from AthenaCommon.AppMgr import ToolSvc

for x in ToolSvc.getChildren():
    print x.name()

from LArByteStream.LArByteStreamConf import LArRodDecoder

ToolSvc += LArRodDecoder()
ToolSvc.LArRodDecoder.UseCellMakerTool = True
ToolSvc.LArRodDecoder.OutputLevel = FATAL
ToolSvc.LArRodDecoder.MultiDSPMode=True

# configurable migration
#theApp.Dlls += [ "LArRawUtils", "LArROD", "LArCalibUtils", "LArTools", "LArRecUtils" ]

from LArROD.LArRODConf import LArCellBuilderDriver,LArRawChannelBuilderToolCubic,LArRawChannelBuilderToolAverage,LArRawChannelBuilderPedestalSampleZero,LArRawChannelBuilderADC2EConstants

ToolSvc+=LArCellBuilderDriver()
t1=LArRawChannelBuilderToolCubic()
t2=LArRawChannelBuilderToolAverage()
t3=LArRawChannelBuilderADC2EConstants()
t4=LArRawChannelBuilderPedestalSampleZero()

TheToolList=[t1,t2]
ToolSvc += TheToolList
ToolSvc += [t3,t4]

ToolSvc.LArCellBuilderDriver.BuilderTools = TheToolList
ToolSvc.LArCellBuilderDriver.DefaultShiftTimeSample=1

ToolSvc.LArRawChannelBuilderToolCubic.minADCforCubic=30 
ToolSvc.LArRawChannelBuilderToolAverage.AverageSamplesEM=4
ToolSvc.LArRawChannelBuilderToolAverage.NScan=0


ToolSvc.LArCellBuilderDriver.ADCtoEnergyTools = [t3]
ToolSvc.LArCellBuilderDriver.PedestalTools = [t4]

ToolSvc.LArCellBuilderDriver.OutputLevel = ERROR
ToolSvc.LArCellBuilderDriver.properties()

#ToolSvc += LArRawChannelBuilderPedestalDataBase()
#ToolSvc.LArRawChannelBuilderPedestalDataBase.LArPedestalKey = "Pedestal"

# using all commissioning conditions data now 
#ToolSvc.LArAutoCorrTotalToolDefault.MCSym = True
#ToolSvc.LArAutoCorrTotalToolDefault.IsMC = True

# recipe for 14.X
#from CaloTools.CaloToolDefault import CaloNoiseToolDefault
#ToolSvc += CaloNoiseToolDefault()

if useCOMCONDDB:
  from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
  ToolSvc += CaloNoiseToolDefault()

  ToolSvc.CaloNoiseToolDefault.keyPedestal = "Pedestal"
  ToolSvc.CaloNoiseToolDefault.IsMC  = False
  ToolSvc.CaloNoiseToolDefault.UseSymmetry  = True
#ToolSvc.CaloNoiseToolDefault.DiagnosticHG = False
#ToolSvc.CaloNoiseToolDefault.DiagnosticLG = False
#ToolSvc.CaloNoiseToolDefault.DiagnosticMG = False

#from LArRecUtils.LArADC2MeVToolDefault import LArADC2MeVToolDefault
#ToolSvc += LArADC2MeVToolDefault("LArADC2MeVTool")

#ToolSvc.LArADC2MeVToolDefault.MCSym = True
#ToolSvc.LArADC2MeVToolDefault.UseMphysOverMcal = True
#ToolSvc.LArADC2MeVToolDefault.keyMphysMcal = "LArMphysOverMcal"

# this is still neede by 

#from LArRecUtils.LArOFCToolDefault import LArOFCToolDefault
#ToolSvc += LArOFCToolDefault("LArOFCTool")

#ToolSvc.LArOFCToolDefault.MCSym = True
#ToolSvc.LArOFCToolDefault.IsMC  = True
#ToolSvc.LArOFCToolDefault.keyOFC = "LArOFC"
#ToolSvc.LArOFCToolDefault.FromDatabase = False

ToolSvc.LArCellBuilderDriver.OutputLevel = ERROR
