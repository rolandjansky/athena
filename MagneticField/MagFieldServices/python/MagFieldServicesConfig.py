# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    
def AtlasFieldCacheCondAlgCfg(flags, **kwargs):
    '''
    Configure the CondAlgsfor the ATLAS magnetic field.
    '''
    result=ComponentAccumulator()
    
    # initialise required conditions DB folders
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    
    if flags.Input.isMC:
        db='GLOBAL_OFL'
    else:
        db='GLOBAL'
        
    result.merge(addFolders(flags,['/GLOBAL/BField/Maps <noover/>'], detDb=db, className="CondAttrListCollection") )
        
    if not flags.Common.isOnline:
        result.merge(addFolders(flags, ['/EXT/DCS/MAGNETS/SENSORDATA'], detDb='DCS_OFL', className="CondAttrListCollection") )
            

    # AtlasFieldMapCondAlg - for reading in map
    afmArgs = {
      "name": "AtlasFieldMapCondAlg",
    }
    if flags.Common.isOnline:
      # online does not use DCS
      afmArgs.update( UseMapsFromCOOL = False )
    else:
      # UseMapsFromCOOL is default for standard running
      afmArgs.update( UseMapsFromCOOL =  True)
      # However, for tests, this must be turned off. It is detected
      # when UseDCS is set to False - UseDCS is directly an option for the field cache alg 
      if 'UseDCS' in kwargs and not kwargs['UseDCS']:
        afmArgs['UseMapsFromCOOL'] = False
    magFieldMapCondAlg = CompFactory.MagField.AtlasFieldMapCondAlg(**afmArgs) 
    result.addCondAlgo(magFieldMapCondAlg)

    # AtlasFieldCacheCondAlg - for reading in current
    afcArgs = {
      "name": "AtlasFieldCacheCondAlg",
    }
    if flags.Common.isOnline:
      afcArgs.update( UseDCS = False )
      afcArgs.update( UseSoleCurrent = 7730 )
      afcArgs.update( UseToroCurrent = 20400 )
      afcArgs.update( LockMapCurrents = True )
    else:
      afcArgs.update( UseDCS = True )
    # For test, UseDCS is set to False
    if 'UseDCS' in kwargs:
      afcArgs['UseDCS'] = kwargs['UseDCS']
    magFieldCacheCondAlg = CompFactory.MagField.AtlasFieldCacheCondAlg(**afcArgs) 
    result.addCondAlgo(magFieldCacheCondAlg)
    
    return result 
    
if __name__=="__main__":
    # To run this, do e.g. 
    # python ../athena/MagneticField/MagFieldServices/python/MagFieldServicesConfig.py

    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import VERBOSE
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    log.setLevel(VERBOSE)
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.Input.isMC = False
    ConfigFlags.lock()

    cfg=ComponentAccumulator()

    acc  = AtlasFieldCacheCondAlgCfg(ConfigFlags)
    log.verbose(acc)
    cfg.merge(acc)

    f=open("MagneticFieldSvc.pkl","wb")
    cfg.store(f)
    f.close()
