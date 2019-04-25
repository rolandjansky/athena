# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# JobOption fragment to set up the AtlasFieldSvc
# Valerio Ippolito - Harvard University

# inspired by https://svnweb.cern.ch/trac/atlasoff/browser/MuonSpectrometer/MuonCnv/MuonCnvExample/trunk/python/MuonCalibConfig.py

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s", __name__)

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import GlobalFlags
from AthenaCommon import CfgMgr
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from MagFieldServices.MagFieldServicesConf import MagField__AtlasFieldSvc

#--------------------------------------------------------------

def AtlasFieldSvc(name="AtlasFieldSvc",**kwargs):
  if athenaCommonFlags.isOnline():
    kwargs.setdefault( "UseDCS", False )
    kwargs.setdefault( "UseSoleCurrent", 7730 )
    kwargs.setdefault( "UseToroCurrent", 20400 )
  else:
    kwargs.setdefault( "UseDCS", True )
  
  return CfgMgr.MagField__AtlasFieldSvc(name,**kwargs)

def H8FieldSvc(name="H8FieldSvc",**kwargs):
  return CfgMgr.MagField__H8FieldSvc(name,**kwargs)

def GetFieldSvc(name="AtlasFieldSvc",**kwargs):
  if GlobalFlags.DetGeo == 'ctbh8':
    return H8FieldSvc(name, **kwargs)
  else:
    return AtlasFieldSvc(name, **kwargs)
    
# The magneticfields is going to need a big update for MT, so this is all temporary. Ed
def MagneticFieldSvcCfg(flags):
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
        
    result.merge(addFolders(flags,['/GLOBAL/BField/Maps <noover/>'],detDb=db) )
    result.getService('IOVDbSvc').FoldersToMetaData     += ['/GLOBAL/BField/Maps'] # TODO fixme?
        
    if not flags.Common.isOnline:
        result.merge(addFolders(flags, ['/EXT/DCS/MAGNETS/SENSORDATA'], detDb='DCS_OFL', className="CondAttrListCollection") )
            
    kwargs={}
        
    if flags.Common.isOnline:
      kwargs.setdefault( "UseDCS", False )
      kwargs.setdefault( "UseSoleCurrent", 7730 )
      kwargs.setdefault( "UseToroCurrent", 20400 )
    else:
      kwargs.setdefault( "UseDCS", True )
    mag_field_svc = MagField__AtlasFieldSvc("AtlasFieldSvc",**kwargs)  
    result.addService(mag_field_svc,primary=True)
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


          
    f=open("MagneticFieldSvc.pkl","w")
    cfg.store(f)
    f.close()
