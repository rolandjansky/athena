# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


import xml.etree.cElementTree as ET
from PyUtils.Decorators import memoize

class TrigXMLElement:
    def __init__(self,element):
        self.element = element
        self.tag = element.tag
        self.items = element.items()
        self.children = element.getchildren()
        self.readchildren()
    def __str__(self):
        s = "<%s%s>" % (" ".join([self.tag] + ['%s="%s"' % x for x in self.items]), "/" if len(self.children)==0 else "")
        return s
    def __repr__(self):
        return self.tag

    def __contains__(self,k):
        return k in dict(self.items)

    def __getitem__(self,k):
        return dict(self.items)[k]
    
    def strippedText(self):
        return self.element.text.strip()

    def readchildren(self):
        self.childtags = []
        self._childtagdict = {}
        for c in self.children:
            self._childtagdict.setdefault(c.tag,[]).append(TrigXMLElement(c))
            if c.tag not in self.childtags: self.childtags += [c.tag]
        for t in self.childtags:
            self.__dict__['%ss'%t] = self._childtagdict[t]
            if len(self._childtagdict[t])==1:
                self.__dict__['%s'%t] = self._childtagdict[t][0]



class TrigXMLDocumentReader(object):
    def __init__(self,filename):
        self.filename=filename
        from TrigConfMuctpi.Utils import findFileInXMLPATH
        self.fullFileName = findFileInXMLPATH(filename)
        self.read(self.fullFileName)

    def read(self,filename):
        self.doc = ET.parse(filename)
        root = TrigXMLElement(self.doc.getroot())
        self.__dict__[root.tag] = root

    def getFileName(self):
        return self.fullFileName


class MioctGeometryXMLReader(TrigXMLDocumentReader):
    def __init__(self,filename):
        super(MioctGeometryXMLReader,self).__init__(filename)

    def getMIOCTs(self):
        return self.MuCTPiGeometry.MIOCTs

    @memoize
    def getMIOCT(self, id):
        for mioct in self.MuCTPiGeometry.MIOCTs:
            if int(mioct["id"]) == id:
                return mioct


if __name__ == "__main__":
    from PathResolver import PathResolver
    xmlfile = PathResolver.FindCalibFile("TrigConfMuctpi/TestMioctGeometry_2016_05_30_CS_2600573263.xml")
    l1menu = MioctGeometryXMLReader(xmlfile)
