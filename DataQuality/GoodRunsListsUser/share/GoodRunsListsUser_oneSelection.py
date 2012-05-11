## Example trigger joboptions
#include ("AMAAthena/Trigger_jobOptions.py")

###########################################################################################
## Configure the goodrunslist selector tool to read and interpret xml files
###########################################################################################

## Configure the goodrunslist selector tool
from GoodRunsLists.GoodRunsListsConf import *
ToolSvc += GoodRunsListSelectorTool() 
GoodRunsListSelectorTool.GoodRunsListVec  = [ 'grl_topmix_mu.xml' ]
GoodRunsListSelectorTool.BlackRunsListVec = [ 'blacklist.xml' ]
#GoodRunsListSelectorTool.OutputLevel = DEBUG

## GRL selector, dummy ntuple dumper
from GoodRunsListsUser.GoodRunsListsUserConf import *

###########################################################################################
## Start of jobsequence
###########################################################################################

## This Athena job consists of algorithms that loop over events;
## here, the (default) top sequence is used:
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
job = AlgSequence()

###########################################################################################
## GRLTriggerSelectorAlg uses GoodRunsListSelectorTool to determine for 
##   each event whether to pass it, or to reject it.
## As good- and black run lists GRLTriggerAlg1 uses: 'grl_topmix_mu' and 'blacklist', 
##   which are the names of the good run list objects in grl_topmix_mu.xml 
##   and blacklist.xml respectively.
## As the dummy ntupledumper DummyDumperAlg1 runs in sequence ModSequence1, 
##   after GRLTriggerAlg1, it is not executed in case GRLTriggerAlg1 rejects an event.
###########################################################################################

## Sequence 1
job += AthSequencer("ModSequence1")
job.ModSequence1 += GRLTriggerSelectorAlg('GRLTriggerAlg1')
job.ModSequence1 += DummyDumperAlg('DummyDumperAlg1')

job.ModSequence1.GRLTriggerAlg1.GoodRunsListArray  = ['grl_topmix_mu']
job.ModSequence1.GRLTriggerAlg1.BlackRunsListArray = ['blacklist']

# job.ModSequence1.DummyDumperAlg1.RootFileName = 'selection1.root'
THistSvc.Output =  {"new DATAFILE='selection1.root' TYP='ROOT' OPT='NEW'"};
