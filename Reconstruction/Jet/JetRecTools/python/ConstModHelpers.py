from AthenaCommon import CfgMgr

import cppyy
try: cppyy.loadDictionary('xAODBaseDict')
except: pass
from ROOT import xAOD

from JetRecTools.JetRecToolsConfig import ctm
from GaudiKernel.Constants import VERBOSE, DEBUG, INFO

from AthenaCommon import Logging
constmodlog = Logging.logging.getLogger('ConstModHelpers')

def getConstModSeq(sequence,inputtype,suffix=""):

    tooltypes = {
        "Origin":"origin",
        "SK06":"softkiller06",
        "CS":"constsub",
        "Vor":"voronoi",
    }

    from JetRecTools.JetRecToolsConfig import ctm

    # May wish to pass an empty sequence for regular PFlow
    modlist = []
    if inputtype=="EMPFlow":
        modlist.append("correctPFO")
    for step in sequence:
        tool = None
        toolname = "ConstitMod{0}_{1}{2}".format(inputtype,step,suffix)
        alias = tooltypes[step]+inputtype+suffix
        if alias in ctm.modifiersMap.keys():
            tool = ctm.modifiersMap[alias]
        else:
            if step=="Origin":
                tool = CfgMgr.CaloClusterConstituentsOrigin(toolname)
            elif step=="Vor":
                tool = CfgMgr.VoronoiWeightTool(toolname, doSpread=False, nSigma=0)
            elif step=="CS":
                tool = CfgMgr.ConstituentSubtractorTool(toolname)
            elif step=="SK06":
                tool = CfgMgr.SoftKillerWeightTool(toolname, SKGridSize=0.6)
            if "PFlow" in inputtype:
                tool.InputType = xAOD.Type.ParticleFlow
                tool.IgnoreChargedPFO=True
                tool.ApplyToChargedPFO=False
            ctm.add(tool,alias)
        modlist.append(alias)

    sequencestr = "".join(sequence)
    seqname = "ConstitMod{0}_{1}{2}".format(sequencestr,inputtype,suffix)
    inputcontainer = ""
    outputcontainer = ""
    if inputtype=="EMPFlow":
        inputcontainer="JetETMiss"
        outputcontainer=sequencestr
        modlist.append('chsPFO')
    else:
        containertemplate = {"EMTopo":"EM{0}TopoClusters",
                             "LCTopo":"LC{0}TopoClusters"}[inputtype]
        inputcontainer = "CaloCalTopoClusters"
        outputcontainer = containertemplate.format(sequencestr)

    inputenum = {"Topo":xAOD.Type.CaloCluster,"PFlow":xAOD.Type.ParticleFlow}[inputtype[2:]]
    modseq = ctm.buildConstitModifSequence( seqname, InputType=inputenum,
                                            OutputContainer = outputcontainer,
                                            InputContainer= inputcontainer,
                                            modList = modlist)
    return modseq

def getJetFinder(label,pjname,modkey):
    from JetRec.JetRecStandard import jtm
    pjg = None
    getterbase = ""
    jetsname = "AntiKt4"+label+"Jets"
    edtool = None
    from EventShapeTools.EventDensityConfig import configEventDensityTool
    if "PFlow" in label:
        pjg = CfgMgr.PFlowPseudoJetGetter(pjname+"get",
                                          InputContainer=label[7:]+"ParticleFlowObjects",
                                          Label=label,
                                          OutputContainer="PseudoJet"+label,
                                          SkipNegativeEnergy = True,
                                          GhostScale = 0.0)
        jtm += pjg
        edtool = configEventDensityTool("Kt4"+label+"EventShape", pjg, 0.4)
        getterbase = "empflow_reduced"
    else:
        tclabel = label.replace('Topo','TopoOrigin')
        pjg = CfgMgr.PseudoJetGetter(pjname+"get",
                                     InputContainer=tclabel.replace("Topo","")+"TopoClusters",
                                     Label=tclabel,
                                     OutputContainer="PseudoJet"+tclabel,
                                     SkipNegativeEnergy = True,
                                     GhostScale = 0.0)
        jtm += pjg
        edtool = configEventDensityTool("Kt4"+tclabel+"EventShape", pjg, 0.4)
        getterbase = "emtopo_reduced"
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += edtool

    getterkey = pjname
    jtm.gettersMap[pjname] = [pjg]+list(jtm.gettersMap[getterbase])[1:]
    finder = jtm.addJetFinder(jetsname, "AntiKt", 0.4, pjname, modkey,
                              ghostArea=0.01, ptmin=2000, ptminFilter=2000, calibOpt="none")

    jtm += edtool
    return finder, edtool

def getConstModMETAlg(suffix, doPFlow):
  from METReconstruction.METRecoFlags import metFlags

  from METReconstruction.METAssocConfig import getMETAssocAlg
  inputtype = ""
  if doPFlow:
    cfg = getEMPFlowConfig(suffix)
    inputtype = "EMPFlow"
  else:
    cfg = getEMTopoConfig(suffix)
    inputtype = "EMTopo"

  metsuffix = "AntiKt4"+inputtype+suffix
  alg = getMETAssocAlg("METAssociation_"+inputtype+suffix,{metsuffix:cfg})

  outputdict = {
    "xAOD::MissingETContainer":"MET_Core_"+metsuffix,
    "xAOD::MissingETAuxContainer":"MET_Core_"+metsuffix+"Aux.",
    "xAOD::MissingETAssociationMap":"METAssoc_"+metsuffix,
    "xAOD::MissingETAuxAssociationMap":"METAssoc_"+metsuffix+"Aux.",
  }

  return alg, outputdict


def getEMTopoConfig(suffix):
  from METReconstruction.METAssocConfig import AssocConfig, METAssocConfig
  metsuffix = 'AntiKt4EMTopo'+suffix
  jetname = metsuffix+"Jets"
  associators = [AssocConfig('EMJet',jetname),
                 AssocConfig('Muon'),
                 AssocConfig('Ele'),
                 AssocConfig('Gamma'),
                 AssocConfig('Tau'),
                 AssocConfig('Soft')]
  cfg = METAssocConfig( metsuffix,
                        associators,
                        doPFlow=False,
                        modConstKey="Orig"+suffix,
                        modClusColls = {"EMOrig"+suffix+"Clusters":"EMOrigin"+suffix+"TopoClusters",
                                        "LCOrig"+suffix+"Clusters":"LCOrigin"+suffix+"TopoClusters"}
                        )
  return cfg


def getEMPFlowConfig(suffix):
  from METReconstruction.METAssocConfig import AssocConfig, METAssocConfig
  metsuffix = 'AntiKt4EMPFlow'+suffix
  jetname = metsuffix+"Jets"
  associators = [AssocConfig('PFlowJet',jetname),
                 AssocConfig('Muon'),
                 AssocConfig('Ele'),
                 AssocConfig('Gamma'),
                 AssocConfig('Tau'),
                 AssocConfig('Soft')]
  cfg = METAssocConfig(metsuffix,
                       associators,
                       doPFlow=True,
                       modConstKey=suffix+"ParticleFlowObjects"
                       )
  return cfg
