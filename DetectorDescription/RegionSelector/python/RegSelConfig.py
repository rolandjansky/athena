#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#



def RegSelConfig( flags ):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from RegionSelector.RegionSelectorConf import RegSelSvc
    from AthenaCommon.SystemOfUnits import mm
    acc = ComponentAccumulator()

    regSel = RegSelSvc()
    regSel.DeltaZ = 225 * mm

    # there will be ifology here enabling only the configured detectors
    from LArRegionSelector.LArRegionSelectorConf import LArRegionSelectorTable
    larTable =  LArRegionSelectorTable(name="LArRegionSelectorTable")
    acc.addPublicTool( larTable )
    regSel.LArRegionSelectorTable      = larTable


    from TileRawUtils.TileRawUtilsConf import TileRegionSelectorTable
    tileTable =  TileRegionSelectorTable(name="TileRegionSelectorTable")
    acc.addPublicTool( tileTable )
    # ??? that is puzzle, the RegSelSvc seems not to ahev such a property
    # while it is set in:RegSelSvcDefault.py
    #regSel.TileRegionSelectorTable     = tileTable

    regSel.enableCalo = True

    acc.addService( regSel )
    return acc

if __name__ == "__main__":
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"]

    acc = RegSelConfig( ConfigFlags )
    acc.store( file( "test.pkl", "w" ) )
    print "All OK"
