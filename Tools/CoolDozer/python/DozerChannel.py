#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
##
# @file DozerChannel.py
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com) 
# @date Apr 22, 2008
# @brief Implementation of DozerChannel and test_DozerChannel classes 

import DozerObject
from DozerUtils import *
from DozerData import DozerData
from types import *
import xml.dom.minidom
import os.path
import unittest

##
# @htmlinclude cool_dozer_api.css
# @class DozerChannel
# @brief Container for DozerData objects.
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
#
# <h3>Usage</h3>
# <ul>
# <li> <h4>Piece of data</h4>
#
# A DozerChannel object is a container for storing DozerData objects. 
# It is identified by:
# <ul>
# <li> it's name (this must be unique in higher level DozerSchema object)
# <li> it's UUID (this is used internally and auto-generated during first creation of an object) 
# </ul> 
#
# To construct DozerChannel you also have to specify the channel description, but this one isn't intrepreted by CoolDozer API.
#
# This piece of information is directly mapped to the cool Channel, which in case of CoolDozer API 
# holds a record of type "String64k" with a XML documenty inside (payload key is "xml", see COOL API for details). 
#
#<div><div class="dozerWarn"><div class="dozerWarnLabel"> Beware! </div> 
#<div class="dozerWarnText"> One DozerChannel is able to hold no more than 64kb XML string! Never and ever break this limit!
#</div></div></div>
#
# <li> <h4>Constructing DozerChannel</h4>
#
# The constructor has two necessary parameters - channel name and channel description. The rest, channel UUID and reference to DozerLogger, 
# are optional. Calling the constructor is straightforward, e.g.:
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText"> 
#<b><pre>
#Python 2.5 (r25:51908, Oct 18 2007, 16:04:48) 
#[GCC 3.4.6 20060404 (Red Hat 3.4.6-8)] on linux2
#Type "help", "copyright", "credits" or "license" for more information.
#>>> import DozerChannel
#>>> dozerChan = DozerChannel.DozerChannel("dummy test channel", "dummy test channel description" )
#>>> print dozerChan 
#<?xml version="1.0" ?>
#<channel desc="dummy test channel description" name="dummy test channel" uuid="7803936b-e00d-43e9-ba83-0c691e6bf72b"/>
#>>> dozerChan.name() 
#'dummy test channel'
#>>> dozerChan.desc() 
#'dummy test channel description'
#>>> dozerChan.uuid() 
#'7803936b-e00d-43e9-ba83-0c691e6bf72b'
#</pre></b>
#</div></div></div>
#
# <li> <h4>Working with DozerData instances</h4>
# 
# <ul>
# <li> Adding DozerData is made by calling "+=" operator of form:
# <b><pre> 
# (DozerChannel instance) "+=" (DozerData instance) 
# </pre></b>
# 
# i.e.:
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#>>> import DozerData
#>>> dozerData = DozerData.DozerData("dummy test data", "Int", "counts")
#>>> dozerChan += dozerData
#>>> dozerChan += DozerData.DozerData("another dummy test data", "int", "counts")
#>>> print dozerChan
#<?xml version="1.0" ?>
#<channel desc="dummy test channel description" name="dummy test channel" uuid="7803936b-e00d-43e9-ba83-0c691e6bf72b">
# <data name="another dummy test data" type="Int" unit="counts"/>
# <data name="dummy test data" type="Int" unit="counts"/>
#</channel>
#</pre></b>
#</div></div></div>
#
#<div><div class="dozerWarn"> <div class="dozerWarnLabel"> Beware! </div> 
#<div class="dozerWarnText">
# Adding two DozerData instances with the same name will overwrite the first one! 
#</div></div></div>
#
# <li> Getting back DozerData which has been added to the channel can be done in two ways:
#
# <ul>
# <li> using python dictionary interface
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#>>> if ( "dummy test data" in dozerChan ): print "DozerData name='%s' found!" % dozerChan["dummy test data"].name()
#... 
#DozerData name='dummy test data' found!
#>>> for key in dozerChan: print "DozerChannel data key='%s' DozerData name='%s'" % ( key, dozerChan[key].name() )
#... 
#DozerChannel data key='another dummy test data' DozerData name='another dummy test data'
#DozerChannel data key='dummy test data' DozerData name='dummy test data'
#>>> for k,v in dozerChan.iteritems() : print "key='%s' data name='%s'" % ( k, v.name() )
#... 
#key='another dummy test data' data name='another dummy test data'
#key='dummy test data' data name='dummy test data'
# </pre></b>
#</div></div></div>
#
# <li> using "file system-like" DozerChannel.get 
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#>>> dozerChan.get("./dummy test data").name()
#'dummy test data'
#>>> dozerChan.get("dummy test data").unit()
#'counts'
#</pre></b>
#</div></div></div>
#
# </ul>
#
# <li> Removing DozerData object from channel is made by:
#
# <ul>
# <li> calling "-=" operator of form:
#
# <b><pre>
# (DozerChannel instance) -= (DozerData name) 
# </pre></b>
# or 
# <b><pre>
# (DozerChannel instance) -= (inner DozerData instance)
# </pre></b> 
# e.g.:
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div><div class="dozerCodeText">
#<b><pre>
#>>> dozerChan -= "dummy test data"                                       [1] removing DozerData using DozerData.name
#>>> print dozerChan
#<?xml version="1.0" ?>
#<channel desc="dummy test channel description" name="dummy test channel" uuid="7803936b-e00d-43e9-ba83-0c691e6bf72b">
# <data name="another dummy test data" type="Int" unit="counts"/>
#</channel>
#>>> dozerChan += DozerData.DozerData("dummy test data", "int", "counts") [2] adding this one once again
#>>> print dozerChan
#<?xml version="1.0" ?>
#<channel desc="dummy test channel description" name="dummy test channel" uuid="7803936b-e00d-43e9-ba83-0c691e6bf72b">
# <data name="another dummy test data" type="Int" unit="counts"/>
# <data name="dummy test data" type="Int" unit="counts"/>
#</channel>
#>>> dozerChan -= dozerChan["dummy test data"]                            [3] removing DozerData using its instance
#>>> print dozerChan
#<?xml version="1.0" ?>
#<channel desc="dummy test channel description" name="dummy test channel" uuid="7803936b-e00d-43e9-ba83-0c691e6bf72b">
# <data name="another dummy test data" type="Int" unit="counts"/>
#</channel>
#</pre></b>
#</div></div></div>
#
# <li> using python <tt>del</tt> statement:
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#>>> del dozerChan["another dummy test data"]
#>>> print dozerChan
#<?xml version="1.0" ?>
#<channel desc="dummy test channel description" name="dummy test channel" uuid="7803936b-e00d-43e9-ba83-0c691e6bf72b"/>
#</pre></b> 
#</div></div></div>
#
# 
# </ul>
# </ul>
#
# <li> <h4>DozerChannel XML (de)serialisation</h4>
# 
# (De)serialisation to (from) XML document is done by DozerChannel.xml, DozerChannel.fromXMLDoc and DozerChannel.fromXMLString functions:
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#>>> xmldoc = dozerChan.xml()                                             # [1] serialisation
#>>> xmlString = dozerChan.xml().toxml()
#>>> xmldoc
#<xml.dom.minidom.Document instance at 0x2a9cc75368>
#>>> xmlString
#'<?xml version="1.0" ?>
#<channel desc="dummy test channel description" name="dummy test channel" uuid="7803936b-e00d-43e9-ba83-0c691e6bf72b"/>'
#>>> fromXMLDoc = DozerChannel.DozerChannel.fromXMLDoc( xmldoc )          # [2] deserialisation from XML DOM Document instance
#>>> fromXMLString = DozerChannel.DozerChannel.fromXMLString( xmlString ) # [3] same using XML string
#>>> fromXMLDoc.name()
#'dummy test channel'
#>>> fromXMLString.name()
#'dummy test channel'
#</pre></b> 
#</div></div></div>
#
# <li> <h4>Other (un)needed functions</h4>
#
# Getters (most of them were used in above examples): 
# <ul>
# <li> for channel name <tt>DozerChannel.name</tt>
# <li> for channel description <tt>DozerChannel.desc</tt>
# <li> for channel UUID <tt>DozerChannel.uuid</tt> 
# <li> for channel path chunk <tt>DozerChannel.pathChunk</tt> (used in higher level CoolDozer classes for "file system-like" interface)
# <li> string representation <tt>str(DozerChannel)</tt> or <tt>DozerChannel.__str__</tt>
# </ul>
# </ul>
#
class DozerChannel( DozerObject.DozerObject ):

    ## c'tor
    # @param self "Me, myself and Irene"
    # @param name channel name
    # @param desc channel description
    # @param uuid channel UUID
    # @param log  DozerLogger instance
    # @throw DozerError if "/" characted found in name  
    def __init__( self, name, desc=None, uuid=None, log=None ):

        super( DozerChannel, self ).__init__( log )
        if ( "/" in name ): raise DozerError("cannot set name of channel to '%s' - '/' char is forbidden in name" % name)
            
        self.__name = name 
    
        if ( desc ):
            self.__desc = desc
        else:
            self.__desc = ""

        if ( uuid ):
            self.__uuid = uuid
        else:
            self.__uuid = getUUID()

        self.__i = 0
        self.__data = { }

        self.debug("constructing DozerChannel name='%s' desc='%s' uuid='%s'" % ( self.__name,
                                                                                 self.__desc,
                                                                                 self.__uuid ) )


    ## channel name getter
    # @param self "Me, myself and Irene"        
    def name( self ):
        return self.__name

    ## channel description getter
    # @param self "Me, myself and Irene"
    def desc( self ):
        return self.__desc

    ## uuid getter
    # @param self "Me, myself and Irene"
    def uuid( self ):
        return self.__uuid 

    ## in operator
    # @param self "Me, myself and Irene"
    # @param name DozerData name to check
    def __contains__( self, name ):
        if ( name in self.__data.keys() ): return True
        return False

    ## data keys getter 
    # @param self "Me, myself and Irene"
    def keys( self ):
        return self.__data.keys()

    ## data items getter 
    # @param self "Me, myself and Irene"
    def items( self ):
        return self.__data.items()

    ## iteritems getter
    # @param self "Me, myself and Irene"
    def iteritems( self ):
        return self.__data.iteritems()

    ## iterkeys getter
    # @param self "Me, myself and Irene"
    def iterkeys( self ):
        return self.__data.iterkeys() 

    ## has_key() 
    # @param self "Me, myself and Irene"
    # @param key DozerData name
    def has_key( self, key ):
        if ( self.__data.has_key( key ) ) : return True
        return False

    ## resetting DozerData 
    # @param self "Me, myself and Irene"
    def reset( self ):
        for key in self.__data:
            self.__data[key].reset()

    ## item getter
    # @param self "Me, myself and Irene"
    # @param name DozerData name
    def __getitem__( self, name ):
        if ( name in self.__data ):
            return self.__data[name]
        else:
            raise KeyError("no DozerData name='%s' in DozerChannel name='%s'" % ( str(name), self.__name ) )


    ## "del" operator for DozerData
    # @param self "Me, myself and Irene"
    # @param what DozerData instance name
    def __delitem__( self, what ):
        if ( type( what) in StringTypes ):
            what = str(what)
            
        if ( what in self.__data ):
            self.debug("deleting DozerData name='%s' from DozerChannel name='%s'" % (what, self.__name ) )
            del self.__data[what]
        else:
            msg = "no DozerData name='%s' in DozerChannel name='%s'" % ( name, self.__name )
            raise KeyError( msg )

    ## XML DOM Document builder 
    # @param self "Me, myself and Irene"
    def __DOMXML( self ):
        self.doc = xml.dom.minidom.Document()

        rootNode = self.doc.createElement( "channel" )
        rootNode.setAttribute("uuid", self.__uuid )
        rootNode.setAttribute("name", self.__name )
        rootNode.setAttribute("desc", self.__desc )
        
        self.doc.appendChild( rootNode )
        
        for dataKey in self.__data:
            for dataNode in self.__data[dataKey].xml().childNodes:
                rootNode.appendChild( dataNode )
        
        return self.doc

    ## XML DOM document getter
    # @param self "Me, myself and Irene"
    def xml( self ):
        return self.__DOMXML() 

    ## += DozerData() operator 
    # @param self "Me, myself and Irene"
    # @param data a DozerData instance
    def __iadd__( self, data ):
        if ( isinstance(data, DozerData) ):
            name = data.name() 
            if ( name not in self.__data ):
                self.debug( "adding DozerData name='%s' to channel" % name )
            else:
                self.warn( "replacing DozerData name='%s' in DozerChannel" % name )
            self.__data[name] = data 
        else:
            self.error("cannot add DozerData, unsupported type for parameter 'data'(=%s)" % type(data))
        return self

    ## "-=" from removing DozerData 
    # @param self "Me, myself and Irene"
    # @param what DozerData instance or DozerData name
    # @throws DozerError if what isn't string or DozerData 
    def __isub__( self, what ):
        if ( isinstance(what, DozerData) ):
            what = what.name()
        elif ( type(what) in StringTypes ):
            what = str(what)
        else:
            raise DozerError("wrong parameter 'what' in DozerChannel '-=' operator, not a string or DozerData type")
        if ( what in self.__data ):
            self.debug("removing DozerData name=%s" % what )
            del self.__data[what]
        else:
            self.error("cannot remove DozerData name='%s' from DozerChannel name='%s', cannot find such data in channel" % (what, self.__name ) )
        return self

    ##
    # @param self "Me, myself and Irene"
    # @brief string representation of CoolChanel
    def __str__( self ):
       return self.__DOMXML().toprettyxml(" ", "\n")

    ## len operator
    # @param self "Me, myself and Irene"
    # @return lenght of DozerData dictionary
    def __len__( self ):
        return len(self.__data)


    ## return lenght of XML String representation 
    # @param self "Me, myself and Irene"
    def xmlLength( self ):
        return len( self.__DOMXML().toxml() )

    ## iterator
    # @param self "Me, myself and Irene"
    def __iter__( self ):
        return self.__data.__iter__()

    ## deserialization from XML string
    # @param cls "Truman Show"
    # @param xmlstring string with XML
    # @param log DozerLogger instance 
    # @brief deserialisation from XML string
    @classmethod 
    def fromXMLString( cls, xmlstring , log=None):
        xmldoc = xml.dom.minidom.parseString( xmlstring )
        return cls.fromXMLDoc( xmldoc, log )

    ## deserialization from XML DOM Document
    # @param cls "Truman Show"
    # @param xmldoc XML DOM Document instance
    # @param log a DozerLogger instance 
    # @brief deserialisation from XML DOM document
    @classmethod 
    def fromXMLDoc( cls, xmldoc, log=None ):
        
        channelNode = xmldoc.documentElement 
            
        if ( channelNode.tagName == "channel" ):
            name = channelNode.getAttribute("name")
            desc = channelNode.getAttribute("desc")
            uuid = channelNode.getAttribute("uuid")
            
            this = cls(name, desc, uuid, log)
            
            for data in channelNode.getElementsByTagName("data"):
                this += DozerData.fromXMLString( data.toxml(), log )
            return this
        else:
            raise DozerError("cannot deserialize DozerChannel from XML Doc, document element tag (=<%s> isn't a <channel>" % channelNode.tagName)

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
                self.debug("get searching recursively key=%s path=%s" % (words[0], searchPath ) )
                return self[words[0]].get( searchPath ) 


    ## path chunk getter
    # @param self "Me, myself and Irene"
    # @return path chunk
    def pathChunk( self ):
        return self.__name
 
