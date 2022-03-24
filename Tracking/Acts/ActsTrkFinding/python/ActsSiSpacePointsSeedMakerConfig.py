#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator 
from AthenaConfiguration.ComponentFactory import CompFactory
from ActsTrkFinding.ActsSeedingToolConfig import ActsSeedingToolCfg

def  ActsSiSpacePointsSeedMakerCfg(ConfigFlags,
                                   name: str = 'ActsSiSpacePointsSeedMaker',
                                   InputCollections: list = None,
                                   **options) -> ComponentAccumulator:
    assert isinstance(name, str)
    if InputCollections is not None:
        assert isinstance(InputCollections, list)

    acc = ComponentAccumulator()

    options['name'] = name

    # Main properties
    options.setdefault('SpacePointsPixelName', 'ITkPixelSpacePoints')
    options.setdefault('SpacePointsStripName', 'ITkStripSpacePoints')
    options.setdefault('SpacePointsOverlapName', 'ITkOverlapSpacePoints')
    options.setdefault('usePixel', True)
    options.setdefault('useStrip', True)
    options.setdefault('useOverlapSpCollection', True)

    if len(InputCollections) > 0 and ConfigFlags.ITk.Tracking.ActivePass.usePrdAssociationTool:
        # not all classes have that property !!!
        options.setdefault('PRDtoTrackMap', 'ITkPRDtoTrackMap'+ ConfigFlags.ITk.Tracking.ActivePass.extension)

    # Acts Seed Tools
    # Do not overwrite if already present in `options`
    seedTool_pixel = None
    if 'SeedToolPixel' not in options:
        seedTool_pixel = acc.popToolsAndMerge(ActsSeedingToolCfg(ConfigFlags, 
                                                                 configuration = 'Pixel'))

    seedTool_strip = None
    if 'SeedToolStrip' not in options:
        seedTool_strip = acc.popToolsAndMerge(ActsSeedingToolCfg(ConfigFlags, 
                                                                 configuration = 'Strip'))

    options.setdefault('SeedToolPixel', seedTool_pixel)
    options.setdefault('SeedToolStrip', seedTool_strip)

    # Validation
    if ConfigFlags.ITk.Tracking.writeSeedValNtuple:
        options.setdefault('WriteNtuple', True)
        HistService = CompFactory.THistSvc(Output = ["valNtuples DATAFILE='SeedMakerValidation.root' OPT='RECREATE'"])
        acc.addService(HistService)

    acc.setPrivateTools(CompFactory.ActsTrk.ActsSiSpacePointsSeedMaker(**options))
    return acc
