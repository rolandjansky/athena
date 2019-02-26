# RecExCommission_RemoveHLT_TrigTauRecMerged
# Emergency hack to remove HLT_TrigTauRecMerged containers from ESD
# because of failure : ERROR CreateRep failed, key = HLT_TrigTauRecMerged
# C. Mora, May 4th, 2009  based on W. Lampl

from AthenaCommon.Logging import logging
log_HLTTau = logging.getLogger( 'RemoveHLT_TrigTauRecMerged' )

try:
    from AthenaCommon.AppMgr import theApp
    StreamESD = theApp.getOutputStream( "StreamESD" )
    StreamESD.ItemList.remove("Analysis::TauJetContainer#HLT_TrigTauRecMerged")
    log_HLTTau.info("Removing Analysis::TauJetContainer#HLT_TrigTauRecMerged from ESD ItemList")
except:
    pass
