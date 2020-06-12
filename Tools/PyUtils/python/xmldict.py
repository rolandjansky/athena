# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file PyUtils/python/xmldict.py
# @purpose converts an XML file into a python dict, back and forth
# @author http://code.activestate.com/recipes/573463
#         slightly adapted to follow PEP8 conventions

__doc__ = """\
functions to convert an XML file into a python dict, back and forth
"""
__author__ = "Sebastien Binet <binet@cern.ch>"


# hack: LCGCMT had the py-2.5 xml.etree module hidden by mistake.
#       this is to import it, by hook or by crook
def import_etree():
    import xml
    # first try the usual way
    try:
        import xml.etree
        return xml.etree
    except ImportError:
        pass
    # do it by hook or by crook...
    import os, imp
    xml_site_package = os.path.join(os.path.dirname(os.__file__), 'xml')
    m = imp.find_module('etree', [xml_site_package])

    etree = imp.load_module('xml.etree', *m)
    setattr(xml, 'etree', etree)
    return etree

etree = import_etree()
from xml.etree import ElementTree

from xml.sax.saxutils import escape as _xml_escape
from xml.sax.saxutils import unescape as _xml_unescape

## module data ----------------------------------------------------------------
__all__ = [
    'xml2dict',
    'dict2xml',
    ]

## module implementation ------------------------------------------------------
class XmlDictObject(dict):
    def __init__(self, initdict=None):
        if initdict is None:
            initdict = {}
        dict.__init__(self, initdict)
    
    def __getattr__(self, item):
        return self.__getitem__(item)
    
    def __setattr__(self, item, value):
        self.__setitem__(item, value)
    
    ## def __getitem__(self, item):
    ##     o = dict.__getitem__(self, item)
    ##     if isinstance(o, str):
    ##         return _xml_unescape(o)
    ##     return o

    ## def __setitem__(self, item, value):
    ##     if isinstance(value, str):
    ##         value = _xml_unescape(value)
    ##     dict.__setitem__(self, item, value)
        
    def __str__(self):
        if '_text' in self:
            return self['_text']
        else:
            return dict.__str__(self)

    @staticmethod
    def wrap(x):
        if isinstance(x, dict):
            return XmlDictObject ((k, XmlDictObject.wrap(v))
                                  for (k, v) in x.iteritems())
        elif isinstance(x, list):
            return [XmlDictObject.wrap(v) for v in x]
        else:
            return x

    @staticmethod
    def _unwrap(x):
        if isinstance(x, dict):
            return dict ((k, XmlDictObject._unwrap(v))
                         for (k, v) in x.iteritems())
        elif isinstance(x, list):
            return [XmlDictObject._unwrap(v) for v in x]
        else:
            return x
        
    def unwrap(self):
        return XmlDictObject._unwrap(self)

def _dict2xml_recurse(parent, dictitem):
    assert type(dictitem) is not type(list)

    if isinstance(dictitem, dict):
        for (tag, child) in dictitem.iteritems():
            if isinstance(child, str):
                child = _xml_escape(child)
            if str(tag) == '_text':
                parent.text = str(child)
            elif type(child) is type(list):
                for listchild in child:
                    elem = ElementTree.Element(tag)
                    parent.append(elem)
                    _dict2xml_recurse (elem, listchild)
            else:                
                elem = ElementTree.Element(tag)
                parent.append(elem)
                _dict2xml_recurse (elem, child)
    else:
        parent.text = str(dictitem)
    
def dict2xml(xmldict):
    """convert a python dictionary into an XML tree"""
    roottag = xmldict.keys()[0]
    root = ElementTree.Element(roottag)
    _dict2xml_recurse (root, xmldict[roottag])
    return root

def _xml2dict_recurse (node, dictclass):
    nodedict = dictclass()
    
    if len(node.items()) > 0:
        # if we have attributes, set them
        nodedict.update(dict((k, _xml_unescape(v) if isinstance(v, str) else v)
                             for k,v in node.items()))
    
    for child in node:
        # recursively add the element's children
        newitem = _xml2dict_recurse (child, dictclass)
        if isinstance(newitem, str):
            newitem = _xml_unescape(newitem)
        if child.tag in nodedict:
            # found duplicate tag, force a list
            if isinstance(nodedict[child.tag], list):
                # append to existing list
                nodedict[child.tag].append(newitem)
            else:
                # convert to list
                nodedict[child.tag] = [nodedict[child.tag], newitem]
        else:
            # only one, directly set the dictionary
            nodedict[child.tag] = newitem

    if node.text is None: 
        text = ''
    else: 
        text = node.text.strip()
    
    if len(nodedict) > 0:            
        # if we have a dictionary add the text as a dictionary value
        # (if there is any)
        if len(text) > 0:
            nodedict['_text'] = text
    else:
        # if we don't have child nodes or attributes, just set the text
        if node.text: nodedict = node.text.strip()
        else:         nodedict = ""
        
    return nodedict
        
def xml2dict (root, dictclass=XmlDictObject):
    """convert an xml tree into a python dictionary
    """
    return dictclass({root.tag: _xml2dict_recurse (root, dictclass)})
