include.block( "MissingETPerformance/TrigVsOfflineMissingET_jobOptions.py" )

from MissingETPerformance.MissingETPerformanceConf import \
    TrigVsOfflineMissingETTool as ConfiguredTrigVsOfflineMissingETTool
TrigVsOfflineMissingETTool = ConfiguredTrigVsOfflineMissingETTool()

ToolSvc += TrigVsOfflineMissingETTool

# output level (INFO by default)
TrigVsOfflineMissingETTool.OutputLevel = INFO

# name of the tool is the name of the top folder 
# in the ROOT file that will have the output histograms
TrigVsOfflineMissingETTool.ToolName = 'TrigVsOfflineMissingET'

# MET reference for computing trigger efficiency
# string should be identical to storegate key
TrigVsOfflineMissingETTool.METReference = 'MET_RefFinal'

# Print trigger details 
TrigVsOfflineMissingETTool.PrintMETTriggerStats = False

# Study all configured MET triggers?
TrigVsOfflineMissingETTool.MakePlotsForAllMETTriggers = True

# Study only the following subset of MET signatures
TrigVsOfflineMissingETTool.METTriggers = ['L1_XE20', 'L1_XE50', 'L1_TE360',
                                          'L2_xe20', 'L2_xe50', 'L2_te360',
                                          'EF_xe20', 'EF_xe50', 'EF_te360'
                                          ]
# Select sample data based on the following (unbiased w.r.t MET triggers)                                          
TrigVsOfflineMissingETTool.SampleSelectionTriggers = ['EF_J10','EF_MinBiasSpacePoints','EF_mu10']

# Produce trigger efficiency turn-on curves?
TrigVsOfflineMissingETTool.ComputeTriggerEfficiency = True

# Delta-R between 
TrigVsOfflineMissingETTool.DeltaRCut = 0.3
            
print   TrigVsOfflineMissingETTool
