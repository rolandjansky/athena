# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
from AthenaConfiguration.ComponentFactory import CompFactory

from GaudiKernel.Constants import INFO
import six

#################################################################################
# Define some default values


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
   'PFlowObj'  :'CHSParticleFlowObjects',
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
                  trkseltool=None,
                  trkisotool=None,caloisotool=None,
                  modConstKey="",
                  modClusColls={}):
    tool = None

    doModClus = (modConstKey!="" and not doPFlow)
    if doModClus:
        modLCClus = modClusColls['LC{0}Clusters'.format(modConstKey)]
        modEMClus = modClusColls['EM{0}Clusters'.format(modConstKey)]
    # Construct tool and set defaults for case-specific configuration
    if config.objType == 'Ele':
        from ROOT import met
        tool = CompFactory.getComp("met::METElectronAssociator")('MET_ElectronAssociator_'+suffix,TCMatchMethod=met.ClusterLink)
    if config.objType == 'Gamma':
        from ROOT import met
        tool = CompFactory.getComp("met::METPhotonAssociator")('MET_PhotonAssociator_'+suffix,TCMatchMethod=met.ClusterLink)
    if config.objType == 'Tau':
        tool = CompFactory.getComp("met::METTauAssociator")('MET_TauAssociator_'+suffix)
    if config.objType == 'LCJet':
        tool = CompFactory.getComp("met::METJetAssocTool")('MET_LCJetAssocTool_'+suffix)
    if config.objType == 'EMJet':
        tool = CompFactory.getComp("met::METJetAssocTool")('MET_EMJetAssocTool_'+suffix)
    if config.objType == 'PFlowJet':
        tool = CompFactory.getComp("met::METJetAssocTool")('MET_PFlowJetAssocTool_'+suffix)
    if config.objType == 'Muon':
        tool = CompFactory.getComp("met::METMuonAssociator")('MET_MuonAssociator_'+suffix)
    if config.objType == 'Soft':
        tool = CompFactory.getComp("met::METSoftAssociator")('MET_SoftAssociator_'+suffix)
        tool.DecorateSoftConst = True
        if doModClus:
            tool.LCModClusterKey = modLCClus
            tool.EMModClusterKey = modEMClus
    if config.objType == 'Truth':
        tool = CompFactory.getComp("met::METTruthAssociator")('MET_TruthAssociator_'+suffix)
        tool.RecoJetKey = config.inputKey
    if doPFlow:
        tool.PFlow = True
        tool.PFlowColl = modConstKey if modConstKey!="" else defaultInputKey["PFlowObj"]
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

    return tool

#################################################################################
# Top level MET configuration

