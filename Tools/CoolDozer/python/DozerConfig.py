#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
##
# @file DozerConfig.py
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @date Apr 24, 2008
# @brief DozerConfig and DozerFolder class implementation

import DozerObject
from DozerUtils import DozerLogger, DozerError, DozerSingleton
from DozerValid import DozerValid
from DozerData import DozerData
from DozerChannel import DozerChannel
from DozerSchema import DozerSchema
from DozerSchemaConfig import DozerSchemaConfig

from types import *
import re
import os
import os.path
import xml.dom.minidom

import unittest

##
# @class DozerFolder
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @date May, 28 2008
# @brief Dictionary which can match keys using regexp, for internal use only!  
#
# @htmlinclude cool_dozer_api.css
# <h3>Usage</h3>
#
# <div><div class="dozerWarn"> <div class="dozerWarnLabel"> Beware! </div> <div class="dozerWarnText">
# This class is only for internal usage inside DozerConfig instance. 
# </div></div></div>
#
# DozerFolder has got full python dictionary interface, but can match keys using 
# limited regular expressions.
# In DozerFolder keys are strings with limited wild card syntax (only '*' is supported).
# Valid key can be either:  
# <ul>
# <li> <i>normal</i> string, e.g. "dev", "i686-slc4-gcc34-opt", "AtlasProduction"
# <li> <i>wild card [asterisk *]</i> string, e.g. "*", "dev*", "i686-slc4-*-opt", "i686-*", etc.
# </ul>
#
# A key matching is done in two steps:
# <ol>
# <li> by built-in Python string comparision ( "bugfix" == "bugfix", "*" == "*", "dev" != "dev*" ) 
# <li> if above fails using regexp matching, where all occurences of "*" in dictionay keys are replaced by
# regexp pattern "[a-z0-9\-_]*", which matches to 0+ occurences of letters, digits, "-" (minus) and "_" (underscore), so 
# e.g. key pairs ("dev*", "dev") or ("i686-*-gcc34-opt","i686-slc4-gcc34-opt") are equal   
# </ol>
#
# DozerFolder together with DozerConfig works like an object factory for DozerSchema, but it should be used only 
# internally by DozerConfig instance, all newly created objects are added to the DozerConfig instance.
#
# @todo add validation of keys
#
class DozerFolder( DozerObject.DozerObject ):
    # maximum level
    __maxLevel = 5

    # reference to DozerFolder 
    __schemaDict = None
    
    # reference to DozerConfig
    __owner = None

    ## c'tor
    # @param self "Me, myself and Irene"
    # @param level internal level number 
    # @param owner reference to higher level DozerFolder (or DozerConfig)
    # @param log DozerLogger instance
    def __init__( self, level, owner, log=None ):
        
        super( DozerFolder, self ).__init__( log )
        self.__log = log

        if ( int(level) > self.__maxLevel ): 
            raise DozerError("cannot construct DozerFolder for level > %d" % self.__maxLevel )
        else:
            self.__level = int(level)

        if ( isinstance(owner, DozerConfig) ):
            self.__owner = owner
        else:
            raise DozerError("cannot construct DozerFolder, parameter 'owner' isn't a DozerConfig instance")

        self.cv = DozerValid( log = self.__log)  
        self.__schemaDict = { }

        self.__validKeys = None
        if ( level == 1 ):
            self.__validKeys = self.cv.nightlies()
        elif ( level == 2 ):
            self.__validKeys = self.cv.installs()
        elif ( level == 3 ):
            self.__validKeys = self.cv.cmtconfigs()
        elif ( level == 4 ):
            self.__validKeys = self.cv.projects()
        elif ( level == 5 ):
            self.__validKeys = None

        self.debug("constructing DozerFolder for level=%d" % self.__level)


 
    ## key matcher
    # @param self "Me, myself and Irene"
    # @param key key to match
    def match( self , key ):
        nMatch = None
        # simple match first
        if ( key in self.__schemaDict.keys() ): 
            return key
        else:
        # and based of regexp
            for nameKey in sorted( self.__schemaDict, reverse=True):
                theName = nameKey
                reName = theName 
                pattern = "[a-z0-9\-_]*"
                if ( "*" in theName ) : reName = theName.replace("*", pattern, theName.count("*") )
                if ( re.compile( reName ).search( key ) ): 
                    nMatch = theName

        return nMatch

    ## in operator
    # @param self "Me, myself and Irene"
    # @param key name to match
    def __contains__( self, key ):
        if ( self.match( key ) ): return True
        return False

    ## iterator
    # @param self "Me, myself and Irene"
    def __iter__( self ):
        return self.__schemaDict.__iter__()
    
    ## next()
    # @param self "Me, myself and Irene"
    def next( self ):
        return self.__schemaDict.next()

    ## items getter
    # @param self "Me, myself and Irene"
    def items( self ):
        return self.__schemaDict.items()
 
    ## iterkeys getter
    # @param self "Me, myself and Irene"
    def iterkeys( self ):
        return self.__schemaDict.iterkeys()

    ## iteritems getter
    # @param self "Me, myself and Irene"
    def iteritems( self ):
        return self.__schemaDict.iteritems()

    ## keys getter
    # @param self "Me, myself and Irene"
    def keys( self ):
        return self.__schemaDict.keys()

    ## old style in operator
    # @param self "Me, myself and Irene"
    # @param key kay to find
    def has_key( self, key ):
        return self.__contains__( key )

    ## str() operator
    # @param self "Me, myself and Irene"
    def __str__( self ):
        return "DozerFolder lvl=%d keys=%s\n" % ( self.__level, 
                                                str(self.__schemaDict.keys()) )
    
        

    ## [] operator
    # @param self "Me, myself and Irene"
    # @param key key to match
    # @throws KeyError if not matching key was found
    def __getitem__( self, key ):

        nMatch = self.match( str(key) )

        self.__owner._holdMatchedPair( self.__level, key, nMatch )
       
        if ( nMatch ): 

            if ( isinstance( self.__schemaDict[nMatch], DozerSchema ) ):

                schema = self.__schemaDict[nMatch]
                isNew = False 

                nightly, matchNightly = self.__owner._getMatchedPair(1)
                if ( nightly != matchNightly ):
                    isNew = True
                
                install, matchInstall = self.__owner._getMatchedPair(2)
                if ( install != matchInstall ): 
                    isNew = True
                   
                cmtconfig, matchCmtconfig = self.__owner._getMatchedPair(3)
                if ( cmtconfig != matchCmtconfig ): 
                    isNew = True
                   
                project, matchProject = self.__owner._getMatchedPair(4)
                if ( project != matchProject ): 
                    isNew = True
                
                jobId, matchJobId = self.__owner._getMatchedPair(5)
                if ( jobId != matchJobId ):
                    isNew = True 

                if ( isNew ):
                    matched = ( matchNightly, matchInstall, matchCmtconfig, matchProject, matchJobId, 
                                nightly, install, cmtconfig, project, jobId )

                    self.info("making copy of schema '%s/%s/%s/%s/%s' to '%s/%s/%s/%s/%s'" %  matched  )

                    schema = DozerSchema.fromXMLDoc( self.__schemaDict[nMatch].xml() , log = self.__log )

                    schema.setNightly( nightly )
                    schema.setInstall( install )
                    schema.setCmtconfig( cmtconfig )
                    schema.setProject( project )
                    schema.setJobId( jobId )

                    self.__owner += schema
                    return schema

                else:
                    return self.__schemaDict[nMatch]
            else:
                return self.__schemaDict[nMatch]

        raise KeyError("key='%s' not found in DozerFolder lvl=%d" % ( str(key), self.__level ) )

    ## dictionary item setter
    # @param self "Me, myself and Irene"
    # @param key key to match
    # @param what value to be set, must be DozerFolder or DozerSchema instance
    # @throw KeyError level is to high or what type isn't the proper one  
    def __setitem__(self, key, what):
        if  ( ( ( self.__level < self.__maxLevel ) and ( isinstance(what, DozerFolder) ) ) or
              ( ( self.__level == self.__maxLevel ) and ( isinstance(what, DozerSchema) ) ) ):
            if ( self.__schemaDict.has_key( key ) ):
                self.warn("overwriting DozerFolder value for key=%s" % key )
            else:
                self.debug("setting DozerFolder for key=%s" % key )
            self.__schemaDict[ key ] = what
        else:
            raise KeyError("key='%s' not found" % key)
    
    ## del operator
    # @param self "Me, myself and Irene"
    # @param key key to match
    # @throw KeyError if key not found ot DozerError if someone want ot remove regexp key using 
    #  specific key (e.g. del ["blah"], which matches only to regexp "*" key )
    def __delitem__( self, key ):
        
        nMatch = self.match( str(key) )
        if ( nMatch ):
            if ( nMatch == str(key) ): 
                self.debug("removing item at key='%s'" % nMatch)
                del self.__schemaDict[nMatch]
            else:
                msg = "cannot remove general key(='%s') using specific one (=%s)" % ( nMatch, str(key) )
                self.error(msg)
                raise DozerError( msg )
        else:
            raise KeyError("key=%s not found in DozerFolder level=%d" % ( key, self.__level) )
    
    ## key finder
    # @param self "Me, myself and Irene"
    # @param key key name to check
    def has_key( self, key ):
        return self.__contains__( str(key) )

    ## general getter from path
    # @param self "Me, myself and Irene"
    # @param path path to get 
    def get( self, path="" ):
        normpath = os.path.normpath( path.strip("/") )
        self.debug("get path='%s' normpath='%s'" % (path, normpath ) )
        if ( normpath == "." ): return self
        else:
            words = str(normpath).split("/")
            if ( len(words) == 1 ):
                return self[words[0]]
            else:
                searchPath = "/".join(words[1:])
                self.debug("get searching recursively key='%s' path='%s'" % (words[0], searchPath ) )

                return self[words[0]].get( searchPath ) 


