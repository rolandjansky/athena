#   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaCommon import Logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
ufolog = Logging.logging.getLogger('TCCorUFO')

#from AthenaConfiguration import UnifyProperties



def _unifyPV0onlyTrkClustAssoc( vxContName1, vxContName2):
    if vxContName2 == vxContName1 : return vxContName1
    if "" in [vxContName1, vxContName2] : return vxContName1 or vxContName2
    raise Exception(" Error : incompatible VertexContainerName of 2 instance of TrackParticleClusterAssociationAlg : '{}' and '{}'".format(vxContName1, vxContName2))


# we make sure a unique TrackParticleAssociationAlg is well behaved when configured for both TCC and UFOInfo
# in the same job. The function below will be used in the de-duplication and implies all tracks (not only PV0)
# have associated clusters only if TCC is scheduled.
#  !! Doesn't seem to work yet ??!!
#UnifyProperties.setUnificationFunction( "TrackParticleClusterAssociationAlg.VertexContainerName", _unifyPV0onlyTrkClustAssoc)
    



def setupTrackVertexAssocTool():
    #from AthenaCommon import CfgMgr
    # do as in jet config :
    return CompFactory.getComp("CP::TrackVertexAssociationTool")("jetLooseTVAtool", WorkingPoint='Loose',)



def tmpSetupTrackServices(inputFlags):
    """This temporary function ensure the tracking services necessary for TCC/UFO building are set up in a RunIII compatible way"""
    
    result=ComponentAccumulator()
    StoreGateSvc=CompFactory.StoreGateSvc
    result.addService(StoreGateSvc("DetectorStore"))
    
    #Setup up general geometry
    from AtlasGeoModel.InDetGMConfig import InDetGeometryCfg
    result.merge(InDetGeometryCfg(inputFlags))
 
    #Setup TRT conditions                                                                                                                                  
    TRTAlignCondAlg=CompFactory.TRTAlignCondAlg
    result.addCondAlgo(TRTAlignCondAlg(name = "TRTAlignCondAlg",UseDynamicFolders = inputFlags.GeoModel.Align.Dynamic))
 
    #Setup Pixel conditions
    PixelAlignCondAlg=CompFactory.PixelAlignCondAlg
    result.addCondAlgo(PixelAlignCondAlg(name = "PixelAlignCondAlg",UseDynamicAlignFolders = inputFlags.GeoModel.Align.Dynamic))
 
    PixelDetectorElementCondAlg=CompFactory.PixelDetectorElementCondAlg
    result.addCondAlgo(PixelDetectorElementCondAlg(name = "PixelDetectorElementCondAlg"))
 
    #Setup SCT conditions
    SCT_AlignCondAlg=CompFactory.SCT_AlignCondAlg
    result.addCondAlgo(SCT_AlignCondAlg(name = "SCT_AlignCondAlg",UseDynamicAlignFolders = inputFlags.GeoModel.Align.Dynamic))
 
    SCT_DetectorElementCondAlg=CompFactory.SCT_DetectorElementCondAlg
    result.addCondAlgo(SCT_DetectorElementCondAlg(name = "SCT_DetectorElementCondAlg"))
 
    GeometryDBSvc=CompFactory.GeometryDBSvc
    result.addService(GeometryDBSvc("InDetGeometryDBSvc"))
    
 
    #Setup TRT geometry
    TRT_DetectorTool=CompFactory.TRT_DetectorTool
    trtDetectorTool = TRT_DetectorTool()
    #These two lines fix ATLASRECTS-5053. I expect eventually we can remove them, once the underlying issue is fixed.
    trtDetectorTool.DoXenonArgonMixture = False
    trtDetectorTool.DoKryptonMixture = False
    result.getService("GeoModelSvc").DetectorTools += [ trtDetectorTool ]
 
    #Setup up material for inner detector
    InDetServMatTool=CompFactory.InDetServMatTool
    result.getService("GeoModelSvc").DetectorTools += [ InDetServMatTool() ]
    
    #Setup up tracking geometry
    from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    acc = TrackingGeometrySvcCfg(inputFlags)
    result.merge(acc)
    
    #load folders needed for Run2 ID alignment
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    result.merge(addFolders(inputFlags,['/TRT/Align'],'TRT_OFL'))
    
    #Setup up muon geometry
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    result.merge(MuonGeoModelCfg(inputFlags))    
 
    #setup magnetic field service
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    result.merge(MagneticFieldSvcCfg(inputFlags))
 
    #hard-code MC conditions tag needed for my ESD file - must be a better way? how to auto-configure?
    #iovDbSvc=result.getService("IOVDbSvc")
    #iovDbSvc.GlobalTag="OFLCOND-MC16-SDR-20"    

    return result 


