# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

class JetHistoManager(object):

    knownTools = {}
    def addTool(self, tool, alias=""):
        tname = tool.getName()
        name =  tname if alias == "" else alias
        if name in self.knownTools:
            print ("ERROR won't add tool ", name," : already existing")
            return

        self.knownTools[name] = tool
        aname = name.replace(':','_').replace('[','_').replace(']','')

        setattr(self, aname , tool)
        return tool

    def hasTool(self,name):
        return name in self.knownTools

    def tool(self, name, build2Difmissing=True):
        t = self.knownTools.get(name, None)
        if t is None and ":" in name and build2Difmissing:
            from JetMonitoring.JetAttributeHistoManager import attributeHistoManager as ahm
            t = ahm.create2DHistoToolFrom1D(name)
        return t
        
    def printTools(self):
        for n, t in self.knownTools.items():
            print ('%20s    %40s'%(n, type(t)))

jetHistoManager = JetHistoManager()
