#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def metaDataKey():
    '''
    Meta data key to store the file source on which the InDet decoration alg has been running.
    '''
    return 'InDetPhysValDecoration'

def monManName():
    '''
    Name of the monitoring manager
    '''
    return 'PhysValMonManager'

def canAddDecorator(flags):
    '''
    check whether the decorator can be added.

    A decorator can be added if a track particle converter alg is in the sequence or
    if ESDs or AODs are read.
    '''

    if not (flags.Detector.GeometryID or flags.Detector.GeometryITk):
        return False

    return flags.PhysVal.IDPVM.runDecoration and ("StreamESD" in flags.Input.ProcessingTags
                                                  or "StreamAOD" in flags.Input.ProcessingTags
                                                  or "StreamDAOD" in flags.Input.ProcessingTags[0]) # Look for substring StreamDAOD in first processing tag to cover all DAOD flavors

    '''
    if rec.readTAG:
        return False

    if rec.readRDO:
        try:
            from AthenaCommon.AlgSequence import AlgSequence
            topSequence = AlgSequence()
            import re
            pat = re.compile('.*(TrackParticleCnvAlg).*')
            for alg in topSequence.getChildren():
                if pat.match(alg.getFullName()) is not None:
                    return True

        except Exception:
            pass

    return False
    '''


def createExtendNameIfNotDefaultCfg(alg,
                                    check_prop,
                                    def_val,
                                    kwargs):
    acc = ComponentAccumulator()

    def_name = alg.getType() if hasattr(alg, 'getType') else alg.__class__.__name__
    the_name = kwargs.pop('name', def_name)
    if check_prop in kwargs and kwargs[check_prop] != def_val:
        if the_name == def_name:
            the_name += '_'
            the_name += kwargs[check_prop]
    elif check_prop not in kwargs:
        kwargs.setdefault(check_prop, def_val)

    cfg_alg = alg(**kwargs)
    acc.addEventAlgo(cfg_alg)

    return acc

def InDetPhysHitDecoratorAlgCfg(flags, **kwargs):
    if flags.Detector.GeometryITk:
        return ITkPhysHitDecoratorAlgCfg(flags, **kwargs)

    '''
    create decoration algorithm which decorates track particles with the unbiased hit residuals and pulls.
    If the collection name TrackParticleContainerName is specified and differs from the default, the name
    of the algorithm will be extended by the collection name
    '''
    acc = ComponentAccumulator()

    if 'InDetTrackHoleSearchTool' not in kwargs:
        from InDetConfig.InDetTrackHoleSearchConfig import InDetTrackHoleSearchToolCfg
        InDetTrackHoleSearchTool = acc.popToolsAndMerge(InDetTrackHoleSearchToolCfg(flags))
        acc.addPublicTool(InDetTrackHoleSearchTool)
        kwargs.setdefault("InDetTrackHoleSearchTool", InDetTrackHoleSearchTool)

    if 'Updator' not in kwargs:
        from TrkConfig.TrkMeasurementUpdatorConfig import InDetUpdatorCfg
        Updator = acc.popToolsAndMerge(InDetUpdatorCfg(flags))
        acc.addPublicTool(Updator)
        kwargs.setdefault( "Updator", Updator )

    if 'LorentzAngleTool' not in kwargs:
        from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleToolCfg
        PixelLorentzAngleTool = acc.popToolsAndMerge(PixelLorentzAngleToolCfg(flags))
        kwargs.setdefault("LorentzAngleTool", PixelLorentzAngleTool )

    acc.merge(createExtendNameIfNotDefaultCfg(CompFactory.InDetPhysHitDecoratorAlg,
                                              'TrackParticleContainerName', 'InDetTrackParticles',
                                              kwargs))
    return acc

def ITkPhysHitDecoratorAlgCfg(flags, **kwargs):
    '''
    create decoration algorithm which decorates track particles with the unbiased hit residuals and pulls.
    If the collection name TrackParticleContainerName is specified and differs from the default, the name
    of the algorithm will be extended by the collection name
    '''
    acc = ComponentAccumulator()

    if 'InDetTrackHoleSearchTool' not in kwargs:
        from InDetConfig.InDetTrackHoleSearchConfig import ITkTrackHoleSearchToolCfg
        ITkTrackHoleSearchTool = acc.popToolsAndMerge(ITkTrackHoleSearchToolCfg(flags))
        acc.addPublicTool(ITkTrackHoleSearchTool)
        kwargs.setdefault("InDetTrackHoleSearchTool", ITkTrackHoleSearchTool)

    if 'Updator' not in kwargs:
        from TrkConfig.TrkMeasurementUpdatorConfig import ITkUpdatorCfg
        Updator = acc.popToolsAndMerge(ITkUpdatorCfg(flags))
        acc.addPublicTool(Updator)
        kwargs.setdefault("Updator", Updator )

    if 'LorentzAngleTool' not in kwargs:
        from SiLorentzAngleTool.ITkPixelLorentzAngleConfig import ITkPixelLorentzAngleToolCfg
        ITkPixelLorentzAngleTool = acc.popToolsAndMerge(ITkPixelLorentzAngleToolCfg(flags))
        kwargs.setdefault("LorentzAngleTool", ITkPixelLorentzAngleTool )

    acc.merge(createExtendNameIfNotDefaultCfg(CompFactory.InDetPhysHitDecoratorAlg,
                                              'TrackParticleContainerName', 'InDetTrackParticles',
                                              kwargs))
    return acc

