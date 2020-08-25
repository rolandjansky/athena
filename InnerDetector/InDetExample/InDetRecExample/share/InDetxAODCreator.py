from InDetRecExample.InDetJobProperties import InDetFlags
from InDetRecExample.InDetKeys import InDetKeys

def getCollectionNameIfInFile(coll_type,coll_name) :
    from RecExConfig.AutoConfiguration import IsInInputFile
    if not IsInInputFile(coll_type,coll_name) :
        printfunc ('DEBUG getRecTrackParticleNameIfInFile set %s' % coll_name)
        return coll_name
    else :
        return ""

def passCollectionName(coll_name,condition) :
    return coll_name if condition else ""

def getRecTrackParticleNameIfInFile(coll_name) :
    return getCollectionNameIfInFile('Rec::TrackParticle',coll_name)

def getRecVertexNameIfInFile(coll_name) :
    return getCollectionNameIfInFile('Rec::Vertex',coll_name)


from AthenaCommon.GlobalFlags import globalflags
is_mc = (globalflags.DataSource == 'geant4')

doCreation = ( InDetFlags.doNewTracking() or InDetFlags.doPseudoTracking() or InDetFlags.doLargeD0() or InDetFlags.doR3LargeD0() \
                   or InDetFlags.doLowPtLargeD0() )  and InDetFlags.doParticleCreation()
doConversion = not InDetFlags.doNewTracking()  and not InDetFlags.doPseudoTracking() and not InDetFlags.doLargeD0() and not InDetFlags.doR3LargeD0()\
                    and not InDetFlags.doLowPtLargeD0() and InDetFlags.doParticleConversion()

if doCreation:
    printfunc ("Creating xAOD::TrackParticles from Trk::Tracks")
if doConversion:
    printfunc ("Converting Rec::TrackParticles to xAOD::TrackParticles")



# Run the xAOD truth builder for PU if needed
if InDetFlags.doSplitReco()  and is_mc:
    from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
    xAODTruthCnvPU = xAODMaker__xAODTruthCnvAlg("xAODTruthCnvPU")
    xAODTruthCnvPU.WriteInTimePileUpTruth = False
    xAODTruthCnvPU.WriteAllPileUpTruth = True
    xAODTruthCnvPU.AODContainerName = "GEN_EVENT_PU"
    xAODTruthCnvPU.xAODTruthEventContainerName = "TruthEvents_PU" #output
    xAODTruthCnvPU.xAODTruthPileupEventContainerName = "TruthPileupEvents_PU" #output
    xAODTruthCnvPU.xAODTruthParticleContainerName = "TruthParticles_PU" #output
    xAODTruthCnvPU.xAODTruthVertexContainerName = "TruthVertices_PU" #output
    xAODTruthCnvPU.TruthLinks = "xAODTruthLinks_PU" #output/intermediate
    xAODTruthCnvPU.MetaObjectName = "TruthMetaData_PU" #output
    topSequence += xAODTruthCnvPU

