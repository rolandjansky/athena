# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
# TODO: I haven't yet migrated any of the new control flags so this is essentially hardcoded to one configuration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from DetDescrCnvSvc.DetDescrCnvSvcConfig import DetDescrCnvSvcCfg
from AtlasGeoModel.GeoModelConfig import GeoModelCfg
from AthenaCommon.Logging import logging

log = logging.getLogger(__name__)


def JTowerMappingDataCondAlgCfg(ConfigFlags):
    """ Create the mapping maker for JTowers """
    result = ComponentAccumulator()
    result.merge(DetDescrCnvSvcCfg(ConfigFlags))
    result.merge(GeoModelCfg(ConfigFlags))
    result.addCondAlgo(
        CompFactory.LVL1.JTowerMappingDataCondAlg(
            "JTowerMappingDataCondAlg",
            MappingData="JTowerMappingData",
            MapTileCells=False,  # TODO: FLAGS
        ),
        primary=True,
    )
    return result


def GTowerMappingDataCondAlgCfg(ConfigFlags):
    """ Create the mapping maker for GTowers """
    result = ComponentAccumulator()
    result.merge(DetDescrCnvSvcCfg(ConfigFlags))
    result.merge(GeoModelCfg(ConfigFlags))
    result.addCondAlgo(
        CompFactory.LVL1.GTowerMappingDataCondAlg(
            "GTowerMappingDataCondAlg",
            MappingData="GTowerMappingData",
            MapTileCells=False,  # TODO: FLAGS
        ),
        primary=True,
    )
    return result


def JTowerBuilderCfg(ConfigFlags, superCellsIn):
    """ Create the JTower building algorithm """
    result = ComponentAccumulator()
    result.merge(JTowerMappingDataCondAlgCfg(ConfigFlags))
    # TODO: FLAGS
    result.addEventAlgo(
        CompFactory.LVL1.JGTowerBuilder(
            "JTowerBuilder",
            UseSCQuality=True,
            UseAllCalo=False,
            InputSuperCells=superCellsIn,
            EmulateSuperCellTiming=False,
            MinSCETp=-1,
            MaxSCETm=1,
            MinTowerET=-9e9,
            MappingData="JTowerMappingData",
            OutputTowers="JTowers",
        ),
        primary=True,
    )
    return result


def GTowerBuilderCfg(ConfigFlags, superCellsIn):
    """ Create the GCaloTower building algorithm """
    result = ComponentAccumulator()
    result.merge(GTowerMappingDataCondAlgCfg(ConfigFlags))
    # TODO: FLAGS
    result.addEventAlgo(
        CompFactory.LVL1.JGTowerBuilder(
            "GTowerBuilder",
            UseSCQuality=True,
            UseAllCalo=False,
            InputSuperCells=superCellsIn,
            EmulateSuperCellTiming=False,
            MinSCETp=-1,
            MaxSCETm=1,
            MinTowerET=-9e9,
            MappingData="GTowerMappingData",
            OutputTowers="GCaloTowers",
        ),
        primary=True,
    )
    return result


def JTowersCfg(ConfigFlags, superCellsIn):
    """Create the algorithms required for the JTower configuration

    Sequences the builder and the noise algorithm
    """
    result = ComponentAccumulator()
    result.merge(JTowerBuilderCfg(ConfigFlags, superCellsIn))
    result.addEventAlgo(
        CompFactory.LVL1.JGTowerNoiseAlg(
            "JTowerNoiseAlg", InputTowers="JTowers", DoJFEX=True
        )
    )
    return result


def GTowersCfg(ConfigFlags, superCellsIn):
    """Create the algorithms required for the GTower configuration

    Sequences the GCaloTower builder, then the GTower and noise algorithms
    """
    result = ComponentAccumulator()
    result.merge(GTowerBuilderCfg(ConfigFlags, superCellsIn))
    result.addEventAlgo(
        CompFactory.LVL1.GTowersFromGCaloTowers(
            "GTowersFromGCaloTowers",
            InputTowers="GCaloTowers",
            OutputTowers="GTowers",
            MappingData="GTowerMappingData",
        )
    )
    result.addEventAlgo(
        CompFactory.LVL1.JGTowerNoiseAlg(
            "GTowerNoiseAlg",
            InputTowers="GTowers",
            DoJFEX=False,
        )
    )
    return result


def AllJGTowerContainersCfg(ConfigFlags, superCellsIn):
    """ Helper method to create all of the J/G tower containers commonly used """
    result = ComponentAccumulator()

    # JTowers
    result.merge(JTowersCfg(ConfigFlags, superCellsIn=superCellsIn))
    result.addEventAlgo(
        CompFactory.LVL1.JTowerRhoSubtractionAlg(
            "JTowerRhoSubtractionAlg",
            InputTowers="JTowers",
            OutputTowers="JTowerRhoSubtracted",
            OutputRho="JFEXRho",
        )
    )

    # GTowers
    result.merge(GTowersCfg(ConfigFlags, superCellsIn))
    result.addEventAlgo(
        CompFactory.LVL1.GTowerRhoSubtractionAlg(
            "GTowerRhoSubtractionAlg",
            InputTowers="GTowers",
            OutputTowers="GTowerRhoSubtracted",
            OutputRho="GFEXRho",
        )
    )
    return result