##
# @class DozerConfig
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief Container class for DozerSchema for one and only one test framework
#
# @htmlinclude cool_dozer_api.css
# <h3>Usage</h3>
# 
# <ul>
# <li> <h4>Concept</h4>
#
# A DozerConfig object is able to hold a bunch of DozerSchema objects (explicit and implicit ones) and works like 
# DozerSchema factory. Every time   
#
# <li> <h4>Constructing</h4>
#
# To get the DozerConfig instance call the constructor:
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#[lxplus215] ~/scratch2/CoolDozerClient/python % python
#Python 2.5 (r25:51908, Oct 18 2007, 16:04:48) 
#[GCC 3.4.6 20060404 (Red Hat 3.4.6-8)] on linux2
#Type "help", "copyright", "credits" or "license" for more information.
#>>> import DozerConfig
#>>> conf = DozerConfig.DozerConfig("RTT")
#>>> conf
#<DozerConfig.DozerConfig object at 0x2a9d758710>
#</pre></b>
#</div></div></div>
# 
#
# <li> <h4>Working with DozerSchema instances</h4>
#
# <ul>
#
# <li> adding schemas to DozerConfig
#
# This can be obtained using "+=" operator: 
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#>>> import DozerSchema, DozerChannel
#>>> schema1 = DozerSchema.DozerSchema("*", "build", "i686-slc4-gcc34-opt", "AtlasProduction", "job_1")      # [1.1] constructing implicit schema
#>>> schema2 = DozerSchema.DozerSchema("dev", "build", "i686-slc4-gcc34-opt", "AtlasProduction", "job_1")    # [1.2] and explicit one
#>>> schema1 += DozerChannel.DozerChannel("channel #1", "channel #1 description")           # [1.3] adding some dummy channels
#>>> schema2 += DozerChannel.DozerChannel("channel #2", "channel #2 description")           # [1.4] and once again
#>>> conf += schema1                                                                        # [2.1] adding DozerSchema 
#>>> conf += schema2                                                                        # [2.2] and once again
#>>> print conf                                                                             # [3] printing DozerConfig
#<?xml version="1.0" ?>
#<rtt>
# <schema cmtconfig="i686-slc4-gcc34-opt" install="build" jobId="job_1" nightly="*" project="AtlasProduction" tag="" version="0">
#  <channel desc="channel #1 description" name="channel #1" uuid="1992790e-e121-4fc7-8242-6291a6f11a83"/>
# </schema>
# <schema cmtconfig="i686-slc4-gcc34-opt" install="build" jobId="job_1" nightly="dev" project="AtlasProduction" tag="" version="0">
#  <channel desc="channel #2 description" name="channel #2" uuid="c6a9b2d2-0b99-4e8d-8644-3022c672f91f"/>
# </schema>
#</rtt>
#</pre></b>
#</div></div></div>
#
# <li> getting back schemas 
#
#  <ul>
#
#   <li> using python dictionary interface
#
# The basic usage is presented below.
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#>>> conf["dev"]["build"]["i686-slc4-gcc34-opt"]["AtlasProduction"]["job_1"]                                          # [1] get DozerSchema 
#<DozerSchema.DozerSchema object at 0x2a9d742d10>
#>>> conf["dev"]["build"]["i686-slc4-gcc34-opt"]["AtlasProduction"]["job_1"]["channel #2"]                            # [2] or DozerChannel in it
#<DozerChannel.DozerChannel object at 0x2a9d7586d0>
#>>> conf["dev"]["build"]["i686-slc4-gcc34-opt"]["AtlasProduction"]["job_1"]["c6a9b2d2-0b99-4e8d-8644-3022c672f91f"]  # [3] if you want to use
#<DozerChannel.DozerChannel object at 0x2a9d7586d0>                                                                   #     DozerChannel UUID
#</pre></b>   
#</div></div></div>
# 
# Of course someone can use all possible python dictionary looping techniques to get back the proper schema.
#
#   <li> using "file system like" get
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#>>> conf.get("dev/build/i686-slc4-gcc34-opt/AtlasProduction/job_1")                                        # [1] get schema
#<DozerSchema.DozerSchema object at 0x2a9d742d10>
#>>> conf.get("./dev/build/i686-slc4-gcc34-opt/AtlasProduction/job_1")                                      # [2] and once again
#<DozerSchema.DozerSchema object at 0x2a9d742d10> 
#>>> conf.get("./dev/build/i686-slc4-gcc34-opt/AtlasProduction/job_1/channel #2")                           # [3] get channel in schema
#<DozerChannel.DozerChannel object at 0x2a9d7586d0>
#>>> conf.get("./dev/build/i686-slc4-gcc34-opt/AtlasProduction/job_1/c6a9b2d2-0b99-4e8d-8644-3022c672f91f") # [4] the same but using channel UUID
#<DozerChannel.DozerChannel object at 0x2a9d7586d0>
#</pre></b>
#</div></div></div>
#
#   <li> from implicit to explicit
#
# For getting explicit schema from implicit template DozerSchema, that has been added previously to the DozerConfig instance,
# one just has to ask for it using dictionary or "file system-like" interface:
# 
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#>>> print conf                                                                              # [1] printing DozerConfig 
#<?xml version="1.0" ?>
#<rtt>
# <schema cmtconfig="i686-slc4-gcc34-opt" install="build" jobId="job_1" nightly="*" project="AtlasProduction" tag="" version="0">
#  <channel desc="channel #1 description" name="channel #1" uuid="1992790e-e121-4fc7-8242-6291a6f11a83"/>
# </schema>
# <schema cmtconfig="i686-slc4-gcc34-opt" install="build" jobId="job_1" nightly="dev" project="AtlasProduction" tag="" version="0">
#  <channel desc="channel #2 description" name="channel #2" uuid="c6a9b2d2-0b99-4e8d-8644-3022c672f91f"/>
# </schema>
#</rtt>
#>>> conf["bugfix"]["build"]["i686-slc4-gcc34-opt"]["AtlasProduction"]["job_1"]              # [2] get explict schema out ot implict one 
#<DozerSchema.DozerSchema object at 0x2a9d758e50>
#>>> print conf                                                                              # [3] printing DozerConfig, notice the new schema 
#<?xml version="1.0" ?>                                                                      # with nightly='bugfix' has been added
#<rtt>
# <schema cmtconfig="i686-slc4-gcc34-opt" install="build" jobId="job_1" nightly="bugfix" project="AtlasProduction" tag="" version="0">
#  <channel desc="channel #1 description" name="channel #1" uuid="1992790e-e121-4fc7-8242-6291a6f11a83"/>
# </schema>
# <schema cmtconfig="i686-slc4-gcc34-opt" install="build" jobId="job_1" nightly="*" project="AtlasProduction" tag="" version="0">
#  <channel desc="channel #1 description" name="channel #1" uuid="1992790e-e121-4fc7-8242-6291a6f11a83"/>
# </schema>
# <schema cmtconfig="i686-slc4-gcc34-opt" install="build" jobId="job_1" nightly="dev" project="AtlasProduction" tag="" version="0">
#  <channel desc="channel #2 description" name="channel #2" uuid="c6a9b2d2-0b99-4e8d-8644-3022c672f91f"/>
# </schema>
#</rtt>
#</pre></b>
#</div></div></div> 
# 
#  </ul>
#
# <li> removing schemas from DozerConfig
#
# To delete the DozerSchema instance one has to use "-=" operator:
#
# <b><pre>
#         (DozerConfig instance) -= (DozerSchema instance)
#                                or
#         (DozerConfig instance) -= (DozerSchemaConfig instance)
#                                or
#         (DozerConfig instance) -= (tuple (nightly, install, cmtconfig, project, jobId) instance)
# </pre></b> 
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#>>> print conf                                                                               # [1] printing DozerConf
#<?xml version="1.0" ?>         
#<rtt>
# <schema cmtconfig="i686-slc4-gcc34-opt" install="build" jobId="job_1" nightly="bugfix" project="AtlasProduction" tag="" version="0">
#  <channel desc="channel #1 description" name="channel #1" uuid="1992790e-e121-4fc7-8242-6291a6f11a83"/>
# </schema>
# <schema cmtconfig="i686-slc4-gcc34-opt" install="build" jobId="job_1" nightly="*" project="AtlasProduction" tag="" version="0">
#  <channel desc="channel #1 description" name="channel #1" uuid="1992790e-e121-4fc7-8242-6291a6f11a83"/>
# </schema>
# <schema cmtconfig="i686-slc4-gcc34-opt" install="build" jobId="job_1" nightly="dev" project="AtlasProduction" tag="" version="0">
#  <channel desc="channel #2 description" name="channel #2" uuid="c6a9b2d2-0b99-4e8d-8644-3022c672f91f"/>
# </schema>
#</rtt>
#>>> schema1Conf = schema1.getConfig()                                                        # [2] getting DozerSchemaConfig instance
#>>> schema1Conf
#<DozerSchemaConfig.DozerSchemaConfig object at 0x2a9d7670d0>
#>>> print schema1Conf
#nightly='*' install='build' cmtconfig='i686-slc4-gcc34-opt' project='AtlasProduction' jobId='job_1'
#>>> conf -= schema1Conf                                                                      # [3] removing schema1 using DozerSchemaConfig instance
#>>> print conf
#<?xml version="1.0" ?>
#<rtt>
# <schema cmtconfig="i686-slc4-gcc34-opt" install="build" jobId="job_1" nightly="bugfix" project="AtlasProduction" tag="" version="0">
#  <channel desc="channel #1 description" name="channel #1" uuid="1992790e-e121-4fc7-8242-6291a6f11a83"/>
# </schema>
# <schema cmtconfig="i686-slc4-gcc34-opt" install="build" jobId="job_1" nightly="dev" project="AtlasProduction" tag="" version="0">
#  <channel desc="channel #2 description" name="channel #2" uuid="c6a9b2d2-0b99-4e8d-8644-3022c672f91f"/>
# </schema>
#</rtt>
#>>> schema2Conf = schema2.getConfig(asTuple=True)                                           # [4] getting schema2 config as tuple 
#>>> schema2Conf
#(u'dev', 'build', 'i686-slc4-gcc34-opt', 'AtlasProduction', 'job_1')
#>>> conf -= schema2Conf                                                                     # [5] removing schema2 using config tuple
#>>> print conf
#<?xml version="1.0" ?>
#<rtt>
# <schema cmtconfig="i686-slc4-gcc34-opt" install="build" jobId="job_1" nightly="bugfix" project="AtlasProduction" tag="" version="0">
#  <channel desc="channel #1 description" name="channel #1" uuid="1992790e-e121-4fc7-8242-6291a6f11a83"/>
# </schema>
#</rtt>
#>>> schema3 = conf["bugfix"]["build"]["i686-slc4-gcc34-opt"]["AtlasProduction"]
#>>> conf -= schema3                                                                         # [6] removing DozerSchema using it's instance
#>>> print conf
#<?xml version="1.0" ?>
#<rtt/>
#</pre></b>
#</div></div></div>
#
#<div><div class="dozerTrick"><div class="dozerTrickLabel"> Trick? </div> <div class="dozerTrickText"> 
# It is impossible to remove implict schema using it's explicit copy. To remove implict one 
# you have to specify exact matching configuration! 
#</div></div></div>
# 
# </ul>
#
# <li> <h4>XML (de)serialisation</h4>
#
# Serialisation to XML DOM Document instance is made by calling DozerConfig.xml:
# 
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#>>> xmldoc = conf.xml()                                                     # [1] serialisation to XML DOM Document
#>>> xmldoc
#<xml.dom.minidom.Document instance at 0x2a9d79a7a0>
#>>> xmlstring = xmldoc.toxml()
#>>> xmlstring
#'<?xml version="1.0" ?>\n<rtt/>'
#>>> confFromXMLDoc = DozerConfig.DozerConfig.fromXMLDoc( xmldoc )           # [2] deserialisation from XML DOM Document
#>>> confFromXMLDoc
#<DozerConfig.DozerConfig object at 0x2a9d1cf9d0>
#>>> confFromXMLString = DozerConfig.DozerConfig.fromXMLString( xmlstring )  # [3] deserialisation from XML string
#>>> confFromXMLString
#<DozerConfig.DozerConfig object at 0x2a9d794c10>
#</pre></b>
#</div></div></div>
#
#<div><div class="dozerTrick"><div class="dozerTrickLabel"> Trick? </div> <div class="dozerTrickText"> 
# Serialisation and deserialisation to XML is used internally, but could be also used to write (read) DozerConfig object to (from) disk file.   
#</div></div></div>
#
# <li> <h4>Other functions</h4>
# 
# <ul>
#  <li> name of test framework getters: DozerConfig.framework, DozerConfig.testFramework
#  <li> saving DozerConfig XML document to disk file: DozerConfig.save 
#  <li> string representation od DozerConfig instance: DozerConfig.__str__ (used in above examples)
# </ul>
#
# </ul> 
# 
class DozerConfig( DozerObject.DozerObject ):

    ## (key, match) pairs
    __keyHold = { 1 : None,
                  2 : None,
                  3 : None,
                  4 : None,
                  5 : None }

    ## c'tor
    # @param self "Me, myself and Irene"
    # @param testFramework valid test framework name
    # @param log DozerLogger instance
    def __init__(self, testFramework, log=None):
        
        super( DozerConfig, self ).__init__( log )
        
        self.__log = log
 
        self.cv = DozerValid( log=log )

        if ( testFramework in self.cv.frameworks() ):
            self.__testFramework = str(testFramework)
            self.__db = str(testFramework)
        else:
            raise DozerError( "testFramework argument missing or not valid" )
    
        self.debug("constructing DozerConfig for test framework='%s'" % self.__testFramework )

        
        self.__schemas = DozerFolder(level=1, owner=self, log=self.__log)
        
    ## 
    # @param self "Me, myself and Irene"
    # @param lvl DozerFolder level
    # @param key key to search
    # @param match matched key
    def _holdMatchedPair( self, lvl, key, match ):        
        self.__keyHold[lvl] = ( key, match )        
     
    ## 
    # @param self "Me, myself and Irene"
    # @param lvl level for DozerFolder
    def _getMatchedPair( self, lvl):
        return self.__keyHold[lvl]

    ## test framework getter
    # @brief test framework getter
    # @param self "Me, myself and Irene"
    def testFramework( self ):
        return self.__testFramework

    ## test framework getter
    # @brief test framework getter
    # @param self "Me, myself and Irene"
    def framework( self ):
        return self.__testFramework

    ## keys getter
    # @param self "Me, myself and Irene"
    def keys( self ):
        return self.__schemas.keys()

    ## 
    # 
    def reset( self, schema ):
        schema = None
        if ( isinstance(schema, DozerSchema) ):
            nightly = schema.nightly()
            install = schema.install()
            cmtconfig = schema.cmtconfig()
            project = schema.project()
            jobId = schema.jobId()
            try:
                schema =  self.__schemas[nightly][install][cmtconfig][project][jobId].reset()
            except:
                pass

    ## += operator for adding DozerSchema
    # @param self "Me, myself and Irene"
    # @param schema DozerSchema instance 
    def __iadd__( self, schema ):

        nightly = install = cmtconfig = project = jobId = None
        if ( isinstance(schema, DozerSchema) ):
            nightly = schema.nightly()
            install = schema.install()
            cmtconfig = schema.cmtconfig()
            project = schema.project()
            jobId = schema.jobId()
            
        toAdd = ( nightly, install, cmtconfig, project, jobId )

        if ( None not in toAdd ):

            nMatch = None
            iMatch = None
            cMatch = None
            pMatch = None  
            jMatch = None 
            if ( nightly in self.__schemas ):
                nMatch = self.__schemas.match( nightly )
                if ( install in self.__schemas[nMatch] ):
                    iMatch = self.__schemas[nMatch].match( install )
                    if ( cmtconfig in self.__schemas[nightly][install] ):
                        cMatch = self.__schemas[nightly][install].match( cmtconfig )
                        if ( project in self.__schemas[nightly][install][cmtconfig] ):
                            pMatch = self.__schemas[nightly][install][cmtconfig].match( project )
                            if ( jobId in self.__schemas[nightly][install][cmtconfig][project] ):
                                jMatch =  self.__schemas[nightly][install][cmtconfig][project].match( jobId )

            tupleMatch = ( nMatch, iMatch, cMatch, pMatch, jMatch )

            if ( tupleMatch == toAdd ):
                self.__schemas[nightly][install][cmtconfig][project][jobId] = schema
                self.debug( "DozerSchema %s/%s/%s/%s/%s has been updated" % (nightly, install, cmtconfig, project, jobId ) )
            else:
                if ( nightly not in self.__schemas.keys() ):
                    self.__schemas[nightly] = DozerFolder(level=2, owner=self, log=self.__log)
            
                if ( install not in self.__schemas[nightly].keys() ):
                    self.__schemas[nightly][install] = DozerFolder(level=3, owner=self, log=self.__log)

                if ( cmtconfig not in self.__schemas[nightly][install].keys() ):
                    self.__schemas[nightly][install][cmtconfig] = DozerFolder(level=4, owner=self, log=self.__log)
                    
                if ( project not in self.__schemas[nightly][install][cmtconfig].keys() ):
                    self.__schemas[nightly][install][cmtconfig][project] =  DozerFolder(level=5, owner=self, log=self.__log)

                self.__schemas[nightly][install][cmtconfig][project][jobId] = schema

                self.debug( "DozerSchema %s/%s/%s/%s/%s has been added" % (nightly, install, cmtconfig, project, jobId ) )    
        return self 

    ## -= operator for removing DozerSchema
    # @param self "Me, myself and Irene"
    # @param schemaConfig DozerSchemaConfig or tuple (nightly, install, cmtconfig, project) or DozerSchema instance
    def __isub__( self, schemaConfig ):
        
        nightly = install = cmtconfig = project = jobId = None
        if ( isinstance(schemaConfig, DozerSchemaConfig ) ):
            nightly = schemaConfig.nightly()
            install = schemaConfig.install()
            cmtconfig = schemaConfig.cmtconfig()
            project = schemaConfig.project()
            jobid = schemaConfig.jobId() 

        elif ( isinstance(schemaConfig, TupleType) and len(schemaConfig) == 5 ):
            nightly, install, cmtconfig, project, jobId = schemaConfig
        elif ( isinstance(schemaConfig, DozerSchema ) ):
            nightly = schemaConfig.nightly()
            install = schemaConfig.install()
            cmtconfig = schemaConfig.cmtconfig()
            project = schemaConfig.project()
            jobId = schemaConfig.jobId()
        if ( None not in ( nightly, install, cmtconfig, project, jobId  ) ):
            
            nMatch = None
            iMatch = None
            cMatch = None
            pMatch = None  
            jMatch = None

            if ( nightly in self.__schemas ):
                nMatch = self.__schemas.match( nightly )
                if ( install in self.__schemas[nightly] ):
                    iMatch = self.__schemas[nightly].match( install )
                    if ( cmtconfig in self.__schemas[nightly][install] ):
                        cMatch = self.__schemas[nightly][install].match( cmtconfig )
                        if ( project in self.__schemas[nightly][install][cmtconfig] ):
                            pMatch = self.__schemas[nightly][install][cmtconfig].match( project )
                            if ( jobId in self.__schemas[nightly][install][cmtconfig][project] ):
                                jMatch = self.__schemas[nightly][install][cmtconfig][project].match( jobId )

            tupleMatch = ( nMatch, iMatch, cMatch, pMatch, jMatch )
            toRemove =   ( nightly, install, cmtconfig, project, jobId )  
 
            if ( None not in tupleMatch and 
                 tupleMatch == toRemove ):
                
                self.debug("removing schema nightly/install/cmtconfig/project/jobId  = %s/%s/%s/%s/%s" % ( nightly, 
                                                                                                           install,
                                                                                                           cmtconfig,
                                                                                                           project,
                                                                                                           jobId ) )

                del self.__schemas[nightly][install][cmtconfig][project][jobId]

                if ( self.__schemas[nightly][install][cmtconfig][project].keys() == [] ):
                    self.debug("removing unnecessary DozerFolder for jobIds")
                    del self.__schemas[nightly][install][cmtconfig][project]

                if ( self.__schemas[nightly][install][cmtconfig].keys() == [] ):
                    self.debug("removing unnecessary DozerFolder for projects")
                    del self.__schemas[nightly][install][cmtconfig]
                     
                if ( self.__schemas[nightly][install].keys() == [] ):
                   
                    self.debug("removing unnecessary DozerFolder for cmtconfigs")
                    del self.__schemas[nightly][install]
                        
                if ( self.__schemas[nightly].keys() == [] ):
                    self.debug("removing unnecessary DozerFolder for installs")
                    del self.__schemas[nightly]

                if ( self.__schemas.keys() == [] ):
                    self.debug("removing unnecessary DozerFolder for nightlies")
                    del self.__schemas
                    self.__schemas = DozerFolder(level=1, owner=self, log=self.__log)
            else:
                msg = "cannot remove schema for schema config=%s, reason:" % str(schemaConfig)
                if ( None in tupleMatch ):
                    msg += "'DozerSchema doesn't exist in DozerConfig'"
                elif ( tupleMatch != toRemove ):
                    msg += "'cannot remove general schema=%s using specific one=%s'" % (str(tupleMatch), str(toRemove)) 
                self.error(msg)
             
        return self


    ## "in" operator 
    # @param self "Me, myself and Irene"
    # @param schemaConfig DozerSchemaConfig instance or tuple ( nightly, install, cmtconfig, project, jobId )
    def __contains__( self, schemaConfig ):
        if ( isinstance(schemaConfig, DozerSchemaConfig) or 
             type(schemaConfig) is TupleType ):
            if ( self.__match(schemaConfig) ): return True
        elif ( schemaConfig in StringTypes ):
            if ( schemaConfig in self.__schemas ): return True
        return False

        
    ## [] operator
    # @param self "Me, myself and Irene"
    # @param key valid nightly name 
    def __getitem__( self, key ):
    
        self.__keyHold = { 1: None, 2: None, 3: None, 4:None, 5:None}

        match = self.__schemas.match( key )
        if ( key in self.__schemas ):
            return self.__schemas[ key ]
        else:
            raise KeyError("key=%s not found in DozerConfig schemas" % str(key) )

    ## iterator 
    # @param self "Me, myself and Irene"
    def __iter__( self ):
        return self.__schemas.__iter__()
    
    ## old style iterator getter
    # @param self "Me, myself and Irene"
    def next( self ):
        return self.__schemas.next()

    ## items getter
    # @param self "Me, myself and Irene"
    def items( self ):
        return self.__schemas.items()

    ## iteritems getter
    # @param self "Me, myself and Irene"
    def iteritems( self ):
        return self.__schemas.iteritems()
    
    ## iterkeys getter
    # @param self "Me, myself and Irene"
    def iterkeys( self ):
        return self.__scehmas.iterkeys()


    ## str() operator for DozerConfig
    # @param self "Me, myself and Irene"
    def __str__( self ):
        return self.xml().toprettyxml(" ", "\n")
    
    ## 
    # @brief XML DOM Document builder
    # @param self "Me, myself and Irene"
    def __DOMXML( self ):
        
        self.__doc =  xml.dom.minidom.Document()
        
        rootNode = self.__doc.createElement( self.__testFramework.lower() )
    
        for nKey in self.__schemas:
            for iKey in self.__schemas[nKey]:
                for cKey in self.__schemas[nKey][iKey]:
                    for pKey in self.__schemas[nKey][iKey][cKey]:
                        for jKey in self.__schemas[nKey][iKey][cKey][pKey]:
                            schema = self.__schemas[nKey][iKey][cKey][pKey][jKey].xml()
                            schemaNode = schema.documentElement
                            rootNode.appendChild( schemaNode )
                        
        self.__doc.appendChild( rootNode )

        return self.__doc 
        
    ##
    # @brief XML representation of DozerConfig
    # @param self "Me, myself and Irene"
    # @return xml.dom.minidom.Document instance
    def xml( self ):
        return self.__DOMXML()

    ##
    # @brief saving CoolDozer XML representation to file
    # @param self "Me, myself and Irene"
    # @param toFile path to file   
    # @param backup flag to create backup file (.bak)
    def save( self, toFile, backup=True ):
        
        toFile = os.path.abspath( toFile )
  
        if ( backup and  os.path.exists( toFile ) ):
            self.warn("file %s exists and will be overwritten!" % toFile )
            toBackup = toFile + ".bak"
            self.debug("will create backup file %s" % toBackup )
            if ( os.path.exists( toBackup ) ):
                self.warn("backup file %s exists and will be overwritten!" % toBackup )
            try:
                os.rename( toFile, toBackup )
            except OSError, value:
                msg = "creation of backup file %s failed - %s" % ( toBackup, str(value) )
                self.epanic( msg )
                raise DozerError( msg )
        try:
            fileXML = open(toFile, "w+")
            xmlstring = self.xml().toprettyxml(" ", "\n")
            fileXML.writelines( xmlstring )
            fileXML.close()
        except IOError, value:
            msg = "creation of XML file %s failed - %s" % (toBackup, str(value) )
            self.epanic( msg )
        
        return True
        
    ##
    # @param cls class
    # @param xmlstring  string representation of XML document
    # @param log DozerLogger instance
    # @brief deserilization from XML string
    @classmethod 
    def fromXMLString( cls, xmlstring , log=None):
        xmldoc = xml.dom.minidom.parseString( xmlstring )
        return cls.fromXMLDoc( xmldoc, log )
      
    ##
    # @param cls class
    # @param xmldoc XML DOM document
    # @param log DozerLogger instance
    # @brief deserialization from XML DOM document
    @classmethod
    def fromXMLDoc( cls, xmldoc, log=None):
        this = None
        testFrameworkNode = xmldoc.documentElement 
        if ( testFrameworkNode ):
            testFramework = testFrameworkNode.tagName.upper()
           
            this = cls(  testFramework, log )

            schemasList = testFrameworkNode.getElementsByTagName("schema")
            for schema in schemasList:
                this += DozerSchema.fromXMLString( schema.toxml(), log )
            
        return this


    ## schema matcher
    # @param self "Me, myself and Irene"
    # @param schemaConfig DozerSchemaConfig instance or tuple ( nightly, install, cmtconfig, project )
    # @return DozerSchemaConfig instance or None
    def __match( self, schemaConfig ):
        
        nightly = install = cmtconfig = project = jobId =  None
        
        if ( isinstance(schemaConfig, DozerSchemaConfig) ):
            nightly = schemaConfig.nightly() 
            install = schemaConfig.install()
            cmtconfig = schemaConfig.cmtconfig()
            project = schemaConfig.project()
            jobId = schemaConfig.jobId() 
            
        elif ( isinstance(schemaConfig, TupleType) and
               len(schemaConfig) == 5):
            nightly, install, cmtconfig, project, jobId = schemaConfig

        if ( None in ( nightly, install, cmtconfig, project, jobId ) ):
            msg = "cannot match schema configuration, requested schemaConfig (%s) is wrong!" % str(schemaConfig)
            self.error( msg )
            raise DozerError( msg )

        if ( nightly in self.__schemas ):
            if ( install in self.__schemas[nightly] ):
                if ( cmtconfig in self.__schemas[nightly][install] ):
                    if ( project in self.__schemas[nightly][install][cmtconfig] ):
                        if ( jobId in self.__schemas[nightly][install][cmtconfig][project] ):
                            return DozerSchemaConfig( nightly, install, cmtconfig, project, jobId )
        return False


        for theNightly in sorted( self.__schemas.keys(), reverse=True):
            for theInstall in sorted( self.__schemas[theNightly].keys(), reverse=True ):
                for theCmtconfig in sorted( self.__schemas[theNightly][theInstall].keys(), reverse=True ):
                    for theProject in sorted( self.__schemas[theNightly][theInstall][theCmtconfig].keys() , reverse=True):
                        
                        reNightly = theNightly
                        reInstall = theInstall
                        reCmtconfig = theCmtconfig
                        reProject = theProject
                        
                        pattern = "[a-z0-9\-_]*"
                        
                        if ( "*" in theNightly ) : reNightly = theNightly.replace("*", pattern, theNightly.count("*") )
                        if ( "*" in theInstall ) : reInstall = theInstall.replace("*", pattern, theInstall.count("*") )
                        if ( "*" in theCmtconfig ) : reCmtonfig = theCmtonfig.replace("*", pattern, theCmtconfig.count("*") )
                        if ( "*" in theProject ) : reProject = theProject.replace("*", pattern, theProject.count("*") )

                        self.debug( "regexs %s %s %s %s" % ( reNightly, reInstall, reCmtconfig, reProject) )

                        nMatch = False 
                        if ( re.compile( reNightly ).search( nightly ) ): 
                            self.debug( "nightly match! %s (%s) %s" % ( theNightly, reNightly , nightly ) )
                            nMatch = True

                        iMatch = False
                        if ( re.compile( reInstall ).search( install ) ): 
                            self.debug( "install match! %s (%s) %s" % ( theInstall, reInstall,  install ) )
                            iMatch = True

                        cMatch = False
                        if ( re.compile( reCmtconfig ).search( cmtconfig ) ) : 
                            self.debug( "cmt match! %s (%s) %s" % ( theCmtconfig, reCmtconfig, cmtconfig ) )
                            cMatch = True

                        pMatch = False 
                        if ( re.compile( reProject ).search( project ) ): 
                            self.debug( "project match! %s (%s) %s" % ( theProject, reProject,  project ) )
                            bMatch = True
                        
                        if ( ( nMatch, iMatch, cMatch, bMatch ) == ( True, True, True, True ) ):
                            self.debug("found schema that matches to arguments configuration")
                            self.debug("runtime nightly/install/cmt/project = %s/%s/%s/%s" % ( nightly,
                                                                                               install, 
                                                                                               cmtconfig, 
                                                                                               project) )
                            self.debug("schema  nightly/install/cmt/project = %s/%s/%s/%s" % ( theNightly,
                                                                                               theInstall, 
                                                                                               theCmtconfig, 
                                                                                               theProject ) )
                            schemaConfig = DozerSchemaConfig( theNightly, theInstall, theCmtconfig, theProject, theJobId )
                            self.__debug( str(schemaConfig) )
                            return schemaConfig
        return None
            


    ## schema getter 
    # @param self "Me, myself and Irene"
    # @param nightly valid nightly name
    # @param install valid install name
    # @param cmtconfig valid cmtconfig name
    # @param project valid project name 
    # @param jobId job id name
    # @return DozerSchema or None 
    def getSchema( self, nightly, install, cmtconfig, project, jobId ):

        print "-+-"*40
        try:
            matchedSchema = self.__match( ( nightly, install, cmtconfig, project, jobId ) )
            self.debug( "matched schema =%s " % str(matchedSchema) )
            if ( matchedSchema ):
                theNightly = matchedSchema.nightly()
                theInstall = matchedSchema.install()
                theCmtconfig = matchedSchema.cmtconfig()
                theProject = matchedSchema.project()    
                theJobId = matchedSchema.jobId()
                
                return self.__schemas[theNightly][theInstall][theCmtconfig][theProject][theJobId]
        except DozerError:
            msg = "DozerSchema for nightly/install/cmtconfig/project/jobId = %s/%s/%s/%s/%s not found" % ( str(nightly),
                                                                                                           str(install),
                                                                                                           str(cmtconfig),
                                                                                                           str(project),
                                                                                                           str(jobId) )
            self.error( msg )

        return None



    ## general getter from path
    # @param self "Me, myself and Irene"
    # @param path path to search 
    def get( self, path="" ):
        normpath = os.path.normpath( path.strip("/") )
        self.debug("get path='%s' normpath='%s'" % ( path, normpath ) )
        if ( normpath == "." ): return self
        else:
            words = str(normpath).split("/")
            searchPath = "/".join(words[1:])
            return self[words[0]].get( searchPath ) 
           

