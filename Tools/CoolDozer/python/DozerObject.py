#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
##
# @file DozerObject.py 
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAgmail.com)
# @brief implementation of CoolDozer API base class - DozerObject.


from DozerUtils import DozerLogger
import logging

## 
# @class DozerObject
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAgmail.com)
# @brief Base class for CoolDozer API 
#
# @htmlinclude cool_dozer_api.css
# <h3>Usage</h3>
#
# <div><div class="dozerWarn"> <div class="dozerWarnLabel"> Beware! </div> <div class="dozerWarnText"> 
# This class is only for internal usage only.  
# </div></div></div>
#
# Thie is the base class for all CoolDozer API classes.
# At the moment it only supports common logging sytem for CoolDozer API classes.
#
class DozerObject( object ):

    ## c'tor
    # @param self "Me, myself and Irene"
    # @param caller reference to inherited object 
    def __init__( self, caller ):
        ## setting up logger
        self.__log = DozerLogger.getLogger( caller )
        

    def getLogger( self ):
        return self.__log.getLogger( self )

    ##
    # 
    def setLevel( self, lvl ):
        logging.getLogger(  ).setLevel( lvl )
            

    ## debug level logger
    # @param self "Me, myself and Irene"
    # @param msg logging string
    def debug( self, msg ):
        if ( self.__log ):
            self.__log.getLogger(self).debug( str(msg) )

    ## info level logger
    # @param self "Me, myself and Irene"
    # @param msg logging string
    def info( self, msg  ):
        if ( self.__log ):
            self.__log.getLogger(self).info( str(msg) )
        
    ## warning level logger
    # @param self "Me, myself and Irene"
    # @param msg logging string
    def warn( self, msg ):
        if ( self.__log ):
            self.__log.getLogger(self).warn( str(msg) )
    
    ## error level logger
    # @param self "Me, myself and Irene"
    # @param msg logging string
    def error( self, msg ):
        if ( self.__log ):
            self.__log.getLogger(self).error( str(msg) )

    ## critical level logger
    # @param self "Me, myself and Irene"
    # @param msg logging string
    def panic( self, msg ):
        if ( self.__log ):
            self.__log.getLogger(self).critical( str(msg) )

    ## exception level logger
    # @param self "Me, myself and Irene"
    # @param msg logging string
    def epanic( self, msg ):
        if ( self.__log ):
            self.__log.getLogger(self).epanic( str(msg) )
            


    
