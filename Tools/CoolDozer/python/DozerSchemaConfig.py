#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
## 
# @file DozerSchemaConfig.py
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief Implementation of DozerSchemaConfig class
# @date May 5, 2008

import re
import unittest

##
# @class DozerSchemaConfig
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief bag object for unvalidated schema config 
class DozerSchemaConfig( object ):
    
    ## c'tor
    # @param self "Me, myself and Irene"
    # @param nightly nightly name
    # @param install install name
    # @param cmtconfig cmtconfig name 
    # @param project project name
    # @param jobId job id
    def __init__( self, nightly, install, cmtconfig, project, jobId ):
        
        pattern = "[a-z0-9\-_]*"

        self.__nightly = nightly
        if ( "*" in nightly ):
            nightly = nightly.replace("*", pattern, nightly.count("*") )
        self.__reNightly = re.compile( nightly )
        
        self.__install = install
        if ( "*" in install ):
            install = install.replace("*", pattern, install.count("*") )
        self.__reInstall = re.compile( install )

        self.__cmtconfig = cmtconfig
        if ( "*" in cmtconfig ):
            cmtconfig = cmtconfig.replace("*", pattern, cmtconfig.count("*") )
        self.__reCmtconfig = re.compile( cmtconfig )

        self.__project = project
        if ( "*" in project ):
            project = project.replace("*", pattern, project.count("*") )
        self.__reProject = re.compile( project )
        
        self.__jobId = jobId
        if ( "*" in jobId ):
            jobId = jobId.replace("*", pattern, jobId.count("*") )
        self.__reJobId = re.compile( jobId )


    ## nightly name getter
    # @param self "Me, myself and Irene"    
    def nightly( self ):
        return self.__nightly

    ## nightly regexp name getter
    # @param self "Me, myself and Irene"    
    def reNightly( self ):
        return self.__reNightly

    ## install name getter
    # @param self "Me, myself and Irene"    
    def install( self ):
        return self.__install

    ## install regexp  name getter
    # @param self "Me, myself and Irene"    
    def reInstall( self ):
        return self.__reInstall

    ## cmtconfig name getter
    # @param self "Me, myself and Irene"    
    def cmtconfig( self ):
        return self.__cmtconfig
    
    ## cmtconfig regexp name getter
    # @param self "Me, myself and Irene" 
    def reCmtconfig( self ):
        return self.__reCmtconfig 

    ## project name getter
    # @param self "Me, myself and Irene" 
    def project( self ):
        return self.__project

    ## project regexp name getter
    # @param self "Me, myself and Irene" 
    def reProject( self ):
        return self.__reProject
    
    ## jobId getter
    # @param self "Me, myself and Irene"
    def jobId( self ):
        return self.__jobId 

    ## jobId regexp getter
    # @param self "me, myself and Irene"
    def reJobId( self ):
        return self.__reJobId 


    ## == operator
    # @param self "Me, myself and Irene"
    # @param other DozerSchemaConfig instance
    def __eq__(self, other):
        return False or ( self.__reNightly.pattern == other.reNightly().pattern and
                          self.__reInstall.pattern == other.reInstall().pattern and
                          self.__reCmtconfig.pattern == other.reCmtconfig().pattern and
                          self.__reProject.pattern == other.reProject().pattern and 
                          self.__reJobId.pattern == other.reJobId().pattern )
    
    ## != operator
    # @param self "Me, myself and Irene"
    # @param other DozerSchemaConfig instance
    def __ne__( self, other ):
       return not self.__eq__( other )


    ## str() operator 
    # @param self "Me, myself and Irene" 
    def __str__( self ):
        return "nightly='%s' install='%s' cmtconfig='%s' project='%s' jobId='%s'" % ( self.nightly(),
                                                                                      self.install(),
                                                                                      self.cmtconfig(),
                                                                                      self.project(),
                                                                                      self.jobId() )


##
# @class test_DozerSchemaConfig
# @author Krzyszotf Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief test case for DozerSchemaConfig class 
#
# @todo add tests
class test_DozerSchemaConfig( unittest.TestCase ):

    def setUp( self ):
        pass

    def test_01_ctor( self ):
        self.dsc = DozerSchemaConfig( )


## test suite execution
if __name__ == "__main__":
    testLoader = unittest.TestLoader()
    suite = testLoader.loadTestsFromTestCase(test_DozerSchemaConfig)      
    unittest.TextTestRunner(verbosity=3).run(suite)
    
