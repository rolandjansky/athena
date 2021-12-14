# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# New configuration for ATLAS extrapolator
# Based heavily on AtlasExtrapolator.py

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
import TrkConfig.AtlasExtrapolatorToolsConfig as TC

# define the class
def AtlasExtrapolatorCfg(flags, name='AtlasExtrapolator'):
    result = ComponentAccumulator()

    acc = MagneticFieldSvcCfg(flags)
    result.merge(acc)

    # PROPAGATOR DEFAULTS --------------------------------------------------

    AtlasRungeKuttaPropagator = result.getPrimaryAndMerge(
        TC.AtlasRKPropagatorCfg(flags))
    AtlasSTEP_Propagator = result.getPrimaryAndMerge(
        TC.AtlasSTEP_PropagatorCfg(flags))
    ITkPropagator = None
    if flags.Detector.GeometryITk:
        ITkPropagator = result.getPrimaryAndMerge(
            TC.ITkPropagatorCfg(flags))

    AtlasPropagators = []
    AtlasPropagators += [AtlasRungeKuttaPropagator]
    AtlasPropagators += [AtlasSTEP_Propagator]
    if flags.Detector.GeometryITk:
        AtlasPropagators += [ITkPropagator]

    # UPDATOR DEFAULTS --------------------------------------------------

    AtlasMaterialEffectsUpdator = result.getPrimaryAndMerge(
        TC.AtlasMaterialEffectsUpdatorCfg(flags))
    AtlasMaterialEffectsUpdatorLandau = result.getPrimaryAndMerge(
        TC.AtlasMaterialEffectsUpdatorLandauCfg(flags))
    ITkMaterialEffectsUpdator = None
    if flags.Detector.GeometryITk:
        ITkMaterialEffectsUpdator = result.getPrimaryAndMerge(
            TC.ITkMaterialEffectsUpdatorCfg(flags))

    AtlasUpdators = []
    AtlasUpdators += [AtlasMaterialEffectsUpdator]
    AtlasUpdators += [AtlasMaterialEffectsUpdatorLandau]
    if flags.Detector.GeometryITk:
        AtlasUpdators += [ITkMaterialEffectsUpdator]

    AtlasNavigator = result.getPrimaryAndMerge(TC.AtlasNavigatorCfg(flags))

    # CONFIGURE PROPAGATORS/UPDATORS ACCORDING TO GEOMETRY SIGNATURE

    AtlasSubPropagators = []
    AtlasSubPropagators += [AtlasRungeKuttaPropagator.name]  # Global
    if flags.Detector.GeometryITk:
        AtlasSubPropagators += [ITkPropagator.name]  # ITk
    else:
        AtlasSubPropagators += [AtlasRungeKuttaPropagator.name]  # ID
    AtlasSubPropagators += [AtlasSTEP_Propagator.name]  # BeamPipe
    AtlasSubPropagators += [AtlasSTEP_Propagator.name]  # Calo
    AtlasSubPropagators += [AtlasSTEP_Propagator.name]  # MS
    AtlasSubPropagators += [AtlasRungeKuttaPropagator.name]  # Cavern

    AtlasSubUpdators = []
    AtlasSubUpdators += [AtlasMaterialEffectsUpdator.name]  # Global
    if flags.Detector.GeometryITk:
        AtlasSubUpdators += [ITkMaterialEffectsUpdator.name]  # ITk
    else:
        AtlasSubUpdators += [AtlasMaterialEffectsUpdator.name]  # ID
    AtlasSubUpdators += [AtlasMaterialEffectsUpdator.name]  # BeamPipe
    AtlasSubUpdators += [AtlasMaterialEffectsUpdator.name]  # Calo
    AtlasSubUpdators += [AtlasMaterialEffectsUpdator.name]  # MS
    AtlasSubUpdators += [AtlasMaterialEffectsUpdator.name]  # Cavern

    # call the base class constructor
    Extrapolator = CompFactory.Trk.Extrapolator(name,
                                                Navigator=AtlasNavigator,
                                                MaterialEffectsUpdators=AtlasUpdators,
                                                Propagators=AtlasPropagators,
                                                SubPropagators=AtlasSubPropagators,
                                                SubMEUpdators=AtlasSubUpdators
                                                )

    result.setPrivateTools(Extrapolator)
    return result


