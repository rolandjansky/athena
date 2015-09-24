########################################################################################################################
## Skeleton showing the basic principles to retrieve EventBookkeepers (metadata info) and SkimDecision (event info).  ##
## Both tools simply print the relevant information on the screen.                                                    ##
## By copying their example, one can however easily create a much more useful tool for analysis (feel free to do it!) ## 
##                                                                                                                    ##
## Creation: David Cote (DESY), November 2009                                                                         ##
########################################################################################################################

# Get some info from the input file
from RecExConfig.InputFilePeeker import inputFileSummary
streamName = inputFileSummary['stream_names'][0]

print "stream_names for this input file is ", streamName

#This is added here as EventLoop Algorithm, but it only needs to run once per file (doable with the IncidentSvc)
#It provides a summary of the event skimming used to create the input file(s)
from EventBookkeeperTools.EventBookkeeperToolsConf import ReadEventBookkeepers
BookkeeperReader = ReadEventBookkeepers( "BookkeeperReader",
                                         OutputLevel = DEBUG,
                                         StreamName = streamName,
                                         EventBookkeeperCollectionName = "EventBookkeepers" )
topSequence += BookkeeperReader


#This tells which of many event filters accepted each event during the creation of the input file(s)
from EventBookkeeperTools.ReadSkimDecisions import SkimDecisionsReader
sd = SkimDecisionsReader( "TestSkimDecisionsReader",
                          OutputLevel = DEBUG,
                          SkimDecisionsContainerName = streamName + "_SkimDecisionsContainer" )
topSequence += sd

# Always expect the skim decisions of the ESD
from EventBookkeeperTools.ReadSkimDecisions import SkimDecisionsReader
sdESD = SkimDecisionsReader( "TestESDSkimDecisionsReader",
                             OutputLevel = DEBUG,
                             SkimDecisionsContainerName = "StreamESD_SkimDecisionsContainer" )
topSequence += sdESD

