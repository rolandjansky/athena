# Dummy global and feb time offsets for 2004 H6 TB
#    16 June 2004 Rob McPherson

TimeOffsetsLog = logging.getLogger( "TimeOffsetsLog" )
TimeOffsetsLog.info( "Dummy global and feb time offsets for 2004 H6 TB ..." )

theApp.TopAlg +=["FakeLArTimeOffset"]
FakeLArTimeOffset=Algorithm("FakeLArTimeOffset")
FakeLArTimeOffset.OutputLevel = INFO

FakeLArTimeOffset.GlobalTimeOffset =  0.
FakeLArTimeOffset.FEBids          += [ 0x3B020000, 0x3B030000, 0x3B040000, 0x3B050000, 0x3B090000, 0x3B098000, 0x3B0A0000, 0x3B0A8000]
FakeLArTimeOffset.FEbTimeOffsets  += [ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 ]


