# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
A class to provide an aml file summary of an RTT run
"""

import xml.dom
from XMLTools    import getText
from xml.dom.ext import PrettyPrint
from StringIO    import StringIO
from Evaluate    import Evaluate

import os.path, types, time


from MethodTimer import MethodTimer  
  
class BaseXMLConverter(MethodTimer):
    def __init__(self, elementCreator, textNodeCreator, logger):
        self.elementCreator  = elementCreator
        self.textNodeCreator = textNodeCreator
        self.callBack        = None
        self.node            = None
        self.logger          = logger
        MethodTimer.__init__(self)
  
    def addTextNode(self, parentNode, item):
        try:
            tn = self.textNodeCreator(str(item[1]).strip("'").strip())
        except xml.dom.InvalidCharacterErr:
            tn = self.textNodeCreator('invalid text')
            
        n  = self.elementCreator(str(item[0]).strip("'").strip())
        
        n.appendChild(tn)

        parentNode.appendChild(n)

    def __str__(self):
        temp = StringIO() 
        PrettyPrint(self.node, temp)
        str = temp.getvalue()
        temp.close()
        return str
        
    def setSummaryCallBack(self, newCallBack):
        self.logger.debug('setting a callback of type %s' % (
            newCallBack.__class__.__name__))
        self.callBack = newCallBack
