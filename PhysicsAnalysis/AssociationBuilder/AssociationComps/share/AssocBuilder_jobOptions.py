####################################
#  AssocBuilder Job Options
#  author: Sebastien Binet
####################################

import EventKernel.ParticleDataType

## get a handle on the alg sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## add our AodAssocBuilder
from AthenaCommon import CfgMgr
topSequence += CfgMgr.AssocBuilder( "AodAssocBuilder" )

