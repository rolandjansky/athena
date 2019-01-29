# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# Common configuration functions setting up common tools
# These tools are used by virtually all other tools, be careful
# about modifying them
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaBTagCalibrationBrokerTool = { 'OneInTotal' : True }
# (Setting up this tool is handled outside the scope of the configuration, but we still need
# the meta data)

#----------------------------------------------------------------------

metaBTagLeptonTruthTool = { 'OneInTotal' : True }

def toolBTagLeptonTruthTool(name, useBTagFlagsDefaults = True, **options):
    """Sets up a BTagLeptonTruthTool tool and returns it.

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                             Note however that this tool has no BTaggingFlags defaults; the option is
                             here only for consistency.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    options['name'] = name
    from ParticleJetTools.ParticleJetToolsConf import Analysis__FindLeptonTruth
    return Analysis__FindLeptonTruth(**options)

#----------------------------------------------------------------------

metathisBTagLabeling = { 'OneInTotal'    : True,
                         'DependsOn'     : ['BTagJetTruthMatching',],
                         'PassByPointer' : {'JetTruthMatchTool' : 'BTagJetTruthMatching',
                                            } }

def toolthisBTagLabeling(name, useBTagFlagsDefaults = True, **options):
    """Sets up a BTagLabeling tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then be added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        btagJetTruthMatching = toolBTagJetTruthMatching('BTagJetTruthMatching')
        defaults = { 'OutputLevel'                : BTaggingFlags.OutputLevel,
                     'JetTruthMatchTool'          : btagJetTruthMatching }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from BTagging.BTaggingConf import Analysis__BTagLabeling
    return Analysis__BTagLabeling(**options)

#----------------------------------------------------------------------

metaBTagJetTruthMatching = { 'OneInTotal' : True }

def toolBTagJetTruthMatching(name, useBTagFlagsDefaults = True, **options):
    """Sets up a BTagJetTruthMatching( tool and returns it. There are actually two
    incarnations of this tool, toolJetQuarkLabel or tooltoolJetTrackTruthMatching depending
    on the options. The relevant option is 'subtype'.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then be added to ToolSvc via ToolSvc += output."""
    options.setdefault('subtype', 'matchQuark')
    if options['subtype'] == 'matchQuark':
        del options['subtype']
        return toolJetQuarkLabel('BTagJetTruthMatching', useBTagFlagsDefaults = useBTagFlagsDefaults, **options)
    elif options['jetTrackTruthMatching']:
        del options['subtype']
        return toolJetTrackTruthMatching('BTagJetTruthMatching', useBTagFlagsDefaults = useBTagFlagsDefaults, **options)
    else:
        raise NotImplementedError, 'Unimplemented request for BTagJetTruthMatching sub-type: '+options['subtype']

def toolJetQuarkLabel(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetQuarkLabel tool and returns it.

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                             Note however that this tool has no BTaggingFlags defaults; the option is
                             here only for consistency.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then be added to ToolSvc via ToolSvc += output."""
    options['name'] = name
    from ParticleJetTools.ParticleJetToolsConf import Analysis__JetQuarkLabel
    return Analysis__JetQuarkLabel(**options)

def toolJetTrackTruthMatching(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetTrackTruthMatching tool and returns it.

    The following options have BTaggingFlags defaults:

    TrackParticlesName                  default: BTaggingFlags.TrackParticleCollectionName
    TrackParticleTruthMapName           default: BTaggingFlags.TrackParticleTruthCollectionName

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then be added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'TrackParticlesName'         : BTaggingFlags.TrackParticleCollectionName,
                     'TrackParticleTruthMapName'  : BTaggingFlags.TrackParticleTruthCollectionName }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from ParticleJetTools.ParticleJetToolsConf import Analysis__JetTrackTruthMatching
    return Analysis__JetTrackTruthMatching(**options)

#----------------------------------------------------------------------

metaBTagTrackToVertexIPEstimator = { 'OneInTotal'    : True,
                                     #'DependsOn'     : ['AtlasExtrapolator',
                                     #                   'BTagFullLinearizedTrackFactory'],
                                     #'PassByPointer' : {'Extrapolator'           : 'AtlasExtrapolator',
                                     #                   'LinearizedTrackFactory' : 'BTagFullLinearizedTrackFactory'} }
                                   }

def toolBTagTrackToVertexIPEstimator(name, useBTagFlagsDefaults = True, **options):
    """Sets up a TrackToVertexIPEstimator tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then be added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        btagFullLinearizedTrackFactory = toolBTagFullLinearizedTrackFactory('FullLinearizedTrkFactory')
        atlasExtrapolator = toolAtlasExtrapolator('AtlasExtrapolator')
        defaults = { 'OutputLevel' : BTaggingFlags.OutputLevel,
                     'Extrapolator' : atlasExtrapolator,
                     'LinearizedTrackFactory' : btagFullLinearizedTrackFactory, }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__TrackToVertexIPEstimator
    return Trk__TrackToVertexIPEstimator(**options)

#----------------------------------------------------------------------

metaAtlasExtrapolator = { 'OneInTotal' : True }

def toolAtlasExtrapolator(name, useBTagFlagsDefaults = True, **options):
    """Sets up a AtlasExtrapolator tool and returns it.

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                             (this function does not have any though, just here for consistency).
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then be added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = {}
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
    return AtlasExtrapolator(**options)

#----------------------------------------------------------------------

metaBTagTrackToVertexTool = { 'OneInTotal'    : True,
                              'DependsOn'     : ['AtlasExtrapolator',],
                              'PassByPointer' : {'Extrapolator' : 'AtlasExtrapolator'} }

def toolBTagTrackToVertexTool(name, useBTagFlagsDefaults = True, **options):
    """Sets up a BTagTrackToVertexTool tool and returns it.

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                             Note however that this tool has no BTaggingFlags defaults; the option is
                             here only for consistency.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then be added to ToolSvc via ToolSvc += output."""
    options['name'] = name
    atlasExtrapolator = toolAtlasExtrapolator('AtlasExtrapolator')
    options.setdefault('Extrapolator', atlasExtrapolator)
    from TrackToVertex.TrackToVertexConf import Reco__TrackToVertex
    return Reco__TrackToVertex(**options)

#----------------------------------------------------------------------

metaBTagFullLinearizedTrackFactory = { 'OneInTotal'    : True,
                                       #'DependsOn'     : ['AtlasExtrapolator',],
                                       #'PassByPointer' : {'Extrapolator' : 'AtlasExtrapolator'}
                                     }

def toolBTagFullLinearizedTrackFactory(name, useBTagFlagsDefaults = True, **options):
    """Sets up a BTagFullLinearizedTrackFactory tool and returns it.

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                             Note however that this tool has no BTaggingFlags defaults; the option is
                             here only for consistency.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then be added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        atlasExtrapolator = toolAtlasExtrapolator('AtlasExtrapolator')
        defaults = { 'Extrapolator'            : atlasExtrapolator}
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__FullLinearizedTrackFactory
    return Trk__FullLinearizedTrackFactory(**options)
