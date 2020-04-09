# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# JobOption fragment to set up the AtlasFieldSvc and conditions algorithms
# Valerio Ippolito - Harvard University

# inspired by https://svnweb.cern.ch/trac/atlasoff/browser/MuonSpectrometer/MuonCnv/MuonCnvExample/trunk/python/MuonCalibConfig.py

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import GlobalFlags
from AthenaCommon import CfgMgr
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from MagFieldServices.MagFieldServicesConf import MagField__AtlasFieldCacheCondAlg
from MagFieldServices.MagFieldServicesConf import MagField__AtlasFieldMapCondAlg

MagField__AtlasFieldSvc=CompFactory.MagField__AtlasFieldSvc

#--------------------------------------------------------------

def AtlasFieldSvc(name="AtlasFieldSvc",**kwargs):
  if athenaCommonFlags.isOnline():
    kwargs.setdefault( "UseDCS", False )
    kwargs.setdefault( "UseSoleCurrent", 7730 )
    kwargs.setdefault( "UseToroCurrent", 20400 )
  else:
    kwargs.setdefault( "UseDCS", True )
    # kwargs.setdefault( "UseDCS", False )
    # kwargs.setdefault( "UseSoleCurrent", 7730 )
    # kwargs.setdefault( "UseToroCurrent", 20400 )

    # kwargs.setdefault( "UseDCS", False )
    # kwargs.setdefault( "UseSoleCurrent", 12000 )
    # kwargs.setdefault( "UseToroCurrent", 20400 )

    
  return CfgMgr.MagField__AtlasFieldSvc(name,**kwargs)

def AtlasMTFieldSvc(name="AtlasMTFieldSvc",**kwargs):
  # if athenaCommonFlags.isOnline():
  #   kwargs.setdefault( "UseSoleCurrent", 7730 )
  #   kwargs.setdefault( "UseToroCurrent", 20400 )
  return CfgMgr.MagField__AtlasMTFieldSvc(name,**kwargs)

def AtlasFieldCacheCondAlg(name="AtlasFieldCacheCondAlg",**kwargs):
  if athenaCommonFlags.isOnline():
    kwargs.setdefault( "UseDCS", False )
    # currents for scaling wrt to map currents
    kwargs.setdefault( "UseSoleCurrent", 7730 )
    kwargs.setdefault( "UseToroCurrent", 20400 )
  else:
    kwargs.setdefault( "UseDCS", True )
    kwargs.setdefault( "UseNewBfieldCache", True )
    # kwargs.setdefault( "UseNewBfieldCache", False )
    # kwargs.setdefault( "UseDCS", False )
    # kwargs.setdefault( "UseSoleCurrent", 12000 )
    # kwargs.setdefault( "UseToroCurrent", 20400 )
  return CfgMgr.MagField__AtlasFieldCacheCondAlg(name,**kwargs)

def AtlasFieldMapCondAlg(name="AtlasFieldMapCondAlg",**kwargs):
  if athenaCommonFlags.isOnline():
    # The following are the defaults - added here to be clear
    kwargs.setdefault( "UseMapsFromCOOL", True )
    # kwargs.setdefault( "MapSoleCurrent", 7730 )
    # kwargs.setdefault( "MapToroCurrent", 20400 )
  else:
    # The following are the defaults - added here to be clear
    kwargs.setdefault( "UseMapsFromCOOL", True )
    # kwargs.setdefault( "MapSoleCurrent", 7730 )
    # kwargs.setdefault( "MapToroCurrent", 20400 )
  return CfgMgr.MagField__AtlasFieldMapCondAlg(name,**kwargs)

    

def H8FieldSvc(name="H8FieldSvc",**kwargs):
  return CfgMgr.MagField__H8FieldSvc(name,**kwargs)

def GetFieldSvc(name="AtlasFieldSvc",**kwargs):
  if GlobalFlags.DetGeo == 'ctbh8':
    return H8FieldSvc(name, **kwargs)
  else:
    return AtlasFieldSvc(name, **kwargs)
    
def GetFieldCacheCondAlg(name="AtlasFieldCacheCondAlg",**kwargs):
    return AtlasFieldCacheCondAlg(name, **kwargs)
    
def GetFieldMapCondAlg(name="AtlasFieldMapCondAlg",**kwargs):
    return AtlasFieldMapCondAlg(name, **kwargs)
    
# The magneticfields is going to need a big update for MT, so this is all temporary. Ed
def MagneticFieldSvcCfg(flags, **kwargs):
    result=ComponentAccumulator()
    
    # initialise required conditions DB folders
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    
    # This handles addFolderSplitMC https://gitlab.cern.ch/atlas/athena/blob/master/Database/IOVDbSvc/python/CondDB.py#L240
    # i.e. replaces conddb.addFolderSplitMC('GLOBAL','/GLOBAL/BField/Maps <noover/>','/GLOBAL/BField/Maps <noover/>')
    # def addFolders(configFlags,folderstrings,detDb=None,className=None):
    if flags.Input.isMC:
        db='GLOBAL_OFL'
    else:
        db='GLOBAL'
        
    result.merge(addFolders(flags,['/GLOBAL/BField/Maps <noover/>'], detDb=db, className="CondAttrListCollection") )
        
    if not flags.Common.isOnline:
        result.merge(addFolders(flags, ['/EXT/DCS/MAGNETS/SENSORDATA'], detDb='DCS_OFL', className="CondAttrListCollection") )
            

    # AtlasFieldSvc - old one
    afsArgs = {
      "name": "AtlasFieldSvc",
    }
    if flags.Common.isOnline:
      afsArgs.update( UseDCS = False )
      afsArgs.update( UseSoleCurrent = 7730 )
      afsArgs.update( UseToroCurrent = 20400 )
    else:
      afsArgs.update( UseDCS = True )
    mag_field_svc = MagField__AtlasFieldSvc(**afsArgs)  
    result.addService(mag_field_svc, primary=True)

    # AtlasFieldMapCondAlg - for reading in map
    afmArgs = {
      "name": "AtlasFieldMapCondAlg",
    }
    afmArgs.update( UseMapsFromCOOL = True )
    mag_field_map_cond_alg = MagField__AtlasFieldMapCondAlg(**afmArgs) 
    result.addCondAlgo(mag_field_map_cond_alg)
    
    # AtlasFieldCacheCondAlg - for reading in current
    afcArgs = {
      "name": "AtlasFieldCacheCondAlg",
    }
    if flags.Common.isOnline:
      afcArgs.update( UseDCS = False )
      afcArgs.update( UseSoleCurrent = 7730 )
      afcArgs.update( UseToroCurrent = 20400 )
    else:
      afcArgs.update( UseDCS = True )
      afcArgs.update( UseNewBfieldCache = True )
    mag_field_cache_cond_alg = MagField__AtlasFieldCacheCondAlg(**afcArgs) 
    result.addCondAlgo(mag_field_cache_cond_alg)
    
    return result 
    
if __name__=="__main__":
    # To run this, do e.g. 
    # python ../athena/MagneticField/MagFieldServices/python/MagFieldServicesConfig.py
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import VERBOSE
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    log.setLevel(VERBOSE)
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.Input.isMC = False
    ConfigFlags.lock()

    cfg=ComponentAccumulator()

    acc  = MagneticFieldSvcCfg(ConfigFlags)
    log.verbose(acc.getPrimary())    
    cfg.merge(acc)


          
    f=open("MagneticFieldSvc.pkl","wb")
    cfg.store(f)
    f.close()
