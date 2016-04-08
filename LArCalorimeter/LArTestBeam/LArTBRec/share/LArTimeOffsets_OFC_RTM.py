#
# OFC/Timing offsets default tag(s)
#

if not "LArTB04FolderTag_OFC_RTM" in dir():
	LArTB04FolderTag_OFC_RTM = "TB04-13"

if ( LArTB04FolderTag_OFC_RTM == "TB04-Default" ) :
	LArTB04FolderTag_OFC_RTM = "TB04-2"

if ( LArTB04FolderTag_OFC_RTM == "TB04-XTC-01" ) :
	LArTB04FolderTag_OFC_RTM == "TB04-XTC-01-fix"

#
# Timing offsets jobOptions
#

if not 'useCustomTimeOffsets' in dir():
	useCustomTimeOffsets = False

if ( useCustomTimeOffsets ):

	#import AthenaCommon.Logging.logging as logging
	TimeOffsetsLog = logging.getLogger( "TimeOffsetsLog" )
	TimeOffsetsLog.warning( "=================================================================" )
	TimeOffsetsLog.warning( "You enabled the use of CUSTOM TIME OFFSETS..." )
	TimeOffsetsLog.warning( "Don't forget to include your own 'TimeOffset<XXX>.py' file!" )
	TimeOffsetsLog.warning( "(and remember: you're supposed to know what you're doing... :-) )" )
	TimeOffsetsLog.warning( "=================================================================" )
	
else:

	if ( LArTB04FolderTag_OFC_RTM == "TB04-1" ) :
		include( "LArTBRec/LArTimeOffsets_OFC_RTM_TB04-1.py" )

	elif ( LArTB04FolderTag_OFC_RTM == "TB04-2" ) :
		include( "LArTBRec/LArTimeOffsets_OFC_RTM_TB04-2.py" )

	elif ( LArTB04FolderTag_OFC_RTM == "TB04-9" ) :
		include( "LArTBRec/LArTimeOffsets_OFC_RTM_TB04-9.py" )

	elif ( LArTB04FolderTag_OFC_RTM == "TB04-11" ) : 
		include( "LArTBRec/LArTimeOffsets_OFC_RTM_TB04-11.py" )

	elif ( LArTB04FolderTag_OFC_RTM == "TB04-12" ) : 
		include( "LArTBRec/LArTimeOffsets_OFC_RTM_TB04-12.py" )

	elif ( LArTB04FolderTag_OFC_RTM == "TB04-13" ) : 
		include( "LArTBRec/LArTimeOffsets_OFC_RTM_TB04-13.py" )

	elif ( LArTB04FolderTag_OFC_RTM == "TB04-XTC-01-fix" ) :
		include( "LArTBRec/LArTimeOffsets_OFC_RTM_TB04-XTC-01.py" )
		useLArStripsXtalkRampCorr = True

	elif ( LArTB04FolderTag_OFC_RTM == "TB04-XTC-01" ) :
		include( "LArTBRec/LArTimeOffsets_OFC_RTM_TB04-XTC-02.py" )
		useLArStripsXtalkRampCorr = True

	elif ( LArTB04FolderTag_OFC_RTM == "TB04-XTC-01" ) :
		include( "LArTBRec/LArTimeOffsets_OFC_RTM_TB04-XTC-03.py" )
		useLArStripsXtalkRampCorr = True


