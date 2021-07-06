
if not 'LArDigitKey' in dir():
    LArDigitKey="LArDigitContainer_MC"

if not 'LArRawChannelKey' in dir():
    LArRawChannelKey="LArRawChannels"

MCFlag = GlobalFlags.DataSource.is_geant4()

if MCFlag :
    PedestalKey = "LArPedestal"
else:
    PedestalKey = "Pedestal"    

#more tools to be configured
from LArRecUtils.LArRecUtilsConf import LArOFPeakRecoTool
theLArOFPeakRecoTool=LArOFPeakRecoTool()
theLArOFPeakRecoTool.KeyShape="LArShape"
theLArOFPeakRecoTool.UseShape=False
ToolSvc += theLArOFPeakRecoTool

from LArRecUtils.LArADC2MeVCondAlgDefault import LArADC2MeVCondAlgDefault
LArADC2MeVCondAlgDefault()

from LArROD.LArRODConf import *

# theApp.TopAlg += [ "LArRawChannelBuilderDriver/LArRawChannelBuilder" ]
LArRawChannelBuilder = LArRawChannelBuilderDriver("LArRawChannelBuilder")
LArRawChannelBuilder.DataLocation    = LArDigitKey
LArRawChannelBuilder.LArRawChannelContainerName = LArRawChannelKey
LArRawChannelBuilder.DefaultShiftTimeSample=1

# builder tools
builderToolOFCIter = LArRawChannelBuilderToolOFCIter("builderToolOFCIter")
builderToolOFCIter.minSample = 2 
builderToolOFCIter.maxSample = 8
builderToolOFCIter.minADCforIter=15
builderToolOFCIter.defaultPhase=12
builderToolCub = LArRawChannelBuilderToolCubic("builderToolCub")
builderToolCub.minADCforCubic=30 
builderToolPar = LArRawChannelBuilderToolParabola("builderToolPar")
builderToolAve = LArRawChannelBuilderToolAverage("builderToolAve")
builderToolAve.NScan=0

# adc2energy tools
adc2eToolDB = LArRawChannelBuilderADC2EDataBase("adc2eToolDB")
adc2eToolCo = LArRawChannelBuilderADC2EConstants("adc2eToolCo")

# pedestal tools
pedestalToolDB = LArRawChannelBuilderPedestalDataBase("pedestalToolDB")
pedestalToolDB.LArPedestalKey = PedestalKey
pedestalToolSZ = LArRawChannelBuilderPedestalSampleZero("pedestalToolSZ")

# make Tools public
ToolSvc += builderToolOFCIter
ToolSvc += builderToolOFCIter
ToolSvc += builderToolCub
ToolSvc += builderToolAve
ToolSvc += adc2eToolDB
ToolSvc += adc2eToolCo
ToolSvc += pedestalToolDB
ToolSvc += pedestalToolSZ

###
LArRawChannelBuilder.BuilderTools     = [ ]
# LArRawChannelBuilder.BuilderTools     += [ builderToolOFC ]
# LArRawChannelBuilder.BuilderTools     += [ builderToolOFCIter ]
LArRawChannelBuilder.BuilderTools     += [ builderToolCub ]
LArRawChannelBuilder.BuilderTools     += [ builderToolAve ]

LArRawChannelBuilder.ADCtoEnergyTools  = [ ]
LArRawChannelBuilder.ADCtoEnergyTools += [ adc2eToolDB ]
LArRawChannelBuilder.ADCtoEnergyTools += [ adc2eToolCo ]

LArRawChannelBuilder.PedestalTools    = [ ]
LArRawChannelBuilder.PedestalTools   += [ pedestalToolDB ]
LArRawChannelBuilder.PedestalTools   += [ pedestalToolSZ ]

# and add the BuilderDriver to the Algorithms
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += LArRawChannelBuilder

print "Rolf"
print LArRawChannelBuilder

