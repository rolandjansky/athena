# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# import the MSVVariablesFactory configurable
Analysis__MSVVariablesFactory=CompFactory.Analysis__MSVVariablesFactory

def MSVVariablesFactoryCfg(name, **options):
    """Sets up a MSVVariablesFactory tool and returns it.

    The following options have BTaggingFlags defaults:

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output. Note however
    that this tool also needs to be added to the main B-tagging tool."""
    acc = ComponentAccumulator()
    options['name'] = name
    acc.setPrivateTools(Analysis__MSVVariablesFactory(**options))

    return acc
