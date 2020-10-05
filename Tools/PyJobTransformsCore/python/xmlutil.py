#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import os
from copy import copy
from xml.sax import saxutils

indent = "   "

def opentag(tag,attribs=None):
    otag = '<' + tag
    if attribs:
        for key,val in attribs.items(): otag += ' %s=%s' % ( key, saxutils.quoteattr( str( val ) ) )
    otag += '>'
    return otag


def closetag(tag):
    return '</%s>' % tag


def simplefield(tag,attribs=None):
    field = '<' + tag
    if attribs:
        for key,val in attribs.items(): field += ' %s=%s' % ( key, saxutils.quoteattr( str( val ) ) )
    field += '/>'
    return field


def onelinefield(tag,value,attribs=None):
    return opentag(tag,attribs) + str(value) + closetag(tag)


def multilinefield(tag,value,attribs=None):
    sep = os.linesep + indent
    return opentag(tag,attribs) + sep + str(value).replace(os.linesep, sep) + os.linesep + closetag(tag)



class XMLNode:
    """Describes an XML node with a name, contents and attributes.
    The contents can be any object convertable to a string,
    or a (nested) XMLNode object or a list of (nested) XMLNode objects."""
    
    def __init__(self,name,contents=None):
        self.__name = name
        self.__contents = None
        if contents is not None: self.setContents(contents)
        self.__attributes = {}


    def __eq__(self,other):
        if not isinstance(other,XMLNode): return False
        return self.__name == other.__name and \
               self.__contents == other.__contents and \
               self.__attributes == other.__attributes
        

    def __ne__(self,other):
        return not self.__eq__(other)



    def __str__(self):
        header = self.__name
        if self.__attributes:
            header += '('
            for n,v in self.__attributes.items():
                header += '%s=%s, ' % (n,v)
            header = header[:-2] + ')'

        toString = header
        contents = self.__contents
        if not contents:
            toString += '='
        else:
            contType = type(contents).__name__
            indent = '  '
            if contType == 'list':
                toString += ':'
                for c in contents:
                    contStr = str(c).replace(os.linesep,os.linesep + indent).strip()
                    toString += os.linesep + indent + contStr
            elif isinstance(contents,XMLNode):
                toString += ':' + os.linesep + indent + str(contents)
            else:
                toString += '='
                contents = str(contents)
                nLines = contents.count(os.linesep) + 1
                if nLines > 1:
                    # make nice indentation
                    indent = ' '*len(toString)
                    toString += str(contents).replace(os.linesep,os.linesep + indent).strip()
                else:
                    toString += contents

        return toString

    def getContents(self):
        """ Produces a dictionary with the contents
        """
        contType = type(self.__contents).__name__
        returnedcont={}
        if contType=='list':
            for i in self.__contents: 
                if isinstance(i,XMLNode): 
                    returnedcont.update({i.__name:i.__contents})
        else:
            returnedcont={self.__name:self.__contents}
        return returnedcont  
                

    def setName(self,name):
        self.__name = name
        return self
    

    def setContents(self,contents):
        contType = type(contents).__name__
        if isinstance(contents,XMLNode):
            self.__contents = contents
#            print ("%s: Setting contents %s" % (self.name(),contents.__class__.__name__))
        elif contType == 'list':
#            print ("%s: Setting contents list" % (self.name()))
            self.__contents = contents
        else:
#            print ("%s: Setting contents %s" % (self.name(),type(contents).__name__))
            self.__contents = str(contents)

        return self
    

    def addContents(self,contents):
        conttype = type(self.__contents).__name__
        if conttype == 'instance': conttype = self.__contents.__class__.__name__
        addtype = type(contents).__name__
        if addtype == 'instance': addtype = contents.__class__.__name__
#        print ("%s: Adding contents %s to %s" % (self.name(),addtype,conttype))
        if self.__contents is None:
            self.setContents(contents)
        elif conttype == 'list':
            if addtype == 'list':
                self.__contents += contents
            elif isinstance(contents,XMLNode):
                self.__contents.append(contents)
            else:
                raise AttributeError('XMLNode: can not add a %s to a %s' % (addtype,conttype))
        elif conttype == 'str':
            if isinstance(contents,XMLNode):
                raise AttributeError('XMLNode: can not add a %s to a %s' % (addtype,conttype))
            else:
                self.__contents += str(contents)
        elif isinstance(self.__contents,XMLNode):
            if isinstance(contents,XMLNode):
                self.__contents = [ copy(self.__contents), contents ]
            else:
                raise AttributeError('XMLNode: can not add a %s to a %s' % (addtype,conttype))
        else:
            raise AttributeError('XMLNode: can not add a %s to a %s' % (addtype,conttype))
            
        return self
    
                 
    def setAttribute(self,name,value):
        self.__attributes[name] = value
        return self
    

    def setAttributes(self,**kwargs):
        self.__attributes.update( kwargs )
        return self


    def name(self):
        return self.__name


    def contents(self):
        return self.__contents


    def attributes(self):
        return self.__attributes
 

    def getAttribute(self,name,default=None):
        return self.__attributes.get(name,default)


    def hasAttribute(self,name):
        return name in self.__attributes


    def find(self,name,depth=-1,**attribs):
        """Return a list of all nested XMLNode members with name <name>.
        It searches <depth> levels deep, where
        depth=0 only checks the name of the current object
        depth=1 checks only the direct children of this object.
        depth=-1 checks too full depth.
        Returns empty list if no matches are found."""
        if depth == 0:
            if name == self.__name:
                for name,value in attribs:
                    myvalue = self._attribute.get(name,None)
                    if myvalue is None or myvalue != value:
                        return []
                return [ self ]
            else:
                return []
        else:
            contents = self.__contents
            if contents is None:
                return []
            elif type(contents).__name__ == 'list':
                all = []
                for c in contents:
                    found = c.find(name,depth-1)
                    if found: all += found
                return all
            elif isinstance(contents,XMLNode):
                if name == contents.name():
                    return [ contents ]
                else:
                    return []

        return []
    

    def getXML(self):
        name = self.__name
        contents = self.__contents
        attrib = self.__attributes
        if contents is None:
            return simplefield(name,attrib)
        elif type(contents).__name__ == 'list':
            fields = []
            for c in contents:
                if isinstance(c,XMLNode):
                    fields.append( c.getXML() )
                else:
                    fields.append( saxutils.quoteattr( str(c) ) )
            xmlStr = os.linesep.join(fields)
            return multilinefield(name,xmlStr,attrib)
        elif isinstance(contents,XMLNode):
            return multilinefield(name,contents.getXML(),attrib)
        else:
            contents = saxutils.quoteattr( str(contents) )
            nLines = contents.count(os.linesep) + 1
            if nLines > 1:
                return multilinefield(name,contents,attrib)
            else:
                return onelinefield(name,contents,attrib)

    
if __name__ == '__main__':
    v = ['from xxx import <>a;d("dsf' 'sdf").Time=0']
    y=XMLNode("FileCatalog")
    y.addContents( XMLNode("META").setAttributes(name="var1", type='string') )
    y.addContents( XMLNode("file1").setAttributes(att_name="apples",att_value=v ) )
    print (y.getXML())
    
