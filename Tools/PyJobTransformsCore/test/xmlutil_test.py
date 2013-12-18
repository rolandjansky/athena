#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import myunittest
import os

from PyJobTransformsCore.xmlutil import *

class TestXMLNode(myunittest.TestCase):

    def setUp(self):
        self.node1 = XMLNode("TestName1")
        self.node2 = XMLNode("TestName2")
        self.toPrint = [ self.node1, self.node2 ]


    def assertNames(self):
        self.assertEqual(self.node1.name(), "TestName1")
        self.assertEqual(self.node2.name(), "TestName2")
        

    def assertContents( self, cont1, cont2 ):
        self.assertEqual( self.node1.contents(), cont1 )
        self.assertEqual( self.node2.contents(), cont2 )


    def assertAttributes( self, attr1, attr2 ):
        self.assertEqual( self.node1.attributes(), attr1 )
        self.assertEqual( self.node2.attributes(), attr2 )
        


    def printXML(self):
        print ""
        for node in self.toPrint:
            print node.getXML()

        
    def testName(self):
        """only a name"""
        self.printXML()
        self.assertNames()
        self.assertContents(None,None)
        self.assertAttributes({},{})


    def testNameAndContents(self):
        """name and contents"""
        contents1 = 'contents1'
        contents2 = 'contents2'
        self.node1.setContents(contents1)
        self.node2.setContents(contents2)
        self.assertNames()
        self.assertContents(contents1,contents2)
        self.assertAttributes({},{})


    def testNameAndAttribs(self):
        """name and attributes"""
        attribs1 = { 'name1':'value1' }
        attribs2 = { 'name2':'value2' }
        self.node1.setAttributes(**attribs1)
        self.node2.setAttributes(**attribs2)
        self.printXML()
        self.assertNames()
        self.assertContents(None,None)
        self.assertAttributes(attribs1,attribs2)



        
if __name__ == "__main__":
    myunittest.main()


