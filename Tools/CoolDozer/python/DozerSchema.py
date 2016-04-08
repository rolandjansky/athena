#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
# @file DozerSchema.py
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @date Apr 22, 2008
# @brief Implementation of DozerSchema class.

# @package DozerSchema
# @brief Implementation of DozerSchema class.


import DozerObject
from DozerUtils import *

from DozerData import DozerData
from DozerChannel import DozerChannel
from DozerValid import DozerValid
from DozerSchemaConfig import DozerSchemaConfig

from types import *
import xml.dom.minidom
import os.path
import unittest

## 
# @class DozerSchema
# @brief container class for DozerChannel objects
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @htmlinclude cool_dozer_api.css
# <h3>Usage</h3>
# 
# <ul>
# <li> <h4>Concept</h4>
# 
# A DozerSchema object keeps configuration for monitored values (set  of DozerChannel objects) in one nightly configuration.          
# It is directly mapped inside COOL database to the cool folders.
# 
# A configuration for one job in nightly build testing framework in the Atlas experiment is explicitly defined by:
#
# <ul>
#  <li> a bundle (nightly) name e.g. <tt>dev</tt>, <tt>bugfix</tt>, <tt>devval</tt>, <tt>mig6</tt> etc.
#  <li> an installation type e.g.:
#  <ul> 
#   <li> afs build directories - <tt>build</tt> 
#   <li> installation from kit distribution  - <tt>kit</tt>
#  </ul>
#  <li> a CMTCONFIG shell variable: <br/>
#    <b><tt> (binary version)-(operating system)-(compiler version)-(type of build) </tt></b><br/>
#    where:
#    <ul>
#     <li> <b><tt>binary version</tt></b> distinguishes between 32bit and 64bit binary 
#     (taken from shell command: <tt>uname -p</tt>)
#     <li> <b><tt>operating system</tt></b> holds OS name abbreviation, e.g. <tt>slc4</tt> - 
#     <b>S</b>cientific <b>L</b>inux <b>C</b>ERN release <b>4</b>
#     <li> <b><tt>compiler version</tt></b> holds the name of compiler used during build process, e.g.: <tt>gcc323</tt> - 
#     <b>G</b>NU <b>c</b>ollection of <b>compilers</b> version <b>3</b>.<b>2</b>.<b>3</b>   
#     <li> <b><tt>type of build</tt></b> indicates if optimisation was switched on during build and 
#     if debugger symbols are present in object files (<b>opt</b> or <b>dbg</b>) 
#    </ul>
#  <li> an Atlas project full name e.g.:
#   <ul> 
#    <li> AtlasProduction 
#    <li> AtlasTier0
#    <li> AtlasP1HLT  
#   </ul>
#  <li> unique job id - for examples check the documentation for test framework...
# </ul>
#
# CoolDozer API uses the same information to distinguish between different nightlies. The mapping between nightlies 
# results and COOL RAL database folders is done by specifying the same four pieces of informations, with the exception that
# if the configuration of the database tracked and monitored value sets are common amongst a few configuration of nightlies
# the user can specify the CoolDozer configuration to cover all of them using limited wild card [asterisk *].
# Hence the valid names for nightly, install, CMTCONFIG and project may be:
#
# <ul>
#  <li> explicit name e.g.: <tt>dev</tt>, <tt>i686-slc4-gcc34-opt</tt>, <tt>AtlasProduction</tt>
#  <li> implicit name e.g.: <tt>*</tt>, <tt>dev*</tt>, <tt>i686-slc4-*-opt</tt>, <tt>*-opt</tt>, <tt>Atlas*</tt>
# </ul>
#     
# If any of (nightly/install/CMTCONFIG/project/jobId) is composed with at least one wild card character, the schema is 
# an implicit one (and vice versa).
#
#<div><div class="dozerTrick"><div class="dozerTrickLabel"> Trick? </div> <div class="dozerTrickText">
# It is a good practice to have (amongst the others) the most general DozerSchema (*/*/*/*) inside the CoolDozer application.
#</div></div></div>  
#
#<div><div class="dozerInfo"><div class="dozerInfoLabel">Notice</div><div class="dozerInfoText">
# An implicit DozerSchema is mapped to the COOL database like this:
# <pre>
#  DozerSchema |  (nightly name)/(install type)/(CMTCONFIG)/(project name)/(jobId)
#  ---------------------------------------------------------------------------------
#  COOL        |   FolderSet     FolderSet      FolderSet    FolderSet     Folder
# </pre>
#</div></div></div>  
#
# <li> <h4>Costructing</h4>
#
# A DozerSchema object can be instated using its constructor (DozerSchema.__init__):
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">  
#<b><pre>
#[lxplus212] ~/scratch2/CoolDozerClient/python % python
#Python 2.5 (r25:51908, Oct 18 2007, 16:04:48) 
#[GCC 3.4.6 20060404 (Red Hat 3.4.6-8)] on linux2
#Type "help", "copyright", "credits" or "license" for more information.
#>>> import DozerSchema
#>>> schema = DozerSchema.DozerSchema("dev", "*", "i686-*", "offline", "job_1")
#>>> schema
#<DozerSchema.DozerSchema object at 0x2a9d2616d0>
#</pre></b>
#</div></div></div>
#
# The optional parameters (not used in above example) are:
# <ul>
# <li> schema <tt>version</tt> - integer number, autoincremented when runtime schema differs with it's database version
# <li> cvs <tt>tag</tt> - a tag name from CVS, this should be extracted and used in particular framework, it is 
# not interpreted by CoolDozer API
# <li> log - DozerLogger instance   
# </ul>
#
# <li> <h4>Sea of getters and setters</h4> 
#
# <ul>
# <li> for nightly 
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText"> 
#<b><pre>
#>>> schema.nightly()
#u'dev'
#>>> schema.setNightly("bugfix")
#True
#>>> schema.nightly()
#u'bugfix'
#>>> schema.setNightly("blah")
#False
#>>> schema.nightly()
#u'bugfix'
#</pre></b>
#</div></div></div>
#
# <li> ...and for install, CMTCONFIG, project, jobId,  version, tag...
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText"> 
#<b><pre>
#>>> schema.install()
#'*'
#>>> schema.setInstall("build")
#True
#>>> schema.cmtconfig()
#'i686-*'
#>>> schema.setCmtconfig("i686-*-opt")
#True
#>>> schema.cmtconfig()
#'i686-*-opt'
#>>> schema.project()
#'offline'
#>>> schema.setProject("hlt")
#True
#>>> schema.project()
#'hlt'
#>>> schema.setJobId("job_2")
#True
#>>> schema.jobId()
#'job_2'
#>>> schema.version()
#'0'
#>>> schema.setVersion(1)
#True
#>>> schema.version()
#'1'
#>>> schema.setVersion("2")
#True
#>>> schema.version()
#'2'
#>>> schema.tag()
#''
#>>> schema.setTag("Dummy-00-00-01")
#True
#>>> schema.tag()
#'Dummy-00-00-01'
#</pre></b>
#</div></div></div>
# </ul>
#
#<div><div class="dozerWarn"> <div class="dozerWarnLabel"> Beware! </div> <div class="dozerWarnText"> 
# Remember to check the returned status (True/False) from every setter call! 
#</div></div></div>
#
# <li> <h4>Working with DozerChannel objects</h4>
#
# <ul>
# <li> adding DozerChannel instances to schema
#
# To add DozerChannel object to DozerSchema use "+=" operator of form:
#
#<b><pre>
# (DozerSchema instance) += (DozerChannel instance)
#</pre></b>
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#>>> import DozerChannel
#>>> chan = DozerChannel.DozerChannel("test channel #1", "test channel #1 description")
#>>> schema += chan
#>>> schema += DozerChannel.DozerChannel("test channel #2", "test channel #2 description")
#>>> print schema
#<?xml version="1.0" ?>
#&lt;!--
#schema for configuration:
# [1] nightly   = bugfix
# [2] install   = build
# [3] cmtconfig = i686-*-opt
# [4] project   = hlt
# [5] jobId     = job_2
#--&gt;
#<schema cmtconfig="i686-*-opt" install="build" jobId="job_2" nightly="bugfix" project="hlt" tag="Dummy-00-00-01" version="2">
# <channel desc="test channel #1 description" name="test channel #1" uuid="f5fe1707-cf13-4c99-9920-e8241af37422"/>
# <channel desc="test channel #2 description" name="test channel #2" uuid="f09583f0-3157-4c7a-af91-e3c48387fe9e"/>
#</schema>
#</pre></b>
#</div></div></div>
#
# <li> getting back DozerChannel objects
#
# <ul>
#
#  <li> using python dictionary interface
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#>>> schema.has_key("test channel #1")                                                                 # [1] is channel of name in?                            
#True
#>>> schema.has_key("f5fe1707-cf13-4c99-9920-e8241af37422")                                            # [2] is channel of UUID in?
#True
#>>> schema["test channel #1"].name()                                                                  # [3] using channel name 
#'test channel #1'
#>>> schema["f5fe1707-cf13-4c99-9920-e8241af37422"].name()                                             # [4] using channel UUID
#'test channel #1'
#>>> for k in schema : print "key='%s' channel name='%s'" % ( k , schema[k].name() )                   # [5] looping using channels name
#... 
#key='test channel #1' channel name='test channel #1'
#key='test channel #2' channel name='test channel #2'
#>>> for k in schema.keys() : print "key='%s' channel name='%s'" % ( k , schema[k].name() )            # [6] and once again
#... 
#key='test channel #1' channel name='test channel #1'
#key='test channel #2' channel name='test channel #2'
#>>> for k in schema.keys(byUUID=True) : print "key='%s' channel name='%s'" % ( k , schema[k].name() ) # [7] looping using channels UUID
#... 
#key='f5fe1707-cf13-4c99-9920-e8241af37422' channel name='test channel #1'
#key='f09583f0-3157-4c7a-af91-e3c48387fe9e' channel name='test channel #2'
#>>> for k,v in schema.iteritems(): print "key='%s' channel name='%s'" % ( k, v.name() )               # [8] looping by name using (key, value) pair 
#... 
#key='test channel #1' channel name='test channel #1'
#key='test channel #2' channel name='test channel #2'
#>>> for k,v in schema.iteritems(byUUID=True): print "key='%s' channel name='%s'" % ( k, v.name() )    # [9] looping by UUID using (key, value) pair 
#... 
#key='f5fe1707-cf13-4c99-9920-e8241af37422' channel name='test channel #1'
#key='f09583f0-3157-4c7a-af91-e3c48387fe9e' channel name='test channel #2'
#</pre></b>
#</div></div></div>
#
# <li> using "XML DOM-like" interface
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#>>> schema.getChannelByName("test channel #1")                        # [1] channel by name 
#<DozerChannel.DozerChannel object at 0x2a9d2618d0>
#>>> schema.getChannelByUUID("f5fe1707-cf13-4c99-9920-e8241af37422")   # [2] channel by UUID
#<DozerChannel.DozerChannel object at 0x2a9d2618d0>
#>>> for k in schema.getChannelsByUUID(): print schema[k].name()       # [3] all channels by UUID
#... 
#test channel #1
#test channel #2
#>>> for k in schema.getChannelsByName(): print schema[k].name()       # [4] all channels by name
#... 
#test channel #1
#test channel #2
# </pre></b> 
#</div></div></div>
#
#  <li> using "file system-like" get
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#>>> schema.get()                                                      # [1] empty path return schema itself
#<DozerSchema.DozerSchema object at 0x2a9d2616d0>
#>>> schema.get(".")                                                   # [2] the same for this directory [dot .]
#<DozerSchema.DozerSchema object at 0x2a9d2616d0>
#>>> schema.get("./f5fe1707-cf13-4c99-9920-e8241af37422")              # [3] get by UUID
#<DozerChannel.DozerChannel object at 0x2a9d2618d0>
#>>> schema.get("test channel #1")                                     # [4] get by name
#<DozerChannel.DozerChannel object at 0x2a9d2618d0>
#>>> schema.get("./test channel #1")                                   # [5] and once again 
#<DozerChannel.DozerChannel object at 0x2a9d2618d0>
#</pre></b>
#</div></div></div>
# </ul>
# 
# <li> removing DozerChannel objects from schema 
#
# This can be done by using "-=" operator:
#
#<b><pre>
#  (DozerSchema instance) -= (DozerChannel instance)
#                         or 
#  (DozerSchema instance) -= (DozerChannel name)
#                         or
#  (DozerSchema instance) -= (DozerChannel UUID)
#</pre></b>
#
# e.g.:
# 
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#>>> schema -= "test channel #2"                                       # [1] removing using channel name
#>>> print schema.keys()
#['test channel #1']
#>>> schema -= "f5fe1707-cf13-4c99-9920-e8241af37422"                  # [2] removing using channel UUID  
#>>> print schema.keys()
#[]
#>>> schema += DozerChannel.DozerChannel("test channel #3", "test channel #3 description")
#>>> chan = schema["test channel #3"]
#>>> schema.keys()
#['test channel #3']
#>>> schema -= chan                                                    # [3] removing using DozerChannel instance
#>>> schema.keys()
#[]
#</pre></b>
#</div></div></div>
#
# </ul>
#
# <li> <h4>XML (de)serialisation</h4>
# 
# Serialisation and deserialisation of DozerSchema is made by DozerSchema.xml (serialisation) and 
# DozerSchema.fromXMLDoc or DozerSchema.fromXMLString (deserialisation) methods. It is used only internally but 
# may be used to save (load) schema to (from) XML file, see the example below:
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre> 
#[lxplus215] ~/scratch2/CoolDozerClient/python % python
#Python 2.5 (r25:51908, Oct 18 2007, 16:04:48) 
#[GCC 3.4.6 20060404 (Red Hat 3.4.6-8)] on linux2
#Type "help", "copyright", "credits" or "license" for more information.
#>>> import DozerSchema
#>>> import DozerChannel
#>>> import DozerData
#>>> schema = DozerSchema.DozerSchema("bugfix", "*", "i686-*", "AtlasProduction", "job_1" )  # [1.1] construct DozerSchema
#>>> chan = DozerChannel.DozerChannel("test channel", "test channel description")            # [1.2] construct DozerChannel 
#>>> chan += DozerData.DozerData("test data", "Int", "counts")                               # [1.3] add some DozerData to channel 
#>>> schema += chan                                                                          # [1.4] add DozerChannel to schema
#>>> schema.xml()                                                                            # [2.1] serialisation
#<xml.dom.minidom.Document instance at 0x2a9d14c560>
#>>> xmldoc = schema.xml()
#>>> print schema                                                                            # [2.2] printing schema
#<?xml version="1.0" ?>
#&lt;!--
#schema for configuration:
# [1] nightly   = bugfix
# [2] install   = *
# [3] cmtconfig = i686-*
# [4] project   = AtlasProduction
# [5] jobId     = job_1
#--&gt;
#<schema cmtconfig="i686-*" install="*" jobId="job_1" nightly="bugfix" project="AtlasProduction" tag="" version="0">
# <channel desc="test channel description" name="test channel" uuid="c12484df-d8af-4550-8008-1fa34f633da7">
#  <data name="test data" type="Int" unit="counts"/>
# </channel>
#</schema>
#>>> xmlFile = open("temp.xml", "w+")                                             # [2.3] file opening
#>>> xmlFile.writelines( str(schema) )                                            # [2.4] xml saving
#>>> xmlFile.close()                                                              # [2.5] file closing
#>>> xmlFile = open("temp.xml", "r")                                              # [3.1] file reopening 
#>>> lines = xmlFile.readlines()                                                  # [3.2] reading from file
#>>> xmlFile.close()
#>>> for line in lines: print line.strip("\n")                                    # [3.3] file contents printing
#... 
#<?xml version="1.0" ?>
#&lt;!--
#schema for configuration:
# [1] nightly   = bugfix
# [2] install   = *
# [3] cmtconfig = i686-*
# [4] project   = AtlasProduction
# [5] jobId     = job_1
#--&gt;
#<schema cmtconfig="i686-*" install="*" jobId="job_1" nightly="bugfix" project="AtlasProduction" tag="" version="0">
# <channel desc="test channel description" name="test channel" uuid="c12484df-d8af-4550-8008-1fa34f633da7">
#  <data name="test data" type="Int" unit="counts"/>
# </channel>
#</schema>
#>>> xmlString = "".join( lines )                                                 # [3.4] making one string from file contents 
#>>> copyOfSchema = DozerSchema.DozerSchema.fromXMLString( xmlString )            # [3.5] deserialisation
#>>> print copyOfSchema                                                           # [3.6] printing copy of schema 
#<?xml version="1.0" ?>
#&lt;!--
#schema for configuration:
# [1] nightly   = bugfix
# [2] install   = *
# [3] cmtconfig = i686-*
# [4] project   = AtlasProduction
# [5] jobId     = job_1
#--&gt;
#<schema cmtconfig="i686-*" install="*" jobId="job_1" nightly="bugfix" project="AtlasProduction" tag="" version="0">
# <channel desc="test channel description" name="test channel" uuid="c12484df-d8af-4550-8008-1fa34f633da7">
#  <data name="test data" type="Int" unit="counts"/>
# </channel>
#</schema>
#</pre></b>
#</div></div></div>
#
# <li> <h4>Other (non-)useful functions</h4>
#
# <ul>
# <li> string representation operator DozerSchema.__str__ (was used in above examples)
# <li> checking if schema is implicit (DozerSchema.isImplicit or DozerSchema.isGeneral ) or
#  explicit (DozerSchema.isExplicit or DozerSchema.isSpecific)
# </ul>
#
# </ul>
class DozerSchema( DozerObject.DozerObject ):
    
    ## c'tor
    # @param self "Me, myself and Irene"
    # @param nightly valid nighlty name ("*", "dev", "devval", "bugfix", etc.)
    # @param install valid install name ("*", "build", "kit")
    # @param cmtconfig valid CMTCONFIG name ("*", "i686-slc4-gcc34-opt", etc.)
    # @param project valid project name ("*", "AtlasProduction", "AtlasTier0", "AtlasP1HLT")
    # @param jobId valid job id number, whatever it is 
    # @param log DozerLogger instance
    # @param version schema version number
    # @param tag CVS tag
    def __init__( self, nightly="*", install="*", cmtconfig="*", project="*", jobId="*", version=None, tag=None, log=None ):
        
        super( DozerSchema, self ).__init__( log )
        self.__log = log

        self.debug("constructing DozerSchema %s/%s/%s/%s/%s" % (nightly,install,cmtconfig,project,jobId ) )

        self.cv = DozerValid( log=log )    

        if ( not self.setNightly( nightly ) ):
            raise DozerError("cannot construct DozerSchema, parameter nightly='%s' not valid" % nightly )

        if ( not self.setInstall( install ) ):
            raise DozerError("cannot construct DozerSchema, parameter install='%s' not valid" % install )
        
        if ( not self.setCmtconfig( cmtconfig ) ):
            raise DozerError("cannot construct DozerSchema, parameter cmtconfig='%s' not valid" % cmtconfig )

        if ( not self.setProject( project ) ):
            raise DozerError("cannot construct DozerSchema, parameter project='%s' not valid" % project )

        if ( not self.setJobId( jobId) ):
            raise DozerError("cannot construct DozerSchema, parameter jobId='%s' not valid" % jobId )

        if ( version ):
            try:
                self.__version = str(int(str(version)))
            except TypeError, value:
                raise DozerError("cannot construct DozerSchema, parameter version='%s' cannot be cast to int" % version)
        else:
            self.__version = "0"

        if ( tag ):
            self.__tag = str(tag) 
        else:
            self.__tag = ""

        self.__channelsByUUID = { } 
        self.__channelsByName = { }

    ## "in" operator
    # @param self "Me, myslef and Irene"
    # @param name channel UUID or name
    def __contains__( self, name ):
        if ( name in self.__channelsByUUID ) : return True
        if ( name in self.__channelsByName ) : return True 
        return False 

    ## has_key 
    # @param self "Me, myself and Irene"
    # @param name key to check (channel name or UUID)
    def has_key( self, name ):
        return self.__contains__( name )

    ## += operator for adding channel
    # @param self "Me, myself and Irene"
    # @param channel DozerChannel instance  
    # @throw DozerError if channel of the same name but different UUID has bben found in schema 
    def __iadd__(self, channel ):
        if ( isinstance(channel, DozerChannel) ):
            uuid = channel.uuid()
            name = channel.name() 
            if ( uuid not in self.__channelsByUUID and 
                 name not in self.__channelsByName ):
                self.__channelsByUUID[ uuid ] = channel 
                self.__channelsByName[ name ] = channel
                self.debug("DozerChannel uuid='%s' name='%s' has been added" % (uuid, name))
            else:
                if ( self.__channelsByName[name] == self.__channelsByUUID[uuid] ):
                    self.__channelsByUUID[ uuid ] = channel 
                    self.__channelsByName[ name ] = channel
                    self.debug( "DozerChannel name='%s' uuid='%s' has been updated" % (name, uuid) )   
                else:
                    msg = "cannot update DozerChannel uuid='%s' name='%s' in schema, name points to diff channel than uuid!"
                    self.error( msg )
                    raise DozerError( msg )
        return self 


    ## -= operator for removing channels 
    # @param self "Me, myself and Irene"
    # @param what DozerChannel instance or channel UUID or channel name 
    def __isub__( self, what ):
        # for UUID
        if ( what in self.__channelsByUUID ):
            uuid = what
            name = self.__channelsByUUID[ uuid ].name()  
            if ( name in self.__channelsByName ):
                del self.__channelsByName[ name ]
            del self.__channelsByUUID[ uuid ]            
            self.debug("DozerChannel uuid='%s' name='%s' has been removed" % ( uuid, name ) )
     
        # for name
        elif ( what in self.__channelsByName ):
            name = what
            uuid = self.__channelsByName[ name ].uuid()
            if ( uuid in self.__channelsByUUID ):
                del self.__channelsByUUID[uuid]
            del self.__channelsByName[name]
            self.debug("DozerChannel uuid='%s' name='%s' has been removed" % ( uuid, name ) )
         
        # for DozerChannel
        elif ( isinstance(what, DozerChannel) ):
            name = what.name()
            uuid = what.uuid()
            if ( ( uuid in self.__channelsByUUID ) and 
                 ( name in self.__channelsByName ) ):
                del self.__channelsByUUID[uuid]
                del self.__channelsByName[name]
                self.debug("DozerChannel uuid='%s' name='%s' has been removed" % (uuid, name ) )
               
            else:
                self.error("cannot remove DozerChannel uuid='%s' name='%s', channel not found in schema!" % ( uuid, 
                                                                                                                name ) )
        else:
            self.error("unsupported type found for parameter 'what' in DozerSchema '-=' operator!")

        return self 

    ## DozerChannel getter using channel name
    # @param self "Me, myself and Irene"
    # @param name DozerChannel name
    def getChannelByName( self, name ):
        if ( name in self.__channelsByName ):
            return self.__channelsByName[name]
        else:
            return None

    ## DozerChannel getter using channel UUID
    # @param self "Me, myself and Irene"
    # @param uuid DozerChannel UUID
    def getChannelByUUID( self, uuid ):
        if ( uuid in self.__channelsByUUID):
            return self.__channelsByUUID[uuid]
        else:
            return None

    ## DozerChannel dictionary getter  
    # @param self "Me, myself and Irene"
    # @return dictionary { DozerChannel.name() : DozerChannel }
    def getChannelsByName( self ):
        return self.__channelsByName 

    ## DozerChannel dictionary getter
    # @param self "Me, myself and Irene"
    # @return dictionary { DozerChannel.uuid() : DozerChannel }    
    def getChannelsByUUID( self ):
        return self.__channelsByUUID

    ## keys getter
    # @param self "Me, myself and Irene"
    # @param byUUID flag to get UUID keys instead name keys
    def keys( self, byUUID = False ):
        if ( byUUID ):
            return self.__channelsByUUID.keys()
        return self.__channelsByName.keys()
            
    ## items getter
    # @param self "Me, myself and Irene"
    # @param byUUID flag to get UUID keys instead name keys
    def items( self, byUUID = False ):
        if ( byUUID ):
            return self.__channelsByUUID.items()
        return self.__channelsByName.items()

    ## iterkeys getter
    # @param self "Me, myself and Irene"
    # @param byUUID flag to get UUID keys instead name keys
    def iterkeys( self, byUUID = False ):
        if ( byUUID ):
            return self.__channelsByUUID.iterkeys()
        return self.__channelsByName.iterkeys()
        
    ## iteritems getter
    # @param self "Me, myself and Irene"
    # @param byUUID flag to get UUID keys instead name keys
    def iteritems( self , byUUID = False ):
        if ( byUUID ):
            return self.__channelsByUUID.iteritems()
        return self.__channelsByName.iteritems()

    ## iterator 
    # @param self "Me, myself and Irene"
    # @param byUUID flag to get iterator from UUID dictionary
    def __iter__( self, byUUID = False ):
        if ( byUUID ):
            return self.__channelsByUUID.__iter__()
        return self.__channelsByName.__iter__()

    ## getitem operator
    # @param self "Me, myself and Irene"
    # @param key channel UUID or name 
    def __getitem__( self, key ):
        if ( key in self.__channelsByName ):
            return self.__channelsByName[key]
        elif ( key in self.__channelsByUUID ):
            return self.__channelsByUUID[key]
        
        msg = "no DozerChannel name or uuid ='%s' in DozerSchema " % ( key )
        raise KeyError( msg )
    
    ## nightly name getter
    # @param self "Me, myself and Irene"
    def nightly( self ):
        return self.__nightly

    ## nightly setter
    # @param self "Me, myself and Irene"
    # @param nightly nightly name
    # @return True/False to mark success
    def setNightly( self, nightly ):
        match = None
        if ( nightly == "*" ):
            match = nightly
        else:
            for key in self.cv.nightlies().keys():

                if ( key != "*" ):
                    nightlyRE = self.cv.nightlies()[key]
                    if ( nightlyRE.match ( nightly ) ):
                        match = key
                        break
        if ( match ):
            self.__nightly = nightly
            self.debug( "nightly has been set to '%s'" % self.__nightly )
            return True
        else:
            self.error( "nighlty not set, not valid nightly name='%s'" % nightly )
            return False

 
    ## install name getter
    # @param self "Me, myself and Irene"
    def install( self ):
        return self.__install

    ## install setter
    # @param self "Me, myself and Irene"
    # @param install install name
    # @return True/False to mark success
    def setInstall( self, install ):
        if ( ( "*" in install ) or ( install in self.cv.installs() )  ):
            self.__install = install
            self.debug("install has been set to '%s'" % self.__install )
            return True
        else:
            self.error("install not set, not valid install name='%s'" % install )
            return False 

    ## cmtconfig name getter
    # @param self "Me, myself and Irene"
    def cmtconfig( self ):
        return self.__cmtconfig

    ## cmtconfig setter
    # @param self "Me, myself and Irene"
    # @param cmtconfig cmtconfig name
    # @return True/False to mark success
    def setCmtconfig( self, cmtconfig ):
        if ( ( "*" in cmtconfig ) or ( cmtconfig in self.cv.cmtconfigs() )  ):
            self.__cmtconfig = cmtconfig
            self.debug("cmtconfig has been set to '%s'" % self.__cmtconfig )
            return True
        else:
            self.error("cmtconfig not set, not valid cmtconfig name='%s'" % cmtconfig )
            return False 


    ## project name getter
    # @param self "Me, myself and Irene"
    def project( self ):
        return self.__project

    ## project setter
    # @param self "Me, myself and Irene"
    # @param project project name
    # @return True/False to mark success
    def setProject( self, project ):
        if ( ( "*" in project ) or ( project in self.cv.projects() )  ):
            self.__project = project
            self.debug("project has been set to '%s'" % self.__project )
            return True
        else:
            self.error("project not set, not valid project name='%s'" % project )
            return False 

    ## jobId getter
    # @param self "Me, myself and Irene"
    def jobId( self ):
        return self.__jobId

    ## jobId setter
    # @param self "Me, myself and Irene"
    # @param jobId valid jobId
    def setJobId( self , jobId ):
        self.__jobId = jobId
        self.debug("jobId has been set to '%s'" % self.__jobId )
        return True

    ## schema version getter
    # @param self "Me, myself and Irene" 
    def version( self ):
        return self.__version
 
    ## CVS tag getter
    # @param self "Me, myself and Irene"
    def tag( self ):
        return self.__tag
            
    ##
    # @param self "Me, myself and Irene"
    # @param tag a tag string
    def setTag( self, tag ):
        self.debug("setting tag to '%s'" % str(tag) )
        self.__tag = str(tag)
        return True

    ## version number setter
    # @param self "Me, myself and Irene"
    # @param version version number (integer or string)
    def setVersion( self, version ):
        version = str(version)
        intVersion = None
        try:
            intVersion = int(version)
        except TypeError, value:
            self.panic("cannot set version to '%s' for DozerSchema name='%s', version isn't an integer" % ( version,
                                                                                                            self.__name ) )
          
            return False
        if ( int(self.__version) > intVersion ):
            self.panic("cannot set version for DozerSchema to '%d', current version='%s' is higher" % ( intVersion,
                                                                                                        self.__version ) )
            return False
        else:
            self.__version = str(intVersion)
            return True
            
    ## resetting DozerChannels
    # @param self "Me, myself and Irene"
    def reset( self ):
        for key in self.__channelsByUUID:
            self.__channelsByUUID[key].reset() 

    ## XML DOM Document builder 
    # @param self "Me, myself and Irene "
    def __DOMXML( self ):

        self.doc = xml.dom.minidom.Document()

        comment = "\nschema for configuration:\n"
        comment += " [1] nightly   = %s\n" % self.__nightly 
        comment += " [2] install   = %s\n" % self.__install
        comment += " [3] cmtconfig = %s\n" % self.__cmtconfig
        comment += " [4] project   = %s\n" % self.__project
        comment += " [5] jobId     = %s\n" % self.__jobId
        commentString = self.doc.createComment(  comment )

        self.doc.appendChild( commentString )

        schemaNode = self.doc.createElement( "schema" )
        schemaNode.setAttribute( "nightly", self.__nightly )
        schemaNode.setAttribute( "install", self.__install )
        schemaNode.setAttribute( "cmtconfig", self.__cmtconfig )
        schemaNode.setAttribute( "project", self.__project )
        schemaNode.setAttribute( "jobId", self.__jobId )
        schemaNode.setAttribute( "version", self.__version )
        schemaNode.setAttribute( "tag", self.__tag )

    

        self.doc.appendChild( schemaNode )

        for uuid in self.__channelsByUUID.keys():
            channel = self.__channelsByUUID[uuid]
            for channelNode in channel.xml().childNodes:
                schemaNode.appendChild( channelNode )


        return self.doc 

    ## XML DOM Document getter
    # @param self "Me, myself and Irene"
    def xml( self ):
        return self.__DOMXML()

    ## string operator
    # @param self "Me, myself and Irene"
    def __str__( self ):
        return self.__DOMXML().toprettyxml(" ", "\n")
                

    ## deserialization from XML String
    # @param cls class reference
    # @param xmlstring string with XML
    # @param log DozerLogger instance 
    # @return DozerSchema instance or None
    @classmethod
    def fromXMLString( cls, xmlstring, log=None ):
        xmldoc = xml.dom.minidom.parseString( xmlstring )
        return DozerSchema.fromXMLDoc( xmldoc, log )

    ## deserialization from XML DOM Document
    # @param cls class reference
    # @param xmldoc XML DOM Document instance
    # @param log DozerLogger instance
    # @return DozerSchema instance or None 
    @classmethod
    def fromXMLDoc( cls, xmldoc, log=None ):
        this = None
        schemaNode = xmldoc.documentElement
        if ( schemaNode.tagName == "schema" ):
            nightly = schemaNode.getAttribute( "nightly" )
            install = schemaNode.getAttribute( "install" )
            cmtconfig = schemaNode.getAttribute( "cmtconfig" )
            project = schemaNode.getAttribute( "project" )
            jobId = schemaNode.getAttribute( "jobId" )
            version = schemaNode.getAttribute( "version" )
            tag = schemaNode.getAttribute( "tag" )
        
            this = DozerSchema( nightly, install, cmtconfig, project, jobId, version, tag, log )
        
            channels = schemaNode.getElementsByTagName("channel")
            for channel in channels:
                this += DozerChannel.fromXMLString( channel.toxml(), log )
        else:
            raise DozerError("cannot deserialize from DOM XML Doc, XML Doc tag name (=<%s>) != <schema>" % schemaNode.tagName )
            
        return this

    ## general getter from path
    # @param self "Me, meself and Irene"
    # @param path path to search 
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
                               
    ## DozerSchema config getter 
    # @param self "Me, myself and Irene"
    # @param asTuple flag to return tuple (nightly, install, cmtconfig, project )
    # @return DozerSchemaConfig instance or tuple (nightly, install, cmtconfig, project )
    def getConfig( self, asTuple=False ):
        if ( asTuple ):
            return ( self.__nightly,
                     self.__install,
                     self.__cmtconfig,
                     self.__project,
                     self.__jobId )
    
        return DozerSchemaConfig( self.__nightly, 
                                  self.__install,
                                  self.__cmtconfig,
                                  self.__project,
                                  self.__jobId )

    ## path chunk getter
    # @param self "Me, myself and Irene"
    def pathChunk( self ):
        return "/".join([ self.__nightly, self.__install, self.__cmtconfig, self.__project, self.__jobId ] )
    
    ## checks if schema describes explicit configuration
    # @param self "Me, myself and Irene" 
    def isSpecific( self ):
        if ( "*" in self.pathChunk() ): return False
        return True

    ## checks if schema describes implicit configuration
    # @param self "Me, myself and Irene" 
    def isGeneral( self ):
        return not self.isSpecific()

    ## checks if schema describes implicit configuration
    # @param self "Me, myself and Irene" 
    def isImplicit( self ):
        return self.isGeneral()

    ##  checks if schema describes explicit configuration
    # @param self "Me, myself and Irene" 
    def isExplicit( self ):
        return self.isSpecific()

