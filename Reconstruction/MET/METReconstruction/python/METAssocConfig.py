# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

#################################################################################
# Define some default values

clusterSigStates = {
    'EMScale':0,
    'LocHad':1
}

defaultInputKey = {
   'Ele'       :'Electrons',
   'Gamma'     :'Photons',
   'Tau'       :'TauJets',
   'LCJet'     :'AntiKt4LCTopoJets',
   'EMJet'     :'AntiKt4EMTopoJets',
   'PFlowJet'  :'AntiKt4EMPFlowJets',
   'Muon'      :'Muons',
   'Soft'      :'',
   'ClusColl'  :'CaloCalTopoClusters',
   'TrkColl'   :'InDetTrackParticles',
   'PrimVxColl':'PrimaryVertices',
   'Truth'     :'TruthParticles',
   }

# # old naming scheme
# defaultInputKey = {
#     'Ele'       :'ElectronCollection',
#     'Gamma'     :'PhotonCollection',
#     'Tau'       :'TauRecContainer',
#     'LCJet'     :'AntiKt4LCTopoJets',
#     'EMJet'     :'AntiKt4EMTopoJets',
#     'PFlowJet'  :'AntiKt4EMPFlowJets',
#     'Muon'      :'Muons',
#     'Soft'      :'',
#     'ClusColl'  :'CaloCalTopoCluster',
#     'TrkColl'   :'InDetTrackParticles',
#     'PrimVxColl':'PrimaryVertices',
#     'Truth'     :'TruthParticle',
#     }

prefix = 'METAssocConfig:   '

#################################################################################
# Configuration of builders

class AssocConfig:
    def __init__(self,objType='',inputKey=''):
        self.objType = objType
        self.inputKey = inputKey

def getAssociator(config,suffix,doPFlow,trkseltool=None,pfotool=None):
    tool = None

    from AthenaCommon.AppMgr import ToolSvc
    # Construct tool and set defaults for case-specific configuration
    if config.objType == 'Ele':
        tool = CfgMgr.met__METElectronAssociator('MET_ElectronAssociator_'+suffix)
    if config.objType == 'Gamma':
        tool = CfgMgr.met__METPhotonAssociator('MET_PhotonAssociator_'+suffix)
    if config.objType == 'Tau':
        tool = CfgMgr.met__METTauAssociator('MET_TauAssociator_'+suffix)
    if config.objType == 'LCJet':
        tool = CfgMgr.met__METJetAssocTool('MET_LCJetAssocTool_'+suffix)
    if config.objType == 'EMJet':
        tool = CfgMgr.met__METJetAssocTool('MET_EMJetAssocTool_'+suffix)
    if config.objType == 'PFlowJet':
        tool = CfgMgr.met__METJetAssocTool('MET_PFlowJetAssocTool_'+suffix)
    if config.objType == 'Muon':
        tool = CfgMgr.met__METMuonAssociator('MET_MuonAssociator_'+suffix)
    if config.objType == 'Soft':
        tool = CfgMgr.met__METSoftAssociator('MET_SoftAssociator_'+suffix)
    if config.objType == 'Truth':
        tool = CfgMgr.met__METTruthAssociator('MET_TruthAssociator_'+suffix)
        ToolSvc == tool
        tool.RecoJetKey = config.inputKey
    if doPFlow:
        if not pfotool:
            pfotool = CfgMgr.CP__RetrievePFOTool('MET_PFOTool_'+suffix)
            ToolSvc += pfotool
        tool.PFOTool = pfotool
        tool.PFlow=True
    # set input/output key names
    if config.inputKey == '':
        tool.InputCollection = defaultInputKey[config.objType]
        config.inputKey = tool.InputCollection
        tool.ClusColl = defaultInputKey['ClusColl']
        tool.TrkColl = defaultInputKey['TrkColl']
    else:
        tool.InputCollection = config.inputKey

    if not trkseltool:
        trkseltool=CfgMgr.InDet__InDetTrackSelectionTool("IDTrkSel_METAssoc",
                                                         CutLevel="TightPrimary",
                                                         maxZ0SinTheta=3,
                                                         maxD0overSigmaD0=2)
    ToolSvc += trkseltool
    tool.TrackSelectorTool = trkseltool
    ToolSvc += tool
    return tool

#################################################################################
# Top level MET configuration

class METAssocConfig:
    def outputCollections(self):
        if doTruth: return 'MET_Core_'+self.suffix
        else: return 'MET_Core_'+self.suffix,'MET_Reference_'+self.suffix
    #
    def outputMap(self):
        return 'METAssoc_'+self.suffix
        return 'METAssoc_'+self.suffix
    #
    def setupAssociators(self,buildconfigs):
        print prefix, 'Setting up associators for MET config '+self.suffix
        for config in buildconfigs:
            if config.objType in self.associators:
                print prefix, 'Config '+self.suffix+' already contains a associator of type '+config.objType
                raise LookupError
            else:
                associator = getAssociator(config,self.suffix,self.doPFlow,self.trkseltool)
                self.associators[config.objType] = associator
                self.assoclist.append(associator)
                print prefix, '  Added '+config.objType+' tool named '+associator.name()
    #
    def __init__(self,suffix,buildconfigs=[],
                 doPFlow=False,doTruth=False,
                 trksel=None):
        if doTruth:
            print prefix, 'Creating MET TruthAssoc config \''+suffix+'\''
        else:
            print prefix, 'Creating MET Assoc config \''+suffix+'\''
        self.suffix = suffix
        self.doPFlow = doPFlow
        self.doTruth = doTruth
        self.trkseltool = trksel
        self.associators = {}
        self.assoclist = [] # need an ordered list
        #
        self.setupAssociators(buildconfigs)

# Set up a top-level tool with mostly defaults
def getMETAssocTool(topconfig,allowOverwrite=False):
    assocTool = None
    from METReconstruction.METRecoFlags import metFlags
    if topconfig.doTruth:
        assocTool = CfgMgr.met__METAssociationTool('MET_TruthAssociationTool_'+topconfig.suffix,
                                                   METAssociators = topconfig.assoclist,
                                                   METSuffix = topconfig.suffix)
    else:
        tcstate = clusterSigStates['LocHad']
        if 'EMTopo' in topconfig.suffix: tcstate = clusterSigStates['EMScale']
        assocTool = CfgMgr.met__METAssociationTool('MET_AssociationTool_'+topconfig.suffix,
                                                   METAssociators = topconfig.assoclist,
                                                   METSuffix = topconfig.suffix,
                                                   TCSignalState=tcstate,AllowOverwrite=allowOverwrite)
    return assocTool

# Allow user to configure reco tools directly or get more default configurations
def getMETAssocAlg(algName='METAssociation',configs={},tools=[],allowOverwrite=False):

    assocTools = []
    assocTools += tools

    from METReconstruction.METRecoFlags import metFlags
    if configs=={} and tools==[]:
        print prefix, 'Taking configurations from METRecoFlags'
        configs = metFlags.METAssocConfigs()
        print configs
    for key,conf in configs.iteritems():
        print prefix, 'Generate METAssocTool for MET_'+key
        assoctool = getMETAssocTool(conf,allowOverwrite)
        assocTools.append(assoctool)
        metFlags.METAssocTools()[key] = assoctool

    from AthenaCommon.AppMgr import ToolSvc
    for tool in assocTools:
        ToolSvc += tool
        print prefix, 'Added METAssocTool \''+tool.name()+'\' to alg '+algName

    assocAlg = CfgMgr.met__METRecoAlg(name=algName,
                                      RecoTools=assocTools)
#    assocAlg.OutputLevel=DEBUG
    return assocAlg
