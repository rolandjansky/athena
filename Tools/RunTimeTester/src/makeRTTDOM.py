# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from  xml.dom.minidom import getDOMImplementation

def makeRTTDOM(docname):        
        impl            = getDOMImplementation()
        docType         = impl.createDocumentType(docname, '', '')
        namespace       = None
        document        = impl.createDocument(namespace, docname, docType)
        
        return document