##
# @class test_DozerChannel
# @author Krzysztof Daniel Ciba (Krzysztof.CIba@NOSPAMgmail.com)
# @date May 23, 2008
# @brief test case for DozerChannel class
class test_DozerChannel( unittest.TestCase ):

    ## setup test suite
    # @param self "Me, myself and Irene" 
    def setUp( self ):
        self.__log = DozerLogger( "DozerChannel_unittest.log", "test_DozerChannel", toConsole=False)
        globals()["blah"] = self.__log
        UUID = "00000000-0000-0000-0000-000000000001"
        self.chan = DozerChannel("testChannel", "test channel description", uuid=UUID, log = self.__log )
        pass

    ## testing __init__()
    # @param self "Me, myself and Irene" 
    def test_01_constructor( self ):
        try:
            self.chan = None
            UUID = "00000000-0000-0000-0000-000000000001"
            self.chan = DozerChannel("testChannel", "test channel description", uuid=UUID,log = self.__log )
        except:
            pass
        
        self.assertEqual( isinstance(self.chan, DozerChannel), True )
    
    ## testing  name(), desc()
    # @param self "Me, myself and Irene" 
    def test_02_getters( self ):
        self.assertEqual( self.chan.name(), "testChannel" )
        self.assertEqual( self.chan.desc(), "test channel description")

    ## testing  += DozerData
    # @param self "Me, myself and Irene" 
    def test_03_iadd_DozerData( self ):
        
        data = DozerData("test int data", "Int", "N_{obs}", log=self.__log)
        try:
            self.chan += data
        except:
            pass
        self.assertEqual( isinstance(self.chan, DozerChannel), True)

    ## testing __contains__()
    # @param self "Me, myself and Irene" 
    def test_04_in_Operator( self ):
        try:
            data = DozerData("test int data", "Int", "N_{obs}", log=self.__log)
            self.chan += data
        except:
            pass
        self.assertEqual( "test int data" in self.chan, True)
        
        self.assertEqual( self.chan.keys() , [u"test int data"] )

        self.assertEqual( self.chan.has_key("test int data"), True)


        for dataKey in self.chan:
            self.__log.getLogger(self).info( self.chan[dataKey] )

    ## testinfg -= DozerData
    # @param self "Me, myself and Irene" 
    def test_05_isub_DozerData( self ):
        
        data = DozerData("test int data", "Int", "N_{obs}", log=self.__log)
        self.chan += data
        self.chan -= "test int data"
        self.assertEqual( self.chan.keys(), [] )
        
    ## testing xml(), fromXMLDoc(), fromXMLString()
    # @param self "Me, myself and Irene" 
    def test_06_XML_serialisation( self ):
        data = DozerData("test int data", "Int", "N_{obs}", log=self.__log)
        self.chan += data
        self.chan["test int data"].setValue( 10 )
        xmldoc = self.chan.xml()
        self.assertEqual( isinstance(xmldoc, xml.dom.minidom.Document ), True)
        self.__log.getLogger(self).info( xmldoc.toxml() )        
        fromXMLDoc = None
        try:
            fromXMLDoc = DozerChannel.fromXMLDoc( xmldoc )
        except:
            pass
        self.assertEqual( isinstance(fromXMLDoc, DozerChannel), True )


        fromXMLString = None 
        try:
            fromXMLString = DozerChannel.fromXMLString( xmldoc.toxml() )
        except:
            pass
        self.assertEqual( isinstance(fromXMLString, DozerChannel), True )
        

        self.assertEqual( fromXMLString.xml().toxml(), fromXMLDoc.xml().toxml() )
        

    ## testing get()
    # @param self "Me, myself and Irene"
    def test_07_get_path( self ):
        data1 = DozerData("test int data", "Int", "N_{obs}", log=self.__log)
        data1.setValue(5)
        data2 = DozerData("test string data", "String", "", log=self.__log)
        data2.setValue("dummy string ! > < & @")

        self.chan += data1
        self.chan += data2

        val = self.chan.get("/test int data").get().value()
        self.assertEqual(val, 5)

        val = self.chan.get("test int data").get().value()
        self.assertEqual(val, 5)

        self.__log.getLogger( self).panic( "don't panic, this is intentional" )
        try:
            val1 = self.chan.get("/a/b/c/")
        except KeyError, value:
            self.__log.getLogger( self).epanic( value )

        val = self.chan.get("./test string data/").get().value()
        self.assertEqual(val,"dummy string ! > < & @" )

## test suite execution
if __name__ == "__main__":
    testLoader = unittest.TestLoader()
    suite = testLoader.loadTestsFromTestCase(test_DozerChannel)      
    unittest.TextTestRunner(verbosity=3).run(suite)

  

