#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
##
# @file DozerApp.py
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief Base class for user applications.
#

import sys
sys.argv.append("-b")

import DozerObject
from DozerUtils import *
from DozerData import DozerData
from DozerChannel import DozerChannel
from DozerSchema import DozerSchema
from DozerSchemaConfig import DozerSchemaConfig
from DozerConfig import DozerConfig
from DozerDB import *
from DozerRecordSet import DozerRecordSet
from DozerGraph import DozerStyle, DozerGraph

##
# @class DozerApp
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief base class for user CoolDozer application 
# 
# @htmlinclude cool_dozer_api.css
# <h3>Usage</h3>
#
#<div><div class="dozerTrick"><div class="dozerTrickLabel"> Trick? </div> <div class="dozerTrickText"> 
# DozerApp class could be used as a base class for user defined applications.
# At the moment it only constructs DozerLogger instance. For derived classes use this code snapshot:
# @code
#\#!/bin/env python
#import DozerApp
#
#class DerivedApp( DozerApp.DozerApp ):
#
#    # @param self "Me, myself and Irene"
#    # @param logName log file name
#    # @param toConsole flag to trigger costruction of DozerLogger console handler    
#    def __init__( self, logName="default_log_name.log", toConsole=True ):
#        super( DerivedApp, self ).__init__( logName, toConsole )
#        ... # yours code follows
# @endcode
# </div></div></div>
#
class DozerApp( DozerObject.DozerObject ):

    ## c'tor
    # @param self "Me, myself and Irene"
    # @param logName log file name for DozerLogger 
    # @param toConsole flag to trigger  costruction of DozerLogger console handler 
    def __init__( self, logName=None, toConsole=True ):
        DozerLogger( logName, toConsole=toConsole )
        super( DozerApp, self ).__init__( self )






