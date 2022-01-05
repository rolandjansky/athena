#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

""" ComponentAccumulator equivalents for the functions in JetRecoSequences """

from .JetRecoConfiguration import (
    interpretRecoAlg,
    defineJets,
    defineTrackMods,
    getFilterCut,
    defineCalibMods,
    getDecorList,
    defineGroomedJets,
    defineReclusteredJets,
)
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from ..CommonSequences.FullScanDefs import fs_cells
from JetRecConfig import JetRecConfig
from JetRecConfig import JetInputConfig
from JetRecConfig.DependencyHelper import solveDependencies, solveGroomingDependencies

from JetRecTools import OnlineMon
from JetRec import JetOnlineMon
import copy
from TrigEDMConfig.TriggerEDMRun3 import recordable
from GaudiKernel import SystemOfUnits

from AthenaConfiguration.AccumulatorCache import AccumulatorCache

_jetNamePrefix = "HLT_"


@AccumulatorCache
def JetRecoCfg(flags, clustersKey, trkcolls=None, **jetRecoDict):
    """The top-level sequence

    Forwards arguments to the standard jet reco, grooming or reclustering sequences.
    If tracking is required then the trkcolls dict should be supplied
    """

    jetalg, jetradius, extra = interpretRecoAlg(jetRecoDict["recoAlg"])
    dataSource = "mc" if flags.Input.isMC else "data"

    if extra == "r":
        return ReclusteredJetRecoCfg(
            flags, dataSource, clustersKey, trkcolls, **jetRecoDict
        )
    elif extra in ["t", "sd"]:
        return GroomedJetRecoCfg(
            flags, dataSource, clustersKey, trkcolls, **jetRecoDict
        )
    else:
        return StandardJetRecoCfg(
            flags, dataSource, clustersKey, trkcolls, **jetRecoDict
        )


@AccumulatorCache
def StandardJetBuildCfg(flags, dataSource, clustersKey, trkcolls=None, **jetRecoDict):
    """ Standard jet reconstruction, no reclustering or grooming 
    
    The clusters (and tracks, if necessary) should be built beforehand and passed into this config,
    but this config will build the PFOs if they are needed.

    The configuration then builds the jet definition, applies any required constituent modifiers
    and creates the JetRecAlg
    """

    acc = ComponentAccumulator()
    use_tracking = jetRecoDict["trkopt"] != "notrk"
    if use_tracking and not trkcolls:
        raise ValueError(
            f"No track collections supplied for trkopt {jetRecoDict['trkopt']}"
        )

    is_pflow = jetRecoDict["constitType"] == "pf"

    # Add PFlow reconstruction if necessary
    if is_pflow:
        if not use_tracking:
            raise ValueError("PFlow jet chain requested with no tracking option!")
        from eflowRec.PFHLTConfig import PFCfg

        acc.merge(
            PFCfg(
                flags,
                jetRecoDict["trkopt"],
                clustersin=clustersKey,
                calclustersin="",
                tracksin=trkcolls["Tracks"],
                verticesin=trkcolls["Vertices"],
                cellsin=fs_cells,
            )
        )
        jetDef = defineJets(
            jetRecoDict,
            pfoPrefix=f"HLT_{jetRecoDict['trkopt']}",
            prefix=_jetNamePrefix,
        )
    else:
        jetDef = defineJets(
            jetRecoDict,
            clustersKey=clustersKey,
            prefix=_jetNamePrefix,
        )

    # Sort and filter
    jetDef.modifiers = [
        "Sort",
        "Filter:{}".format(getFilterCut(jetRecoDict["recoAlg"])),
        "ConstitFourMom_copy",
    ]
    if jetRecoDict["recoAlg"] == "a4":
        jetDef.modifiers += ["CaloEnergies"]  # needed for GSC
    if use_tracking:
        jetDef.modifiers += defineTrackMods(jetRecoDict["trkopt"])
        
    jetsOut = recordable(jetDef.fullname())
    jetDef = solveDependencies(jetDef)

    if not (
        jetRecoDict["constitMod"] == ""
        and jetRecoDict["constitType"] == "tc"
        and jetRecoDict["clusterCalib"] == "lcw"
    ):
        alg = JetRecConfig.getConstitModAlg(
            jetDef, jetDef.inputdef,
            monTool=OnlineMon.getMonTool_Algorithm(f"HLTJets/{jetsOut}/"),
        )
        # getConstitModAlg will return None if there's nothing for it to do
        if alg is not None:
            acc.addEventAlgo(alg)

    pj_alg = JetRecConfig.getConstitPJGAlg(jetDef.inputdef)
    pj_name = pj_alg.OutputContainer
    acc.addEventAlgo(pj_alg)

    if use_tracking:

        # Make sure that the jets are constructed with the ghost tracks included
        merge_alg = CompFactory.PseudoJetMerger(
            f"PJMerger_{pj_name}",
            InputPJContainers=[pj_name, trkcolls["GhostTracks"]],
            OutputContainer=f"{pj_name}MergedWithGhostTracks",
        )
        # update the pseudo jet name
        pj_name = merge_alg.OutputContainer
        acc.addEventAlgo(merge_alg)
    jetDef._internalAtt["finalPJContainer"] = pj_name


    acc.addEventAlgo(
        JetRecConfig.getJetRecAlg(
            jetDef, JetOnlineMon.getMonTool_TrigJetAlgorithm(f"HLTJets/{jetsOut}/")
        ),
        primary=True,
    )

    return acc, jetsOut, jetDef


