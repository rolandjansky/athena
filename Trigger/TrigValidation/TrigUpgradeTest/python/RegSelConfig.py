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
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    
    ConfigFlags.lock()

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()


    acc.addConfig( RegSelConfig, ConfigFlags )

    f=open('RegSelConfig.pkl','w')
    acc.store(f)
    f.close()
