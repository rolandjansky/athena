# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory

#################################################################################
# Define some default values

defaultInputKey = {
   'Ele'      :'Electrons',
   'Gamma'    :'Photons',
   'Tau'      :'TauJets',
   'Jet'      :'AntiKt4EMTopoJets',
   'Muon'     :'Muons',
   'SoftTrk'  :'InDetTrackParticles',
   'SoftClus' :'CaloCalTopoClusters',
   'SoftPFlow':'JetETMissNeutralParticleFlowObjects',
   'PrimaryVx':'PrimaryVertices',
   'Truth'    :'TruthEvents',
   'Calo'     :'AllCalo',
   'LCOCSoftClus':'LCOriginTopoClusters',
   'EMOCSoftClus':'EMOriginTopoClusters',
   }

defaultOutputKey = {
    'Ele'      :'RefEle',
    'Gamma'    :'RefGamma',
    'Tau'      :'RefTau',
    'Jet'      :'RefJet',
    'Muon'     :'Muons',
    'SoftTrk'  :'SoftTrk',
    'SoftClus' :'SoftClus',
    'SoftPFlow':'SoftPFlow',
    'Total'    :'Final',
    'Truth'    :'Truth',
    'Calo'     :'Calo'
    }

prefix = 'METRecoConfig:   '

#################################################################################
# Configuration of builders

class BuildConfig:
    def __init__(self,objType='',outputKey='',inputKey=''):
        self.objType = objType
        self.outputKey = outputKey
        self.inputKey = inputKey

def getBuilder(config,suffix,doTracks,doCells,doTriggerMET,doOriginCorrClus):
    tool = None
    # Construct tool and set defaults for case-specific configuration
    if config.objType == 'SoftTrk':
        tool = CompFactory.getComp("met::METSoftTermsTool")('MET_SoftTrkTool_'+suffix)
        tool.InputComposition = 'Tracks'
    if config.objType.endswith('SoftClus'):
        tool = CompFactory.getComp("met::METSoftTermsTool")('MET_SoftClusTool_'+suffix)
        tool.InputComposition = 'Clusters'
    if config.objType == 'SoftPFlow':
        tool = CompFactory.getComp("met::METSoftTermsTool")('MET_SoftPFlowTool_'+suffix)
        tool.InputComposition = 'PFlow'
        pfotool = CompFactory.RetrievePFOTool('MET_PFOTool_'+suffix)
        tool.PFOTool = pfotool
    if suffix == 'Truth':
        tool = CompFactory.getComp("met::METTruthTool")('MET_TruthTool_'+config.objType)
        tool.InputComposition = config.objType
        config.inputKey = defaultInputKey['Truth']
        config.outputKey = config.objType
    if suffix == 'Calo':
        from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
        CaloNoiseCondAlg ('totalNoise')
        tool = CompFactory.getComp("met::METCaloRegionsTool")('MET_CaloRegionsTool')
        if doCells:
            tool.UseCells     = True
            tool.DoTriggerMET = doTriggerMET
            config.inputKey   = defaultInputKey['Calo'] 
        else:
            tool.UseCells     = False                   
            tool.DoTriggerMET = False
            config.inputKey   = defaultInputKey['SoftClus']
        config.outputKey = config.objType

    # set input/output key names
    if config.inputKey == '':
        tool.InputCollection = defaultInputKey[config.objType]
        config.inputKey = tool.InputCollection
    else:
        tool.InputCollection = config.inputKey
    if not suffix=='Calo':
        if config.outputKey == '':
            tool.MissingETKey = defaultOutputKey[config.objType]
            config.outputKey = tool.MissingETKey
        else:
            tool.MissingETKey = config.outputKey
    return tool

#################################################################################
# Configuration of refiners

class RefConfig:
    def __init__(self,myType='',outputKey=''):
        self.type = myType
        self.outputKey = outputKey

