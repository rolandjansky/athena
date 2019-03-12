from AthenaCommon import CfgMgr

## This Athena job consists of algorithms that loop over events;
## here, the (default) top sequence is used:
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
topSequence = AlgSequence()

seq = AthSequencer("AthMasterSeq")
seq += CfgMgr.xAODMaker__EventInfoCnvAlg()

if not 'TriggerTypes' in dir():
    TriggerTypes = []
 
## AthMasterSeq is always executed before the top sequence, and is configured such that
## any follow-up sequence (eg. top sequence) is not executed in case TileRecFilter does
## not pass the event
## In short, the sequence AthMasterSeq makes sure that all algs in the job sequence
## are skipped when an event gets rejects
seq += CfgMgr.TileEventFilter(name = 'TileEventFilter'
                            , TriggerTypes = TriggerTypes)


