#RecExCommission_RemoveTRTBSERR
# Emergency hack to remove TRT BS Error containers from ESD
# because of unresonably large disk-space consumption
# Walter Lampl, Dec 16th, 2008

from AthenaCommon.Logging import logging
log_TRT_BS_ERR = logging.getLogger( 'RemoveTRTBSErrFromESD' )

# Setup the output sequence
from AthenaCommon.AppMgr import theApp
StreamESD = theApp.getOutputStream( "StreamESD" )

try:
    StreamESD.ItemList.remove("TRT_BSErrContainer#TRT_ByteStreamErrs")
    log_TRT_BS_ERR.info("Removing TRT_BSErrContainer#TRT_ByteStreamErrs from ESD ItemList")
except:
    pass
try:
    StreamESD.ItemList.remove("TRT_BSIdErrContainer#TRT_ByteStreamIdErrs")
    log_TRT_BS_ERR.info("Removing TRT_BSIdErrContainer#TRT_ByteStreamIdErrs from ESD ItemList")
except:
    pass
