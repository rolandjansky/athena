# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

Analysis__ParticleToJetAssociator=CompFactory.Analysis.ParticleToJetAssociator

def BTagMuonToJetAssociatorCfg(flags, name, useBTagFlagsDefaults = True, **options):
    """Sets up a BTagMuonToJetAssociator tool and returns it.

    The following options have BTaggingFlags defaults:

    trackCone                           default: 1.0 (for the time being... has to be studied)
    useVariableSizedTrackCone           default: False

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    acc=ComponentAccumulator()

    if useBTagFlagsDefaults:
        defaults = {
                     'trackCone'                 : 1.0,
                     'useVariableSizedTrackCone' : False }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    acc.setPrivateTools(Analysis__ParticleToJetAssociator(**options))
 
    return acc
