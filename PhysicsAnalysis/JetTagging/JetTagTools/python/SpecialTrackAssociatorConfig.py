# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# import the TrackToVertexIPEstimator configurable
CP__TrackVertexAssociationTool=CompFactory.CP.TrackVertexAssociationTool

def SpecialTrackAssociatorCfg( name = 'SpecialTrackAssociator', PrimaryVertexCollectionName="", useBTagFlagsDefaults = True, **options ):
    """Sets up a SpecialTrackAssociator tool and returns it.

    The following options have BTaggingFlags defaults:

    input:             name: The name of the tool (should be unique).
          useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool."""
    acc = ComponentAccumulator()
    if useBTagFlagsDefaults:
        defaults = { 'WorkingPoint'           : 'Loose',
                   }
    for option in defaults:
        options.setdefault(option, defaults[option])
    options['name'] = name
    options['VertexContainer'] = PrimaryVertexCollectionName
    acc.setPrivateTools(CP__TrackVertexAssociationTool( **options))
 
    return acc
