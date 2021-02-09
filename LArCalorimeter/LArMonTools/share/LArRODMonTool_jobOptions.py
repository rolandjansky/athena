# --- for athena online running ---
if 'EventBlockSize' not in dir():
    EventBlockSize=0

if 'dumpdir' not in dir():
    dumpdir = "" # ""

######## NOTE ON OPTIONS FOR LArRODMonTool ######
### TimeOFCUnitOnline --> This depends on the version of OnlineDB/WeightsMaker (old version prepare OFC with LSB = 1ns, new version prepare OFC with LSB = 10ps)
### TimeOFCUnitOffline --> Time offline is given in picoseconds, so this should be set to 1. If you want time offline in ns, set to 1000.
###
### WARNING : This goes along with a proper version of LArCnv/LArByteStream/LArRodBlockPhysicsV5 ! ( >= LArByteStream-00-01-13 for "new" WeightsMaker)
### WARNING : Ranges and precision values should be adapted to these units ! :
###
### If "old" weightsmaker, precision is 1ns for all ranges:
###
###    PrecisionTRange0 = 1000, # ps
###    PrecisionTRange1 = 1000,
###    PrecisionTRange2 = 1000,
###    PrecisionTRange3 = 1000,
###    PrecisionTRangeMax = 1000,
###
### If "new" weightsmaker, precision is :
###    PrecisionTRange0 = 10, # ps
###    PrecisionTRange1 = 20,
###    PrecisionTRange2 = 30,
###    PrecisionTRange3 = 50,
###    PrecisionTRangeMax = 50,
###
###
### TQthreshold --> threshold to calculate T and Q in the DSP. Typical value is 250 MeV, however some tests are made with this threshold at 0.
#################################################


# Retrieve first sample from DB if possible. Default is 3, most usual case in 7 samples.
# This info will be used to tune the timeOffset option (default is 0 for now)
from LArConditionsCommon.LArCool import larcool
if (larcool is not None):
    firstsample = larcool.firstSample()
else:
    firstsample = 3

#timeoffset = (firstsample - 3) or (3 - firstsample) ... to be checked.

#from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
#theCaloNoiseTool = CaloNoiseToolDefault()
#ToolSvc += theCaloNoiseTool

from LArMonTools.LArMonToolsConf import LArRODMonTool
theLArRODMonTool = LArRODMonTool(name="LArRODMonTool",
                                 LArDigitContainerKey      = LArMonFlags.LArDigitKey(),
                                 useEvtCounter             = True,
                                 LArRawChannelKey_fromBytestream = "LArRawChannels",  #for Tier0
                                 LArRawChannelKey_fromDigits = "LArRawChannels_FromDigits", #for Tier0
#                                 LArRawChannelKey_fromBytestream = "LArRawChannels_fB", #for lxplus and online
#                                 LArRawChannelKey_fromDigits = "LArRawChannels", #for lxplus and online
                                 DigitsFileName = dumpdir + "digits.txt",
                                 EnergyFileName = dumpdir + "energy.txt",
                                 AiFileName = dumpdir + "Calib_ai.dat",
                                 DumpCellsFileName = dumpdir + "DumpCells.txt",
                                 DoDspTestDump = False, #F
                                 DoCellsDump = False, #F
                                 DoCheckSum = True,   # Check the Checksum
                                 DoRodStatus = True,   # Check the Rod Status
                                 PrintEnergyErrors = True, #True,
                                 # Energy ranges (in MeV)
                                 ERange0 = 8192, # MeV
                                 ERange1 = 65536,
                                 ERange2 = 524288,
                                 ERange3 = 4194304,
                                 # Time ranges (in ps)
                                 TRange0 = 1000, # ps
                                 TRange1 = 5000,
                                 TRange2 = 25000,
                                 TRange3 = 50000,
                                 # Quality factor ranges
                                 # Only 1 range for now, and Q < 2^16 anyway
                                 QRange0 = 65536, # only one range for now ...
                                 QRange1 = 65536,
                                 QRange2 = 65536,
                                 QRange3 = 65536,
                                 # Expected precision for E calculation (in MeV)
                                 #PrecisionERange0 = 1, # MeV (Precision on E is on Eoff - Eonl)
                                 #PrecisionERange1 = 8,
                                 #PrecisionERange2 = 64,
                                 #PrecisionERange3 = 512,
                                 #PrecisionERangeMax = 8192,

                                 #Adding 1MeV on request of Alexis (truncating difference) (May 2016):

                                 PrecisionERange0 = 2, # MeV (Precision on E is on Eoff - Eonl)
                                 PrecisionERange1 = 9,
                                 PrecisionERange2 = 65,
                                 PrecisionERange3 = 513,
                                 PrecisionERangeMax = 8192,


                                 # Expected precision for T calculation (in ps)
                                 PrecisionTRange0 = 340, # ps (Precision on T is on Toff - Tonl)
                                 PrecisionTRange1 = 340,
                                 PrecisionTRange2 = 340,
                                 PrecisionTRange3 = 340,
                                 PrecisionTRangeMax = 340,
                                 # Expected precision for Q calculation
                                 PrecisionQRange0 = 3, # ADC (Precision on Q is on (Qoff - Qonl)/Sqrt(Qoff))
                                 PrecisionQRange1 = 3,
                                 PrecisionQRange2 = 3,
                                 PrecisionQRange3 = 3,
                                 PrecisionQRangeMax = 3,
                                 # Some other flags
                                 LArBadChannelMask = theLArBadChannelsMasker,
                                 SkipKnownProblematicChannels = False,
                                 SkipNullPed = False,
                                 SkipNullQT = True,
                                 TimeOFCUnitOnline = 1.,
                                 TimeOFCUnitOffline = 1.,
                                 TimeOffset = 0., # BC # just to keep the possibility to offset the time by hand
                                 ADCthreshold = 0,
                                 peakTimeCut = 5.,
                                 IsOnline = False,
                                 numberOfLB = 3000.,
                                 Streams = ["express","L1Calo","L1CaloEM","CosmicCalo","MinBias"],
                                 ProcessNEvents = EventBlockSize
                                 )

LArMon.AthenaMonTools+=[ theLArRODMonTool ] 
