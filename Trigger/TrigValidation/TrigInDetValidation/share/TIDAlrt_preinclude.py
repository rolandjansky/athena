
from AthenaCommon.Logging import logging 
log = logging.getLogger("TrigInDetValidation")

log.info( "preinclude: TIDAlrt_preinclude.py" ) 

from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doR3LargeD0.set_Value_and_Lock(True)
# InDetFlags.storeSeparateLargeD0Container.set_Value_and_Lock(False) temporarily disable as it causes  MuonCombinedInDetCandidateAlg_LRT FATAL Failed to retrieve  ( 'CaloExtensionCollection' , 'StoreGateSvc+ParticleCaloExtension_LRT' )