@AccumulatorCache
def StandardJetRecoCfg(flags, dataSource, clustersKey, trkcolls=None, **jetRecoDict):
    """ Full reconstruction for 'simple' (ungroomed, not reclustered) jets

    First the uncalibrated jets are built, then (if necessary) the calibrated jets are provided
    as a shallow copy.
    """
    if jetRecoDict["jetCalib"] == "nojcalib":
        return StandardJetBuildCfg(
            flags, dataSource, clustersKey, trkcolls, **jetRecoDict
        )

    # Schedule reconstruction w/o calibration
    jrdNoJCalib = copy.copy(jetRecoDict)
    jrdNoJCalib["jetCalib"] = "nojcalib"

    acc, jetsNoCalib, jetDefNoCalib = StandardJetBuildCfg(
        flags, dataSource, clustersKey, trkcolls, **jrdNoJCalib
    )
    # Get the calibration tool
    jetDef = jetDefNoCalib.clone()
    jetDef.suffix = jetDefNoCalib.suffix.replace("nojcalib", jetRecoDict["jetCalib"])

    if "sub" in jetRecoDict["jetCalib"]:
        # Add the event shape alg for area subtraction
        # WARNING : offline jets use the parameter voronoiRf = 0.9 ! we might want to harmonize this.
        eventShapeAlg = JetInputConfig.buildEventShapeAlg(jetDef, _jetNamePrefix,voronoiRf = 1.0 )
        acc.addEventAlgo(eventShapeAlg)
        rhoKey = str(eventShapeAlg.EventDensityTool.OutputContainer)
    else:
        rhoKey = "auto"


    # If we need JVT rerun the JVT modifier
    use_tracking = jetRecoDict["trkopt"] != "notrk"
    is_pflow = jetRecoDict["constitType"] == "pf"

    decorList = getDecorList(use_tracking, is_pflow)
    decorList += ["Jvt"]
    
    jetDef.modifiers = defineCalibMods(jetRecoDict, dataSource, rhoKey)
    if use_tracking:
        jetDef.modifiers += [f"JVT:{jetRecoDict['trkopt']}"]

    if not is_pflow and jetRecoDict["recoAlg"] == "a4":
        jetDef.modifiers += ["CaloQuality"]
        from TriggerMenuMT.HLT.Jet.JetRecoConfiguration import cleaningDict
        jetDef.modifiers += [f'Cleaning:{clean_wp}' for _,clean_wp in cleaningDict.items()]

    # make sure all modifiers info is ready before passing jetDef to JetRecConfig helpers
    jetDef = solveDependencies(jetDef) 
    # This algorithm creates the shallow copy and then also applies the calibration as part of the
    # modifiers list
    acc.addEventAlgo(
        JetRecConfig.getJetCopyAlg(
            jetsin=jetsNoCalib,
            jetsoutdef=jetDef,
            decorations=decorList,
            monTool=JetOnlineMon.getMonTool_TrigJetAlgorithm(
                "HLTJets/{}/".format(jetDef.fullname())
            ),
        )
    )

    # Filter the copied jet container so we only output jets with pt above jetPtMin
    jetPtMin = 10 * SystemOfUnits.GeV # 10 GeV minimum pt for jets to be seen by hypo
    jetsOut = "{}_pt10".format(jetDef.fullname())
    acc.addEventAlgo(
        CompFactory.JetViewAlg(
            f"jetview_{jetsOut}",
            InputContainer=jetDef.fullname(),
            OutputContainer=jetsOut,
            PtMin=jetPtMin,
            DecorDeps=decorList,
        )
    )

    return acc, jetsOut, jetDef