def getInDetxAODParticleCreatorTool(prd_to_track_map=None, suffix="") :
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc,'InDetxAODParticleCreatorTool'+suffix) :
        return getattr(ToolSvc,'InDetxAODParticleCreatorTool')

    _perigee_expression=InDetFlags.perigeeExpression()
    # need to treat Vertex specifically because at the time of
    # the track particle creation the primary vertex does not yet exist.
    # The problem is solved by first creating track particles wrt. the beam line
    # and correcting the parameters after the vertex finding.
    if _perigee_expression == 'Vertex' :
        _perigee_expression = 'BeamLine'

    from InDetRecExample                import TrackingCommon as TrackingCommon
    from InDetRecExample.TrackingCommon import setDefaults
    if prd_to_track_map is None :
        track_summary_tool = TrackingCommon.getInDetTrackSummaryToolSharedHits()
    else :
        prop_args          = setDefaults({}, nameSuffix = suffix)
        asso_tool          = TrackingCommon.getConstPRD_AssociationTool(**setDefaults(prop_args,
                                                                                      PRDtoTrackMap = prd_to_track_map))
        helper_tool        = TrackingCommon.getInDetSummaryHelperSharedHits(**setDefaults(prop_args,
                                                                                          AssoTool = asso_tool) )
        track_summary_tool = TrackingCommon.getInDetTrackSummaryToolSharedHits(**setDefaults(prop_args,
                                                                                             InDetSummaryHelperTool=helper_tool))

    from TrkParticleCreator.TrkParticleCreatorConf import Trk__TrackParticleCreatorTool
    InDetxAODParticleCreatorTool = Trk__TrackParticleCreatorTool(name = "InDetxAODParticleCreatorTool"+suffix,
                                                                 Extrapolator            = InDetExtrapolator,
                                                                 TrackSummaryTool        = track_summary_tool,
                                                                 BadClusterID            = InDetFlags.pixelClusterBadClusterID(),
                                                                 KeepParameters          = True,
                                                                 KeepFirstParameters     = InDetFlags.KeepFirstParameters(),
                                                                 PerigeeExpression       = _perigee_expression)

    ToolSvc += InDetxAODParticleCreatorTool
    if InDetFlags.doPrintConfigurables():
        printfunc (InDetxAODParticleCreatorTool)
    return InDetxAODParticleCreatorTool


def isValid(name) :
    return name is not None and name != ""

def createTrackParticles(track_in, track_particle_truth_in,track_particle_out, topSequence, prd_to_track_map=None, suffix="") :
    '''
    create algorithm to convert the input tracks into track xAOD track particles.
    @param track_in the name of the input TrackCollection
    @param track_particle_truth_in optional truth track collection to link to
    @param track_particle_out the name of the output xAOD track particle collection
    @param topSequence the sequence to which the algorithm is added
    @param prd_to_track_map None or if shared hits are to be recomputed a PRDtoTrackMap filled by a preceding
           algorithms e.g. a TrackCollectionMerger.
    @param suffix which makes the names of the particle creator tool and sub-tools unique in case a prd_to_track_map
           is provided.
    '''
    if isValid(track_in) and isValid(track_particle_out) :
        from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
        xAODTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg(track_particle_out)
        xAODTrackParticleCnvAlg.xAODContainerName = ""
        xAODTrackParticleCnvAlg.xAODTrackParticlesFromTracksContainerName = track_particle_out
        xAODTrackParticleCnvAlg.TrackContainerName = track_in
        xAODTrackParticleCnvAlg.TrackParticleCreator = getInDetxAODParticleCreatorTool(prd_to_track_map, suffix)
        xAODTrackParticleCnvAlg.AODContainerName = ""
        xAODTrackParticleCnvAlg.AODTruthContainerName = ""
        xAODTrackParticleCnvAlg.ConvertTrackParticles = False
        xAODTrackParticleCnvAlg.ConvertTracks = True
        xAODTrackParticleCnvAlg.AddTruthLink = InDetFlags.doTruth() and is_mc and isValid(track_particle_truth_in)
        xAODTrackParticleCnvAlg.xAODTruthLinkVector =  passCollectionName( 'xAODTruthLinks', InDetFlags.doTruth() and is_mc and isValid(track_particle_truth_in) )
        xAODTrackParticleCnvAlg.TrackTruthContainerName = passCollectionName(track_particle_truth_in,(is_mc and InDetFlags.doTruth()))
        from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier
        xAODTrackParticleCnvAlg.MCTruthClassifier = MCTruthClassifier
        topSequence += xAODTrackParticleCnvAlg