def ParameterErrDecoratorAlgCfg(flags, **kwargs):
    '''
    create decoration algorithm which decorates track particles with the uncertainties of the track parameters.
    If the collection name TrackParticleContainerName is specified and differs from the default, the name
    of the algorithm will be extended by the collection name
    '''
    return createExtendNameIfNotDefaultCfg(CompFactory.ParameterErrDecoratorAlg,
                                           'TrackParticleContainerName', 'InDetTrackParticles',
                                           kwargs)
        

def InDetPhysValTruthDecoratorAlgCfg(flags, **kwargs):
    '''
    create decoration algorithm which decorates truth particles with track parameters at the perigee.
    If the collection name TruthParticleContainerName is specified and differs from the default, the name
    of the algorithm will be extended by the collection name
    '''
    acc = ComponentAccumulator()

    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    extrapolator = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
    acc.addPublicTool(extrapolator)  # TODO: migrate to private?
    kwargs.setdefault("Extrapolator", extrapolator)

    acc.merge(createExtendNameIfNotDefaultCfg(CompFactory.InDetPhysValTruthDecoratorAlg,
                                              'TruthParticleContainerName', 'TruthParticles',
                                              kwargs))
    return acc

def TruthClassDecoratorAlgCfg(flags, **kwargs):
    '''
    create decoration algorithm which decorates truth particles with origin and type from truth classifier.
    if the collection name TruthParticleContainerName is specified and differs from the default, the name
    of the algorithm will be extended by the collection name
    '''
    return createExtendNameIfNotDefaultCfg(CompFactory.TruthClassDecoratorAlg,
                                           'TruthParticleContainerName', 'TruthParticles',
                                           kwargs)

def TrackDecoratorsCfg(flags, **kwargs):
    '''
    Get track particle decorators needed for the InDetPhysValMonitoring tool
    If the collection name TrackParticleContainerName is specified and differs from the default, the name
    of the algorithms will be extended by the collection name.
    '''
    acc = ComponentAccumulator()

    if "CombinedInDetTracks" in flags.Input.Collections:
        acc.merge(InDetPhysHitDecoratorAlgCfg(flags,**kwargs))

    acc.merge(ParameterErrDecoratorAlgCfg(flags,**kwargs))

    return acc

def DBMTrackDecoratorsCfg(flags, **kwargs):
    kwargs.setdefault("TrackParticleContainerName","ResolvedDBMTracks")
    return TrackDecoratorsCfg(flags,**kwargs)

def GSFTrackDecoratorsCfg(flags, **kwargs):
    kwargs.setdefault("TrackParticleContainerName","GSFTrackParticles")
    return TrackDecoratorsCfg(flags,**kwargs)

def AddDecoratorCfg(flags,**kwargs):
    '''
    Add the track particle decoration algorithm to the top sequence.
    The algorithm is to be run on RAW/RDO since it depends on full hit information
    which is generally not available at later stages. The decorations added by this
    algorithm are used by InDetPhysValMonitoring tool.
    '''
    acc = ComponentAccumulator()

    acc.merge(TrackDecoratorsCfg(flags))
  
    if flags.Input.isMC:
        from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
        acc.merge(BeamSpotCondAlgCfg(flags))

        acc.merge(InDetPhysValTruthDecoratorAlgCfg(flags))

    if flags.PhysVal.IDPVM.doValidateGSFTracks:
        acc.merge(AddGSFTrackDecoratorAlgCfg(flags))

    if flags.PhysVal.IDPVM.doValidateDBMTracks and ("DBMTrackParticles" in flags.Input.Collections):
        acc.merge(DBMTrackDecoratorsCfg(flags))

    return acc


def AddGSFTrackDecoratorAlgCfg(flags,**kwargs):
    
    #Search egamma algorithm and add the GSF TrackParticle decorator after the it.
    acc = ComponentAccumulator()

    if flags.PhysVal.IDPVM.doValidateGSFTracks:
        # print ('DEBUG add addGSFTrackDecoratorAlg')

        acc.merge(GSFTrackDecoratorsCfg(flags))

        from  InDetPhysValMonitoring.ConfigUtils import extractCollectionPrefix
        for col in flags.PhysVal.IDPVM.validateExtraTrackCollections :
            prefix=extractCollectionPrefix(col)
            decorator = acc.popToolsAndMerge(TrackDecoratorsCfg(flags))
            decorator.TrackParticleContainerName=prefix+"TrackParticles"


def AddDecoratorIfNeededCfg(flags):
    '''
     Add the InDet decoration algorithm if it has not been ran yet.
    '''

    acc = ComponentAccumulator()

    if not canAddDecorator(flags):
        print('DEBUG addDecoratorIfNeeded ? Stage is too early or too late for running the decoration. Needs reconstructed tracks. Try again during next stage ?')
        return acc

    acc.merge(AddDecoratorCfg(flags))

    return acc


