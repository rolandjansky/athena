# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Logging import logging


def ISFCollectionMergerCfg(flags,name="ISF_CollectionMerger", **kwargs):
    kwargs.setdefault( "InputBCMHits",              [ ] )
    kwargs.setdefault( "InputBLMHits",              [ ] )
    kwargs.setdefault( "InputPixelHits",            [ ] )
    kwargs.setdefault( "InputSCTHits",              [ ] )
    kwargs.setdefault( "InputTRTUncompressedHits",  [ ] )

    kwargs.setdefault( "InputITkPixelHits",         [ ] )
    kwargs.setdefault( "InputITkStripHits",         [ ] )
    kwargs.setdefault( "InputPLRHits",              [ ] )
    kwargs.setdefault( "InputHGTDHits",             [ ] )

    kwargs.setdefault( "InputLArEMBHits",           [ ] )
    kwargs.setdefault( "InputLArEMECHits",          [ ] )
    kwargs.setdefault( "InputLArFCALHits",          [ ] )
    kwargs.setdefault( "InputLArHECHits",           [ ] )

    kwargs.setdefault( "InputTileHits",             [ ] )
    kwargs.setdefault( "InputMBTSHits",             [ ] )

    kwargs.setdefault( "InputCSCHits",              [ ] )
    kwargs.setdefault( "InputMDTHits",              [ ] )
    kwargs.setdefault( "InputRPCHits",              [ ] )
    kwargs.setdefault( "InputTGCHits",              [ ] )
    kwargs.setdefault( "InputsTGCHits",             [ ] )
    kwargs.setdefault( "InputMMHits",               [ ] )

    hardscatterSG=""
    try:
        if flags.Sim.DoFullChain and (flags.Digitization.PileUp is True):
            hardscatterSG = "OriginalEvent_SG+"
    except RuntimeError as err:
        msg = logging.getLogger(name)
        msg.info("Caught {!r}. "
                 "Digitization flags are unavailable in AthSimulation."
                 .format(err))
        # FIXME: Digitization is not the AthSimulation project;
        # support for FastChain may need to be added in the future.
    from SimulationConfig.SimEnums import SimulationFlavour
    if flags.Detector.EnableBCM and flags.Sim.ISF.Simulator not in [SimulationFlavour.ATLFASTIIF_G4MS, SimulationFlavour.ATLFASTIIFMT, SimulationFlavour.ATLFAST3F_G4MS]:
        kwargs.setdefault( "OutputBCMHits",             hardscatterSG+"BCMHits"             )
        kwargs.setdefault( "OutputBLMHits",             hardscatterSG+"BLMHits"             )
    if flags.Detector.EnablePixel:
        kwargs.setdefault( "OutputPixelHits",           hardscatterSG+"PixelHits"           )
    if flags.Detector.EnableSCT:
        kwargs.setdefault( "OutputSCTHits",             hardscatterSG+"SCT_Hits"            )
    if flags.Detector.EnableTRT:
        kwargs.setdefault( "OutputTRTUncompressedHits", hardscatterSG+"TRTUncompressedHits" )

    if flags.Detector.EnableITkPixel:
        kwargs.setdefault( "OutputITkPixelHits",        hardscatterSG+"ITkPixelHits"        )
    if flags.Detector.EnableITkStrip:
        kwargs.setdefault( "OutputITkStripHits",        hardscatterSG+"ITkStripHits"        )
    if flags.Detector.EnablePLR:
        kwargs.setdefault( "OutputPLRHits",             hardscatterSG+"PLR_Hits"            )
    if flags.Detector.EnableHGTD:
        kwargs.setdefault( "OutputHGTDHits",            hardscatterSG+"HGTD_Hits"           )

    if flags.Detector.EnableLAr:
        kwargs.setdefault( "OutputLArEMBHits",          hardscatterSG+"LArHitEMB"           )
        kwargs.setdefault( "OutputLArEMECHits",         hardscatterSG+"LArHitEMEC"          )
        kwargs.setdefault( "OutputLArFCALHits",         hardscatterSG+"LArHitFCAL"          )
        kwargs.setdefault( "OutputLArHECHits",          hardscatterSG+"LArHitHEC"           )

    if flags.Detector.EnableTile:
        kwargs.setdefault( "OutputTileHits",            hardscatterSG+"TileHitVec"          )
    if flags.Detector.EnableMBTS:
        kwargs.setdefault( "OutputMBTSHits",            hardscatterSG+"MBTSHits"            )

    if flags.Detector.EnableCSC:
        kwargs.setdefault( "OutputCSCHits",             hardscatterSG+"CSC_Hits"            )
    if flags.Detector.EnableMDT:
        kwargs.setdefault( "OutputMDTHits",             hardscatterSG+"MDT_Hits"            )
    if flags.Detector.EnableRPC:
        kwargs.setdefault( "OutputRPCHits",             hardscatterSG+"RPC_Hits"            )
    if flags.Detector.EnableTGC:
        kwargs.setdefault( "OutputTGCHits",             hardscatterSG+"TGC_Hits"            )
    if flags.Detector.EnablesTGC:
        kwargs.setdefault( "OutputsTGCHits",            hardscatterSG+"sTGC_Hits"           )
    if flags.Detector.EnableMM:
        kwargs.setdefault( "OutputMMHits",              hardscatterSG+"MM_Hits"             )
    return CompFactory.ISF.CollectionMerger(name, **kwargs)


def CollectionMergerCfg(ConfigFlags,
                        bare_collection_name,
                        mergeable_collection_suffix,
                        merger_input_property,
                        region):
    """Generates and returns a collection name that is also registered to
       the ISF CollectionMerger algorithm.

       :param bare_collection_name: name of the collection if no merging
                                    is taking place.
       :param mergeable_collection_suffix: suffix to the collection in
                                           case merging is taking place.
       :param merger_input_property: name of the Input* property in the
                                     CollectionMerger algorithm to add the
                                     mergeable collection to."""

    result = ComponentAccumulator()
    if ConfigFlags.Sim.ISFRun and ConfigFlags.Sim.ISF.HITSMergingRequired.get(region, True):
        mergeable_collection = f'{bare_collection_name}{mergeable_collection_suffix}'

        from ISF_Algorithms.CollectionMergerConfig import ISFCollectionMergerCfg
        algo = ISFCollectionMergerCfg(ConfigFlags)
        if ConfigFlags.Sim.ISF.ReSimulation:
            result.addEventAlgo(algo,'SimSequence') # TODO ideally this would be configurable
        else:
            result.addEventAlgo(algo)

        input_attribute_name = f'Input{merger_input_property}'
        merger_input_collections = getattr(algo, input_attribute_name)
        merger_input_collections.append(mergeable_collection)
    else:
        mergeable_collection = bare_collection_name
    return result, mergeable_collection