def getDecorationKeyFunc(trackParticleName, assocPostfix):
    """Simple helper returning a function to build decoration keys """
    return lambda d : trackParticleName+'.'+d+assocPostfix

def setupTrackCaloAssoc(configFlags, caloClusterName="CaloCalTopoClusters",trackParticleName="InDetTrackParticles", assocPostfix = "TCC", onlyPV0Tracks=False):
    """ Schedule a TrackParticleClusterAssociationAlg in the top sequence, taking as input clusters and tracks defined 
    by the keys caloClusterName and trackParticleName.

    onlyPV0Tracks : calculate associated clusters only for PV0 tracks. Avoids unnecessary calculation (used in the UFO case).
       (IMPORTANT CaloExtensionBuilderAlg does extrapolate all tracks : if too much time consuming, it needs a new option to mask non-PV0 tracks)
    """
    ###################################


    decorKey = getDecorationKeyFunc(trackParticleName,assocPostfix)
    
    components = ComponentAccumulator()

    components.merge( tmpSetupTrackServices(configFlags) )

    from TrackToCalo.CaloExtensionBuilderAlgCfg import CaloExtensionBuilderAlgCfg
    caloExtAlg =CaloExtensionBuilderAlgCfg( configFlags )
    caloExtAlg.TrkPartContainerName = trackParticleName

    components.merge(caloExtAlg)
    

    trackParticleClusterAssociation = CompFactory.TrackParticleClusterAssociationAlg(
        "TrackClusterAssociationAlg"+assocPostfix,
        TrackParticleContainerName = trackParticleName,
        PtCut = 400.,
        CaloExtensionName = "ParticleCaloExtension",
        CaloClusterLocation = caloClusterName,
        TrackVertexAssoTool=setupTrackVertexAssocTool(), # will associate trks from PV0 only
        VertexContainerName = "PrimaryVertices" if onlyPV0Tracks else "",
        AssociatedClusterDecorKey = decorKey("AssoClusters"),
        OutputLevel=2)


    components.addEventAlgo( trackParticleClusterAssociation )
    return components

    