##
# @class test_DozerConfig
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief test case for DozerConfig class
class test_DozerConfig( unittest.TestCase ):
    
    ## setup tests 
    # @param self "Me, myself and Irene"
    def setUp( self ):
        self.__log = DozerLogger("test_DozerConfig.log", "test_DozerConfig", toConsole=True)
       
        self.data1 = DozerData("test int data", "Int", "N_{obs}", log=self.__log)
        self.chan1 = DozerChannel("test channel", log=self.__log)
        self.schema1 = DozerSchema("14.2.1X", "build", "i686-slc4-gcc34-opt", "AtlasProduction", "job_1", log=self.__log )

        self.chan1 += self.data1
        self.schema1 += self.chan1

        self.data2 = DozerData("test string data", "Clob", "", log=self.__log )
        self.chan2 = DozerChannel("test channel", log=self.__log)
        self.schema2 = DozerSchema("*", "build", "i686-slc4-gcc34-opt", "AtlasProduction", "job_1", log=self.__log )
       
        self.chan2 += self.data2
        self.schema2 += self.chan2
                

        self.schema3 = DozerSchema("14.2.1X.Y-VAL", "*", "i686-*-gcc34-opt", "AtlasProduction", "job_1", log=self.__log )
        
        self.conf = DozerConfig("RTT", log=self.__log)

        self.conf += self.schema1
        self.conf += self.schema2
        self.conf += self.schema3

    ## __init__()
    # @param self "Me, myself and Irene"
    def test_01_constructor( self ):
        try:
            self.conf = DozerConfig("RTT", log=self.__log)
        except:
            pass
        self.assertEqual( isinstance(self.conf, DozerConfig), True )


    ## framework() testFramework()
    # @param self "Me, myself and Irene"
    def test_02_getters( self ):
        
        self.assertEqual( self.conf.framework(), "RTT" )
        self.assertEqual( self.conf.testFramework(), "RTT" )

    ## +=, -=, in, get()
    # @param self "Me, myself and Irene"
    def test_03_operators_DozerSchema( self ):

        self.conf += self.schema1
        self.conf += self.schema2
        self.conf += self.schema3
        
        schema1Conf = self.schema1.getConfig()
        print "DozerSchemaConf %s" % str(schema1Conf)
        schema2Conf = self.schema2.getConfig( asTuple=True )
        print "tuple %s" % str(schema2Conf)

        self.assertEqual( isinstance( self.conf, DozerConfig), True )
        
       
        atuple1 = ( "14.2.1X", "build", "i686-slc4-gcc34-opt", "AtlasProduction", "job_1")
        self.assertEqual( atuple1 in self.conf, True )
        
        try:
            schema = self.conf["bugfix"]["build"]["i686-slc4-gcc34-opt"]["AtlasProduction"]["job_1"]
            self.assertEqual(isinstance(schema, DozerSchema), True)
        except:
            pass
        
        schemaConfig = DozerSchemaConfig("bugfix", "build", "i686-slc4-gcc34-opt", "AtlasProduction", "job_1")
        self.assertEqual( schemaConfig in self.conf, True )

        
        schema = self.conf.getSchema("bugfix", "build", "i686-slc4-gcc34-opt", "AtlasProduction", "job_1")
        self.assertNotEqual( schema, None )

        print self.conf.keys()

        self.__log.getLogger(self).info( self.conf )
        self.conf -= schema1Conf
        print self.conf.keys()

        self.assertEqual( isinstance(self.conf, DozerConfig), True )

        self.__log.getLogger(self).info( self.conf )
        self.conf -= schema2Conf
        print self.conf.keys()

        self.__log.getLogger(self).info( self.conf )
        self.conf -= self.schema3
        print self.conf.keys()
        self.__log.getLogger(self).info( self.conf )

    ## tesing xml(), fromXMLString(), fromXMLDoc()
    # @param self "Me, myself and Irene"
    def test_04_XML_serialization( self ):
        xmldoc = None
        try:
            xmldoc = self.conf.xml()
        except:
            pass
        self.assertEqual(isinstance(xmldoc,xml.dom.minidom.Document), True )
       
        fromXMLDoc = None
        try:
            fromXMLDoc = DozerConfig.fromXMLDoc( xmldoc )
        except:
            pass
        
        self.assertEqual( isinstance(fromXMLDoc, DozerConfig), True )

        fromXMLString = None 
        try:
            fromXMLString = DozerConfig.fromXMLString( xmldoc.toxml() )
        except:
            pass
        self.assertEqual( isinstance(fromXMLString, DozerConfig), True )
        
        self.assertEqual( fromXMLString.xml().toxml(), fromXMLDoc.xml().toxml() )
        
    ## tesing in, keys(), has_key(), []
    # @param self "Me, myself and Irene"
    def test_05_dictionary_interface( self ):
       
        self.conf += self.schema1
        self.conf += self.schema2

        schema = self.conf["dev"]["build"]["i686-slc4-gcc34-opt"]["AtlasProduction"]["job_1"]
        self.assertEqual( isinstance(schema, DozerSchema), True)
     
        #print self.conf
        
        schema = self.conf["14.2.1X.Y-VAL"]["build"]["i686-slc4-gcc34-opt"]["AtlasProduction"]["job_1"]
        self.assertEqual( isinstance(schema, DozerSchema), True)
        
        #print self.conf

        keys = self.conf["14.2.1X"]["build"]["i686-slc4-gcc34-opt"].keys()
        has_key = self.conf["bugfix"]["build"]["i686-slc4-gcc34-opt"].has_key("AtlasProduction")

        self.assertEqual(keys, ["AtlasProduction"])
        self.assertEqual(has_key, True)

        keys =  self.conf["dev"]["build"]["i686-slc4-gcc34-opt"]["AtlasProduction"].keys()
        #print keys



    ## tesing get()
    # @param self "Me, myself and Irene"
    def test_06_get( self ):
        
        self.conf += self.schema1
        self.conf += self.schema2

        n1=  self.schema1.nightly()
        i1 = self.schema1.install()
        c1 = self.schema1.cmtconfig()
        p1 = self.schema1.project()
        j2 = self.schema1.jobId()

        path = n1
        self.assertEqual( isinstance( self.conf.get(path), DozerFolder), True )
        path += "/" + i1 
        self.assertEqual( isinstance( self.conf.get(path), DozerFolder), True )
        path += "/" + c1
        self.assertEqual( isinstance( self.conf.get(path), DozerFolder), True )
        path += "/" + p1
        self.assertEqual( isinstance( self.conf.get(path), DozerFolder), True )
        path += "/" + j2
        self.assertEqual( isinstance( self.conf.get(path), DozerSchema), True )

        #print self.conf

        n2 = self.schema2.nightly()
        i2 = self.schema2.install()
        c2 = self.schema2.cmtconfig()
        p2 = self.schema2.project()
        j2 = self.schema2.jobId()
        
        path = "14.2.1X"
        self.assertEqual( isinstance( self.conf.get(path), DozerFolder), True )
        path += "/" + i2 
        self.assertEqual( isinstance( self.conf.get(path), DozerFolder), True )
        path += "/" + c2
        self.assertEqual( isinstance( self.conf.get(path), DozerFolder), True )
        path += "/" + p2
        self.assertEqual( isinstance( self.conf.get(path), DozerFolder), True )
        path += "/" + j2
        self.assertEqual( isinstance( self.conf.get(path), DozerSchema), True )

        sch = self.conf["14.2.1X.Y-VAL"][i2][c2][p2][j2]
        
        #print self.conf



## test suite execution
if __name__ == "__main__":     

    testLoader = unittest.TestLoader()
    suite = testLoader.loadTestsFromTestCase(test_DozerConfig)      
    unittest.TextTestRunner(verbosity=3).run(suite)