def convertTrackParticles(aod_track_particles_in, track_particle_truth_in,track_particle_out, topSequence) :
    if isValid(aod_track_particles_in) and isValid(track_particle_out) :
        from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
        xAODTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg(track_particle_out)
        xAODTrackParticleCnvAlg.xAODContainerName = track_particle_out
        xAODTrackParticleCnvAlg.xAODTrackParticlesFromTracksContainerName = ""
        xAODTrackParticleCnvAlg.TrackContainerName =  ""
        xAODTrackParticleCnvAlg.TrackParticleCreator = getInDetxAODParticleCreatorTool()
        xAODTrackParticleCnvAlg.AODContainerName = aod_track_particles_in
        xAODTrackParticleCnvAlg.AODTruthContainerName = passCollectionName(track_particle_truth_in,(is_mc and InDetFlags.doTruth()) )
        xAODTrackParticleCnvAlg.ConvertTrackParticles = True
        xAODTrackParticleCnvAlg.ConvertTracks = False
        xAODTrackParticleCnvAlg.AddTruthLink = InDetFlags.doTruth() and is_mc and isValid(track_particle_truth_in)
        xAODTrackParticleCnvAlg.xAODTruthLinkVector =  passCollectionName( 'xAODTruthLinks', InDetFlags.doTruth() and is_mc and isValid(track_particle_truth_in) )
        xAODTrackParticleCnvAlg.TrackTruthContainerName = ""
        from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier
        xAODTrackParticleCnvAlg.MCTruthClassifier = MCTruthClassifier
        topSequence += xAODTrackParticleCnvAlg 

if (doCreation or doConversion):# or InDetFlags.useExistingTracksAsInput()) : <---- [XXX JDC Should we included this?
                                #                                                    problems appear when nothing should
                                #                                                    be done but
                                #                                                    useExistinTracksAsInput...
    # [XXX JDC: to deal with the MergedTracks case, the truth collections are
    #           defined in the InputTrackCollectionTruth variable. To be deprecated
    #           if finally there is no need of the special "MergedTrack" name
    if 'InputTrackCollectionTruth' not in dir():
        InputTrackCollectionTruth = InDetKeys.TracksTruth()
    if not InDetFlags.doDBMstandalone(): 
        if doCreation :
            createTrackParticles(InputTrackCollection, InputTrackCollectionTruth, InDetKeys.xAODTrackParticleContainer(),topSequence)
            from  InDetPhysValMonitoring.InDetPhysValJobProperties import InDetPhysValFlags
            from  InDetPhysValMonitoring.ConfigUtils import extractCollectionPrefix
            for col in InDetPhysValFlags.validateExtraTrackCollections() :
                prefix=extractCollectionPrefix(col)
                createTrackParticles(col,"", prefix+"TrackParticles", topSequence)
        if doConversion :
            convertTrackParticles(getRecTrackParticleNameIfInFile(InDetKeys.TrackParticles()),
                                  InDetKeys.TrackParticlesTruth() ,
                                  InDetKeys.xAODTrackParticleContainer(),
                                  topSequence)


    if (InDetFlags.doDBMstandalone() or InDetFlags.doDBM() ) and doCreation :
        # or instead of InDetKeys.DBMTracksTruth()  rather InDetKeys.DBMDetailedTracksTruth() ? 
        createTrackParticles( InDetKeys.xAODDBMTrackParticleContainer(), InDetKeys.DBMTracksTruth(), InDetKeys.xAODDBMTrackParticleContainer(),topSequence)

if not InDetFlags.doVertexFinding():
    if (not InDetFlags.doDBMstandalone() and
        not IsInInputFile ('xAOD::VertexContainer', InDetKeys.xAODVertexContainer()) and
        IsInInputFile ('VxContainer', InDetKeys.PrimaryVertices())) :
        if len(getRecVertexNameIfInFile(InDetKeys.PrimaryVertices()))>0 :
            from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__VertexCnvAlg
            xAODVertexCnvAlg = xAODMaker__VertexCnvAlg("VertexCnvAlg")
            xAODVertexCnvAlg.xAODContainerName = InDetKeys.xAODVertexContainer() 
            xAODVertexCnvAlg.AODContainerName = InDetKeys.PrimaryVertices()
            xAODVertexCnvAlg.TPContainerName = InDetKeys.xAODTrackParticleContainer()
            topSequence += xAODVertexCnvAlg

    if InDetFlags.doDBMstandalone() or InDetFlags.doDBM():
        if (IsInInputFile ('VxContainer', InDetKeys.PrimaryVertices())) :
            from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__VertexCnvAlg
            xAODVertexCnvAlgDBM = xAODMaker__VertexCnvAlg("VertexCnvAlgDBM")
            xAODVertexCnvAlgDBM.xAODContainerName = InDetKeys.xAODVertexContainer()
            xAODVertexCnvAlgDBM.AODContainerName = InDetKeys.PrimaryVertices()
            xAODVertexCnvAlgDBM.TPContainerName = InDetKeys.xAODDBMTrackParticleContainer()
            topSequence += xAODVertexCnvAlgDBM

