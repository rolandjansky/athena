# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import xml.etree.cElementTree as ET

class TrigXMLElement:
    def __init__(self,element):
        self.element = element
        self.tag = element.tag
        self.items = element.items()
        self.children = element.getchildren()
        self.readchildren()
    def __str__(self):
        s = "<%s %s>" % (self.tag, " ".join(['%s="%s"' % x for x in self.items]))
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
            if not c.tag in self.childtags: self.childtags += [c.tag]
        for t in self.childtags:
            self.__dict__['%ss'%t] = self._childtagdict[t]
            if len(self._childtagdict[t])==1:
                self.__dict__['%s'%t] = self._childtagdict[t][0]

class TrigXMLDocumentReader(object):
    def __init__(self,filename):
        self.filename=filename
        from TrigConfigSvc.TrigConfigSvcConfig import findFileInXMLPATH
        self.read(findFileInXMLPATH(filename))
    def read(self,filename):
        self.doc = ET.parse(filename)
        root = TrigXMLElement(self.doc.getroot())
        self.__dict__[root.tag] = root


class L1MenuXMLReader(TrigXMLDocumentReader):
    def __init__(self,filename):
        super(L1MenuXMLReader,self).__init__(filename)
    def getL1Items(self):
        return self.LVL1Config.TriggerMenu.TriggerItems
    def getL1Thresholds(self):
        return self.LVL1Config.TriggerThresholdList.TriggerThresholds

if __name__ == "__main__":
    l1menu = L1MenuXMLReader("TriggerMenuXML/LVL1config_Cosmic2009_v1_7-bit_trigger_types.xml")
