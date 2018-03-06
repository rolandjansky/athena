#
#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ConfigFlags import makeFlag

# set of flags driving services configuration for athena MT

# this should be actual getting name from some other trigger flag deriving actual name from the TriggerMenu
# for the moment it has to be specified
inputLVL1configFile = makeFlag( "inputLVL1configFile", "Unspecified" )

readLVL1configFromXML = makeFlag( "readLVL1configFromXML", True )