#For forward tracks, no separate collection for ITK, since they are already merged
if (InDetFlags.doForwardTracks() and InDetFlags.doParticleCreation() and not InDetFlags.doSLHC()) or doConversion:
    if doCreation :
        createTrackParticles(InDetKeys.ResolvedForwardTracks(), InDetKeys.ResolvedForwardTracksTruth(), InDetKeys.xAODForwardTrackParticleContainer(),topSequence)
    if doConversion :
        convertTrackParticles(getRecTrackParticleNameIfInFile(InDetKeys.ResolvedForwardTrackParticles()),
                              InDetKeys.ResolvedForwardTrackParticlesTruth(),
                              InDetKeys.xAODForwardTrackParticleContainer(),
                              topSequence)

if InDetFlags.doPseudoTracking():
    if doCreation :
        createTrackParticles(InDetKeys.PseudoTracks(), InDetKeys.PseudoTracksTruth(), InDetKeys.xAODPseudoTrackParticleContainer(),topSequence)
    if doConversion :
        convertTrackParticles(getRecTrackParticleNameIfInFile(InDetKeys.PseudoTracks()),
                              InDetKeys.TrackParticlesTruth(),
                              InDetKeys.xAODPseudoTrackParticleContainer(),
                              topSequence)


if InDetFlags.doR3LargeD0() and InDetFlags.storeSeparateLargeD0Container():
    if doCreation :
        createTrackParticles(InDetKeys.ExtendedLargeD0Tracks(), InDetKeys.ExtendedLargeD0TracksTruth(), InDetKeys.xAODLargeD0TrackParticleContainer(),topSequence)
        
 
if InDetFlags.doTrackSegmentsPixel() and InDetFlags.doParticleCreation():
    if doCreation :
        createTrackParticles(InDetKeys.PixelTracks(), InDetKeys.PixelTracksTruth(), InDetKeys.xAODPixelTrackParticleContainer(),topSequence)
    if doConversion :
        convertTrackParticles(getRecTrackParticleNameIfInFile(InDetKeys.xAODPixelTrackParticleContainer()),
                              "",
                              InDetKeys.xAODPixelTrackParticleContainer(),
                              topSequence)


if InDetFlags.doTrackSegmentsDisappearing() and InDetFlags.doParticleCreation():
    if doCreation :
        createTrackParticles(InDetKeys.DisappearingTracks(),
                             InDetKeys.DisappearingTracksTruth(),
                             InDetKeys.xAODDisappearingTrackParticleContainer(),
                             topSequence)


if InDetFlags.doTrackSegmentsSCT() and InDetFlags.doParticleCreation():
    if doCreation :
        createTrackParticles(InDetKeys.SCTTracks(),
                             "",
                             InDetKeys.xAODSCTTrackParticleContainer(),
                             topSequence)


if InDetFlags.doTrackSegmentsTRT() and InDetFlags.doParticleCreation():
    if doCreation :
        if InDetFlags.doCosmics() :
            # to reproduce shared hits of TRTTrackParticles stored in the ESD of q220
            createTrackParticles(InDetKeys.TRTTracks(),
                                 "",
                                 InDetKeys.xAODTRTTrackParticleContainer(),
                                 topSequence,
                                 ("PRDtoTrackMap" + InDetKeys.UnslimmedTracks()),
                                 "TRTTracks")
        else :
            createTrackParticles(InDetKeys.TRTTracks(),
                                 "",
                                 InDetKeys.xAODTRTTrackParticleContainer(),
                                 topSequence)

if InDetFlags.doStoreTrackSeeds() and InDetFlags.doParticleCreation():
    if doCreation :
        createTrackParticles(InDetKeys.SiSPSeedSegments(),
                             "",
                             InDetKeys.SiSPSeedSegments()+"TrackParticle",
                             topSequence)
