# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from JetMonitoring.HistoDefinitionHelpers import createHistoDefTool as hdef
from JetMonitoring.HistoDefinitionHelpers import mergeHistoDefinition
from JetMonitoring.JetMonitoringConf import JetAttributeHisto, JetSelectorAttributeRunII
from JetMonitoring.JetHistoManager import jetHistoManager as jhm

class AttributeHistoManager(object):

    def __init__(self, jhm ):
        self.jhm = jhm
        
    def buildKnownTools(self, compactSpecification):
        for name, spec in compactSpecification.items():
            if len(spec) == 2 :
                binning, attributeInfo = spec
                self.add1DHistoTool(name, binning, attributeInfo)
            elif len(spec) == 3 :
                binning, attributeInfo1, attributeInfo2 = spec
                doTProfile = name.beginswith("Prof_")
                self.add2DHistoTool(name, binning, attributeInfo1, attributeInfo2, DoTProfile=doTProfile)

    def add1DHistoTool(self, name, binning, attributeInfo,**otherArgs):

        if self.jhm.hasTool(name):
            print ("ERROR  JetAttributeHisto with name ", name ," already exists. Can't add a new one")
            return None

        tool = create1DHistoTool(name, binning, attributeInfo, **otherArgs)
        return self.jhm.addTool( tool)



    def add2DHistoTool(self, name, binning=None, attributeInfo1=None, attributeInfo2=None,**otherArgs):        
        if self.jhm.hasTool(name):
            print ("ERROR  JetAttributeHisto with name ", name ," already exists. Can't add a new one")
            return None

        tool = create2DHistoTool(name, binning, attributeInfo1, attributeInfo2, **otherArgs)
        return self.jhm.addTool( tool )

    
    def create2DHistoToolFrom1D(self, name, **otherArgs):
        tool = self.jhm.tool(name, build2Difmissing=False)
        if tool is not None : return tool

        # else try to build it
        # We are trying to build a 2D histo tool from existing 1D histo tools. 
        n1, n2 = name.split(':')
        t1 = self.jhm.tool(n1)
        t2 = self.jhm.tool(n2)

        if None in (t1, t2):
            missing = n1 if t1 is None else n2
            print ("ERROR : can't build 2D histo", name, "  : ",missing, " is unknonw")
            return None
        
        binning = mergeHistoDefinition( t1.HistoDef, t2.HistoDef)
        def rebuildSuffix(index):
            if index==-1: return ''
            return '['+str(index)+']'
        attInfo1 = (t1.AttributeNames[0]+rebuildSuffix(t1.SelectIndex), t1.AttributeTypes[0])
        attInfo2 = (t2.AttributeNames[0]+rebuildSuffix(t2.SelectIndex), t2.AttributeTypes[0])

        tool = create2DHistoTool(name, binning, attInfo1, attInfo2, **otherArgs)

        return self.jhm.addTool( tool)

    def addSelector(self, selectString, name="", typ="float"):
        if name != "" and self.jhm.hasTool(name) :
            print ("ERROR  JetSelectorAttributeRunII with name ", name ," already exists. Can't add a new one")
            return None
        if  self.jhm.hasTool(selectString) :
            print ("ERROR  JetSelectorAttributeRunII  ", selectString ," already exists. Can't add a new one")
            return None        
        tool = createAttSelector( selectString, name=name, typ=typ)
        # selectors are public tools :
        from AthenaCommon.AppMgr import ToolSvc

        ToolSvc += tool
        return self.jhm.addTool( tool , alias=selectString)




def create1DHistoTool( name, binning, attributeInfo,**otherArgs):

    attName, attType, attGeV = unpackto3(attributeInfo)
    attName, selectIndex = findSelectIndex(attName) # 'JVF[1]' --> 'JVF', 1

    #hname = name if selectIndex==-1 else (name+'_'+str(selectIndex))
    hname = sanitizeName(name) # remove [ and ] which can be problematic in histo names

    return JetAttributeHisto( name, HistoDef = hdef(hname, *binning),
                              AttributeTypes = [ attType ],
                              AttributeNames = [ attName ],
                              AttributeInGeV = [ bool(attGeV) ],
                              SelectIndex = selectIndex , **otherArgs)


def create2DHistoTool( name, binning=None, attributeInfo1=None, attributeInfo2=None,**otherArgs):
    attName1, attType1, attGeV1 = unpackto3(attributeInfo1)
    attName1, selectIndex1 = findSelectIndex(attName1)

    attName2, attType2, attGeV2 = unpackto3(attributeInfo2)
    attName2, selectIndex2 = findSelectIndex(attName2)

    # currently support only vector<float> vs float, so there can be only one selected index.
    selectIndex = max ( selectIndex1, selectIndex2)

    #hname = name if selectIndex==-1 else (name+'_'+str(selectIndex))
    hname = sanitizeName(name) # remove [ and ] which can be problematic in histo names

    return JetAttributeHisto( name, HistoDef = hdef(hname, *binning),
                              AttributeTypes = [ attType1, attType2 ],
                              AttributeNames = [ attName1, attName2 ],
                              AttributeInGeV = [ bool(attGeV1), bool(attGeV2) ],
                              SelectIndex = selectIndex , **otherArgs)


def createAttSelector(selectString, name="", typ="float"):
    """A short cut to create JetSelectorAttributeRunII out of a simple string """
    cmin, att, cmax = interpretSelStr(selectString)
    att, ind = findSelectIndex(att)
    if ind>-1 and 'vector' not in typ :
        typ = 'vector<'+typ+'>'

    if name == "":
        # try to build a unique name
        name = selectString.replace('<','_inf_')
        name = name.replace('[','_')
        name = name.replace(']','_')
        name = name.replace('.','_')
        name = 'sel_'+name
    tool = JetSelectorAttributeRunII(name, Attribute=att, AttributeType=typ, VectorIndex=ind)
    if cmin is not None: tool.CutMin = cmin
    if cmax is not None: tool.CutMax = cmax
    return tool


def sanitizeName(name):
    return name.replace('[','_').replace(']','_')

def unpackto3(t):
    if len(t)==2:
        return t+(False,)
    return t

def findSelectIndex( name):
    try:
        name, index = name.split('[')
    except Exception:
        name, index = name, ''
    if not index.endswith(']'):
        return name, -1
    index = int(index[:-1])
    return name, index
    

def interpretSelStr(selStr):
    """Interpret a selection string in the form '12.3<var<42.0'
    and returns a tuple.
     '12.3<var<42.0' -> returns (12.3, 'var', 42.)
     'var<42.0' -> returns (None, 'var', 42.)
     '12.3<var' -> returns (12.3, 'var', None)
    """
    parts = selStr.split('<')
    cmin, cmax = None, None
    var = selStr
    if len(parts)==2:
        ismin = False
        try :
            var, cut = parts[0] , float(parts[1])
        except Exception:
            cut, var = float(parts[0]) ,parts[1]
            ismin=True
        if ismin : cmin = cut
        else: cmax = cut
    elif len(parts)==3:
        cmin, var, cmax = parts
        cmin = float(cmin)
        cmax = float(cmax)

    return cmin, var, cmax


attributeHistoManager = AttributeHistoManager(jhm)