def runTCCReconstruction(configFlags, caloClusterName="CaloCalTopoClusters",trackParticleName="InDetTrackParticles",
                         assocPostfix="TCC", doCombined=False, doNeutral=True, doCharged=True, outputTCCName="TrackCaloClusters"):
    """Create a TrackCaloCluster collection from clusters and tracks (caloClusterName and trackParticleName). 
    Depending on options, the collection contains combined, neutral and/or charged TCC.
    This functions schedules 2 TCC spécific algs : 
       * a TrackCaloClusterInfoAlg to build the TrackCaloClusterInfo object
       * a TrackCaloClusterAlg to build the TCC
    
    """

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

    decorKey = getDecorationKeyFunc(trackParticleName,assocPostfix)

    components = ComponentAccumulator()    
    
    components.merge(
        setupTrackCaloAssoc(configFlags, caloClusterName, trackParticleName, assocPostfix, onlyPV0Tracks=False)
    )

    


        
    ###################################
    # Schedule the TrackCaloClusterInfoAlg to create the weights for clusters/tracks and store them in a TrackCaloClusterInfo object.
    #from TrackCaloClusterRecAlgs.TrackCaloClusterRecAlgsConf import TrackCaloClusterAlg, TrackCaloClusterInfoAlg
    tccInfoAlg = CompFactory.TrackCaloClusterInfoAlg(
        "TCCInfoAlg",
        TCCInfoName = "TCCInfo",
        InputTracks = trackParticleName,
        InputClusters = caloClusterName,
        VertexContainer = "PrimaryVertices",
        AssoClustersDecor = decorKey("AssoClusters"),
    )

    components.addEventAlgo( tccInfoAlg) 
    
    ###################################
    # Create the TCC creator alg. TrackCaloClusterAlg makes use of the TrackCaloClusterInfo object
    # and a list of tools to build the various TCC types.
    tccTools = []
    
    from TrackVertexAssociationTool.TTVAToolConfig import TTVAToolCfg
    commonArgs=dict(
        TrackVertexAssoTool = components.popToolsAndMerge(TTVAToolCfg(configFlags,"jetLooseTVAtool",WorkingPoint="Loose")),
        AssoClustersDecor=decorKey("AssoClusters"),        
    )
    
    if doCombined:
        tccCombined = CompFactory.TCCCombinedTool("TCCcombined", **commonArgs)
        
        tccTools.append(tccCombined)
    if doCharged:
        tccCharged = CompFactory.TCCChargedTool("TCCCharged", **commonArgs )
        tccTools.append(tccCharged)
    if doNeutral:
        tccNeutral = CompFactory.TCCNeutralTool("TCCNeutral", **commonArgs )        
        tccTools.append(tccNeutral)

    
    FEContainerName=""
    if(doNeutral):
        FEContainerName="JetETMissNeutralFlowElements"
        neutraloutputTCCName=outputTCCName+"Neutral"
        neutraltccAlg = CompFactory.TrackCaloClusterAlg(name = "TrackCaloClusterAlg_neutral",
                                                        OutputTCCName = neutraloutputTCCName,
                                                        TCCInfo = "TCCInfo",
                                                        TCCTools = tccTools,
                                                        OutputLevel = 2,
                                                        AppendToTCCName = FEContainerName
                                                        
                                                    )
        components.addEventAlgo(neutraltccAlg)

    if(doCharged):
        FEContainerName="JetETMissChargedFlowElements"
        chargedoutputTCCName=outputTCCName+"Charged"
        chargedtccAlg = CompFactory.TrackCaloClusterAlg(name = "TrackCaloClusterAlg_charged",
                                                        OutputTCCName = chargedoutputTCCName,
                                                        TCCInfo = "TCCInfo",
                                                        TCCTools = tccTools,
                                                        OutputLevel = 2,
                                                        AppendToTCCName = FEContainerName                                                        
                                                    )
        components.addEventAlgo(chargedtccAlg)

    if(doCombined):
        print("TCC: Combined mode not setup")
        exit(0)
    if((not doCombined) and (not doCharged) and (not doNeutral)):
        print("runTCCReco: no mode set for input, this is an unexpected state")
        exit(0)


    return components


def runUFOReconstruction( constits, configFlags, caloClusterName="CaloCalTopoClusters", trackParticleName="InDetTrackParticles",
                         assocPostfix="TCC", ):
    """Create a TrackCaloCluster collection from PFlow and tracks (PFO retrieved from PFOPrefix and tracks directly from trackParticleName). 
    This functions schedules 2 UFO specific algs : 
       * a TrackCaloClusterInfoUFOAlg to build the TrackCaloClusterInfo object
       * a TrackCaloClusterAlg to build the UFO
    """

    from JetRecConfig.JetRecConfig import JetInputCfg

    components = JetInputCfg(constits, configFlags)
    constitAlg = components.getEventAlgos()[0]
    PFOPrefix = constitAlg.Tools[0].OutputContainer
    
    
    components.merge(
        setupTrackCaloAssoc(configFlags, caloClusterName, trackParticleName, assocPostfix, onlyPV0Tracks=True)
    )

        

    tccInfoAlg = CompFactory.TrackCaloClusterInfoUFOAlg("UFOInfoAlg_"+PFOPrefix,
                                            TCCInfoName = PFOPrefix+"UFOInfo",
                                            InputTrackCaloAssoc = trackParticleName+"ClusterAssociationsTCC",
                                            InputTracks = trackParticleName,
                                            InputClusters = caloClusterName,
                                            VertexContainer = "PrimaryVertices",
                                            TrackVertexAssoTool = setupTrackVertexAssocTool(), 
                                            PFOPrefix = PFOPrefix,
                                            ClusterECut = 0.,
    )

    components.addEventAlgo( tccInfoAlg) 

    tccUFO = CompFactory.UFOTool("UFOtool",
                                 TrackVertexAssoTool = setupTrackVertexAssocTool(), 
                                 PFOPrefix = PFOPrefix,
                                 ClusterECut = tccInfoAlg.ClusterECut,                     
                                 )

    tccAlg = CompFactory.TrackCaloClusterAlg(name = "TrackCaloClusterAlgUFO"+PFOPrefix,
                                 OutputTCCName = "UFO"+PFOPrefix,
                                 TCCInfo = tccInfoAlg.TCCInfoName,
                                 TCCTools = [tccUFO,]
    )
    components.addEventAlgo( tccAlg)
    return components

