# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# Configuration functions for DL1Tag
# Author: Marie Lanfermann (September 2015)
from BTagging.BTaggingFlags import BTaggingFlags
from BTagging.JetCollectionToTrainingMaps import postTagDL2JetToTrainingMap
from BTagging.JetCollectionToTrainingMaps import blacklistedJetCollections



def buildDL1(basename, calibrationName=''):
  if not calibrationName:
    calibrationName = basename

  metaInstance = { 'IsATagger'          : False,
                  'xAODBaseName'       : basename,
                  'DependsOn'          : ['AtlasExtrapolator',
                                          'BTagCalibrationBrokerTool',
                                          'BTagTrackToVertexTool'],
                  'CalibrationFolders' : [calibrationName],
                  'PassByPointer'      : {'calibrationTool' : 'BTagCalibrationBrokerTool'},
                  'ToolCollection'     : basename+'Tag'}

  def DL1Instance(name, useBTagFlagsDefaults = True, **options):
    """Sets up a DL1Tag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    taggerName                          default: "DL1"
    taggerNameBase                      default: "DL1"
    forceDL1CalibrationAlias            default: BTaggingFlags.ForceDL1CalibrationAlias
    DL1CalibAlias                       default: BTaggingFlags.DL1CalibAlias

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'forceDL1CalibrationAlias'         : BTaggingFlags.ForceDL1CalibrationAlias,
                     'DL1CalibAlias'                    : BTaggingFlags.DL1CalibAlias,
                     'calibration_directory'            : calibrationName,
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    options['LocalNNConfigurationFile'] = BTaggingFlags.DL1LocalNNConfig
    from JetTagTools.JetTagToolsConf import Analysis__DL1Tag
    vetoed = postTagDL2JetToTrainingMap.keys() + blacklistedJetCollections
    if BTaggingFlags.Do2019Retraining:
        options['vetoCollections'] = vetoed
    return Analysis__DL1Tag(**options)

  return DL1Instance, metaInstance

toolDL1rTag, metaDL1rTag = buildDL1("DL1r")
toolDL1Tag, metaDL1Tag = buildDL1("DL1")
toolDL1rmuTag, metaDL1rmuTag = buildDL1("DL1rmu")
toolDL1muTag, metaDL1muTag = buildDL1("DL1mu")
