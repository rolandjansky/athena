# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration



from AthenaCommon import CfgMgr

#################################################################################
# Define some default values

clusterSigStates = {
    'EMScale':0,
    'LocHad':1,
    'Mod':1
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
   'Clusters'  :'CaloCalTopoClusters',
   'Tracks'    :'InDetTrackParticles',
   'PrimVxColl':'PrimaryVertices',
   'Truth'     :'TruthEvents',
   }

prefix = 'METAssocConfig:   '

#################################################################################
# Configuration of builders

class AssocConfig:
    def __init__(self,objType='',inputKey=''):
        self.objType = objType
        self.inputKey = inputKey

def getAssociator(config,suffix,doPFlow=False,
                  trkseltool=None,trkisotool=None,caloisotool=None,
                  modClusKey="",
                  modClusColls={}):
    tool = None

    import cppyy
    try: cppyy.loadDictionary('METReconstructionDict')
    except: pass

    doModClus = (modClusKey!="")
    modLCClus = modClusColls['LC{0}Clusters'.format(modClusKey)]
    modEMClus = modClusColls['EM{0}Clusters'.format(modClusKey)]
    from AthenaCommon.AppMgr import ToolSvc
    # Construct tool and set defaults for case-specific configuration
    if config.objType == 'Ele':
        from ROOT import met
        tool = CfgMgr.met__METElectronAssociator('MET_ElectronAssociator_'+suffix,TCMatchMethod=met.ClusterLink)
    if config.objType == 'Gamma':
        from ROOT import met
        tool = CfgMgr.met__METPhotonAssociator('MET_PhotonAssociator_'+suffix,TCMatchMethod=met.ClusterLink)
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
        tool.DecorateSoftConst = True
        if doModClus:
            tool.LCModClusterKey = modLCClus
            tool.EMModClusterKey = modEMClus
    if config.objType == 'Truth':
        tool = CfgMgr.met__METTruthAssociator('MET_TruthAssociator_'+suffix)
        tool.RecoJetKey = config.inputKey
    if doPFlow:
        pfotool = CfgMgr.CP__RetrievePFOTool('MET_PFOTool_'+suffix)
        ToolSvc += pfotool
        tool.PFOTool = pfotool
        pfoweighttool = CfgMgr.CP__WeightPFOTool('MET_PFOWeightTool_'+suffix)
        ToolSvc += pfoweighttool
        tool.PFOWeightTool = pfoweighttool
        tool.PFlow = True
    else:
        tool.UseModifiedClus = doModClus
    # set input/output key names
    if config.inputKey == '':
        tool.InputCollection = defaultInputKey[config.objType]
        config.inputKey = tool.InputCollection
    else:
        tool.InputCollection = config.inputKey
    if doModClus:
        tool.ClusColl = modLCClus
        if 'EMTopo' in suffix: tool.ClusColl = modEMClus
        tool.TrkColl = defaultInputKey['Tracks']

    from METReconstruction.METRecoFlags import metFlags
    tool.UseTracks = metFlags.UseTracks()
    #
    tool.TrackSelectorTool = trkseltool
    #
    tool.TrackIsolationTool = trkisotool
    tool.CaloIsolationTool = caloisotool

    if not hasattr(ToolSvc,tool.name()):
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
                associator = getAssociator(config=config,suffix=self.suffix,
                                           doPFlow=self.doPFlow,
                                           trkseltool=self.trkseltool,
                                           trkisotool=self.trkisotool,
                                           caloisotool=self.caloisotool,
                                           modClusKey=self.modClusKey,
                                           modClusColls=self.modClusColls)
                from METReconstruction.METRecoFlags import metFlags
                if config.objType == 'Soft' and metFlags.DecorateSoftConst:
                    print "activate soft term decoration"
                    associator.DecorateSoftConst = True
                self.associators[config.objType] = associator
                self.assoclist.append(associator)
                print prefix, '  Added '+config.objType+' tool named '+associator.name()
    #
    def __init__(self,suffix,buildconfigs=[],
                 doPFlow=False,doTruth=False,
                 trksel=None,
                 modClusKey="OriginCorr",
                 modClusColls={'LCOriginCorrClusters':'LCOriginTopoClusters',
                               'EMOriginCorrClusters':'EMOriginTopoClusters'}
                 ):
        if doTruth:
            print prefix, 'Creating MET TruthAssoc config \''+suffix+'\''
        else:
            print prefix, 'Creating MET Assoc config \''+suffix+'\''
        self.suffix = suffix
        self.doPFlow = doPFlow
        self.modClusKey=modClusKey
        self.modClusColls=modClusColls
        self.doTruth = doTruth
        from AthenaCommon.AppMgr import ToolSvc
        if trksel:
            self.trkseltool = trksel
        else:
            self.trkseltool=CfgMgr.InDet__InDetTrackSelectionTool("IDTrkSel_METAssoc",
                                                                  CutLevel="TightPrimary",
                                                                  maxZ0SinTheta=3,
                                                                  maxD0=2,
                                                                  minPt=500)
            if not hasattr(ToolSvc,self.trkseltool.name()):
                ToolSvc += self.trkseltool

        self.trkisotool = CfgMgr.xAOD__TrackIsolationTool("TrackIsolationTool_MET")
        self.trkisotool.TrackSelectionTool = self.trkseltool # As configured above
        if not hasattr(ToolSvc,self.trkisotool.name()):
            ToolSvc += self.trkisotool

        self.caloisotool = CfgMgr.xAOD__CaloIsolationTool("CaloIsolationTool_MET",
                                                          saveOnlyRequestedCorrections=True,
                                                          addCaloExtensionDecoration=False)
        if not hasattr(ToolSvc,self.caloisotool.name()):
            ToolSvc += self.caloisotool

        self.associators = {}
        self.assoclist = [] # need an ordered list
        #
        self.setupAssociators(buildconfigs)

# Set up a top-level tool with mostly defaults
def getMETAssocTool(topconfig):
    assocTool = None
    from METReconstruction.METRecoFlags import metFlags
    if topconfig.doTruth:
        assocTool = CfgMgr.met__METAssociationTool('MET_TruthAssociationTool_'+topconfig.suffix,
                                                   METAssociators = topconfig.assoclist,
                                                   METSuffix = topconfig.suffix)
    else:
        tcstate = clusterSigStates['LocHad']
        if 'EMTopo' in topconfig.suffix: tcstate = clusterSigStates['EMScale']
        if topconfig.modClusKey!="":
            tcstate = clusterSigStates['Mod']
        assocTool = CfgMgr.met__METAssociationTool('MET_AssociationTool_'+topconfig.suffix,
                                                   METAssociators = topconfig.assoclist,
                                                   METSuffix = topconfig.suffix,
                                                   TCSignalState=tcstate,
                                                   TimingDetail=0)
        if metFlags.AllowOverwrite:
            assocTool.AllowOverwrite = True
    return assocTool

# Allow user to configure reco tools directly or get more default configurations
def getMETAssocAlg(algName='METAssociation',configs={},tools=[]):

    assocTools = []
    assocTools += tools

    from METReconstruction.METRecoFlags import metFlags
    if configs=={} and tools==[]:
        print prefix, 'Taking configurations from METRecoFlags'
        configs = metFlags.METAssocConfigs()
        print configs
    for key,conf in configs.iteritems():
        print prefix, 'Generate METAssocTool for MET_'+key
        assoctool = getMETAssocTool(conf)
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
