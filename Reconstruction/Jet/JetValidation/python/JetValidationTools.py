# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def PhysValJetToolCfg(flags,**kwargs):
    '''Following the logic defined for JetMonitoring JetMonitoringStandard.py but adding
    collections that were present in the old PhysVal config''' 
    acc = ComponentAccumulator()

    # create a list of JetMonitoringAlg specifications
    jetcollections = [
                        "AntiKt4LCTopoJets",
                        "AntiKt4EMTopoJets",
                        "AntiKt4EMPFlowJets",
                        "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                        "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets",
                        "AntiKtVR30Rmax4Rmin02PV0TrackJets",
                    ]
    
    if flags.Input.isMC:
        jetcollections +=[
            "AntiKt4TruthJets",
            "AntiKt10TruthTrimmedPtFrac5SmallR20Jets",
            "AntiKt10TruthSoftDropBeta100Zcut10Jets",
        ]
    
    fillers = []
    for col in jetcollections:
        truthJetCollection = ''
        if "Truth" in col:
            truthJetCollection = 'AntiKt4TruthJets'
        fillers += [ acc.popToolsAndMerge(JetMonToolCfg(col, flags, refcontainer=truthJetCollection)) ]

    kwargs.setdefault("HistoTools", fillers)
    kwargs.setdefault("IntervalType", 8)
    acc.setPrivateTools(CompFactory.JetMonitoringTool(**kwargs))

    return acc

def JetMonToolCfg(container, flags, refcontainer='', onlyKinematics=False, globalSelection='', **kwargs):
    acc = ComponentAccumulator()
    filler = CompFactory.JetContainerHistoFiller(container+"HistoFiller", JetContainer=container)

    if globalSelection !='':
        print("WARNING global selection is not yet supported in CA, returning plots with no selection.")

    filler.HistoTools = [ 
       acc.popToolsAndMerge(JetKinematicHistosCfg('kinematics', plotoccupancy=False,plotaveragept=False, plotnjet=True))
    ]
  
    if onlyKinematics: #TODO add JetValidation flags for these
        acc.setPrivateTools(filler)
        return acc

    filler.HistoTools += [
        acc.popToolsAndMerge(JetHistogramsAndSelectionCfg("leadingjet", ["basickinematics"])),
        acc.popToolsAndMerge(JetHistogramsAndSelectionCfg("subleadingjet", ["basickinematics"])),
        acc.popToolsAndMerge(JetHistogramsAndSelectionCfg("40000<pt<50000",["pt"]))
    ]


    from JetValidation.JetValidationHistoDefs import GetJetVariables
    vars = GetJetVariables(container, refcontainer)

    for var in vars:
        if "kinematics" in var:
            tool = acc.popToolsAndMerge(JetKinematicHistosCfg(var))
        elif "effresponse" in var:
            tool = acc.popToolsAndMerge(JetEfficiencyResponseHistosCfg(var))
        else:
            from JetMonitoring.JetHistoTools import compactSpecification
            spec = compactSpecification[var]
            if len(spec) == 2:
                binning, attributeInfo = spec
                tool = acc.popToolsAndMerge(Create1DHistoToolCfg(var, binning, attributeInfo))
            if len(spec) == 3:
                binning, attributeInfo1, attributeInfo2 = spec
                doTProfile = var.beginswith("Prof_")
                tool = acc.popToolsAndMerge(Create2DHistoToolCfg(var, binning, attributeInfo1, attributeInfo2, doTProfile))

        filler.HistoTools += [ tool ]

    acc.setPrivateTools(filler)
    return acc


def JetKinematicHistosCfg(name, plotoccupancy=False, plotaveragept=False, plotnjet=True):
    acc = ComponentAccumulator()

    if "emscale" in name: 
        jetscale = "JetEMScaleMomentum"
    elif "constscale" in name:
        jetscale = "JetConstitScaleMomentum"
    else:
        jetscale = ""

    acc.setPrivateTools(CompFactory.JetKinematicHistos(name, PlotOccupancy=plotoccupancy, PlotAveragePt=plotaveragept, PlotNJet=plotnjet, JetScale=jetscale))

    return acc

def JetHistogramsAndSelectionCfg(selection, histos):
    acc = ComponentAccumulator()

    selTool = acc.popToolsAndMerge(AddSelectorCfg(selection))

    if "alljet" or "leadingjet" or "subleadingjet" in selection:
        selectiontype = 0
        if "alljets" == selection:
            selectiontype=0
        elif "leadingjet" == selection:
            selectiontype=1
        elif "subleadingjet" == selection:
            selectiontype=2
        tool = CompFactory.HistosForJetSelection(selection, SelectionType=selectiontype)
    else:
        tool = CompFactory.HistosForJetSelection("hjsel_"+selTool.name, SelectionType=3,
                                                JetSelectorTool = selTool,
                                                HistoTitleSuffix ='('+selection+')',
                                                HistoNameSuffix = selTool.name)  
        histotools = []
        for histo in histos:
            histotools += [
                acc.popToolsAndMerge(CreateHistoDefToolCfg(histo))
            ]

    acc.setPrivateTools(tool)
    return acc