# Based on Reconstruction/egamma/egammaTools/python/egammaExtrapolators.py
def egammaCaloExtrapolatorCfg(flags, name='egammaCaloExtrapolator'):
    result = ComponentAccumulator()

    egammaExtrapolator = result.popToolsAndMerge(
        AtlasExtrapolatorCfg(flags, name))

    RungeKuttaPropagator = result.getPrimaryAndMerge(
        TC.AtlasRKPropagatorCfg(flags))
    NoMatSTEP_Propagator = result.getPrimaryAndMerge(
        TC.AtlasNoMatSTEP_PropagatorCfg(flags))
    ITkPropagator = None
    if flags.Detector.GeometryITk:
        ITkPropagator = result.getPrimaryAndMerge(
            TC.ITkPropagatorCfg(flags))

    egammaPropagators = []
    egammaPropagators += [RungeKuttaPropagator]
    egammaPropagators += [NoMatSTEP_Propagator]
    if flags.Detector.GeometryITk:
        egammaPropagators += [ITkPropagator]

    MaterialEffectsUpdator = result.getPrimaryAndMerge(
        TC.AtlasMaterialEffectsUpdatorCfg(flags))
    NoElossMaterialEffectsUpdator = result.getPrimaryAndMerge(
        TC.AtlasNoElossMaterialEffectsUpdatorCfg(flags))
    ITkMaterialEffectsUpdator = None
    if flags.Detector.GeometryITk:
        ITkMaterialEffectsUpdator = result.getPrimaryAndMerge(
            TC.ITkMaterialEffectsUpdatorCfg(flags))

    egammaUpdators = []
    egammaUpdators += [MaterialEffectsUpdator]
    egammaUpdators += [NoElossMaterialEffectsUpdator]
    if flags.Detector.GeometryITk:
        egammaUpdators += [ITkMaterialEffectsUpdator]

    # CONFIGURE PROPAGATORS/UPDATORS ACCORDING TO GEOMETRY SIGNATURE

    egammaSubPropagators = []
    egammaSubPropagators += [RungeKuttaPropagator.name]  # Global
    if flags.Detector.GeometryITk:
        egammaSubPropagators += [ITkPropagator.name]  # ITk
    else:
        egammaSubPropagators += [RungeKuttaPropagator.name]  # ID
    # BeamPipe (default is STEP)
    egammaSubPropagators += [RungeKuttaPropagator.name]
    # Calo (default is STEP)
    egammaSubPropagators += [RungeKuttaPropagator.name]
    egammaSubPropagators += [NoMatSTEP_Propagator.name]  # MS (default is STEP)
    egammaSubPropagators += [RungeKuttaPropagator.name]  # Cavern

    egammaSubUpdators = []
    egammaSubUpdators += [MaterialEffectsUpdator.name]  # Global
    if flags.Detector.GeometryITk:
        egammaSubUpdators += [ITkMaterialEffectsUpdator.name]  # ID
    else:
        egammaSubUpdators += [MaterialEffectsUpdator.name]  # ID
    egammaSubUpdators += [MaterialEffectsUpdator.name]  # BeamPipe
    # Calo (default is Mat)
    egammaSubUpdators += [NoElossMaterialEffectsUpdator.name]
    # MS (default is Mat)
    egammaSubUpdators += [NoElossMaterialEffectsUpdator.name]
    egammaSubUpdators += [MaterialEffectsUpdator.name]  # Cavern

    egammaExtrapolator.MaterialEffectsUpdators = egammaUpdators
    egammaExtrapolator.SubMEUpdators = egammaSubUpdators
    egammaExtrapolator.Propagators = egammaPropagators
    egammaExtrapolator.SubPropagators = egammaSubPropagators
    # egamma STEP with no eloss for calo intersections
    egammaExtrapolator.STEP_Propagator = NoMatSTEP_Propagator

    result.setPrivateTools(egammaExtrapolator)
    return result


