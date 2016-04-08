#-----------------------------------------------------------------------------
#
# Generic generation with Alpgen2.14 + Pythia6.425 in MC12
#
#-----------------------------------------------------------------------------

# output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 3

from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence("TopAlg")

include ( "MC12JobOptions/Pythia_Perugia2011C_Common.py" )
evgenConfig.generators += ["Alpgen"]

topAlg.Pythia.PythiaCommand += ["pyinit user alpgen",
                                "pydat1 parj 90 20000.",
                                "pydat3 mdcy 15 1 0",
                                "pypars mstp 143 1"
                                ]

include ( "MC12JobOptions/Tauola_Fragment.py" )
include ( "MC12JobOptions/Photos_Fragment.py" )

#-----------------------------------------------------------------------------
