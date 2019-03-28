from AthenaCommon.Logging import logging
log = logging.getLogger( 'TrigT1CaloFexSim.ConfigSummary' )
log.setLevel(logging.DEBUG)

#svcMgr.StoreGateSvc.Dump=True

import AthenaCommon.Configurable as Configurable
configLogLevel = Configurable.log.level
Configurable.log.level = logging.INFO # to show more than just the title of the algs

svcMgr.MessageSvc.debugLimit = 5000

log.debug("=== AthAlgSeq ===")
algseq = CfgMgr.AthSequencer("AthAlgSeq") 
log.debug(algseq.getSequence())

log.debug( "=== Top Sequence ===" )
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
log.debug( topSequence.getSequence() )

log.debug( "=== Stream output ===" )
if 'StreamAOD' in dir():
    for coll in sorted(StreamAOD.ItemList):
        log.debug(coll)
if 'StreamRDO' in dir():
    for coll in sorted(StreamRDO.ItemList):
        log.debug(coll)

# setting all algorithms to DEBUG
log.debug( "=== all algorithms in AlqSequence ===" )
for alg in algseq:
    if alg.name() in ["TrigSteer_HLT"]: continue
    alg.OutputLevel = DEBUG
    print alg

if hasattr(svcMgr,"THistSvc"):
    print svcMgr.THistSvc

Configurable.log.level = configLogLevel

