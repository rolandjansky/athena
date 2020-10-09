# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define method to construct configured Tile info loader service"""


from __future__ import print_function
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def TileInfoLoaderCfg(flags, **kwargs):
    """Return component accumulator with configured Tile info loader service

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    """

    from AthenaCommon.Logging import logging
    msg = logging.getLogger('TileInfoLoaderCfg')

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    acc = ComponentAccumulator()
    acc.merge (TileCablingSvcCfg(flags))

    if 'NoiseScaleIndex' not in kwargs:
        if flags.Tile.doOpt2 and not flags.Tile.doOptATLAS:
            msg.info("Adjusting TileInfo to return cell noise for Opt.Filter with iterations")
            kwargs['NoiseScaleIndex'] = 2 # Noise for Optimal Filter with iterations
        else:
            msg.info("Adjusting TileInfo to return cell noise for Opt.Filter without iterations")
            kwargs['NoiseScaleIndex'] = 1 # Noise for Optimal Filter without iterations


    if (flags.Input.isMC or flags.Detector.OverlayTile) and ('TileHitVec' in flags.Input.Collections or 'TileHitVec' in flags.Input.SecondaryCollections):

        G4Version = flags.Sim.G4Version
        G4VersionMajor, G4VersionMinor = G4Version.split(".")[1:3]
        G4V = int(G4VersionMajor) + int(G4VersionMinor) / 100.

        physicsList = flags.Sim.PhysicsList

        if 'EmScaleA' not in kwargs:

            # Default value since May-2011
            EmScaleA = 34.0

            # Default value for G4 9.6 since Nov-2013 (need to check G4 version as well)
            if physicsList == 'FTFP_BERT' or (physicsList == 'QGSP_BERT' and G4V > 9.05999) :
                EmScaleA = 33.9

            # Default value for G4 10.0 since June-2016
            # see https://indico.cern.ch/event/489520/contributions/2193913/attachments/1285565/1914309/sampling_fractions.pdf
            if G4V >= 10.0 :
                EmScaleA = 33.7

            # Value for G4 10.6 since September-2020
            # see https://its.cern.ch/jira/browse/ATLASSIM-4401
            if G4V >= 10.05 :
                EmScaleA = 32.9

            # Old value
            if physicsList == 'QGSP_EMV' or physicsList == 'QGSP_BERT_EMV' or physicsList == '':
                EmScaleA = 35.9

            kwargs['EmScaleA'] = EmScaleA  # 1/s.f. value for all normal cells

        else:
            EmScaleA = kwargs['EmScaleA']

        msg.info("Using 1/s.f. = %s for %s physics list and G4version %s (%s)", EmScaleA, physicsList, G4V, G4Version)

        kwargs.setdefault('TileNoise', flags.Digitization.DoCaloNoise)
        if kwargs['TileNoise']:
            msg.info("Switching ON noise in Tile Digitization" )
        else:
            msg.info("Switching OFF noise in Tile Digitization" )


    TileInfoLoader=CompFactory.TileInfoLoader
    acc.addService(TileInfoLoader(**kwargs), primary = True, create=True)

    return acc


if __name__ == "__main__":

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    
    # Test setup
    log.setLevel(DEBUG)

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()

    acc = ComponentAccumulator()
    acc.merge( TileInfoLoaderCfg(ConfigFlags) )

    acc.printConfig(withDetails = True, summariseProps = True)
    print(acc.getService('TileInfoLoader'))
    acc.store( open('TileInfoLoader.pkl','wb') )

    print('All OK')