@AccumulatorCache
def GroomedJetRecoCfg(flags, dataSource, clustersKey, trkcolls=None, **jetRecoDict):
    """ Create the groomed jets

    First the ungroomed jets are created (using the standard configuration), then the grooming
    is applied
    """
    # Grooming needs the ungroomed jets to be built first,
    # so call the basic jet reco seq, then add a grooming alg

    ungroomedJRD = copy.copy(jetRecoDict)
    ungroomedJRD["recoAlg"] = ungroomedJRD["recoAlg"].rstrip(
        "tsd"
    )  # Drop grooming spec
    ungroomedJRD["jetCalib"] = "nojcalib"  # No need to calibrate

    acc, ungroomedJetsName, ungroomedDef = StandardJetBuildCfg(
        flags,
        dataSource,
        clustersKey,
        trkcolls,
        **ungroomedJRD,
    )

    groomDef = defineGroomedJets(jetRecoDict, ungroomedDef)
    jetsOut = recordable(groomDef.fullname())
    groomDef.modifiers = defineCalibMods(jetRecoDict, dataSource)
    groomDef.modifiers += [
        "Sort",
        "Filter:{}".format(getFilterCut(jetRecoDict["recoAlg"])),
    ]

    groomDef = solveGroomingDependencies(groomDef)
    acc.addEventAlgo(
        JetRecConfig.getJetGroomAlg(
            groomDef,
            JetOnlineMon.getMonTool_TrigJetAlgorithm(f"HLTJets/{jetsOut}/"),
        )
    )
    return acc, jetsOut, groomDef


@AccumulatorCache
def ReclusteredJetRecoCfg(flags, dataSource, clustersKey, trkcolls=None, **jetRecoDict):
    """ Create the reclustered jets

    First the input jets are built, then the reclustering algorithm is run
    """
    acc = ComponentAccumulator()

    basicJetRecoDict = copy.copy(jetRecoDict)
    basicJetRecoDict["recoAlg"] = "a4"  # Standard size for reclustered

    acc, basicJetsFiltered, basicJetDef = StandardJetRecoCfg(
        flags, dataSource, clustersKey, trkcolls, **basicJetRecoDict
    )

    rcJetPtMin = 15 * SystemOfUnits.GeV
    jetsOut = basicJetDef.fullname() + "_pt15"
    acc.addEventAlgo(
        CompFactory.JetViewAlg(
            f"jetview_{jetsOut}",
            InputContainer=basicJetDef.fullname(),
            OutputContainer=jetsOut,
            PtMin=rcJetPtMin,
        )
    )

    rcJetDef = defineReclusteredJets(
        jetRecoDict,
        jetsOut,
        basicJetDef.inputdef.label,
        _jetNamePrefix,
        f"_{jetRecoDict['jetCalib']}",
    )

    rcJetDef.modifiers = []

    rcConstitPJAlg = JetRecConfig.getConstitPJGAlg(rcJetDef.inputdef)
    rcConstitPJKey = str(rcConstitPJAlg.OutputContainer)
    acc.addEventAog(rcConstitPJAlg)

    rcJetDef._internalAtt["finalPJContainer"] = rcConstitPJKey

    acc.addEventAlgo(
        JetRecConfig.getJetRecAlg(
            rcJetDef,
            JetOnlineMon.getMonTool_TrigJetAlgorithm(
                "HLTJets/{}/".format(rcJetDef.fullname())
            ),
        )
    )

    return acc, recordable(rcJetDef.fullname()), rcJetDef
