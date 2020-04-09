# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

Analysis__ParticleToJetAssociator=CompFactory.Analysis.ParticleToJetAssociator

def BTagTrackToJetAssociatorCfg(flags, name, options = {}):

    """Sets up a BTagTrackToJetAssociator tool and returns it.

    The following options have BTaggingFlags defaults:

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""

    acc=ComponentAccumulator()
    options = dict(options)
    options['name'] = name
    acc.setPrivateTools(Analysis__ParticleToJetAssociator(**options))

    return acc
