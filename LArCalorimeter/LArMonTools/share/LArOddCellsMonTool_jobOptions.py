####################################################
# Maintenance : benjamin trocme
####################################################

if 'ChannelSelection' not in dir() :
   ChannelSelection = ""

if 'ReadCOOL' not in dir():
   ReadCOOL = True
      
if 'MaskBadChannel' not in dir() :
   MaskBadChannel = True

# --- for athena online running ---
if 'EventBlockSize' not in dir():
    EventBlockSize=0

###################################
#   Configure  LArOddCell         #
###################################

from LArMonTools.LArMonToolsConf import LArOddCellsMonTool
theLArOddCellsMonTool = LArOddCellsMonTool(name="LArOddCellsMonTool",
                                           feedthroughNames          = [],
                                           sampleNumber              = -1,
                                           gainMonitored             = [LArMonFlags.LArDigitKey(),LArMonFlags.LArDigitKey()], # [EM,HEC] ; Monitor only one gain. 
                                           oddnessThreshold          = 3,
                                           posNeg                    = 0, # -1 : negative tail / 0 : both tail / 1 : positive tail
                                           minimumRefNoise           = [2.5,2.5],   # [EM,HEC] - No longer cut for the moment
                                           maximumRefNoise           = [10000,10000], # [EM,HEC] - Use BadChannel db instead!
                                           burstyEventsThreshold     = 1.,
                                           maskBadChannels           = MaskBadChannel,   # Mask bad channels in monitoring
                                           LArBadChannelMask         = theLArBadChannelsMasker,
                                           nbEvtsComputePeds         = 500,     # Nb of events to compute pedestal (if not from Cool)
                                           retrieveFromCool          = True,    # Retrieve pedestal from Cool
                                           LArDigitContainerKey      = LArMonFlags.LArDigitKey(),
                                           useEvtCounter             = True,
                                           ProcessNEvents            = EventBlockSize,
                                           createTree                = False,
                                           storeRefs                 = False,
                                           monitoredStreams          =[]  
                                           )
from AthenaCommon.BeamFlags import jobproperties

if jobproperties.Global.DataSource.get_Value() == 'data':
   theLArOddCellsMonTool.LArPedestalKey='Pedestal'
else:
   theLArOddCellsMonTool.LArPedestalKey='LArPedestal'


#The next line is actually getting all the information. I'm sure this is available offline at the T0, but
#this shouldn't be available online, so be careful when you use it....

#TK: Only do this when trigger is enabled.

from AthenaMonitoring.DQMonFlags import DQMonFlags

if DQMonFlags.useTrigger() and not athenaCommonFlags.isOnline():
   from RecExConfig.InputFilePeeker import inputFileSummary
#this retrieve the stream the file open belongs to the output is like: express_express or physics_CosmicCalo
#so retrieve the chain, split it in the "_" char and get the second part (ie CosmicCalo or express...)
   if inputFileSummary:
   #Try to find the bitestreamMetaData:
      if "bs_metadata" in inputFileSummary:
      #Try to find the Stream:
         if "Stream" in inputFileSummary['bs_metadata']:
            #Try the input format should be like physics_L1Calo
            if not inputFileSummary['bs_metadata']['Stream'].find("_")==-1:
               #if ok get the stream name:
               StreamFromFile=inputFileSummary['bs_metadata']['Stream'].split("_")[1]
               if StreamFromFile=='express':
                  theLArOddCellsMonTool.monitoredStreams=["RNDM"]  

ToolSvc += theLArOddCellsMonTool
LArMon.AthenaMonTools+=[ theLArOddCellsMonTool ]


#if jobproperties.Beam.beamType() == 'cosmics':
   # In physic/cosmic run, consider only sample 0 and high gain for EMEC and medium gain for HEC - Removed for 15.3.0
   # theLArOddCellsMonTool.sampleNumber              = 0
theLArOddCellsMonTool.gainMonitored             = ["HIGH","MEDIUM"]
theLArOddCellsMonTool.maxOfTimingHisto          = 40000 # Increase the time axis for long cosmic run

if not (ReadCOOL):
   # Pedestal/noise reference will be computed from nbEvtsComputePeds first events
   theLArOddCellsMonTool.retrieveFromCool = False

if (MaskBadChannel) :
   
   from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
   theLArOddCellsMasker=LArBadChannelMasker("LArOddCellsMasker")
   theLArOddCellsMasker.DoMasking=True
   theLArOddCellsMasker.ProblemsToMask=[
      "deadReadout","deadPhys","almostDead","short",
      "sporadicBurstNoise",
      "highNoiseHG","highNoiseMG","highNoiseLG"
      ]
   ToolSvc+=theLArOddCellsMasker
   ToolSvc.LArOddCellsMonTool.LArBadChannelMask=theLArOddCellsMasker


#ToolSvc.LArOddCellsMonTool.OutputLevel = DEBUG