# Based on PhysicsAnalysis/MCTruthClassifier/python/MCTruthClassifierBase.py
def MCTruthClassifierExtrapolatorCfg(flags, name='MCTruthClassifierExtrapolator'):
    result = ComponentAccumulator()

    MCTruthExtrapolator = result.popToolsAndMerge(
        AtlasExtrapolatorCfg(flags, name))

    MCTruthUpdators = []

    NoElossMaterialEffectsUpdator = result.getPrimaryAndMerge(
        TC.AtlasNoElossMaterialEffectsUpdatorCfg(flags))
    MCTruthUpdators += [NoElossMaterialEffectsUpdator]

    MCTruthSubUpdators = []

    # -------------------- set it depending on the geometry ---------------------------
    MCTruthSubUpdators += [NoElossMaterialEffectsUpdator.name]  # Global
    MCTruthSubUpdators += [NoElossMaterialEffectsUpdator.name]  # ID
    MCTruthSubUpdators += [NoElossMaterialEffectsUpdator.name]  # beampipe
    MCTruthSubUpdators += [NoElossMaterialEffectsUpdator.name]  # calo
    MCTruthSubUpdators += [NoElossMaterialEffectsUpdator.name]  # MS
    MCTruthSubUpdators += [NoElossMaterialEffectsUpdator.name]  # cavern

    MCTruthExtrapolator.MaterialEffectsUpdators = MCTruthUpdators
    MCTruthExtrapolator.SubMEUpdators = MCTruthSubUpdators

    result.setPrivateTools(MCTruthExtrapolator)
    return result


def InDetExtrapolatorCfg(flags, name='InDetExtrapolator', **kwargs):
    result = ComponentAccumulator()

    # FIXME copied from the old config, also needs fixing on the c++ side.
    if 'Propagators' not in kwargs:
        InDetPropagator = result.getPrimaryAndMerge(
            TC.InDetPropagatorCfg(flags))
        Propagators = [InDetPropagator]
        kwargs.setdefault("Propagators", Propagators)

    propagator = kwargs.get('Propagators')[0].name if kwargs.get(
        'Propagators', None) is not None and len(kwargs.get('Propagators', None)) > 0 else None

    if 'MaterialEffectsUpdators' not in kwargs:
        InDetMaterialEffectsUpdator = result.getPrimaryAndMerge(
            TC.InDetMaterialEffectsUpdatorCfg(flags))
        MaterialEffectsUpdators = [InDetMaterialEffectsUpdator]
        kwargs.setdefault("MaterialEffectsUpdators", MaterialEffectsUpdators)
    material_updator = kwargs.get('MaterialEffectsUpdators')[0].name if kwargs.get(
        'MaterialEffectsUpdators', None) is not None and len(kwargs.get('MaterialEffectsUpdators', None)) > 0 else None

    if 'Navigator' not in kwargs:
        AtlasNavigator = result.getPrimaryAndMerge(TC.AtlasNavigatorCfg(flags, name="InDetNavigator"))
        kwargs.setdefault("Navigator", AtlasNavigator)

    sub_propagators = []
    sub_updators = []

    # -------------------- set it depending on the geometry --------------------------------
    # default for ID is (Rk,Mat)
    sub_propagators += [propagator]
    sub_updators += [material_updator]

    # default for Calo is (Rk,MatLandau)
    sub_propagators += [propagator]
    sub_updators += [material_updator]

    # default for MS is (STEP,Mat)
    #  sub_propagators += [ InDetStepPropagator.name() ]
    sub_updators += [material_updator]
    # @TODO should check that all sub_propagators and sub_updators are actually defined.

    kwargs.setdefault("SubPropagators", sub_propagators)
    kwargs.setdefault("SubMEUpdators", sub_updators)

    extrapolator = CompFactory.Trk.Extrapolator(name, **kwargs)
    result.addPublicTool(extrapolator, primary=True)
    return result


if __name__ == "__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.ComponentAccumulator import printProperties
    from AthenaCommon.Configurable import Configurable
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaCommon.Logging import logging
    Configurable.configurableRun3Behavior = True

    ConfigFlags.Input.Files = defaultTestFiles.RDO
    ConfigFlags.fillFromArgs()
    ConfigFlags.lock()
    ConfigFlags.dump()

    cfg = ComponentAccumulator()
    mlog = logging.getLogger("AtlasExtrapolatorConfigTest")
    mlog.info("Configuring AtlasExtrapolator : ")
    printProperties(mlog, cfg.popToolsAndMerge(
        AtlasExtrapolatorCfg(ConfigFlags)),
        nestLevel=1,
        printDefaults=True)
    mlog.info("Configuring egammaCaloExtrapolator : ")
    printProperties(mlog, cfg.popToolsAndMerge(
        egammaCaloExtrapolatorCfg(ConfigFlags)),
        nestLevel=1,
        printDefaults=True)
    mlog.info("Configuring MCTruthClassifierExtrapolator : ")
    printProperties(mlog, cfg.popToolsAndMerge(
        MCTruthClassifierExtrapolatorCfg(ConfigFlags)),
        nestLevel=1,
        printDefaults=True)

    f = open("atlasextrapolator.pkl", "wb")
    cfg.store(f)
    f.close()