def getRefiner(config,suffix,trkseltool=None,trkvxtool=None,trkisotool=None,caloisotool=None):
    tool = None

    if config.type == 'TrackFilter':
        tool = CompFactory.getComp("met::METTrackFilterTool")('MET_TrackFilterTool_'+suffix)
        tool.InputPVKey = defaultInputKey['PrimaryVx']
        tool.TrackSelectorTool=trkseltool
        tool.TrackVxAssocTool=trkvxtool
        #
        tool.UseIsolationTools = False #True
        tool.TrackIsolationTool = trkisotool
        tool.CaloIsolationTool = caloisotool
        from METReconstruction.METRecoFlags import metFlags
        tool.DoPVSel = metFlags.UseTracks()
        tool.DoVxSep = metFlags.UseTracks()
    tool.MissingETKey = config.outputKey
    return tool

#################################################################################
# Region tools are a special case of refiners

def getRegions(config,suffix):
    if suffix == 'Truth':
        config.outputKey = config.objType
    tool = CompFactory.getComp("met::METRegionsTool")('MET_'+config.outputKey+'Regions_'+suffix)
    tool.InputMETContainer = 'MET_'+suffix
    tool.InputMETMap = 'METMap_'+suffix
    tool.InputMETKey = config.outputKey
    tool.RegionValues = [ 1.5, 3.2, 10 ]
    return tool

#################################################################################
# Top level MET configuration

class METConfig:
    def outputCollection(self):
        return 'MET_'+self.suffix
    #
    def outputMap(self):
        return 'METMap_'+self.suffix
    #
    def setupBuilders(self,buildconfigs):
        print("{} Setting up builders for MET config {}".format(prefix,self.suffix))
        for config in buildconfigs:
            if config.objType in self.builders:
                print("{} Config {} already contains a builder of type {}".format(prefix,self.suffix,config.objType))
                raise LookupError
            else:
                builder = getBuilder(config,self.suffix,self.doTracks,self.doCells,
                                     self.doTriggerMET,self.doOriginCorrClus)
                self.builders[config.objType] = builder
                self.buildlist.append(builder)
                print("{} Added {} tool named {}".format(prefix,config.objType,builder.name))
    #
    def setupRefiners(self,refconfigs):
        print("{} Setting up refiners for MET config {}".format(prefix,self.suffix))
        for config in refconfigs:
            # need to enforce this?
            if config.type in self.refiners:
                print("Config {} already contains a refiner of type {}".format(self.suffix,config.type))
                raise LookupError
            else:
                refiner = getRefiner(config=config,suffix=self.suffix,
                                     trkseltool=self.trkseltool,trkvxtool=self.trkvxtool,
                                     trkisotool=self.trkisotool,caloisotool=self.caloisotool)
                self.refiners[config.type] = refiner
                self.reflist.append(refiner)
                print("{} Added {} tool named {}".format(prefix,config.type,refiner.name))
    #
    def setupRegions(self,buildconfigs):
        print("{} Setting up regions for MET config {}".format(prefix,self.suffix))
        for config in buildconfigs:
            if config.objType in self.regions:
                print("{} Config {} already contains a region tool of type {}".format(prefix,self.suffix,config.objType))
                raise LookupError
            else:
                regions = getRegions(config,self.suffix)
                self.regions[config.objType] = regions
                self.reglist.append(regions)
                print("{} Added {} region tool named {}".format(prefix,config.objType,regions.name))
    #
    def __init__(self,suffix,inputFlags,buildconfigs=[],refconfigs=[],
                 doTracks=False,doSum=False,doRegions=False,
                 doCells=False,doTriggerMET=True,duplicateWarning=True,
                 doOriginCorrClus=False):
        print("{} Creating MET config {}".format(prefix,suffix))
        self.suffix = suffix
        self.doSum = doSum
        self.doTracks = doTracks
        self.doRegions = doRegions
        self.doCells = doCells,
        self.doOriginCorrClus = doOriginCorrClus
        self.doTriggerMET = doTriggerMET
        self.duplicateWarning = duplicateWarning
        #
        self.builders = {}
        self.buildlist = [] # need an ordered list
        #
        self.refiners = {}
        self.reflist = [] # need an ordered list
        #
        self.regions = {}
        self.reglist = [] # need an ordered list
        if doRegions:
            self.setupRegions(buildconfigs)
        #
        from AthenaConfiguration.ComponentFactory import CompFactory
        self.trkseltool=CompFactory.getComp("InDet::InDetTrackSelectionTool")("IDTrkSel_MET",
                                                              CutLevel="TightPrimary",
                                                              maxZ0SinTheta=3,
                                                              maxD0=2,
                                                              minPt=500)
        #
        self.trkvxtool=CompFactory.getComp("CP::TrackVertexAssociationTool")("TrackVertexAssociationTool_MET", WorkingPoint="Nominal")
        #
        self.trkisotool = CompFactory.getComp("xAOD::TrackIsolationTool")("TrackIsolationTool_MET")
        self.trkisotool.TrackSelectionTool = self.trkseltool # As configured above
        ###
        from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
        extrapCfg = AtlasExtrapolatorCfg(inputFlags)
        CaloExtensionTool= CompFactory.getComp("Trk::ParticleCaloExtensionTool")(Extrapolator = extrapCfg.popPrivateTools())
        CaloCellAssocTool = CompFactory.getComp("Rec::ParticleCaloCellAssociationTool")(ParticleCaloExtensionTool = CaloExtensionTool)
        self.caloisotool = CompFactory.getComp("xAOD::CaloIsolationTool")("CaloIsolationTool_MET",
                                                          saveOnlyRequestedCorrections=True,
                                                          addCaloExtensionDecoration=False,
                                                          ParticleCaloExtensionTool = CaloExtensionTool,
                                                          ParticleCaloCellAssociationTool = CaloCellAssocTool)

        self.setupBuilders(buildconfigs)
        self.setupRefiners(refconfigs)

