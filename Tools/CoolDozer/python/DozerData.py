#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
##
# @file DozerData.py
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @date Apr, 21st, 2008
# @brief Implementation of DozerData and test_DozerData classes. 

from types import *
import DozerObject
from DozerUtils import DozerError, DozerLogger
import xml.dom.minidom 
import os.path
import unittest

## 
# @htmlinclude cool_dozer_api.css
# @class DozerData
# @author Krzysztof Daniel Ciba (Krzysztof.Ciab@NOSPAgmail.com)
# @brief Class DozerData represents the quanta of data entity that could be stored inside DozerChannel.
#
# <h3>Usage</h3>
# <ul>
# <li> <h4>Piece of data</h4>
#
# The DozerData instance is the smallest possible data piece that can be stored inside CoolDozer database. 
# It is identified by:
# <ul>
# <li> unique name (the name should be unique inside one DozerChannel instance, you can't store two DozerData objects
#  with the same in one DozerChannel)
# <li> type
# <li> unit
# <li> MIME string for blob
# </ul>
#
# The data type should be one of:
# <ul>
# <li> <tt>"Bool"</tt> == python True or False 
# <li> <tt>"Int"</tt> == python int, <tt>"Long"</tt> == python long 
# <li> <tt>"Float"</tt>, <tt>"Double"</tt> == python FP number (C++ double) 
# <li> <tt>"String"</tt>, <tt>"String255"</tt>, <tt>"String4k"</tt>, <tt>"String64k"</tt>, <tt>"Clob"</tt> == python string 
# <li> <tt>"Blob"</tt>, <tt>"Blob64k"</tt> == whatever you want in a python string representation
# </ul>
# 
# The unit is required for DozerData with <tt>"Int"</tt>, <tt>"Long"</tt>, <tt>"Float"</tt> and <tt>"Double"</tt> types and 
# can be omitted otherwise. 
#
# The MIME is required for DozerData with <tt>"Blob"</tt> and <tt>"Blob64k"</tt> types and ignored for all the others. 
#
#<div><div class="dozerWarn"> <div class="dozerWarnLabel"> Beware! </div> 
#<div class="dozerWarnText"> 
#Try to store much less than 64kb in one DozerData instance!
#</div></div></div>
#
# <li> <h4>Constructing and XML (de)serialisation</h4>
# 
# Follow the example python session:
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div> <div class="dozerCodeText"> 
#<b><pre>
#[lxplus236] ~/scratch2/CoolDozerClient/python % python
#Python 2.5 (r25:51908, Oct 18 2007, 16:04:48) 
#[GCC 3.4.6 20060404 (Red Hat 3.4.6-8)] on linux2
#Type "help", "copyright", "credits" or "license" for more information.
#>>> import DozerData                                                     #[1] importing module
#>>> dummyData = DozerData.DozerData("dummy data", "int", "dummy unit")   #[2] constructing DozerData instance 
#>>> dummyData                                                            #[3] listing properties
#<DozerData.DozerData object at 0x2a9d205190>                             
#>>> dummyData.name()                                                     #[3.1] it's name
#'dummy data'
#>>> dummyData.type()                                                     #[3.2] it's type
#'Int'
#>>> dummyData.unit()                                                     #[3.3] it's unit, if any
#'dummy unit'
#>>> dummyData.mime()                                                     #[3.4] it's MIME type if any
#''
#>>> print dummyData                                                      #[4.1] calling DozerData::__str__ 
#<?xml version="1.0" ?>
#<data name="dummy data" type="Int" unit="dummy unit"/>
#>>> xmldoc = dummyData.xml()                                             #[4.2] calling DozerData::xml 
#>>> xmldoc
#<xml.dom.minidom.Document instance at 0x2a9d203a70>
#>>> xmlstring = xmldoc.toxml()
#>>> xmlstring
#'<?xml version="1.0" ?>\n<data name="dummy data" type="Int" unit="dummy unit"/>'
#>>> copyOfDummyData = DozerData.DozerData.fromXMLDoc( xmldoc )           #[5.1] deserialisation from XML DOM doc
#>>> copyOfDummyData
#<DozerData.DozerData object at 0x2a9d205c10>
#>>> print copyOfDummyData
#<?xml version="1.0" ?>
#<data name="dummy data" type="Int" unit="dummy unit"/>
#>>> yetAnotherCopy = DozerData.DozerData.fromXMLString( xmlstring )      #[5.2] deserialisation from XML string
#>>> yetAnotherCopy
#<DozerData.DozerData object at 0x2a9d205ed0>
#>>> print yetAnotherCopy
#<?xml version="1.0" ?>
#<data name="dummy data" type="Int" unit="dummy unit"/>
#</pre></b>
#</div></div></div>
# 
# First we are importing DozerData module ([1]), then we are creating <b>dummyData</b> instance of 
# DozerData ([2]). After that we are getting it's name ([3.1]), type ([3.2]), unit ([3.3]) 
# and MIME type ([3.4]).
#
# The serialisation could be done in two ways:
# <ul>
# <li> by calling DozerData::__str__ operator (as in [4.1]), when string with XML representation is returned 
# <li> by calling DozerData::xml method which returns Python XML DOM document instance ([4.2])
# </ul>
# 
# For deserialisation you can use DozerData::fromXMLDoc ([5.1]) and DozerData::fromXMLString methods ([5.2]).
#
#<div><div class="dozerInfo"><div class="dozerInfoLabel">Notice</div><div class="dozerInfoText">
# Serialisation and deserialisation is handled automaticly by higher level CoolDozer 
# classes, you are always working with complete DozerData.DozerData instances, not with their XML 
# representations. 
#</div></div></div>  
#
# <li> <h4>Setting and getting value</h4>
#  
# Setting the value of DozerData instance is as easy as call to DozerData.setValue method, e.g.:
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div> <div class="dozerCodeText"> 
#<b><pre>
#>>> dummyData.setValue( 10 )
#True
#</pre></b> 
#</div></div></div>
#
# The value type is checked against declared DozerData instance type and it is sometimes converted, if the runtime type 
# follows <b>the non-loose precision</b> type casting convntions, e.g.: 
# <b><pre> "Int" <= "Long" <= "Double" </pre></b>
# hence:
# <ul>
# <li> if declared data type is <tt>"Double"</tt>, you can specify the value as python int, long or double (1.0, 1L, 1)
# <li> if declared data type is <tt>"Long"</tt>, you can specify the value as python int or long (1 or 1L)
# <li> if declared data type is <tt>"Int"</tt>, you can only set the value as python int (1) 
# </ul>
# 
# For CLOBs and BLOBs you have to use just their string representations...  
#
#<div><div class="dozerWarn"> <div class="dozerWarnLabel"> Beware! </div> <div class="dozerWarnText">
# Remember to check the returned status from DozerData.setValue! If it's equal to python False, the value hasn't been set!
#</div></div></div>  
# 
# Getting DozerData value back from the object is done by calling DozerData.value method:
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div> <div class="dozerCodeText"> 
#<b><pre>
#>>> dummyData.value()
#10
#</pre></b>
#</div></div></div>
#
# </ul>
#
class DozerData( DozerObject.DozerObject ):

      knownTypes = { 
            "Bool"      : "Bool",
            "Int"       : "Int", 
            "Long"      : "Long", 
            "Float"     : "Double", 
            "Double"    : "Double", 
            "String"    : "String", 
            "String255" : "String",
            "String4k"  : "String",
            "String64k" : "String",
            "Clob"      : "String",
            "Blob64k"   : "Blob",
            "Blob"      : "Blob" } 
      

      ## c'tor
      # @param self  "Me, myself and Irene"
      # @param name  value for name attribute
      # @param type  value for type attribute
      # @param unit  optional value for unit attribute
      # @param mime  optional value for mime attribute
      # @param log   optional DozerLogger instance  
      def __init__( self, name, type, unit=None, mime=None, log=None ):
            
            super( DozerData, self ).__init__( self  )
            
            self.__name = ""
            self.__type = ""
            self.__unit = ""
            self.__mime = ""
            self.__value = ""

            self.debug("constructing DozerData name='%s' type='%s' unit='%s' mime='%s'" % ( str(name), 
                                                                                            str(type),
                                                                                            str(unit),
                                                                                            str(mime) ) )
            self.__name = str( name )

            aType = str( type ).capitalize()
            if ( aType in self.knownTypes ):
                  self.__type = self.knownTypes[aType]
            else:
                  raise DozerError("not known or unsupported type=%s for DozerData name=%s" %
                                   ( self.__name, aType ) )
            # set MIME for Blobs
            if ( "Blob" in aType ):
                  if ( not mime ): 
                        raise DozerError( "MIME not set for DozerData name=%s, but type is Blob" % self.__name )
                  else:
                        self.__mime = str(mime) 

            # set unit 
            if ( unit ):
                  self.__unit = str(unit)

                  
      ## CDATA getter
      # @param nodeList list of XML DOM Elements 
      @staticmethod 
      def __getCData( nodeList ):
            cdata = ""
            for node in nodeList:
                  if node.nodeType == node.TEXT_NODE:
                        cdata += node.data
            return cdata

      ## value clean up
      # @param self "Me, myself and Irene"
      def reset( self ):
            self.__value = None
      
      ##
      # @param cls class
      # @param xmlstring string representation of XML document
      # @param log DozerLogger instance
      # @brief deserilization from XML string
      @classmethod
      def fromXMLString( cls,  xmlstring, log=None ):
            xmldoc = xml.dom.minidom.parseString( xmlstring )
            return cls.fromXMLDoc( xmldoc, log )

      ## 
      # @param cls class
      # @param xmldoc XML DOM document
      # @param log DozerLogger instance
      # @brief deserialization from XML DOM document
      @classmethod
      def fromXMLDoc( cls, xmldoc, log=None ):
            
            dataNode = xmldoc.getElementsByTagName("data")[0]
            if ( dataNode ):
                  name = dataNode.getAttribute("name")
                  type = dataNode.getAttribute("type")
                  unit = dataNode.getAttribute("unit")
                  mime = dataNode.getAttribute("mime")

                  this = cls( name, type, unit, mime , log)

                  value = DozerData.__getCData( dataNode.childNodes )
                  
                  if ( value != "" ):
                        if ( type == "Bool" ):
                              if ( value == "True"): value = True
                              else: value = False

                        elif ( type == "Int" ):
                              value = int( value )

                        elif ( type == "Long" ):
                              value = long( value )

                        elif ( type == "Double" ):
                              value = float( value )
            
                        this.setValue( value )

                  return this
              

      ## value setter
      # @param self "Me, myself and Irene"
      # @param value a value to set
      # @brief value setter      
      # @return True in value was set, False if type mismatch 
      def setValue( self, value ):
            
            valType = type( value )
            typeOK = False 

            # boolean type
            if ( ( valType is BooleanType ) and self.__type == "Bool" ):
                  typeOK = True

            # int type
            elif ( ( valType is IntType ) and self.__type == "Int" ):
                  typeOK = True

            # long type
            elif ( ( valType in ( IntType, LongType ) ) and self.__type == "Long" ):
                  typeOK = True
                  
            # float (Float or Double)
            elif ( ( valType in ( FloatType, IntType, LongType ) ) and self.__type in ( "Float", "Double" ) ):
                  typeOK = True  

            # strings
            elif ( ( valType in StringTypes) and ( "String" in self.__type ) ):
                  typeOK = True

            # blob???
            elif ( "Blob" in self.__type ):
                  typeOK = True

            if ( typeOK ):
                  self.__value = value 
            else:
                  msg = "value not set for DozerData name=%s, mismatch type? DozerData type=%s value type=%s" % ( self.__name, 
                                                                                                                  self.__type, 
                                                                                                                  str(valType) ) 
                  self.panic( msg )
                  

            return typeOK
    
      ## value getter
      # @param self "Me, myself and Irene"
      # @brief value getter
      def value( self ):
            return self.__value 

      ## name getter
      # @param self "Me, myself and Irene"
      # @brief name getter
      def name( self ):
            return self.__name

      ## unit getter
      # @param self "Me, myself and Irene"
      # @brief unit getter
      def unit( self ):
            return self.__unit 

      ## mime getter
      # @param self "Me, myself and Irene"
      # @brief MIME getter
      def mime( self ):
            return self.__mime 

      ## type getter
      # @param self "Me, myself and Irene"
      # @brief type getter 
      def type( self ):
            return self.__type 
      

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

        
      ## XML DOM document builder
      # @param self "Me, myself and Irene"
      # @brief XML DOM document builder
      def __DOMXML( self ):

            self.doc = xml.dom.minidom.Document()

            dataNode = self.doc.createElement("data")
            
            dataNode.setAttribute( "name", self.__name )
            dataNode.setAttribute( "type", self.__type )
            
            if ( self.__unit != "" ):
                  dataNode.setAttribute( "unit", self.__unit )
                  
            if ( self.__mime != "" ):
                  dataNode.setAttribute( "mime", self.__mime )
                        
            if ( self.__value not in ( "", None ) ):
                  dataValue = self.doc.createTextNode( str( self.__value ) )
                  dataNode.appendChild( dataValue )

            self.doc.appendChild( dataNode )

            return self.doc

      ## XML DOM document getter  
      # @param self "Me, myself and Irene"
      # @brief XML DOM representation of DozerData object
      def xml( self ):
            return self.__DOMXML()

      ##
      # @param self "Me, myself and Irene"
      # @brief string representation of DozerData object
      def __str__( self ):
            return self.__DOMXML().toprettyxml(" ", "\n")


