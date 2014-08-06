# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: BunchConfigIDD3PDObject.py 345964 2011-02-15 15:25:18Z ssnyder $
#
# D3PD object saving the bunch crossing configuration ID which was active in the current
# event. The code is a bit tricky, in that BunchConfigIDD3PDObject is not a
# D3PDObject, but a function creating a D3PDObject. So you'd need to call it like
# this to add it to your D3PD algorithm:
#
# <code>
# d3pdalg += BunchConfigIDD3PDObject()( X )
# </code>
#
# or
#
# <code>
# d3pdalg += BunchConfigIDD3PDObject( "LHC" )( X )
# </code>
#
# I've found no better way of telling the code where it should get its configuration
# without introducing a global variable. (Which I try to avoid as much as I can...)

from D3PDMakerCoreComps.D3PDObject import make_Void_D3PDObject
from TrigBunchCrossingTool.BunchCrossingConfProvider import BunchCrossingConfProvider
import TriggerD3PDMaker

def BunchConfigIDD3PDObject( source = "" ):
    object = make_Void_D3PDObject( "bunch_", "BunchConfigIDD3PDObject",
                                   default_name = "BunchConfigIDFiller" )

    object.defineBlock( 0, "ConfigID",
                        TriggerD3PDMaker.BunchConfigIDFillerTool,
                        BCConfProvider = BunchCrossingConfProvider( source ) )

    return object
