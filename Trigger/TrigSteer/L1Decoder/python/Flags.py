#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ConfigFlags import makeFlag

# if set the 
forceEnableAllChains = makeFlag( "forceEnableAllChains",True )

doCalo = makeFlag( "doCalo", True )
doMuon = makeFlag( "doMuon", True )