def JetEfficiencyResponseHistosCfg(name):
    acc = ComponentAccumulator()

    tool = CompFactory.EfficiencyResponseHistos(name)
    tool.HistoDef = [
        acc.popToolsAndMerge(CreateHistoDefToolCfg('erhEfficiencyR1', "Jet p_{T} Efficiency #DeltaR = 0.1;p_{T}^{Truth} (GeV);Efficiency",50,0,100 )),
        acc.popToolsAndMerge(CreateHistoDefToolCfg('erhEfficiencyR2', "Jet p_{T} Efficiency #DeltaR = 0.2;p_{T}^{Truth} (GeV);Efficiency",50,0,100 )),
        acc.popToolsAndMerge(CreateHistoDefToolCfg('erhEfficiencyR3', "Jet p_{T} Efficiency #DeltaR = 0.3;p_{T}^{Truth} (GeV);Efficiency",50,0,100 )),

        acc.popToolsAndMerge(CreateHistoDefToolCfg('erhResponse', "Jet p_{T} Response;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}};Number of jets",50,-1,1 )),
        acc.popToolsAndMerge(CreateHistoDefToolCfg('erhResponseVsEta', "Jet p_{T} Response vs #eta;#eta of jet;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,-5,5 )),
        acc.popToolsAndMerge(CreateHistoDefToolCfg('erhResponseVsPt', "Jet p_{T} Response vs p_{T};p_{T}^{Truth} of jet;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,0,1000 )),

        acc.popToolsAndMerge(CreateHistoDefToolCfg('erhResponse_noShift', "Jet p_{T} Response;#frac{p_{T}^{Jet}}{p_{T}^{Truth}};Number of jets",50,0,2 )),
        acc.popToolsAndMerge(CreateHistoDefToolCfg('erhResponseVsEta_noShift', "Jet p_{T} Response vs #eta;#eta of jet;#frac{p_{T}^{Jet}}{p_{T}^{Truth}}",50,-5,5 )),
        acc.popToolsAndMerge(CreateHistoDefToolCfg('erhResponseVsPt_noShift', "Jet p_{T} Response vs p_{T};p_{T}^{Truth} of jet;#frac{p_{T}^{Jet}}{p_{T}^{Truth}}",50,0,1000 )),

        acc.popToolsAndMerge(CreateHistoDefToolCfg('erhDeltaR', "#DeltaR between Jet and closest Truth Jet;#DeltaR;Number of jets",50,0,4 )),
    ]

    acc.setPrivateTools(tool)
    return acc


def CreateHistoDefToolCfg( name, title=None,nbinsx=10, xlow=10.0, xup=1.0, nbinsy=10, ylow=0.0, yup=1.0, hname=None):
    """Short cut to return a HistoDefinitionTool from a compact list of arguments"""   
    acc = ComponentAccumulator()

    if title is  None : title = name
    if hname is None :  hname = name
    name = "hdef_"+name # athena can causes conflicts when tools of different types have same names
    acc.setPrivateTools(CompFactory.HistoDefinitionTool(name,hname=hname, title=title,nbinsx=nbinsx, xlow=xlow, xup=xup, nbinsy=nbinsy, ylow=ylow, yup=yup))
    return acc

def Create1DHistoToolCfg( name, binning, attributeInfo):
    acc = ComponentAccumulator()

    from JetMonitoring.JetAttributeHistoManager import unpackto3, findSelectIndex, sanitizeName
    attName, attType, attGeV = unpackto3(attributeInfo)
    attName, selectIndex = findSelectIndex(attName) # 'JVF[1]' --> 'JVF', 1

    #hname = name if selectIndex==-1 else (name+'_'+str(selectIndex))
    hname = sanitizeName(name) # remove [ and ] which can be problematic in histo names

    tool = CompFactory.JetAttributeHisto( name,
                              AttributeTypes = [ attType ],
                              AttributeNames = [ attName ],
                              AttributeInGeV = [ bool(attGeV) ],
                              SelectIndex = selectIndex)
    
    hdef = acc.popToolsAndMerge(CreateHistoDefToolCfg(hname, *binning))
    tool.HistoDef = hdef

    acc.setPrivateTools(tool)
    return acc

def Create2DHistoToolCfg( name, binning=None, attributeInfo1=None, attributeInfo2=None):
    acc = ComponentAccumulator()

    from JetMonitoring.JetAttributeHistoManager import unpackto3, findSelectIndex, sanitizeName
    attName1, attType1, attGeV1 = unpackto3(attributeInfo1)
    attName1, selectIndex1 = findSelectIndex(attName1)

    attName2, attType2, attGeV2 = unpackto3(attributeInfo2)
    attName2, selectIndex2 = findSelectIndex(attName2)

    # currently support only vector<float> vs float, so there can be only one selected index.
    selectIndex = max ( selectIndex1, selectIndex2)

    #hname = name if selectIndex==-1 else (name+'_'+str(selectIndex))
    hname = sanitizeName(name) # remove [ and ] which can be problematic in histo names

    tool = CompFactory.JetAttributeHisto( name, 
                              AttributeTypes = [ attType1, attType2 ],
                              AttributeNames = [ attName1, attName2 ],
                              AttributeInGeV = [ bool(attGeV1), bool(attGeV2) ],
                              SelectIndex = selectIndex)

    hdef = acc.popToolsAndMerge(CreateHistoDefToolCfg(hname, *binning))
    tool.HistoDef = hdef
    
    acc.setPrivateTools(tool)
    return acc

def AddSelectorCfg(selectString, name="", typ="float"):
    acc = ComponentAccumulator()

    from JetMonitoring.JetAttributeHistoManager import interpretSelStr,findSelectIndex
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

    tool = CompFactory.JetSelectorAttributeRunII(name, Attribute=att, AttributeType=typ, VectorIndex=ind)
    if cmin is not None: tool.CutMin = cmin
    if cmax is not None: tool.CutMax = cmax
    acc.setPrivateTools(tool)
    return acc

