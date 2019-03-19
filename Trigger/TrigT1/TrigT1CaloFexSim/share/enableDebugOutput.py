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
log.debug( topSequence.getSequence() )

log.debug( "=== StreamAOD ===" )
for coll in sorted(StreamAOD.ItemList):
    log.debug(coll)

# setting all algorithms to DEBUG
log.debug( "=== all algorithms in AlqSequence ===" )
for alg in algseq:
    alg.OutputLevel = DEBUG
    print alg

print CfgMgr.THistSvc()

Configurable.log.level = configLogLevel

del log
