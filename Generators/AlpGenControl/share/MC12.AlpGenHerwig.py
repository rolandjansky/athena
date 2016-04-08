###############################################################
#
# Generic generation with AlpGen2.14 + Herwig in MC12
#
#==============================================================

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 3

from AthenaCommon.AlgSequence import AlgSequence 
topAlg = AlgSequence("TopAlg") 

include( 'MC12JobOptions/AlpgenJimmy_AUET2_CTEQ6L1_Common.py' )
 
topAlg.Herwig.HerwigCommand += [ "iproc alpgen",
                                 "taudec TAUOLA",
                               ]

include ( "MC12JobOptions/Tauola_Fragment.py" )
include ( "MC12JobOptions/Photos_Fragment.py" )

#---------------------------------------------------------------------------------------------