##
# @class test_DozerSchema
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief test case for DozerSchema class
#
class test_DozerSchema( unittest.TestCase ):

    ## setup test suite 
    # @param self "Me, myself and Irene"
    def setUp( self ):
        self.__log = DozerLogger("DozerSchema_unittest.log", "test_DozerSchema", toConsole=True)

        globals()["test_DozerSchema"] = self.__log

        self.schema = DozerSchema("14.2.X.Y", "build", "i686-slc4-gcc34-opt", "AtlasProduction", "job_1")
        self.data1 = DozerData("int data name", "int", unit="kB", log=self.__log )
        self.data2 = DozerData("double data name", "double", unit="s", log=self.__log ) 
        self.chan = DozerChannel("data channel", "channel description", log=self.__log )

    ## testing __init__()
    # @param self "Me, myself and Irene"
    def test_01_constructor( self ):
        try:
            self.schema = DozerSchema("14.2.X.Y", "build", "i686-slc4-gcc34-opt", "AtlasProduction", "job_1", log=self.__log)
        except:
            pass

        self.assertEqual( isinstance(self.schema, DozerSchema), True)


    ## testing nightly(), install(), cmtconfig(), project()
    # @param self "me, myself and Irene"
    def test_02_getters( self ):
        self.assertEqual(self.schema.nightly(), "14.2.X.Y" )
        self.assertEqual(self.schema.install(), "build" )
        self.assertEqual(self.schema.cmtconfig(), "i686-slc4-gcc34-opt" )
        self.assertEqual(self.schema.project(), "AtlasProduction" )
        self.assertEqual(self.schema.jobId(), "job_1" )
        self.assertEqual(self.schema.isExplicit(), True )

    ## testing +=, -=, in, keys(), []  
    # @param self "Me, myself and Irene"
    def test_03_operators_DozerChannel( self ):
        try:
            self.chan += self.data1
            self.chan += self.data2
            self.schema += self.chan
        except:
            pass

        print self.schema.items()
        print self.schema.iterkeys()
        print self.schema.iteritems()

        for key in self.schema.keys( byUUID=True ):
            self.__log.getLogger(self).panic( "uuid=%s name=%s" % (key, self.schema[key].name() ) )


        for k,v in self.schema.iteritems( byUUID=True ):
            self.__log.getLogger(self).panic( "uuid=%s name=%s" % (k, v ) )

        print self.schema.items( byUUID = True)
        print self.schema.iterkeys( byUUID = True)
        print self.schema.iteritems( byUUID = True )


        self.assertEqual(isinstance(self.schema, DozerSchema ), True)
        
        self.assertEqual( self.chan.name() in self.schema, True )
        
        self.assertEqual( self.chan.uuid() in self.schema, True )
   
        for key in self.schema:
            self.__log.getLogger(self).info( "uuid=%s name=%s" % (key, self.schema[key].name() ) )
            
        self.schema -= self.chan.name()
        
        try:
            self.schema += self.chan
        except:
            pass

        self.schema -= self.chan.uuid()

        

    ## testing fromXMLString(), fromXMLDoc(), xml()
    # @param self "Me, myself and Irene"
    def test_05_XML_serialisation( self ):
        fromXMLDoc = None
        fromXMLString = None
       
        self.chan += self.data1
        self.chan += self.data2
        self.schema += self.chan
        fromXMLDoc = self.schema.xml()
        fromXMLString = fromXMLDoc.toxml()
        
        fromXMLDoc = DozerSchema.fromXMLDoc( fromXMLDoc, log=self.__log ) 
        fromXMLString = DozerSchema.fromXMLString( fromXMLString, log=self.__log)

        self.assertEqual( fromXMLDoc.xml().toxml(), fromXMLString.xml().toxml() )
        
        self.__log.getLogger(self).info( fromXMLDoc )
        self.__log.getLogger(self).info( fromXMLString )
 
    ## testing setVersion()
    # @param self "Me, myself and Irene"
    def test_06_setters( self ):
        version = int(self.schema.version())
        try:
            self.schema.setVersion(version+1)
        except:
            pass
        
        self.assertEqual( version+1, int(self.schema.version()) )


    ## testing get()
    # @param self "Me, myself and Irene"
    def test_07_get( self ):
        
        self.chan += self.data1
        self.chan += self.data2
        self.schema += self.chan

        sche = self.schema.get()
        self.assertEqual( isinstance(sche, DozerSchema), True)

        sche = self.schema.get("")
        self.assertEqual( isinstance(sche, DozerSchema), True)

        path = self.chan.uuid() + "/"
        
        chan = self.schema.get( path )
        self.assertEqual( isinstance(chan, DozerChannel), True)
        
        path = self.chan.uuid() + "/" + self.data1.name()
        data = self.schema.get(path)
        self.assertEqual( isinstance(data, DozerData), True)
     
        path = self.chan.name() + "/" + self.data2.name()
        data = self.schema.get(path)
        self.assertEqual( isinstance(data, DozerData), True)

        path = self.schema.pathChunk()
        print path

## test suite entry point
if __name__ == "__main__":
    testLoader = unittest.TestLoader()
    suite = testLoader.loadTestsFromTestCase(test_DozerSchema)      
    unittest.TextTestRunner(verbosity=3).run(suite)