##
# @class test_DozerData
# @brief Test case foe DozerData 
class test_DozerData( unittest.TestCase ):

      ## setup test suite
      # @param self "Me, myself and Irene"
      def setUp( self ):

            self.__log = DozerLogger("DozerData_unittest.log", toConsole=True)
            globals()["mylog"] = self.__log

            print globals()["mylog"]

            self.data1 = None
            self.data2 = None
            self.data3 = None
            self.data4 = None
            self.data5 = None
            self.data6 = None
            self.data7 = None
            self.data8 = None

            self.data1 = DozerData( "test_Bool", "Bool", "") 
            self.data2 = DozerData( "test_Int", "Int", "1/N" ) 
            self.data3 = DozerData( "test_Long", "Long" ) 
            self.data4 = DozerData( "test_Float", "Float" ) 
            self.data5 = DozerData( "test_Double", "Double", "GeV" ) 
            self.data6 = DozerData( "test_String", "String255") 
            self.data7 = DozerData( "test_Clob", "Clob") 
            self.data8 = DozerData( "test_Blob", "Blob", mime = "image/gif" ) 
            
      ## testing __init__()
      # @param self "Me, myself and Irene"
      def test_01_constructor( self ):
            self.data1 = DozerData( "test_Bool", "Bool", "" ) 
            self.data2 = DozerData( "test_Int", "Int", "1/N", log = self.__log ) 
            self.data3 = DozerData( "test_Long", "Long", log = self.__log ) 
            self.data4 = DozerData( "test_Float", "Float" , log = self.__log ) 
            self.data5 = DozerData( "test_Double", "Double", "GeV", log = self.__log ) 
            self.data6 = DozerData( "test_String", "String255", log = self.__log) 
            self.data7 = DozerData( "test_Clob", "Clob", log = self.__log) 
            self.data8 = DozerData( "test_Blob", "Blob", mime = "image/gif", log =self.__log ) 
        
            self.assertEqual( isinstance(self.data1, DozerData), True )
            self.assertEqual( isinstance(self.data2, DozerData), True )
            self.assertEqual( isinstance(self.data3, DozerData), True )
            self.assertEqual( isinstance(self.data4, DozerData), True )
            self.assertEqual( isinstance(self.data5, DozerData), True )
            self.assertEqual( isinstance(self.data6, DozerData), True )
            self.assertEqual( isinstance(self.data7, DozerData), True )
            self.assertEqual( isinstance(self.data8, DozerData), True )

      ## testing xml()
      # @param self "Me, myself and Irene"
      def test_02_XML_serialisation( self ):
            self.assertEqual( isinstance(self.data1.xml(), type(xml.dom.minidom.Document()) ), True )
            self.assertEqual( isinstance(self.data2.xml(), type(xml.dom.minidom.Document()) ), True )
            self.assertEqual( isinstance(self.data3.xml(), type(xml.dom.minidom.Document()) ), True )
            self.assertEqual( isinstance(self.data4.xml(), type(xml.dom.minidom.Document()) ), True )
            self.assertEqual( isinstance(self.data5.xml(), type(xml.dom.minidom.Document()) ), True )
            self.assertEqual( isinstance(self.data6.xml(), type(xml.dom.minidom.Document()) ), True )
            self.assertEqual( isinstance(self.data7.xml(), type(xml.dom.minidom.Document()) ), True )
            self.assertEqual( isinstance(self.data8.xml(), type(xml.dom.minidom.Document()) ), True )

    
      ## testing fromXMLDoc(), fromXMLString()
      # @param self "Me, myself and Irene"
      def test_03_XML_deserialisation( self ):    
            self.data1 = DozerData.fromXMLDoc( self.data1.xml(), self.__log )
            self.data2 = DozerData.fromXMLDoc( self.data2.xml(), self.__log )
            self.data3 = DozerData.fromXMLDoc( self.data3.xml(), self.__log )
            self.data4 = DozerData.fromXMLDoc( self.data4.xml(), self.__log )
            self.data5 = DozerData.fromXMLDoc( self.data5.xml(), self.__log )
            self.data6 = DozerData.fromXMLDoc( self.data6.xml(), self.__log )
            self.data7 = DozerData.fromXMLDoc( self.data7.xml(), self.__log )
            self.data8 = DozerData.fromXMLDoc( self.data8.xml(), self.__log )

            self.assertEqual( isinstance(self.data1, DozerData), True )
            self.assertEqual( isinstance(self.data2, DozerData), True )
            self.assertEqual( isinstance(self.data3, DozerData), True )
            self.assertEqual( isinstance(self.data4, DozerData), True )
            self.assertEqual( isinstance(self.data5, DozerData), True )
            self.assertEqual( isinstance(self.data6, DozerData), True )
            self.assertEqual( isinstance(self.data7, DozerData), True )
            self.assertEqual( isinstance(self.data8, DozerData), True )
                 
            self.data1 = DozerData.fromXMLString( self.data1.xml().toxml(), self.__log )
            self.data2 = DozerData.fromXMLString( self.data2.xml().toxml(), self.__log )
            self.data3 = DozerData.fromXMLString( self.data3.xml().toxml(), self.__log )
            self.data4 = DozerData.fromXMLString( self.data4.xml().toxml(), self.__log )
            self.data5 = DozerData.fromXMLString( self.data5.xml().toxml(), self.__log )
            self.data6 = DozerData.fromXMLString( self.data6.xml().toxml(), self.__log )
            self.data7 = DozerData.fromXMLString( self.data7.xml().toxml(), self.__log )
            self.data8 = DozerData.fromXMLString( self.data8.xml().toxml(), self.__log )
                  
            self.assertEqual( isinstance(self.data1, DozerData), True )
            self.assertEqual( isinstance(self.data2, DozerData), True )
            self.assertEqual( isinstance(self.data3, DozerData), True )
            self.assertEqual( isinstance(self.data4, DozerData), True )
            self.assertEqual( isinstance(self.data5, DozerData), True )
            self.assertEqual( isinstance(self.data6, DozerData), True )
            self.assertEqual( isinstance(self.data7, DozerData), True )
            self.assertEqual( isinstance(self.data8, DozerData), True )
                 
      ## testing setValue()
      # @param self "Me, myself and Irene"
      def test_05_setValue( self ):
            self.assertEqual( self.data1.setValue(True), True )
            self.assertEqual( self.data2.setValue(1), True )
            self.assertEqual( self.data3.setValue(1L), True )
            self.assertEqual( self.data4.setValue(1.0), True )
            self.assertEqual( self.data5.setValue(1.0), True )
            self.assertEqual( self.data6.setValue("test"), True )
            self.assertEqual( self.data7.setValue("test"), True )
            self.assertEqual( self.data8.setValue("\x030\x031\x032"), True )

      ## tesinf value()
      # @param self "Me, myself and Irene"
      def test_06_value( self ):
            val1 = self.data1.value()
            self.assertNotEqual( val1, True )
            val2 = self.data2.value()
            self.assertNotEqual( val2, 1 )
            val3 = self.data3.value()
            self.assertNotEqual( val3, 1 )
            val4 = self.data4.value()
            self.assertNotEqual( val4, 1.0 )
            val5 = self.data5.value()
            self.assertNotEqual( val5, 1.0 )
            val6 = self.data6.value()
            self.assertNotEqual( val6, "test" )
            val7 = self.data7.value()
            self.assertNotEqual( val7, "test" )
            val8 = self.data8.value()
            self.assertNotEqual( val8, "\x030\x031\x032" )
                  
            val1 = self.data1.get()
            self.assertNotEqual( val1, True )
            val2 = self.data2.get()
            self.assertNotEqual( val2, 1 )
            val3 = self.data3.get()
            self.assertNotEqual( val3, 1 )
            val4 = self.data4.get()
            self.assertNotEqual( val4, 1.0 )
            val5 = self.data5.get()
            self.assertNotEqual( val5, 1.0 )
            val6 = self.data6.get()
            self.assertNotEqual( val6, "test" )
            val7 = self.data7.get()
            self.assertNotEqual( val7, "test" )
            val8 = self.data8.get()
            self.assertNotEqual( val8, "\x030\x031\x032" )

      ## tesing get()
      # @param self "Me, myself and Irene"
      def test_07_get( self ):
            self.assertEqual( self.data1.get(),    self.data1 )
            self.assertEqual( self.data1.get("."), self.data1 )
            self.assertEqual( self.data1.get(""),  self.data1 )
            self.assertEqual( self.data1.get("/"), self.data1 )


## test suite execution 
if __name__ == "__main__":
      testLoader = unittest.TestLoader()
      suite = testLoader.loadTestsFromTestCase(test_DozerData)      
      unittest.TextTestRunner(verbosity=3).run(suite)
    

