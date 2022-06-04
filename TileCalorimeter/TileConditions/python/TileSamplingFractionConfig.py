# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

"""Define methods to construct configured Tile sampling fraction conditions algorithm"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import LHCPeriod

def TileSamplingFractionCondAlgCfg(flags, **kwargs):
    """Return component accumulator with configured Tile sampling fraction conditions algorithm

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    Keyword arguments:
        Source -- source of Tile SamplingFraction conditions (COOL, FILE). Defaults to COOL.
        TileSamplingFraction -- name of Tile sampling fraction conditions object. Defaults to TileSamplingFraction.
    """

    acc = ComponentAccumulator()

    source = kwargs.get('Source', 'COOL')
    samplingFraction = kwargs.get('TileSamplingFraction', 'TileSamplingFraction')

    name = samplingFraction + 'CondAlg'

    if source == 'COOL':
        # Connect COOL Tile conditions proxies to the algorithm (default)

        samplingFractionFolder = '/TILE/OFL02/CALIB/SFR'
        
        TileCondProxyCoolFlt = CompFactory.getComp("TileCondProxyCool<TileCalibDrawerFlt>")
        samplingFractionProxy = TileCondProxyCoolFlt('TileCondProxyCool_SamplingFraction', Source = samplingFractionFolder)

        samplingFractionTag = None # Tag connected to global conditions tag is used by default
        if flags.GeoModel.Run >= LHCPeriod.Run4 or flags.Overlay.DataOverlay or flags.Sim.ISF.Simulator.usesFastCaloSim():
            samplingFractionTag = 'TileOfl02CalibSfr-SIM-07'

        from IOVDbSvc.IOVDbSvcConfig import addFolders
        acc.merge( addFolders(flags, samplingFractionFolder + f'<key>{samplingFractionFolder}</key>',
                              detDb='TILE_OFL', className='CondAttrListCollection', tag=samplingFractionTag, db='OFLP200') )

    elif source == 'FILE':
        # Connect FILE Tile conditions proxies to the algorithm
        TileCondProxyFileFlt = CompFactory.getComp("TileCondProxyFile<TileCalibDrawerFlt>")
        samplingFractionProxy = TileCondProxyFileFlt('TileCondProxyFile_SamplingFraction', Source = 'TileDefault.sfr')
    else:
        raise(Exception("Invalid source: %s" % source))

    G4Version = flags.Sim.G4Version
    G4VersionMajor, G4VersionMinor = G4Version.split(".")[1:3]
    G4V = int(G4VersionMajor) * 100 + int(G4VersionMinor)

    TileSamplingFractionCondAlg = CompFactory.getComp("TileSamplingFractionCondAlg")
    samplingFractionCondAlg = TileSamplingFractionCondAlg( name = name, G4Version = G4V,
                                                           ConditionsProxy = samplingFractionProxy,
                                                           TileCondData = samplingFraction)

    acc.addCondAlgo(samplingFractionCondAlg)

    return acc



if __name__ == "__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    
    # Test setup
    log.setLevel(DEBUG)

    ConfigFlags.Input.Files = defaultTestFiles.HITS_RUN2
    ConfigFlags.lock()

    acc = ComponentAccumulator()

    acc.merge( TileSamplingFractionCondAlgCfg(ConfigFlags) )

    acc.printConfig(withDetails = True, summariseProps = True)
    print(acc.getService('IOVDbSvc'))
    acc.store( open('TileSamplingFraction.pkl','wb') )

    print('All OK')