# Set up a top-level tool with mostly defaults
def getMETRecoTool(topconfig):
    recoTool = CompFactory.getComp("met::METRecoTool")('MET_RecoTool_'+topconfig.suffix,
                                       METBuilders = topconfig.buildlist,
                                       METRefiners = topconfig.reflist,
                                       METContainer = topconfig.outputCollection(),
                                       METComponentMap = topconfig.outputMap(),
                                       WarnIfDuplicate = topconfig.duplicateWarning,
                                       TimingDetail=0)
    if topconfig.doSum:
        recoTool.METFinalName = defaultOutputKey['Total']    

    return recoTool

# Set up a METRecoTool that builds MET regions
def getRegionRecoTool(topconfig):
    regTool = CompFactory.getComp("met::METRecoTool")('MET_RegionTool_'+topconfig.suffix,
                                       METBuilders = [],
                                       METRefiners = topconfig.reglist,
                                       METContainer = topconfig.outputCollection()+'Regions',
                                       METComponentMap = topconfig.outputMap()+'Regions',
                                       WarnIfDuplicate = topconfig.duplicateWarning)
    return regTool

# Allow user to configure reco tools directly or get more default configurations
def getMETRecoAlg(algName='METReconstruction',configs={}):
    recoTools = []
    for key,conf in configs.items():
        print("{} Generate METRecoTool for MET_{}".format(prefix,key))
        recotool = getMETRecoTool(conf)
        recoTools.append(recotool)
        if conf.doRegions:
            regiontool = getRegionRecoTool(conf)
            recoTools.append(regiontool)
    for tool in recoTools:
        print("{} Added METRecoTool {} to alg {}".format(prefix,tool.name,algName))
    recoAlg = CompFactory.getComp("met::METRecoAlg")(name=algName,RecoTools=recoTools)
    return recoAlg
