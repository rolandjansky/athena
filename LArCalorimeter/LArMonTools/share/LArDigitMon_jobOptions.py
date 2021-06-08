from LArCellRec.LArNoisyROFlags import larNoisyROFlags

# --- for athena online running ---
if 'EventBlockSize' not in dir():
    EventBlockSize=0

#Run is Online or Offline:
OnlineMode=athenaCommonFlags.isOnline()

#retrieve the condition for the run in cool offline, in IS online
if OnlineMode:
    peakSample_expected = FirstSample
    LArSampleNumber = NSamples   
else:
    from LArConditionsCommon.LArCool import larcool
    if (larcool is not None):
        peakSample_expected = larcool.firstSample()
        LArSampleNumber = larcool.nSamples()
    else:
        peakSample_expected = 0
        LArSampleNumber = 0

#Run is Cosmic or Beam:
CosmicMode=( jobproperties.Beam.beamType()== 'cosmics')

#Declare stream if Cosmic or Beam:
if CosmicMode:
	StreamsToMonitor = ["CosmicCalo","Standby","LArCellsEmpty","Background"]
else:
	StreamsToMonitor = ["CosmicCalo","ZeroBias","Standby","express","Main","L1Calo","L1Topo","LArCells","LArCellsEmpty","Background","LArCellsEmpty"]


###### LArDigitMon Configuration ###############
from LArMonTools.LArMonToolsConf import LArDigitMon
theLArDigitMon = LArDigitMon(name="LArDigitMon",
#                             OutputLevel=VERBOSE,
                             LArDigitContainerKey  = LArMonFlags.LArDigitKey(),
                             IsOnline              = OnlineMode,
#                             SampleRangeLow        = 5, # If no value set, range defined 
#                             SampleRangeUp         = 7, # dynamically in the code
                             ExpectedSampleMax     = peakSample_expected,
                             SampleNumberFromDB    = LArSampleNumber,
                             ComputeHistError      = False,
                             IgnoreBadChannels     = True,
                             ProblemsToMask        = ProblemsToMask,
                             ProcessNEvents        = EventBlockSize,
                             NumberBadFebs         = larNoisyROFlags.BadFEBCut(),
                             Streams = StreamsToMonitor
                             )

LArMon.AthenaMonTools+=[ theLArDigitMon ] 


