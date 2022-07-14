# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

__doc__ = """
          Instantiate more custom track isolation in derivation
          """

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def DerivationTrackIsoCfg(flags,**jwarg):

    mlog = logging.getLogger('DerivationTrackIsoConfig')
    mlog.info('Starting track isolation for derivation')

    acc = ComponentAccumulator()

    from IsolationAlgs.IsoToolsConfig import (
        isoTTVAToolCfg, TrackIsolationToolCfg )
    from InDetConfig.InDetTrackSelectionToolConfig import (
        isoTrackSelectionToolCfg )
    from xAODPrimitives.xAODIso import xAODIso as isoPar
    # dR in decreasing order
    ptcone_list = [ [ isoPar.ptcone40, isoPar.ptcone30, isoPar.ptcone20 ] ]
    trkcor_list = [ [ isoPar.coreTrackPtr ] ]

    if 'object_types' not in jwarg:
        object_types = ("Electrons", "Photons", "Muons")
    else:
        object_types = jwarg['object_types']

    if 'WP' not in jwarg:
        WP = 'Nonprompt_All_MaxWeight'
    else:
        WP = jwarg['WP']
        # TrackIsolationTool need a specific track-vertex association tool
        #   if WP is not Nonprompt_All_MaxWeight
        ttvaCA = acc.popToolsAndMerge(
            isoTTVAToolCfg(flags, WorkingPoint = WP))

    postfix = ''
    if 'postfix' in jwarg:
        postfix = jwarg['postfix']

    if 'ptCuts' not in jwarg:
        ptCuts = (500, 1000)
    else:
        ptCuts = jwarg['ptCuts']

    do_egamma = any(x in object_types for x in ("Electrons", "Photons"))

    mlog.info('will isolate objects '+" ".join(object_types)+' with TTVA WP = '+WP)

    for track_pt in ptCuts:
        for loose_cone in (True, False):
            if loose_cone and not do_egamma:
                # Loose cone isolation variables only for electrons and photons
                continue

            cone_str = "LooseCone" if loose_cone else ""

            # The custom variable suffix
            name = f"{WP}TTVA{cone_str}_pt{track_pt}"

            # Build up extra IsolationBuilder kwargs
            kwargs = {}
            if "Electrons" in object_types:
                kwargs["ElIsoTypes"] = ptcone_list
                kwargs["ElCorTypes"] = trkcor_list
                kwargs["ElCorTypesExtra"] = [[]]
                kwargs["CustomConfigurationNameEl"] = name
            if "Electrons" in object_types and "LRT" in postfix:
                kwargs["ElectronCollectionContainerName"] = "LRTElectrons"
            if "Photons" in object_types:
                kwargs["PhIsoTypes"] = ptcone_list
                kwargs["PhCorTypes"] = trkcor_list
                kwargs["PhCorTypesExtra"] = [[]]
                kwargs["CustomConfigurationNamePh"] = name
            if "Muons" in object_types and not loose_cone:
                kwargs["MuIsoTypes"] = ptcone_list
                kwargs["MuCorTypes"] = trkcor_list
                kwargs["MuCorTypesExtra"] = [[]]
                kwargs["CustomConfigurationNameMu"] = name
            if "Muons" in object_types and not loose_cone and "LRT" in postfix:
                kwargs["MuonCollectionContainerName"] = "MuonsLRT"
                
            ## Track isolation tool 
            toolkwargs = {}
            # the TTVA tool is not the default choice
            if 'WP' in jwarg:
                toolkwargs['TTVATool'] = ttvaCA
            # and a track selection tool
            toolkwargs['TrackSelectionTool'] = acc.popToolsAndMerge(
                isoTrackSelectionToolCfg(flags, minPt = track_pt))
            #
            if loose_cone:
                toolkwargs['CoreTrackEtaRange'] = 0.01
            kwargs['TrackIsolationTool'] = acc.popToolsAndMerge(
                TrackIsolationToolCfg(flags,**toolkwargs))

            # The algorithm
            kwargs['name'] = f"IsolationBuilder{WP}{cone_str}{track_pt}{postfix}"
            acc.addEventAlgo(CompFactory.IsolationBuilder(**kwargs))

    return acc

def iso_vars(WP = "Nonprompt_All_MaxWeight"):
    # Get the list of isolation variables calculated by these functions
    iso_vars = []
    for track_pt in 500, 1000:
        for cone_str in "", "LooseCone":
            name = f"{WP}TTVA{cone_str}_pt{track_pt}"
            iso_vars += [ "ptconeCorrBitset_"+name,
                             "ptconecoreTrackPtrCorrection_"+name ]
            for cone_size in 20, 30, 40:
                for var_str in "", "var":
                    iso_vars.append(f"pt{var_str}cone{cone_size}_{name}")
    return iso_vars

if __name__ == "__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.ComponentAccumulator import printProperties
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    flags.Input.Files = defaultTestFiles.RDO_RUN2
    flags.Output.doWriteESD = True  # To test the ESD parts
    flags.Output.doWriteAOD = True  # To test the AOD parts
    flags.lock()

    mlog = logging.getLogger("derivationTrackIsolationConfigTest")
    mlog.info("Configuring derivation track isolation: ")

    acc = MainServicesCfg(flags)
    acc.merge(DerivationTrackIsoCfg(flags))
    # print config of some of the algs :
    for par in ('500', '1000', 'LooseCone1000'):
        print('------ Config for ',par,' ----------')
        name = "IsolationBuilderNonprompt_All_MaxWeight"+par
        printProperties(mlog,
                        acc.getEventAlgo(name),
                        nestLevel=1,
                        printDefaults=True)
