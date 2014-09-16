#taken from LArNoisyRoMonTool to get the default cut to skip an event.
def LArNoisyFEBHelper():
    from AthenaCommon.AlgSequence import AlgSequence
    job = AlgSequence()
    print job
    if hasattr(job, 'LArNoisyROAlg'):
        alg = getattr(job, 'LArNoisyROAlg')

        BadFEBCut = None
        if hasattr(alg, 'BadFEBCut'):
            BadFEBCut = getattr(alg,'BadFEBCut')
        else:
            BadFEBCut = alg.getDefaultProperty('BadFEBCut')

        return BadFEBCut

    else:
        return 9999999


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
	StreamsToMonitor = ["CosmicCalo","ZeroBias","Standby","CosmicMuons","Egamma","IDCosmic","MinBias","JetTauEtmiss","Muons","bulk"]
else:
	StreamsToMonitor = ["JetTauEtmiss","Egamma","bulk","CosmicCalo","ZeroBias","Standby","CosmicMuons","IDCosmic","MinBias","Muons"]


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
                             LArBadChannelMask     = theLArBadChannelsMasker,
                             ProcessNEvents        = EventBlockSize,
                             NumberBadFebs         = LArNoisyFEBHelper(),
                             Streams = StreamsToMonitor
                             )

from AthenaCommon.BeamFlags import jobproperties
if jobproperties.Global.DataSource.get_Value() == 'data':
    theLArDigitMon.LArPedestalKey='Pedestal'
else:
    theLArDigitMon.LArPedestalKey='LArPedestal'
                

ToolSvc += theLArDigitMon
LArMon.AthenaMonTools+=[ theLArDigitMon ] 


