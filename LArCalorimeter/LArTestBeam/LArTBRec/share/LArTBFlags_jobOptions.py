######################################################################
#
# jobOptions to define global tags for LAr CTB 2004 data
#
######################################################################

#
# CondDB NOVA/IOV server
#
#
# Tags for CondDB folders
#
if not 'LArTB04FolderTag_Calib' in dir():
   LArTB04FolderTag_Calib    = "TB04-Default"

if not 'LArTB04FolderTag_Pedestal' in dir():
   LArTB04FolderTag_Pedestal = "TB04-Default"

if not 'LArTB04FolderTag_AutoCorr' in dir():
   LArTB04FolderTag_AutoCorr = "TB04-Default"

if not 'LArTB04FolderTag_Ramp' in dir():
   LArTB04FolderTag_Ramp     = "TB04-Default"

if not 'LArTB04FolderTag_OFC_RTM' in dir():
   LArTB04FolderTag_OFC_RTM  = "TB04-XTC-01-fix"

if not 'LArTB04FolderTag_OFC_TCM' in dir():
   LArTB04FolderTag_OFC_TCM  = "TB04-Default"

if not 'LArTB04FolderTag_MphyMcal' in dir():
   LArTB04FolderTag_MphyMcal  = "TB04-XTC-01-fix2"

#
# Fix for swapped LArFEBs
#
ToolSvc = Service( "ToolSvc" )
if (RunNumber>=1000931):
   if (RunNumber<=1000969):
       ToolSvc.LArRodDecoder.FebExchange=True
       ToolSvc.LArRodDecoder.FebId1=0x39020000
       ToolSvc.LArRodDecoder.FebId2=0x39040000

#
# DB folder tags output
#
#import AthenaCommon.Logging.logging as logging
LArTBFlagsLog = logging.getLogger( "LArTBFlagsLog" )
LArTBFlagsLog.info( " ======================================================== " )
LArTBFlagsLog.info( " *** LAr DB folder tags summary (empty = HEAD) *** " )
LArTBFlagsLog.info( " ======================================================== " )
LArTBFlagsLog.info( " LArTB04FolderTag_Calib    = "+LArTB04FolderTag_Calib )
LArTBFlagsLog.info( " LArTB04FolderTag_AutoCorr = "+LArTB04FolderTag_AutoCorr )
LArTBFlagsLog.info( " LArTB04FolderTag_Pedestal = "+LArTB04FolderTag_Pedestal )
LArTBFlagsLog.info( " LArTB04FolderTag_Ramp     = "+LArTB04FolderTag_Ramp )
LArTBFlagsLog.info( " LArTB04FolderTag_OFC_RTM  = "+LArTB04FolderTag_OFC_RTM )
LArTBFlagsLog.info( " LArTB04FolderTag_OFC_TCM  = "+LArTB04FolderTag_OFC_TCM )
LArTBFlagsLog.info( " LArTB04FolderTag_MphyMcal = "+LArTB04FolderTag_MphyMcal )
LArTBFlagsLog.info( " ======================================================== " )