class METAssocConfig:
    def outputCollections(self):
        if self.doTruth: return 'MET_Core_'+self.suffix
        else: return 'MET_Core_'+self.suffix,'MET_Reference_'+self.suffix
    #
    def outputMap(self):
        return 'METAssoc_'+self.suffix
        return 'METAssoc_'+self.suffix
    #
    def setupAssociators(self,buildconfigs):
        print("{} Setting up associators for MET config {}".format(prefix,self.suffix))
        for config in buildconfigs:
            if config.objType in self.associators:
                print ("{} Config {} already contains a associator of type {}".format(prefix,self.suffix,config.objType))
                raise LookupError
            else:
                associator = getAssociator(config=config,suffix=self.suffix,
                                           doPFlow=self.doPFlow,
                                           trkseltool=self.trkseltool,
                                           trkisotool=self.trkisotool,
                                           caloisotool=self.caloisotool,
                                           modConstKey=self.modConstKey,
                                           modClusColls=self.modClusColls)
                from METReconstruction.METRecoFlags import metFlags
                if config.objType == 'Soft' and metFlags.DecorateSoftConst:
                    print ("activate soft term decoration")
                    associator.DecorateSoftConst = True
                self.associators[config.objType] = associator
                self.assoclist.append(associator)
                print("{} Added {} tool named {}".format(prefix,config.objType,associator.name))
    #
    def __init__(self,suffix,inputFlags,buildconfigs=[],
                 doPFlow=False, doTruth=False,
                 trksel=None,
                 modConstKey="",
                 modClusColls={}
                 ):
        # Set some sensible defaults
        modConstKey_tmp = modConstKey
        modClusColls_tmp = modClusColls
        if doPFlow:
            if modConstKey_tmp == "": modConstKey_tmp = "CHSParticleFlowObjects"
        else:
            if modConstKey_tmp == "": modConstKey_tmp = "OriginCorr"
            if modClusColls_tmp == {}: modClusColls_tmp = {'LCOriginCorrClusters':'LCOriginTopoClusters',
                                                           'EMOriginCorrClusters':'EMOriginTopoClusters'}
        if doTruth:
            print ("{} Creating MET TruthAssoc config {}".format(prefix,suffix))
        else:
            print ("{} Creating MET Assoc config {}".format(prefix,suffix))
        self.suffix = suffix
        self.doPFlow = doPFlow                
        self.modConstKey=modConstKey_tmp
        self.modClusColls=modClusColls_tmp
        self.doTruth = doTruth
        if trksel:
            self.trkseltool = trksel
        else:
            self.trkseltool=CompFactory.getComp("InDet::InDetTrackSelectionTool")("IDTrkSel_METAssoc",
                                                                  CutLevel="TightPrimary",
                                                                  maxZ0SinTheta=3,
                                                                  maxD0=2,
                                                                  minPt=500)

        self.trkisotool = CompFactory.getComp("xAOD::TrackIsolationTool")("TrackIsolationTool_MET")
        self.trkisotool.TrackSelectionTool = self.trkseltool # As configured above
        from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg  
        extrapCfg = AtlasExtrapolatorCfg(inputFlags)
        CaloExtensionTool= CompFactory.getComp("Trk::ParticleCaloExtensionTool")(Extrapolator = extrapCfg.popPrivateTools())
        CaloCellAssocTool =  CompFactory.getComp("Rec::ParticleCaloCellAssociationTool")(ParticleCaloExtensionTool = CaloExtensionTool)
        self.caloisotool = CompFactory.getComp("xAOD::CaloIsolationTool")("CaloIsolationTool_MET",
                                                          saveOnlyRequestedCorrections=True,
                                                          addCaloExtensionDecoration=False,
                                                          ParticleCaloExtensionTool = CaloExtensionTool,
                                                          ParticleCaloCellAssociationTool = CaloCellAssocTool)
        self.associators = {}
        self.assoclist = [] # need an ordered list
        #
        self.setupAssociators(buildconfigs)

# Set up a top-level tool with mostly defaults
def getMETAssocTool(topconfig,msglvl=INFO):
    assocTool = None
    from METReconstruction.METRecoFlags import metFlags
    if topconfig.doTruth:
        assocTool = CompFactory.getComp("met::METAssociationTool")('MET_TruthAssociationTool_'+topconfig.suffix,
                                                   METAssociators = topconfig.assoclist,
                                                   METSuffix = topconfig.suffix)
    else:
        assocTool = CompFactory.getComp("met::METAssociationTool")('MET_AssociationTool_'+topconfig.suffix,
                                                   METAssociators = topconfig.assoclist,
                                                   METSuffix = topconfig.suffix,
                                                   TimingDetail=0,
                                                   OutputLevel=msglvl)
        if metFlags.AllowOverwrite:
            assocTool.AllowOverwrite = True
    return assocTool

# Allow user to configure reco tools directly or get more default configurations
def getMETAssocAlg(algName='METAssociation',configs={},tools=[],msglvl=INFO):

    assocTools = []
    assocTools += tools

    from METReconstruction.METRecoFlags import metFlags
    if configs=={} and tools==[]:
        print ("{} Taking configurations from METRecoFlags".format(prefix))
        configs = metFlags.METAssocConfigs()
        print (configs)
    for key,conf in six.iteritems(configs):
        print ("{} Generate METAssocTool for MET_{}".format(prefix,key))
        assoctool = getMETAssocTool(conf,msglvl)
        assocTools.append(assoctool)

    for tool in assocTools:
        print ("{} Added METAssocTool {} to alg {}".format(prefix,tool.name,algName))
    assocAlg = CompFactory.getComp("met::METRecoAlg")(name=algName,
                                      RecoTools=assocTools)
    return assocAlg
