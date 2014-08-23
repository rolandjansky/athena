#
# JobOptions for setting some output levels in Muon HLT Trigger Algorithms
# Available output level variables to tune the levels:
# ConfigOutputLevel
# L2OutputLvl    for muFast and muComb
# TMEFOutputLvl  for TrigMuonEF algorithms
# TMEFSummary    for TrigMuonEF algorithms printing summary per event per stage
# MGOutputLvl    for TrigMuGirl algorithms
#

from AthenaCommon.Logging import logging

# a few default OUTPUT levels depending on the number of events processing
log = logging.getLogger()
defaultOutputLvl = log.mapLevelLoggingToGaudi(log.getEffectiveLevel())

if athenaCommonFlags.EvtMax() == -1 or athenaCommonFlags.EvtMax() > 1000:
    if "ConfigOutputLevel" not in dir(): ConfigOutputLevel = defaultOutputLvl
    if "L2OutputLvl"       not in dir(): L2OutputLvl = defaultOutputLvl
    if "TMEFOutputLvl"     not in dir(): TMEFOutputLvl = defaultOutputLvl
    if "MGOutputLvl"       not in dir(): MGOutputLvl = defaultOutputLvl
    if "TMEFSummary"       not in dir(): TMEFSummary = False

elif athenaCommonFlags.EvtMax() > 100:
    if "ConfigOutputLevel" not in dir(): ConfigOutputLevel = VERBOSE
    if "L2OutputLvl"       not in dir(): L2OutputLvl = INFO
    if "TMEFOutputLvl"     not in dir(): TMEFOutputLvl = DEBUG
    if "MGOutputLvl"       not in dir(): MGOutputLvl = DEBUG
    if "TMEFSummary"       not in dir(): TMEFSummary = True
else: # <= 100 events
    if "ConfigOutputLevel" not in dir(): ConfigOutputLevel = VERBOSE
    if "L2OutputLvl"       not in dir(): L2OutputLvl = INFO
    if "TMEFOutputLvl"     not in dir(): TMEFOutputLvl = VERBOSE
    if "MGOutputLvl"       not in dir(): MGOutputLvl = VERBOSE
    if "TMEFSummary"       not in dir(): TMEFSummary = True


# helper function for nice OutputLevel setting printout
OutputLevelNames = {}
OutputLevelNames[ALL] = "ALL"
OutputLevelNames[VERBOSE]="VERBOSE"
OutputLevelNames[DEBUG]="DEBUG"
OutputLevelNames[INFO]="INFO"
OutputLevelNames[WARNING]="WARNING"
OutputLevelNames[ERROR]="ERROR"
OutputLevelNames[FATAL]="FATAL"

def setOutputLevel(conf,level):
    print "Setting %s.OutputLevel = %s" % (conf.getJobOptName(),OutputLevelNames[level])
    conf.OutputLevel = level
    

MessageSvc.debugLimit = 10000
MessageSvc.Format = "% F%65W%S%7W%R%T %0W%M"

try:
    L2_sequence = topSequence.TrigSteer_HLT.getChildren()
except AttributeError:
    try:
        L2_sequence = topSequence.TrigSteer_L2.getChildren()
    except AttributeError:
        L2_sequence = []

################################################################################
# L2 tweeking
################################################################################
for L2_alg in L2_sequence:
    algType = L2_alg.getType()

    # set output levels
    if algType.startswith("Mufast") or algType.startswith("Mucomb"):
        setOutputLevel(L2_alg,L2OutputLvl)


################################################################################
# EF tweeking
################################################################################
try:
    EF_sequence = topSequence.TrigSteer_HLT.getChildren()
except AttributeError:
    try:
        EF_sequence = topSequence.TrigSteer_EF.getChildren()
    except AttributeError:
        EF_sequence = []

#
# here we add the algorithm used for debugging TrigMuSuperEF
#
for EF_alg in EF_sequence:
    algType = EF_alg.getType()

    # set output levels
    if algType.startswith('TrigMuonEF'):
        setOutputLevel(EF_alg,TMEFOutputLvl)            
    elif algType.startswith('TrigMuGirl'):
       setOutputLevel(EF_alg,MGOutputLvl)            
    elif algType.startswith('TrigMuSuperEF'):
       setOutputLevel(EF_alg,min(MGOutputLvl,TMEFOutputLvl))
    elif algType.startswith('TrigCaloClusterMaker'):
        setOutputLevel(EF_alg,INFO)

    TMEF_SegmentFinder = None
    TMEF_TrackBuilder = None
    TMEF_Extrapolator = None
    TMEF_Combiner = None
    TMEF_CombinerHypo = None
    TMEF_Isolation = None
    TMEF_IsolationHypo = None
    TrigMuGirl = None
    TrigMuGirlHypo = None


    if algType == 'TrigMuSuperEF':
        TMEF_SegmentFinder = getattr(ToolSvc,EF_alg.TMEF_standaloneTrackTool.getName())
        TMEF_TrackBuilder  = getattr(ToolSvc,EF_alg.TMEF_standaloneTrackTool.getName())
        TMEF_Extrapolator  = getattr(ToolSvc,EF_alg.TMEF_standaloneTrackTool.getName())
        TMEF_Combiner      = getattr(ToolSvc,EF_alg.TMEF_combinerTool.getName())
        TrigMuGirl         = getattr(ToolSvc,EF_alg.TrigMuGirlTool.getName())
    elif algType == 'TrigMuonEFSegmentFinder':
        TMEF_SegmentFinder = EF_alg
    elif algType == 'TrigMuonEFTrackBuilder':
        TMEF_TrackBuilder = EF_alg
    elif algType == 'TrigMuonEFExtrapolator':
        TMEF_Extrapolator = EF_alg
    elif algType == 'TrigMuonEFCombiner':
        TMEF_Combiner = EF_alg
    elif algType == 'TrigMuGirl':
        TrigMuGirl = EF_alg
    else:
        continue            

    # set some properties
    if TMEF_SegmentFinder is not None:
        setOutputLevel(TMEF_SegmentFinder,TMEFOutputLvl)
        # activate summaries for offline tools (or not)
        TMEF_SegmentFinder.SegmentsFinderTool.DoSummary = TMEFSummary


    if TMEF_TrackBuilder is not None:
        setOutputLevel(TMEF_TrackBuilder,TMEFOutputLvl)
        # activate summaries for offline tools (or not)
        TMEF_TrackBuilder.TrackBuilderTool.DoSummary = TMEFSummary

    if TMEF_Extrapolator is not None:
        setOutputLevel(TMEF_Extrapolator,TMEFOutputLvl)

    if TMEF_Combiner is not None:
        setOutputLevel(TMEF_Combiner,TMEFOutputLvl)

    if TrigMuGirl is not None:
        setOutputLevel(TrigMuGirl,MGOutputLvl)
        

# less output on some algs
# only way to shut up CTPSimulation
from TriggerJobOpts import Lvl1TriggerGetter
Lvl1TriggerGetter.Lvl1OutputLevel = defaultOutputLvl
try:
    topSequence.CTPSimulation.OutputLevel = defaultOutputLvl
except AttributeError:
    pass

if ConfigOutputLevel <= DEBUG:
    from AthenaCommon import CfgGetter
    CfgGetter.printStats()

from AthenaCommon.ConfigurationShelve import saveToAscii
from MuonRecExample.MuonRecUtils import dumpDetFlags
saveToAscii("config.txt")
dumpDetFlags("config.txt")

