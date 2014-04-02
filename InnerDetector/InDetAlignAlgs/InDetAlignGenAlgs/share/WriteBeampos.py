# setup for writing beam position information to COOL using OutputConditionsAlg
#
# setup InDetAlignWrt algorithm with special option to put beamspot in TDS
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from InDetAlignGenAlgs.InDetAlignGenAlgsConf import InDetAlignWrt
topSequence+=InDetAlignWrt(Create=False,Write=False,WriteBeamPos=True)

# initialise BeamCondSvc and set data values which will be input
from AthenaCommon.AppMgr import ServiceMgr
from InDetCondServices.InDetCondServicesConf import BeamCondSvc
ServiceMgr+=BeamCondSvc(useDB=False,
                        posX=0.0,
                        posY=0.0,
                        posZ=0.0,
                        sigmaX=0.015,
                        sigmaY=0.015,
                        sigmaZ=53.,
                        tiltX=0.0,
                        tiltY=0.0
                        )
#
# setup OutputConditionsAlg to take data in TDS and write out
from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
myOCA=OutputConditionsAlg("BeamPosWriter",outputFile="mydummy1.root",
                          ObjectList=[ "AthenaAttributeList#/Indet/Beampos" ],
                          WriteIOV=True,IOVTagList=["nominal"])
