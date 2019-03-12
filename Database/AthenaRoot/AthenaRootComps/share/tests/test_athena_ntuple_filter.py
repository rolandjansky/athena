### simple event filter joboptions file ###
import AthenaCommon.Constants as Lvl
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# get a handle on the job main sequence
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
job = AlgSequence()

## read a set of input files
DOWRITE=0
include("AthenaRootComps/test_athena_ntuple_dumper.py")

## filter configuration ##
##  -> we use the special sequence 'AthMasterSeq' which
##      is run before any other algorithm (which are usually in the
##      'TopAlg' sequence
seq = AthSequencer("AthMasterSeq")

from GaudiSequencer.PyComps import PyEvtFilter
seq += PyEvtFilter(
   'alg',
   # the store-gate key. leave as an empty string to take any eventinfo instance
   evt_info='TTreeEventInfo', 
   OutputLevel=Lvl.INFO)

#seq.alg.evt_list = [1, 4, 5, 6]
# for the list of event numbers above, apply the following
# filter policy (ie: here we accept event numbers 1,4,5,6)
seq.alg.filter_policy = 'accept' # 'reject'

# we could have use a lambda function selecting 'even' event numbers
# NOTE: you can't use both 'evt_list' and 'filter_fct'
# NOTE: the signature of the lambda function *has* to be 'run,evt'
seq.alg.filter_fct = lambda run,evt: evt%2 == 0

## write out a few ntuples
svcMgr += CfgMgr.DecisionSvc()
import AthenaRootComps.WriteAthenaRoot as arcw
out = arcw.createNtupleOutputStream("StreamD3PD", "d3pd.root", "egamma",
                                    asAlg=True)
out.ForceRead = True
# force reading and accessing all input keys
out.ForceRead = True
# copy the whole input tree layout
out.ItemList = ["*"]
# honor the event selection
out.AcceptAlgs = [seq.name()]

# we now add an event counter in the usual TopAlg sequence to see
# the result of the filtering above
job += CfgMgr.AthEventCounter('counter', OutputLevel=Lvl.INFO)

svcMgr.MessageSvc.OutputLevel = Lvl.ERROR
theApp.EvtMax = 5
### EOF ###
